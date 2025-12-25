#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <future>
#include <string>

using namespace std;

// Example class with a member function used as thread entry
class Worker {
public:
    Worker(string name) : name_(move(name)) {}

    void doWork(int id) {
        cout << "Worker " << name_ << " processing id " << id
             << " on thread " << this_thread::get_id() << "\n";
        this_thread::sleep_for(chrono::milliseconds(200));
    }

    int accumulate(int a, int b) {
        cout << "Worker " << name_ << " accumulate on thread "
             << this_thread::get_id() << "\n";
        this_thread::sleep_for(chrono::milliseconds(100));
        return a + b;
    }

private:
    string name_;
};

// Functor (callable object)
struct Functor {
    void operator()(int times) {
        for (int i = 0; i < times; ++i) {
            cout << "Functor running iteration " << i
                 << " on thread " << this_thread::get_id() << "\n";
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
};

int main() {
    cout << "Main thread id: " << this_thread::get_id() << "\n\n";

    // 1) Create a thread and call a member function
    Worker w("A");
    // When starting a thread with a pointer-to-member function we must provide the
    // object to call it on as the first argument. Passing `&w` gives the thread
    // the address of the instance so it can invoke `Worker::doWork` on that
    // specific object (the object must outlive the thread).
    thread t1(&Worker::doWork, &w, 42); // member function called on instance 'w'
    t1.join();

    // 2) Pass-by-value vs pass-by-reference to threads
    int val = 10;

    // Pass by value (copy); changes inside the thread do not affect main's 'val'
    thread t2([](int val) mutable {
        val += 5;
        cout << "In thread (by value) val = " << val
             << " on thread " << this_thread::get_id() << "\n";
    }, val);

    // Pass by reference using std::ref; changes affect the original 'val'
    thread t3([](int &r) {
        r += 20;
        cout << "In thread (by reference) val = " << r
             << " on thread " << this_thread::get_id() << "\n";
    }, ref(val));

    t2.join();
    t3.join();

    cout << "Back in main, val = " << val << "\n\n";

    // 3) Returning a value from a thread using promise/future
    promise<int> p;
    future<int> fut = p.get_future();

    thread t4([&p]() {
        this_thread::sleep_for(chrono::milliseconds(100));
        int result = 123;
        cout << "Thread setting promise with " << result << "\n";
        p.set_value(result);
    });

    // Wait for and retrieve the value
    int received = fut.get();
    cout << "Future received: " << received << "\n\n";
    t4.join();

    // Alternative: using std::async to get a return value (simpler)
    future<int> f2 = async(launch::async, &Worker::accumulate, &w, 3, 4);
    cout << "async result: " << f2.get() << "\n\n";

    // 4) Creating a thread with a functor
    Functor fn;
    thread t5(fn, 3); // calls fn.operator()(3)
    t5.join();

    cout << "Done." << endl;
    return 0;
}
