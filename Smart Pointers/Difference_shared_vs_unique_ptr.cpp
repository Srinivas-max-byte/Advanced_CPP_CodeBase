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
    - ALSO supports move semantics (transfers ownership without incrementing ref count).
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

    cout << "\n--- SHARED_PTR EXAMPLE (Copy Semantics) ---\n";
    shared_ptr<MyResource> shared1 = make_shared<MyResource>("Shared1");
    {
        shared_ptr<MyResource> shared2 = shared1; // Now TWO shared_ptr own the resource
        cout << "shared1 use_count: " << shared1.use_count() << endl;
        cout << "shared2 use_count: " << shared2.use_count() << endl;
        shared2->greet();
    } // shared2 goes out of scope, resource NOT destroyed (still owned by shared1)
    cout << "After shared2 is gone, shared1 use_count: " << shared1.use_count() << endl;
    shared1->greet();

    // ========================================================================
    // SHARED_PTR WITH MOVE SEMANTICS
    // ========================================================================
    /*
     * YES! shared_ptr DOES support move semantics!
     * 
     * DIFFERENCE BETWEEN COPY AND MOVE for shared_ptr:
     * 
     * COPY: shared_ptr<T> ptr2 = ptr1;
     *   - Reference count INCREASES by 1
     *   - Both ptr1 and ptr2 are valid and point to the same object
     *   - Involves atomic increment operation (thread-safe but has cost)
     * 
     * MOVE: shared_ptr<T> ptr2 = std::move(ptr1);
     *   - Reference count STAYS THE SAME (no increment!)
     *   - Ownership transferred: ptr1 becomes empty (nullptr)
     *   - More efficient: no atomic operations on ref count
     *   - Useful when you don't need the source pointer anymore
     * 
     * PERFORMANCE:
     * - Copy: Atomic increment + atomic decrement operations
     * - Move: Just pointer transfer (faster, especially in multi-threaded code)
     */

    cout << "\n--- SHARED_PTR WITH MOVE SEMANTICS ---\n";
    
    // Create a shared_ptr with reference count = 1
    shared_ptr<MyResource> moveSource = make_shared<MyResource>("MoveExample");
    cout << "moveSource use_count: " << moveSource.use_count() << endl;
    moveSource->greet();

    cout << "\n--- Demonstrating COPY vs MOVE ---\n";
    
    // COPY: Reference count increases
    cout << "\n1. COPY (ref count increases):\n";
    shared_ptr<MyResource> copyDest = moveSource;
    cout << "After COPY:" << endl;
    cout << "  moveSource use_count: " << moveSource.use_count() << endl;
    cout << "  copyDest use_count: " << copyDest.use_count() << endl;
    cout << "  Both pointers are valid!" << endl;
    
    // MOVE: Reference count stays the same, ownership transferred
    cout << "\n2. MOVE (ref count stays same, ownership transferred):\n";
    cout << "Before move - copyDest use_count: " << copyDest.use_count() << endl;
    
    shared_ptr<MyResource> moveDest = std::move(copyDest);
    
    cout << "After MOVE:" << endl;
    cout << "  copyDest is now: " << (copyDest ? "valid" : "empty (nullptr)") << endl;
    cout << "  moveDest use_count: " << moveDest.use_count() << endl;
    cout << "  moveSource use_count: " << moveSource.use_count() << endl;
    cout << "  Reference count stayed at 2 (no increment happened!)" << endl;

    // ========================================================================
    // PRACTICAL USE CASE: Moving shared_ptr into a container
    // ========================================================================
    cout << "\n--- PRACTICAL EXAMPLE: Moving into vector ---\n";
    
    vector<shared_ptr<MyResource>> resources;
    shared_ptr<MyResource> temp = make_shared<MyResource>("VectorItem");
    
    cout << "Before push_back with move:" << endl;
    cout << "  temp use_count: " << temp.use_count() << endl;
    
    resources.push_back(std::move(temp)); // Move instead of copy - more efficient!
    
    cout << "After push_back with move:" << endl;
    cout << "  temp is now: " << (temp ? "valid" : "empty") << endl;
    cout << "  resources[0] use_count: " << resources[0].use_count() << endl;
    cout << "  Moved into vector without incrementing ref count!" << endl;

    // ========================================================================
    // WHY USE MOVE WITH shared_ptr?
    // ========================================================================
    /*
     * WHEN TO USE MOVE with shared_ptr:
     * 
     * 1. PERFORMANCE in tight loops or multi-threaded code
     *    - Avoids atomic increment/decrement operations
     *    - Especially important when creating/destroying many shared_ptrs
     * 
     * 2. SEMANTIC CLARITY
     *    - Makes it explicit that you're transferring ownership
     *    - Shows you don't need the source pointer anymore
     * 
     * 3. CONTAINERS
     *    - When adding to vectors/maps and don't need original pointer
     *    - Example: vec.push_back(std::move(ptr));
     * 
     * 4. RETURN VALUES
     *    - When returning from functions (though RVO often handles this)
     * 
     * EXAMPLE PERFORMANCE DIFFERENCE:
     * 
     * // SLOWER (copy):
     * for (int i = 0; i < 1000000; ++i) {
     *     shared_ptr<int> copy = original; // Atomic increment
     *     vec.push_back(copy);              // Another atomic increment
     * } // Atomic decrements happen
     * 
     * // FASTER (move):
     * for (int i = 0; i < 1000000; ++i) {
     *     shared_ptr<int> temp = make_shared<int>(i);
     *     vec.push_back(std::move(temp));   // No atomic operations!
     * }
     */

    cout << "\n--- SUMMARY ---\n";
    cout << "shared_ptr supports BOTH copy and move semantics:\n";
    cout << "  - Copy: Shares ownership (ref count++)\n";
    cout << "  - Move: Transfers ownership (ref count unchanged)\n";
    cout << "  - Use move when you don't need the source pointer anymore!\n";

    // When shared1, moveSource, moveDest go out of scope, resource is destroyed

    return 0;
}

