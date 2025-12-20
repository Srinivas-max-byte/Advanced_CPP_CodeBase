/*
Difference between std::unique_ptr and std::shared_ptr:
---------------------------------------------------------
- unique_ptr: "Unique ownership" smart pointer. Only ONE unique_ptr can own a pointer at a time.
    - No copying allowed (move-only semantics).
    - When unique_ptr goes out of scope, the owned object is deleted automatically.
    - Lightweight, no reference counting.

- shared_ptr: "Shared ownership" smart pointer. Multiple shared_ptr can point to (share) the same object.
    - Uses reference counting: deletes the object when last shared_ptr to it is destroyed.
    - Allows copy and assignment (increases/decreases reference count).
    - Slightly more overhead due to reference counting.
*/

// Example code for unique_ptr and shared_ptr usage:

#include <iostream>
#include <memory>
using namespace std;

class MyResource {
public:
    MyResource(const string& name) : name_(name) {
        cout << "MyResource " << name_ << " acquired\n";
    }
    ~MyResource() {
        cout << "MyResource " << name_ << " released\n";
    }
    void greet() const {
        cout << "Hello from " << name_ << endl;
    }
private:
    string name_;
};

int main() {
    cout << "--- UNIQUE_PTR EXAMPLE ---\n";
    unique_ptr<MyResource> uniq1 = make_unique<MyResource>("Unique1");
    uniq1->greet();

    // unique_ptr<MyResource> uniq2 = uniq1; // Error! Cannot copy
    unique_ptr<MyResource> uniq2 = std::move(uniq1); // Ownership transferred
    if (!uniq1) cout << "uniq1 is now empty after move\n";

    uniq2->greet();

    cout << "--- SHARED_PTR EXAMPLE ---\n";
    shared_ptr<MyResource> shared1 = make_shared<MyResource>("Shared1");
    {
        shared_ptr<MyResource> shared2 = shared1; // Now TWO shared_ptr own the resource
        cout << "shared1 use_count: " << shared1.use_count() << endl;
        cout << "shared2 use_count: " << shared2.use_count() << endl;
        shared2->greet();
    } // shared2 goes out of scope, resource NOT destroyed (still owned by shared1)
    cout << "After shared2 is gone, shared1 use_count: " << shared1.use_count() << endl;
    shared1->greet();

    // When shared1 goes out of scope, resource is destroyed

    return 0;
}

