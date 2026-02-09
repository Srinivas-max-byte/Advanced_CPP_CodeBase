/*
 * Producer-Consumer Problem using Mutex and Condition Variables
 * 
 * The Producer-Consumer problem is a classic synchronization problem in concurrent programming.
 * 
 * Problem Description:
 * - Producers generate data and put it into a shared buffer
 * - Consumers take data from the shared buffer and process it
 * - The buffer has a fixed size (bounded buffer)
 * 
 * Challenges:
 * 1. Producers must wait if the buffer is full
 * 2. Consumers must wait if the buffer is empty
 * 3. Access to the shared buffer must be synchronized to avoid race conditions
 * 
 * Solution using:
 * - std::mutex: For mutual exclusion when accessing the shared buffer
 * - std::condition_variable: For signaling between producers and consumers
 * - std::queue: As the shared buffer
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <vector>
#include <random>

using namespace std;

// Shared buffer class that encapsulates the synchronized queue
class BoundedBuffer {
private:
    queue<int> buffer;              // The shared buffer (queue)
    const size_t capacity;          // Maximum capacity of the buffer
    mutex mtx;                      // Mutex for protecting the buffer
    condition_variable notFull;     // Condition variable: buffer is not full
    condition_variable notEmpty;    // Condition variable: buffer is not empty
    
    // Statistics
    int totalProduced;
    int totalConsumed;

public:
    BoundedBuffer(size_t cap) : capacity(cap), totalProduced(0), totalConsumed(0) {
        cout << "Buffer initialized with capacity: " << capacity << endl;
    }
    
    // Producer calls this method to add an item to the buffer
    void produce(int item, int producerId) {
        // Lock the mutex to ensure exclusive access to the buffer
        unique_lock<mutex> lock(mtx);
        
        // Wait while the buffer is full
        // The condition variable releases the lock while waiting
        // and reacquires it when notified
        notFull.wait(lock, [this] {
            return buffer.size() < capacity;
        });
        
        // Add item to the buffer
        buffer.push(item);
        totalProduced++;
        
        cout << "Producer " << producerId << " produced: " << item 
             << " | Buffer size: " << buffer.size() << "/" << capacity << endl;
        
        // Notify one waiting consumer that the buffer is not empty
        notEmpty.notify_one();
        
        // Lock is automatically released when unique_lock goes out of scope
    }
    
    // Consumer calls this method to remove an item from the buffer
    int consume(int consumerId) {
        // Lock the mutex to ensure exclusive access to the buffer
        unique_lock<mutex> lock(mtx);
        
        // Wait while the buffer is empty
        notEmpty.wait(lock, [this] {
            return !buffer.empty();
        });
        
        // Remove item from the buffer
        int item = buffer.front();
        buffer.pop();
        totalConsumed++;
        
        cout << "Consumer " << consumerId << " consumed: " << item 
             << " | Buffer size: " << buffer.size() << "/" << capacity << endl;
        
        // Notify one waiting producer that the buffer is not full
        notFull.notify_one();
        
        return item;
        
        // Lock is automatically released when unique_lock goes out of scope
    }
    
    // Get current buffer size (thread-safe)
    size_t size() {
        unique_lock<mutex> lock(mtx);
        return buffer.size();
    }
    
    // Print statistics
    void printStatistics() {
        unique_lock<mutex> lock(mtx);
        cout << "\n=== Statistics ===" << endl;
        cout << "Total items produced: " << totalProduced << endl;
        cout << "Total items consumed: " << totalConsumed << endl;
        cout << "Items remaining in buffer: " << buffer.size() << endl;
    }
};

// Producer function: produces items and adds them to the buffer
void producer(BoundedBuffer& buffer, int producerId, int itemCount) {
    // Random number generator for simulating variable production time
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> productionTime(100, 500);
    uniform_int_distribution<> itemValue(1, 100);
    
    for (int i = 0; i < itemCount; ++i) {
        // Simulate time taken to produce an item
        this_thread::sleep_for(chrono::milliseconds(productionTime(gen)));
        
        // Generate a random item value
        int item = itemValue(gen);
        
        // Add the item to the buffer (will block if buffer is full)
        buffer.produce(item, producerId);
    }
    
    cout << "Producer " << producerId << " finished producing " << itemCount << " items" << endl;
}

// Consumer function: consumes items from the buffer
void consumer(BoundedBuffer& buffer, int consumerId, int itemCount) {
    // Random number generator for simulating variable consumption time
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> consumptionTime(150, 600);
    
    for (int i = 0; i < itemCount; ++i) {
        // Get an item from the buffer (will block if buffer is empty)
        int item = buffer.consume(consumerId);
        
        // Simulate time taken to process the consumed item
        this_thread::sleep_for(chrono::milliseconds(consumptionTime(gen)));
    }
    
    cout << "Consumer " << consumerId << " finished consuming " << itemCount << " items" << endl;
}

// Main function demonstrating the Producer-Consumer pattern
int main() {
    cout << "=== Producer-Consumer Problem using Mutex ===" << endl;
    cout << endl;
    
    // Configuration
    const size_t bufferCapacity = 5;       // Maximum buffer size
    const int numProducers = 2;            // Number of producer threads
    const int numConsumers = 2;            // Number of consumer threads
    const int itemsPerProducer = 10;       // Items each producer will produce
    const int itemsPerConsumer = 10;       // Items each consumer will consume
    
    // Create the shared bounded buffer
    BoundedBuffer buffer(bufferCapacity);
    cout << endl;
    
    // Vectors to store thread objects
    vector<thread> producers;
    vector<thread> consumers;
    
    cout << "Starting " << numProducers << " producers and " 
         << numConsumers << " consumers..." << endl;
    cout << "Each producer will produce " << itemsPerProducer << " items" << endl;
    cout << "Each consumer will consume " << itemsPerConsumer << " items" << endl;
    cout << endl;
    
    // Create and start producer threads
    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back(producer, ref(buffer), i, itemsPerProducer);
    }
    
    // Create and start consumer threads
    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(consumer, ref(buffer), i, itemsPerConsumer);
    }
    
    // Wait for all producer threads to complete
    for (auto& prod : producers) {
        prod.join();
    }
    cout << "\nAll producers have finished" << endl;
    
    // Wait for all consumer threads to complete
    for (auto& cons : consumers) {
        cons.join();
    }
    cout << "All consumers have finished" << endl;
    
    // Print final statistics
    buffer.printStatistics();
    
    cout << "\n=== Program completed successfully ===" << endl;
    
    return 0;
}

/*
 * Key Concepts Demonstrated:
 * 
 * 1. Mutual Exclusion (Mutex):
 *    - Only one thread can access the buffer at a time
 *    - Prevents race conditions and data corruption
 * 
 * 2. Condition Variables:
 *    - notFull: Producers wait on this when buffer is full
 *    - notEmpty: Consumers wait on this when buffer is empty
 *    - Efficient waiting mechanism (no busy-waiting/polling)
 * 
 * 3. Unique Lock:
 *    - RAII-style lock management (automatic unlock on scope exit)
 *    - Can be temporarily unlocked by condition variables
 * 
 * 4. Thread Synchronization:
 *    - Producers notify consumers when they add items
 *    - Consumers notify producers when they remove items
 * 
 * 5. Bounded Buffer:
 *    - Fixed capacity prevents unlimited memory growth
 *    - Backpressure mechanism (slow consumers slow down producers)
 * 
 * Real-World Applications:
 * - Message queues in distributed systems
 * - Request handling in web servers
 * - Data pipelines and stream processing
 * - Event-driven architectures
 * - Task scheduling systems
 * 
 * Thread Safety Guarantees:
 * - No race conditions: Mutex ensures exclusive access
 * - No deadlocks: Single mutex, proper lock ordering
 * - No lost wakeups: Condition variables with predicates
 * - Progress guarantee: Threads make progress when resources available
 */
