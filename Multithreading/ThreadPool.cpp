/*
 * Thread Pool Implementation in C++
 * 
 * A thread pool is a software design pattern for managing multiple threads efficiently.
 * Instead of creating and destroying threads for each task, a thread pool:
 * 1. Pre-creates a fixed number of worker threads
 * 2. Maintains a queue of tasks to be executed
 * 3. Worker threads continuously fetch and execute tasks from the queue
 * 4. Reduces overhead of thread creation/destruction
 * 5. Controls the level of parallelism
 */

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <chrono>

using namespace std;

// ThreadPool class manages a pool of worker threads
class ThreadPool {
private:
    // Vector to store worker threads
    vector<thread> workers;
    
    // Queue to store pending tasks (functions to be executed)
    queue<function<void()>> tasks;
    
    // Mutex for synchronizing access to shared task queue
    mutex queueMutex;
    
    // Condition variable to notify waiting threads when tasks are available
    condition_variable condition;
    
    // Flag to indicate if the thread pool should stop
    bool stop;

public:
    // Constructor: Creates a thread pool with specified number of threads
    ThreadPool(size_t numThreads) : stop(false) {
        // Create worker threads
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this, i] {
                // Each worker thread runs this infinite loop
                while (true) {
                    function<void()> task;
                    
                    // Critical section: Access shared resources
                    {
                        unique_lock<mutex> lock(this->queueMutex);
                        
                        // Wait until there's a task or stop signal
                        // The condition variable releases the lock while waiting
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });
                        
                        // Exit if stop flag is set and no more tasks
                        if (this->stop && this->tasks.empty()) {
                            cout << "Worker thread " << i << " exiting" << endl;
                            return;
                        }
                        
                        // Get the next task from the queue
                        task = move(this->tasks.front());
                        this->tasks.pop();
                    }
                    
                    // Execute the task outside the critical section
                    cout << "Thread " << i << " executing task" << endl;
                    task();
                }
            });
        }
        cout << "Thread pool initialized with " << numThreads << " threads" << endl;
    }
    
    // Template method to enqueue a task and return a future for the result
    // Uses decltype to deduce the return type of the function at compile time
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> future<decltype(f(args...))> {
        
        using return_type = decltype(f(args...));
        
        // Create a packaged_task to wrap the function and capture its return value
        // We use shared_ptr because:
        // 1. packaged_task is move-only (not copyable), but the lambda we queue needs to be copyable
        // 2. The lambda [task]() captures the shared_ptr by value, allowing multiple copies
        // 3. The task must outlive the enqueue call and be accessible when the worker thread executes it hence not passing by reference.
        // 4. shared_ptr ensures the task isn't destroyed until the worker thread completes execution
        auto task = make_shared<packaged_task<return_type()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        
        // Get the future associated with the packaged_task
        future<return_type> result = task->get_future();
        
        // Critical section: Add task to queue
        {
            unique_lock<mutex> lock(queueMutex);
            
            // Don't allow enqueueing after stopping the pool
            if (stop) {
                throw runtime_error("Cannot enqueue on stopped ThreadPool");
            }
            
            // Add the task to the queue
            tasks.emplace([task]()-> void { 
                (*task)(); 
                });

			// Not needed here as {} will call lock.unlock();
        }
        
        // Notify one waiting worker thread that a task is available
        condition.notify_one();
        
        return result;
    }
    
    // Destructor: Cleanup and join all threads
    ~ThreadPool() {
        // Set stop flag and notify all threads
        {
            unique_lock<mutex> lock(queueMutex);
            stop = true;
        }
        
        // Wake up all waiting threads
        condition.notify_all();
        
        // Wait for all threads to finish
        for (thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        
        cout << "Thread pool destroyed" << endl;
    }
    
    // Get the number of pending tasks
    size_t getPendingTaskCount() {
        unique_lock<mutex> lock(queueMutex);
        return tasks.size();
    }
};

// Example task functions

// Simple task that prints a message
void simpleTask(int id) {
    cout << "  Task " << id << " started" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "  Task " << id << " completed" << endl;
}

// Task that returns a computed value
int computeTask(int a, int b) {
    cout << "  Computing " << a << " + " << b << endl;
    this_thread::sleep_for(chrono::milliseconds(300));
    return a + b;
}

// CPU intensive task
long long factorial(int n) {
    cout << "  Computing factorial of " << n << endl;
    long long result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    this_thread::sleep_for(chrono::milliseconds(200));
    return result;
}

// Main function demonstrating thread pool usage
int main() {
    cout << "=== Thread Pool Demonstration ===" << endl;
    cout << "Hardware concurrency: " << thread::hardware_concurrency() << " threads" << endl;
    cout << endl;
    
    // Create a thread pool with 4 worker threads
    ThreadPool pool(4);
    cout << endl;
    
    // Example 1: Enqueue simple tasks without return values
    cout << "Example 1: Simple tasks without return values" << endl;
    for (int i = 0; i < 6; ++i) {
        pool.enqueue(simpleTask, i);
    }
    this_thread::sleep_for(chrono::seconds(2));
    cout << endl;
    
    // Example 2: Enqueue tasks with return values
    cout << "Example 2: Tasks with return values" << endl;
    vector<future<int>> results;
    
    for (int i = 0; i < 5; ++i) {
        results.emplace_back(pool.enqueue(computeTask, i, i * 10));
    }
    
    // Retrieve results from futures
    cout << "Results:" << endl;
    for (size_t i = 0; i < results.size(); ++i) {
        cout << "  Result " << i << ": " << results[i].get() << endl;
    }
    cout << endl;
    
    // Example 3: CPU intensive tasks
    cout << "Example 3: CPU intensive tasks (factorial calculations)" << endl;
    vector<future<long long>> factorialResults;
    
    for (int i = 5; i <= 10; ++i) {
        factorialResults.emplace_back(pool.enqueue(factorial, i));
    }
    
    // Display factorial results
    cout << "Factorial Results:" << endl;
    for (size_t i = 0; i < factorialResults.size(); ++i) {
        cout << "  Factorial(" << (i + 5) << ") = " << factorialResults[i].get() << endl;
    }
    cout << endl;
    
    // Example 4: Lambda functions
    cout << "Example 4: Lambda functions as tasks" << endl;
    auto future1 = pool.enqueue([] {
        cout << "  Lambda task 1 executing" << endl;
        this_thread::sleep_for(chrono::milliseconds(300));
        return "Lambda task 1 completed";
    });
    
    auto future2 = pool.enqueue([](const string& msg) {
        cout << "  Lambda task 2: " << msg << endl;
        return 42;
    }, "Processing data");
    
    cout << "  " << future1.get() << endl;
    cout << "  Lambda task 2 result: " << future2.get() << endl;
    cout << endl;
    
    // Show pending tasks
    cout << "Pending tasks: " << pool.getPendingTaskCount() << endl;
    
    cout << "\n=== All tasks completed, thread pool will be destroyed ===" << endl;
    
    // ThreadPool destructor will be called automatically
    // It will wait for all threads to complete their work
    
    return 0;
}

/*
 * Key Concepts Demonstrated:
 * 
 * 1. Thread Pool Creation: Fixed number of worker threads are created upfront
 * 2. Task Queue: Tasks are stored in a queue and executed by available threads
 * 3. Synchronization: Mutex and condition variables ensure thread-safe operations
 * 4. Future/Promise: Allows retrieving results from asynchronous tasks
 * 5. Resource Management: Proper cleanup and thread joining in destructor
 * 
 * Advantages of Thread Pools:
 * - Reduced overhead: No need to create/destroy threads repeatedly
 * - Better resource management: Limits the number of concurrent threads
 * - Improved performance: Threads are reused for multiple tasks
 * - Easier to manage: Centralized thread management
 * 
 * Use Cases:
 * - Web servers handling multiple client requests
 * - Background job processing
 * - Parallel data processing
 * - Concurrent I/O operations
 */
