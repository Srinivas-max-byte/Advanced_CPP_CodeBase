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
 * 
 * =========================================================================
 * WHY TWO CONDITION VARIABLES? CAN WE USE JUST ONE?
 * =========================================================================
 * 
 * This implementation uses TWO condition variables:
 * 1. cv_producer - Producers wait on this when buffer is full
 * 2. cv_consumer - Consumers wait on this when buffer is empty
 * 
 * ADVANTAGES OF TWO CONDITION VARIABLES:
 * 
 * 1. Targeted Signaling:
 *    - When a producer adds an item → only wake consumers (via cv_consumer.notify_one())
 *    - When a consumer removes an item → only wake producers (via cv_producer.notify_one())
 *    - This avoids waking up threads that cannot make progress
 * 
 * 2. Better Performance:
 *    - Fewer spurious wakeups (threads woken up unnecessarily)
 *    - Less contention and context switching
 *    - More efficient use of CPU resources
 * 
 * 3. Clearer Intent:
 *    - The code explicitly shows what condition each thread type is waiting for
 *    - Easier to understand and maintain
 * 
 * CAN WE USE JUST ONE CONDITION VARIABLE?
 * 
 * YES, it is possible! Here's how it would change:
 * 
 * SINGLE CONDITION VARIABLE IMPLEMENTATION:
 * 
 *   class BoundedBuffer {
 *   private:
 *       queue<int> buffer;
 *       const size_t capacity;
 *       mutex mtx;
 *       condition_variable cv;  // Single CV for both producers and consumers
 *   
 *   public:
 *       void produce(int item, int producerId) {
 *           unique_lock<mutex> lock(mtx);
 *           cv.wait(lock, [this] { return buffer.size() < capacity; });
 *           buffer.push(item);
 *           cv.notify_all();  // MUST use notify_all(), not notify_one()!
 *       }
 *   
 *       int consume(int consumerId) {
 *           unique_lock<mutex> lock(mtx);
 *           cv.wait(lock, [this] { return !buffer.empty(); });
 *           int item = buffer.front();
 *           buffer.pop();
 *           cv.notify_all();  // MUST use notify_all(), not notify_one()!
 *           return item;
 *       }
 *   };
 * 
 * CRITICAL DIFFERENCE WITH SINGLE CV:
 * 
 * - MUST use notify_all() instead of notify_one()
 * - Why? Because you don't know which type of thread is waiting
 * - If you use notify_one(), you might wake the wrong type of thread
 * 
 * Example problem with notify_one() + single CV:
 *   1. Buffer is full, 3 producers are waiting
 *   2. Consumer removes an item and calls cv.notify_one()
 *   3. It might wake up a producer (good!) OR another consumer (bad!)
 *   4. If it wakes a consumer, that consumer will find buffer not empty,
 *      consume an item, and call notify_one() again
 *   5. This could wake another consumer, leaving all producers asleep
 *   6. Result: Deadlock or poor performance
 *          Now you have deadlock risk:
 *           Buffer is full, both producers waiting on cond
 *           A consumer consumes and calls cond.notify_one()
 *           It might wake the other consumer (not a producer!)
 *           That consumer sees empty buffer (other consumer took the item), waits again
 *           Everyone sleeping = DEADLOCK
 *
 * PERFORMANCE COMPARISON:
 * 
 * Two CVs (current implementation):
 *   ✓ notify_one() is sufficient and efficient
 *   ✓ Only relevant threads are woken up
 *   ✓ Minimal spurious wakeups
 *   ✓ Better performance
 *   ✓ Clearer code
 * 
 * One CV (alternative):
 *   ✓ Simpler (one less variable)
 *   ✗ MUST use notify_all() (less efficient)
 *   ✗ All waiting threads wake up on each notify
 *   ✗ More spurious wakeups and context switches
 *   ✗ Slightly worse performance
 *   ✗ Less clear intent
 * 
 * BOTH APPROACHES ARE FUNCTIONALLY CORRECT:
 * 
 * The wait() predicate lambda protects against spurious wakeups in both cases:
 *   - Producers check: buffer.size() < capacity
 *   - Consumers check: !buffer.empty()
 *   - Even if woken unnecessarily, threads recheck their condition
 * 
 * CONCLUSION:
 * 
 * Two condition variables is the PREFERRED approach for Producer-Consumer:
 *   - Industry best practice
 *   - Better performance
 *   - Clearer intent
 *   - More maintainable
 * 
 * Use one CV only when:
 *   - Simplicity is paramount
 *   - Performance difference is negligible for your use case
 *   - You understand the tradeoffs
 * 
 * =========================================================================
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <random>

using namespace std;

// Shared buffer class that encapsulates the synchronized queue
class BoundedBuffer {
private:
    queue<int> buffer;              // The shared buffer (queue)
    const size_t capacity;          // Maximum capacity of the buffer
    mutex mtx;                      // Mutex for protecting the buffer
    condition_variable cv_producer;     // Condition variable: for producers (buffer is not full)
    condition_variable cv_consumer;     // Condition variable: for consumers (buffer is not empty)
    
    // Statistics
    int totalProduced;
    int totalConsumed;

public:
    BoundedBuffer(size_t cap) : capacity(cap), totalProduced(0), totalConsumed(0) {
        cout << "Buffer initialized with capacity: " << capacity << endl;
    }
    
    void produce(int item, int producerId) {
        unique_lock<mutex> lock(mtx);
        
        cv_producer.wait(lock, [this] {
            return buffer.size() < capacity;
        });
        
        buffer.push(item);
        totalProduced++;
        
        cout << "Producer " << producerId << " produced: " << item 
             << " | Buffer size: " << buffer.size() << "/" << capacity << endl;
        
        cv_consumer.notify_one();
    }
    
    int consume(int consumerId) {
        unique_lock<mutex> lock(mtx);
        
        cv_consumer.wait(lock, [this] {
            return !buffer.empty();
        });
        
        int item = buffer.front();
        buffer.pop();
        totalConsumed++;
        
        cout << "Consumer " << consumerId << " consumed: " << item 
             << " | Buffer size: " << buffer.size() << "/" << capacity << endl;
        
        cv_producer.notify_one();
        
        return item;
    }
    
    size_t size() {
        unique_lock<mutex> lock(mtx);
        return buffer.size();
    }
    
    void printStatistics() {
        unique_lock<mutex> lock(mtx);
        cout << "\n=== Statistics ===" << endl;
        cout << "Total items produced: " << totalProduced << endl;
        cout << "Total items consumed: " << totalConsumed << endl;
        cout << "Items remaining in buffer: " << buffer.size() << endl;
    }
};

void producer(BoundedBuffer& buffer, int producerId, int itemCount) {
    for (int i = 0; i < itemCount; ++i) {
        int item = producerId * 100 + i;
        buffer.produce(item, producerId);
    }
    
    cout << "Producer " << producerId << " finished producing " << itemCount << " items" << endl;
}

void consumer(BoundedBuffer& buffer, int consumerId, int itemCount) {
    for (int i = 0; i < itemCount; ++i) {
        int item = buffer.consume(consumerId);
    }
    
    cout << "Consumer " << consumerId << " finished consuming " << itemCount << " items" << endl;
}

// Main function demonstrating the Producer-Consumer pattern
int main() {
    cout << "=== Producer-Consumer Problem using Mutex ===" << endl;
    cout << endl;
    
    const size_t bufferCapacity = 5;
    const int numProducers = 2;
    const int numConsumers = 2;
    const int itemsPerProducer = 10;
    const int itemsPerConsumer = 10;
    
    BoundedBuffer buffer(bufferCapacity);
    cout << endl;
    
    vector<thread> producers;
    vector<thread> consumers;
    
	//Push and also starts the threads, advantage is avoids copying the thread object and also allows us to pass arguments directly
    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back(producer, ref(buffer), i, itemsPerProducer);
    }    
    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(consumer, ref(buffer), i, itemsPerConsumer);
    }
    
    for (auto& prod : producers) {
        prod.join();
    }    
    for (auto& cons : consumers) {
        cons.join();
    }
    
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
 *    - cv_producer: Producers wait on this when buffer is full
 *    - cv_consumer: Consumers wait on this when buffer is empty
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
