/*
 * Producer-Consumer Problem using Semaphores
 * 
 * The Producer-Consumer problem solved using semaphores instead of mutex and condition variables.
 * 
 * Problem Description:
 * - Producers generate data and put it into a shared buffer
 * - Consumers take data from the shared buffer and process it
 * - The buffer has a fixed size (bounded buffer)
 * 
 * Semaphores Used:
 * 1. empty: Counts the number of empty slots in the buffer (initially = buffer capacity)
 * 2. full: Counts the number of filled slots in the buffer (initially = 0)
 * 3. mutex: Binary semaphore for mutual exclusion when accessing the buffer (initially = 1)
 * 
 * Advantages of Semaphores over Mutex + Condition Variables:
 * - More intuitive for counting resources (empty/full slots)
 * - Clearer separation of concerns (counting vs. mutual exclusion)
 * - Classic solution to the Producer-Consumer problem
 * 
 * Note: Requires C++20 for std::counting_semaphore and std::binary_semaphore
 */

#include <iostream>
#include <thread>
#include <semaphore>
#include <queue>
#include <chrono>
#include <vector>
#include <random>
#include <mutex>

using namespace std;

// Shared buffer class using semaphores for synchronization
class SemaphoreBuffer {
private:
    queue<int> buffer;                      // The shared buffer (queue)
    const size_t capacity;                  // Maximum capacity of the buffer
    
    // Semaphores for synchronization
    counting_semaphore<> empty;             // Counts empty slots (producer waits on this)
    counting_semaphore<> full;              // Counts full slots (consumer waits on this)
    binary_semaphore mutex;                 // Binary semaphore for mutual exclusion
    
    // Statistics (using regular mutex for thread-safe statistics only)
    std::mutex statsMutex;
    int totalProduced;
    int totalConsumed;

public:
    // Constructor initializes semaphores
    // empty semaphore = capacity (all slots are empty initially)
    // full semaphore = 0 (no slots are filled initially)
    // mutex = 1 (binary semaphore for mutual exclusion)
    SemaphoreBuffer(size_t cap) 
        : capacity(cap), 
          empty(cap),           // Initialize with capacity (all empty)
          full(0),              // Initialize with 0 (none full)
          mutex(1),             // Binary semaphore (1 = unlocked)
          totalProduced(0), 
          totalConsumed(0) {
        cout << "Semaphore Buffer initialized with capacity: " << capacity << endl;
        cout << "  - empty semaphore = " << capacity << endl;
        cout << "  - full semaphore = 0" << endl;
        cout << "  - mutex semaphore = 1" << endl;
    }
    
    // Producer calls this method to add an item to the buffer
    void produce(int item, int producerId) {
        // Step 1: Wait for an empty slot (decrement empty semaphore)
        // If empty = 0 (buffer is full), producer blocks here
        empty.acquire();
        
        // Step 2: Acquire mutex for exclusive access to the buffer
        mutex.acquire();
        
        // Critical Section: Add item to buffer
        buffer.push(item);
        size_t currentSize = buffer.size();
        
        // Update statistics
        {
            lock_guard<std::mutex> lock(statsMutex);
            totalProduced++;
        }
        
        cout << "Producer " << producerId << " produced: " << item 
             << " | Buffer size: " << currentSize << "/" << capacity << endl;
        
        // Step 3: Release mutex
        mutex.release();
        
        // Step 4: Signal that buffer has one more full slot (increment full semaphore)
        full.release();
    }
    
    // Consumer calls this method to remove an item from the buffer
    int consume(int consumerId) {
        // Step 1: Wait for a full slot (decrement full semaphore)
        // If full = 0 (buffer is empty), consumer blocks here
        full.acquire();
        
        // Step 2: Acquire mutex for exclusive access to the buffer
        mutex.acquire();
        
        // Critical Section: Remove item from buffer
        int item = buffer.front();
        buffer.pop();
        size_t currentSize = buffer.size();
        
        // Update statistics
        {
            lock_guard<std::mutex> lock(statsMutex);
            totalConsumed++;
        }
        
        cout << "Consumer " << consumerId << " consumed: " << item 
             << " | Buffer size: " << currentSize << "/" << capacity << endl;
        
        // Step 3: Release mutex
        mutex.release();
        
        // Step 4: Signal that buffer has one more empty slot (increment empty semaphore)
        empty.release();
        
        return item;
    }
    
    // Get current buffer size (thread-safe)
    size_t size() {
        mutex.acquire();
        size_t sz = buffer.size();
        mutex.release();
        return sz;
    }
    
    // Print statistics
    void printStatistics() {
        lock_guard<std::mutex> lock(statsMutex);
        cout << "\n=== Statistics ===" << endl;
        cout << "Total items produced: " << totalProduced << endl;
        cout << "Total items consumed: " << totalConsumed << endl;
        cout << "Items remaining in buffer: " << size() << endl;
    }
};

// Producer function: produces items and adds them to the buffer
void producer(SemaphoreBuffer& buffer, int producerId, int itemCount) {
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
void consumer(SemaphoreBuffer& buffer, int consumerId, int itemCount) {
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

// Main function demonstrating the Producer-Consumer pattern with semaphores
int main() {
    cout << "=== Producer-Consumer Problem using Semaphores ===" << endl;
    cout << endl;
    
    // Configuration
    const size_t bufferCapacity = 5;       // Maximum buffer size
    const int numProducers = 2;            // Number of producer threads
    const int numConsumers = 2;            // Number of consumer threads
    const int itemsPerProducer = 10;       // Items each producer will produce
    const int itemsPerConsumer = 10;       // Items each consumer will consume
    
    // Create the shared semaphore-based buffer
    SemaphoreBuffer buffer(bufferCapacity);
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
 * 1. Counting Semaphores (empty and full):
 *    - Track the number of empty and full slots in the buffer
 *    - Producers wait on 'empty' semaphore (decremented when producing)
 *    - Consumers wait on 'full' semaphore (decremented when consuming)
 *    - Natural way to represent resource counting
 * 
 * 2. Binary Semaphore (mutex):
 *    - Acts as a mutex for mutual exclusion
 *    - Ensures only one thread accesses the buffer at a time
 *    - Value is either 0 (locked) or 1 (unlocked)
 * 
 * 3. Semaphore Operations:
 *    - acquire(): Decrements the semaphore (wait/P operation)
 *                 Blocks if value becomes negative
 *    - release(): Increments the semaphore (signal/V operation)
 *                 Wakes up waiting threads
 * 
 * 4. Producer Algorithm:
 *    a) empty.acquire()  ? Wait for empty slot
 *    b) mutex.acquire()  ? Enter critical section
 *    c) Add item to buffer
 *    d) mutex.release()  ? Exit critical section
 *    e) full.release()   ? Signal new full slot
 * 
 * 5. Consumer Algorithm:
 *    a) full.acquire()   ? Wait for full slot
 *    b) mutex.acquire()  ? Enter critical section
 *    c) Remove item from buffer
 *    d) mutex.release()  ? Exit critical section
 *    e) empty.release()  ? Signal new empty slot
 * 
 * Semaphore vs. Mutex + Condition Variables:
 * 
 * Semaphores:
 * + More intuitive for resource counting
 * + Classic textbook solution
 * + Cleaner code for this specific problem
 * - Requires C++20
 * 
 * Mutex + Condition Variables:
 * + More flexible for complex conditions
 * + Available in C++11 and later
 * + Better for predicates beyond simple counting
 * 
 * Thread Safety Guarantees:
 * - No race conditions: Binary semaphore ensures mutual exclusion
 * - No buffer overflow: Empty semaphore limits producers
 * - No buffer underflow: Full semaphore limits consumers
 * - No deadlocks: Proper semaphore ordering
 * - Progress guarantee: Threads wake up when resources available
 * 
 * Real-World Applications:
 * - Print job spooling (multiple print jobs, limited printer resources)
 * - Disk I/O scheduling (multiple requests, limited disk heads)
 * - Network packet buffering (packets produced by network, consumed by application)
 * - Database connection pooling (limited connections, multiple clients)
 * - Resource management in operating systems
 * 
 * Compilation Requirements:
 * - C++20 or later (for std::counting_semaphore and std::binary_semaphore)
 * - Compile with: g++ -std=c++20 -pthread ProducerConsumer_Semaphore.cpp -o producer_consumer_sem
 */
