# Multithreading Producer-Consumer Cheat Sheet

## 1. Producer-Consumer with 2 Condition Variables

**Use case:** Separate notifications for producer (full buffer) and consumer (empty buffer)

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv_empty, cv_full;
const int MAX_SIZE = 5;

void producer() {
    for (int i = 0; i < 10; i++) {
        std::unique_lock<std::mutex> lock(mtx);
        cv_full.wait(lock, [] { return buffer.size() < MAX_SIZE; });
        buffer.push(i);
        std::cout << "Produced: " << i << std::endl;
        cv_empty.notify_all();
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv_empty.wait(lock, [] { return !buffer.empty(); });
        if (!buffer.empty()) {
            int val = buffer.front();
            buffer.pop();
            std::cout << "Consumed: " << val << std::endl;
        }
        cv_full.notify_all();
    }
}

int main() {
    std::thread t1(producer), t2(consumer);
    t1.join();
    return 0;
}
```

**Key Points:**
- `cv_empty.notify_all()` - wakes consumer when item produced
- `cv_full.notify_all()` - wakes producer when space available
- Producer waits on `cv_full` (buffer full)
- Consumer waits on `cv_empty` (buffer empty)

---

## 2. Producer-Consumer with 1 Condition Variable

**Use case:** Single notification for both producer and consumer (simpler approach)

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv;
const int MAX_SIZE = 5;

void producer() {
    for (int i = 0; i < 10; i++) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return buffer.size() < MAX_SIZE; });
        buffer.push(i);
        std::cout << "Produced: " << i << std::endl;
        cv.notify_all();
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty(); });
        if (!buffer.empty()) {
            int val = buffer.front();
            buffer.pop();
            std::cout << "Consumed: " << val << std::endl;
        }
        cv.notify_all();
    }
}

int main() {
    std::thread t1(producer), t2(consumer);
    t1.join();
    return 0;
}
```

**Key Points:**
- Single `cv` used for both producer and consumer
- `notify_all()` wakes all waiting threads
- Each thread checks its own condition in predicate lambda
- Less explicit but simpler code

---

## 3. Producer-Consumer with Semaphore

**Use case:** Classic semaphore-based synchronization (C++20)

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <queue>

std::queue<int> buffer;
std::mutex mtx;
std::binary_semaphore empty_sem(1), full_sem(0);

void producer() {
    for (int i = 0; i < 10; i++) {
        empty_sem.acquire();
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push(i);
            std::cout << "Produced: " << i << std::endl;
        }
        full_sem.release();
    }
}

void consumer() {
    while (true) {
        full_sem.acquire();
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!buffer.empty()) {
                int val = buffer.front();
                buffer.pop();
                std::cout << "Consumed: " << val << std::endl;
            }
        }
        empty_sem.release();
    }
}

int main() {
    std::thread t1(producer), t2(consumer);
    t1.join();
    return 0;
}
```

**Key Points:**
- `binary_semaphore` = 0/1 (binary), `counting_semaphore` = 0+ (counting)
- `acquire()` - decrements semaphore (waits if 0)
- `release()` - increments semaphore
- `empty_sem` initialized to 1 (producer can start)
- `full_sem` initialized to 0 (consumer waits initially)

---

## 4. Bounded Buffer with Counting Semaphore (Advanced)

**Use case:** Buffer with multiple items (not binary)

```cpp
#include <iostream>
#include <thread>
#include <semaphore>
#include <queue>
#include <mutex>

std::queue<int> buffer;
std::mutex mtx;
std::counting_semaphore<10> empty_slots(10);
std::counting_semaphore<10> full_slots(0);

void producer() {
    for (int i = 0; i < 20; i++) {
        empty_slots.acquire();
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push(i);
            std::cout << "Produced: " << i << std::endl;
        }
        full_slots.release();
    }
}

void consumer() {
    for (int i = 0; i < 20; i++) {
        full_slots.acquire();
        {
            std::lock_guard<std::mutex> lock(mtx);
            int val = buffer.front();
            buffer.pop();
            std::cout << "Consumed: " << val << std::endl;
        }
        empty_slots.release();
    }
}

int main() {
    std::thread t1(producer), t2(consumer);
    t1.join();
    t2.join();
    return 0;
}
```

**Key Points:**
- `counting_semaphore<10>` allows up to 10 items
- `empty_slots` tracks available buffer space
- `full_slots` tracks items ready to consume
- More scalable for bounded buffers

---

## Quick Comparison Table

| Feature | 2 CV | 1 CV | Semaphore |
|---------|------|------|-----------|
| **Overhead** | Moderate | Low | Very Low |
| **Readability** | High | Medium | Low |
| **Flexibility** | High | Medium | Low |
| **Thread-safe** | Yes | Yes | Yes |
| **C++ Version** | C++11 | C++11 | C++20 |

---

## Key Concepts

**Condition Variables:**
- `wait(lock, predicate)` - atomically unlock and wait for notification
- `notify_one()` - wake one waiting thread
- `notify_all()` - wake all waiting threads
- Predicate prevents spurious wakeups

**Semaphore:**
- Counter-based synchronization primitive
- `acquire()` - block if counter = 0
- `release()` - increment counter
- No mutex needed if only synchronizing (not protecting data)

**Mutex Pattern:**
- Always protect shared data with mutex
- Use `lock_guard` or `unique_lock` for RAII
- Avoid deadlocks with consistent lock ordering
