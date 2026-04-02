/*
===============================================================================
                    std::weak_ptr - COMPREHENSIVE GUIDE
===============================================================================

WHAT IS weak_ptr?
-----------------
weak_ptr is a "non-owning" smart pointer that holds a weak reference to an 
object managed by shared_ptr. It does NOT increase the reference count.

KEY PROPERTIES:
---------------
1. Does NOT participate in ownership (ref count stays the same)
2. Does NOT prevent the object from being destroyed
3. Must be converted to shared_ptr (via lock()) to access the object
4. Can detect if the object has been destroyed (via expired())
5. Breaks circular reference problems with shared_ptr

SYNTAX:
-------
    std::weak_ptr<T> wp = shared_ptr_obj;  // Create from shared_ptr
    
    if (auto sp = wp.lock()) {  // Convert to shared_ptr to use
        sp->doSomething();
    } else {
        // Object has been destroyed
    }

WHY DO WE NEED weak_ptr?
------------------------
1. BREAK CIRCULAR REFERENCES (most important use case!)
2. OBSERVE without owning (cache, observer pattern)
3. CHECK if object still exists before using it
4. AVOID dangling pointers while still being safe

===============================================================================
*/

#include <iostream>
#include <memory>
#include <vector>
#include <string>

using namespace std;

// ============================================================================
// DEMONSTRATION 1: BASIC weak_ptr USAGE
// ============================================================================

void basic_weak_ptr_demo() {
    cout << "\n=== 1. BASIC weak_ptr USAGE ===" << endl;
    
    weak_ptr<int> wp;
    
    {
        shared_ptr<int> sp = make_shared<int>(100);
        cout << "Created shared_ptr with value: " << *sp << endl;
        cout << "shared_ptr use_count: " << sp.use_count() << endl;
        
        // Create weak_ptr from shared_ptr
        wp = sp;
        cout << "\nAfter creating weak_ptr:" << endl;
        cout << "shared_ptr use_count: " << sp.use_count() << " (unchanged!)" << endl;
        cout << "weak_ptr expired: " << (wp.expired() ? "YES" : "NO") << endl;
        
        // Access object through weak_ptr using lock()
        if (auto sp2 = wp.lock()) {
            cout << "Accessed value via weak_ptr.lock(): " << *sp2 << endl;
            cout << "Temporary shared_ptr use_count: " << sp2.use_count() << endl;
        }
        
    } // sp goes out of scope, object is destroyed
    
    cout << "\nAfter shared_ptr destroyed:" << endl;
    cout << "weak_ptr expired: " << (wp.expired() ? "YES" : "NO") << endl;
    
    if (auto sp3 = wp.lock()) {
        cout << "Object still exists" << endl;
    } else {
        cout << "Object has been destroyed - lock() returned nullptr" << endl;
    }
}

// ============================================================================
// DEMONSTRATION 2: CIRCULAR REFERENCE PROBLEM (without weak_ptr)
// ============================================================================

class BadNode {
public:
    string name;
    shared_ptr<BadNode> next;  // PROBLEM: shared_ptr creates circular reference
    
    BadNode(string n) : name(n) {
        cout << "  [Constructor] BadNode " << name << " created" << endl;
    }
    
    ~BadNode() {
        cout << "  [Destructor] BadNode " << name << " destroyed" << endl;
    }
};

void circular_reference_problem() {
    cout << "\n=== 2. CIRCULAR REFERENCE PROBLEM ===" << endl;
    cout << "Creating circular reference with shared_ptr (MEMORY LEAK!):" << endl;
    
    {
        shared_ptr<BadNode> node1 = make_shared<BadNode>("Node1");
        shared_ptr<BadNode> node2 = make_shared<BadNode>("Node2");
        
        // Create circular reference
        node1->next = node2;  // node1 points to node2
        node2->next = node1;  // node2 points to node1 (CIRCULAR!)
        
        cout << "node1 use_count: " << node1.use_count() << " (expected: 2)" << endl;
        cout << "node2 use_count: " << node2.use_count() << " (expected: 2)" << endl;
        
    } // node1 and node2 go out of scope
    
    cout << "Exited scope - BUT DESTRUCTORS NOT CALLED!" << endl;
    cout << "MEMORY LEAK: Each node keeps the other alive (ref count never reaches 0)" << endl;
    
    /*
     * WHY THIS IS A PROBLEM:
     * ----------------------
     * node1.use_count = 2 (node1 variable + node2->next)
     * node2.use_count = 2 (node2 variable + node1->next)
     * 
     * When scope ends:
     * - node1 variable destroyed ? node1.use_count becomes 1 (still referenced by node2->next)
     * - node2 variable destroyed ? node2.use_count becomes 1 (still referenced by node1->next)
     * 
     * Neither reaches 0, so neither is deleted = MEMORY LEAK!
     */
}

// ============================================================================
// DEMONSTRATION 3: SOLVING CIRCULAR REFERENCE with weak_ptr
// ============================================================================

class GoodNode {
public:
    string name;
    shared_ptr<GoodNode> next;  // Strong reference (ownership)
    weak_ptr<GoodNode> prev;     // Weak reference (NO ownership) - SOLUTION!
    
    GoodNode(string n) : name(n) {
        cout << "  [Constructor] GoodNode " << name << " created" << endl;
    }
    
    ~GoodNode() {
        cout << "  [Destructor] GoodNode " << name << " destroyed" << endl;
    }
};

void circular_reference_solved() {
    cout << "\n=== 3. CIRCULAR REFERENCE SOLVED with weak_ptr ===" << endl;
    cout << "Creating circular structure with weak_ptr (NO MEMORY LEAK!):" << endl;
    
    {
        shared_ptr<GoodNode> node1 = make_shared<GoodNode>("Node1");
        shared_ptr<GoodNode> node2 = make_shared<GoodNode>("Node2");
        
        // Create circular structure
        node1->next = node2;  // node1 points to node2 (shared_ptr)
        node2->prev = node1;  // node2 points to node1 (weak_ptr - no ownership!)
        
        cout << "node1 use_count: " << node1.use_count() << " (expected: 1)" << endl;
        cout << "node2 use_count: " << node2.use_count() << " (expected: 2)" << endl;
        
        // Accessing prev node safely
        if (auto prev_node = node2->prev.lock()) {
            cout << "node2's previous node is: " << prev_node->name << endl;
        }
        
    } // node1 and node2 go out of scope
    
    cout << "Exited scope - DESTRUCTORS CALLED PROPERLY!" << endl;
    cout << "NO MEMORY LEAK: weak_ptr doesn't prevent destruction" << endl;
    
    /*
     * WHY THIS WORKS:
     * ---------------
     * node1.use_count = 1 (only node1 variable owns it)
     * node2.use_count = 2 (node2 variable + node1->next)
     * 
     * When scope ends:
     * - node1 variable destroyed ? node1.use_count becomes 0 ? node1 DELETED
     * - When node1 deleted, node1->next released ? node2.use_count becomes 1
     * - node2 variable destroyed ? node2.use_count becomes 0 ? node2 DELETED
     * 
     * Both are properly deleted! weak_ptr (node2->prev) doesn't prevent deletion.
     */
}

// ============================================================================
// DEMONSTRATION 4: OBSERVER PATTERN with weak_ptr
// ============================================================================

class Subject {
public:
    string name;
    
    Subject(string n) : name(n) {
        cout << "  [Subject] " << name << " created" << endl;
    }
    
    ~Subject() {
        cout << "  [Subject] " << name << " destroyed" << endl;
    }
    
    void doSomething() {
        cout << "  [Subject] " << name << " is doing something..." << endl;
    }
};

class Observer {
private:
    weak_ptr<Subject> subject_;  // Observer doesn't own Subject
    string observer_name_;
    
public:
    Observer(shared_ptr<Subject> subject, string name) 
        : subject_(subject), observer_name_(name) {
        cout << "  [Observer] " << observer_name_ << " created" << endl;
    }
    
    ~Observer() {
        cout << "  [Observer] " << observer_name_ << " destroyed" << endl;
    }
    
    void checkSubject() {
        cout << "\n  [Observer] " << observer_name_ << " checking subject..." << endl;
        
        if (auto sp = subject_.lock()) {
            // Subject still exists
            cout << "  Subject exists: " << sp->name << endl;
            sp->doSomething();
        } else {
            // Subject has been destroyed
            cout << "  Subject has been destroyed (weak_ptr expired)" << endl;
        }
    }
};

void observer_pattern_demo() {
    cout << "\n=== 4. OBSERVER PATTERN with weak_ptr ===" << endl;
    
    Observer* observer1 = nullptr;
    Observer* observer2 = nullptr;
    
    {
        shared_ptr<Subject> subject = make_shared<Subject>("DataSource");
        
        observer1 = new Observer(subject, "Observer1");
        observer2 = new Observer(subject, "Observer2");
        
        cout << "\nSubject exists, observers checking:" << endl;
        observer1->checkSubject();
        observer2->checkSubject();
        
        cout << "\nSubject use_count: " << subject.use_count() << " (only owned by shared_ptr)" << endl;
        
    } // subject goes out of scope and is destroyed
    
    cout << "\nSubject destroyed, observers checking:" << endl;
    observer1->checkSubject();
    observer2->checkSubject();
    
    delete observer1;
    delete observer2;
    
    /*
     * KEY BENEFIT:
     * ------------
     * - Observers can monitor Subject without keeping it alive
     * - When Subject is no longer needed, it can be destroyed
     * - Observers safely detect that Subject is gone (no dangling pointers)
     * - No memory leak: Subject lifetime independent of observer count
     */
}

// ============================================================================
// DEMONSTRATION 5: CACHE IMPLEMENTATION with weak_ptr
// ============================================================================

class ExpensiveResource {
public:
    string id;
    int data;
    
    ExpensiveResource(string id, int data) : id(id), data(data) {
        cout << "  [ExpensiveResource] " << id << " CREATED (expensive operation!)" << endl;
    }
    
    ~ExpensiveResource() {
        cout << "  [ExpensiveResource] " << id << " DESTROYED" << endl;
    }
};

class ResourceCache {
private:
    weak_ptr<ExpensiveResource> cached_resource_;
    
public:
    shared_ptr<ExpensiveResource> getResource(string id, int data) {
        // Try to get from cache
        if (auto sp = cached_resource_.lock()) {
            cout << "  [Cache] HIT! Returning cached resource: " << sp->id << endl;
            return sp;
        }
        
        // Cache miss - create new resource
        cout << "  [Cache] MISS! Creating new resource..." << endl;
        shared_ptr<ExpensiveResource> new_resource = 
            make_shared<ExpensiveResource>(id, data);
        
        // Store in cache (weak_ptr doesn't prevent destruction)
        cached_resource_ = new_resource;
        
        return new_resource;
    }
};

void cache_demo() {
    cout << "\n=== 5. CACHE IMPLEMENTATION with weak_ptr ===" << endl;
    
    ResourceCache cache;
    
    cout << "\n1. First access (cache miss):" << endl;
    {
        shared_ptr<ExpensiveResource> res1 = cache.getResource("Resource1", 100);
        cout << "Using resource: " << res1->id << ", data: " << res1->data << endl;
        
        cout << "\n2. Second access while resource still alive (cache hit):" << endl;
        shared_ptr<ExpensiveResource> res2 = cache.getResource("Resource1", 100);
        cout << "Same resource? " << (res1 == res2 ? "YES" : "NO") << endl;
        
    } // res1 and res2 go out of scope, resource destroyed
    
    cout << "\n3. Third access after resource destroyed (cache miss):" << endl;
    shared_ptr<ExpensiveResource> res3 = cache.getResource("Resource1", 100);
    cout << "New resource created" << endl;
    
    /*
     * KEY BENEFIT:
     * ------------
     * - Cache doesn't keep resources alive unnecessarily
     * - Resources are destroyed when no longer needed (when all shared_ptrs gone)
     * - Cache can still provide resource if it exists
     * - Prevents memory waste from cache holding resources forever
     */
}

// ============================================================================
// DEMONSTRATION 6: weak_ptr MEMBER FUNCTIONS
// ============================================================================

void weak_ptr_api_demo() {
    cout << "\n=== 6. weak_ptr API FUNCTIONS ===" << endl;
    
    shared_ptr<int> sp1 = make_shared<int>(42);
    weak_ptr<int> wp1 = sp1;
    weak_ptr<int> wp2;
    
    // 1. expired() - Check if object still exists
    cout << "\n1. expired() function:" << endl;
    cout << "wp1.expired(): " << (wp1.expired() ? "true" : "false") << endl;
    cout << "wp2.expired(): " << (wp2.expired() ? "true" : "false") << endl;
    
    // 2. lock() - Get shared_ptr if object exists
    cout << "\n2. lock() function:" << endl;
    if (auto sp2 = wp1.lock()) {
        cout << "wp1.lock() succeeded, value: " << *sp2 << endl;
        cout << "use_count after lock: " << sp2.use_count() << endl;
    }
    
    if (auto sp3 = wp2.lock()) {
        cout << "wp2.lock() succeeded" << endl;
    } else {
        cout << "wp2.lock() failed (returns nullptr)" << endl;
    }
    
    // 3. use_count() - Get reference count
    cout << "\n3. use_count() function:" << endl;
    cout << "wp1.use_count(): " << wp1.use_count() << endl;
    cout << "sp1.use_count(): " << sp1.use_count() << endl;
    
    {
        shared_ptr<int> sp4 = sp1;
        cout << "After creating sp4 from sp1:" << endl;
        cout << "wp1.use_count(): " << wp1.use_count() << endl;
    }
    
    // 4. reset() - Release the reference
    cout << "\n4. reset() function:" << endl;
    wp1.reset();
    cout << "After wp1.reset():" << endl;
    cout << "wp1.expired(): " << (wp1.expired() ? "true" : "false") << endl;
    cout << "sp1.use_count(): " << sp1.use_count() << " (unchanged)" << endl;
    
    // 5. swap() - Exchange contents
    cout << "\n5. swap() function:" << endl;
    weak_ptr<int> wp3 = sp1;
    weak_ptr<int> wp4;
    
    cout << "Before swap:" << endl;
    cout << "wp3.expired(): " << (wp3.expired() ? "true" : "false") << endl;
    cout << "wp4.expired(): " << (wp4.expired() ? "true" : "false") << endl;
    
    wp3.swap(wp4);
    
    cout << "After swap:" << endl;
    cout << "wp3.expired(): " << (wp3.expired() ? "true" : "false") << endl;
    cout << "wp4.expired(): " << (wp4.expired() ? "true" : "false") << endl;
}

// ============================================================================
// DEMONSTRATION 7: COMMON USE CASES SUMMARY
// ============================================================================

void use_cases_summary() {
    cout << "\n=== 7. COMMON USE CASES FOR weak_ptr ===" << endl;
    
    cout << "\n1. BREAKING CIRCULAR REFERENCES:" << endl;
    cout << "   - Parent-child relationships (parent owns child, child observes parent)" << endl;
    cout << "   - Doubly-linked lists (forward = shared_ptr, backward = weak_ptr)" << endl;
    cout << "   - Graph structures to prevent cycles" << endl;
    
    cout << "\n2. OBSERVER PATTERN:" << endl;
    cout << "   - Observers monitor subjects without owning them" << endl;
    cout << "   - Subject can be destroyed when no longer needed" << endl;
    cout << "   - Observers detect when subject is gone" << endl;
    
    cout << "\n3. CACHING:" << endl;
    cout << "   - Cache holds weak_ptr to objects" << endl;
    cout << "   - Objects can be destroyed when no longer used" << endl;
    cout << "   - Cache doesn't keep objects alive unnecessarily" << endl;
    
    cout << "\n4. CALLBACKS AND EVENT HANDLERS:" << endl;
    cout << "   - Register callbacks with weak_ptr to target" << endl;
    cout << "   - Target can be destroyed without notifying callbacks" << endl;
    cout << "   - Callbacks check if target still exists before calling" << endl;
    
    cout << "\n5. TEMPORARY ACCESS:" << endl;
    cout << "   - Need to check if object exists before using" << endl;
    cout << "   - Don't want to extend object's lifetime" << endl;
    cout << "   - Safe alternative to raw pointers" << endl;
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    cout << "========================================" << endl;
    cout << "     std::weak_ptr COMPLETE GUIDE      " << endl;
    cout << "========================================" << endl;
    
    basic_weak_ptr_demo();
    circular_reference_problem();
    circular_reference_solved();
    observer_pattern_demo();
    cache_demo();
    weak_ptr_api_demo();
    use_cases_summary();
    
    cout << "\n========================================" << endl;
    cout << "         Tutorial Complete!             " << endl;
    cout << "========================================" << endl;
    
    /*
     * KEY TAKEAWAYS:
     * ==============
     * 
     * 1. weak_ptr is a NON-OWNING smart pointer
     *    - Does NOT increase reference count
     *    - Does NOT prevent object destruction
     * 
     * 2. PRIMARY USE: Breaking circular references
     *    - Prevents memory leaks in cyclic data structures
     *    - One direction = shared_ptr, other = weak_ptr
     * 
     * 3. SAFE ACCESS: Must use lock() to access object
     *    - lock() returns shared_ptr if object exists
     *    - Returns nullptr if object destroyed
     *    - Always check before using!
     * 
     * 4. COMMON PATTERNS:
     *    - Parent-child relationships
     *    - Observer pattern
     *    - Caching
     *    - Callbacks
     * 
     * 5. COMPARISON:
     *    shared_ptr: "I own this object"
     *    weak_ptr:   "I know about this object, but don't own it"
     *    unique_ptr: "I am the ONLY owner of this object"
     * 
     * 6. WHEN TO USE:
     *    - Need to observe without owning
     *    - Want to break circular references
     *    - Need to check if object still exists
     *    - Don't want to extend object lifetime
     */
    
    return 0;
}
