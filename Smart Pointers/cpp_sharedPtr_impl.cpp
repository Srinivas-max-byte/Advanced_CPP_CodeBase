#include <iostream>
#include <memory>  // Required for std::shared_ptr
#include <vector>
using namespace std;

// Example class to demonstrate shared_ptr with custom types
class MyClass {
public:
    int value;
    string name;
    
    MyClass(int v, string n) : value(v), name(n) {
        cout << "MyClass constructor called: " << name << " (value: " << value << ")" << endl;
    }
    
    ~MyClass() {
        cout << "MyClass destructor called: " << name << endl;
    }
    
    void display() const {
        cout << "Name: " << name << ", Value: " << value << endl;
    }
};

int main() {
    cout << "=== C++ Standard shared_ptr Demonstration ===\n" << endl;
    
    // ============================================
    // 1. CONSTRUCTION METHODS
    // ============================================
    cout << "--- 1. CONSTRUCTION METHODS ---" << endl;
    
    // 1.1 Default construction (empty shared_ptr)
    shared_ptr<int> sp1;
    cout << "sp1 is " << (sp1 ? "valid" : "empty") << endl;
    cout << "sp1 use_count: " << sp1.use_count() << endl; //Get the number of references to the shared_ptr
    
    // 1.2 Construction from raw pointer
    shared_ptr<int> sp2(new int(42));
    cout << "\nsp2 created with raw pointer, value: " << *sp2 << endl;
    cout << "sp2 use_count: " << sp2.use_count() << endl;
    
    // 1.3 make_shared (RECOMMENDED - more efficient, exception-safe)
    shared_ptr<int> sp3 = make_shared<int>(100); //Create a shared_ptr and allocate memory for the int value 100
    cout << "\nsp3 created with make_shared, value: " << *sp3 << endl;
    cout << "sp3 use_count: " << sp3.use_count() << endl;
    
    // 1.4 make_shared with custom type
    shared_ptr<MyClass> sp4 = make_shared<MyClass>(10, "Object1"); //Create a shared_ptr and allocate memory for the MyClass object with the value 10 and the name "Object1"
    cout << "sp4 use_count: " << sp4.use_count() << endl;
    
    // ============================================
    // 2. COPY CONSTRUCTION AND ASSIGNMENT
    // ============================================
    cout << "\n--- 2. COPY CONSTRUCTION AND ASSIGNMENT ---" << endl;
    
    // 2.1 Copy construction
    shared_ptr<int> sp5 = sp3;  // Copy constructor
    cout << "After copy construction:" << endl;
    cout << "sp3 use_count: " << sp3.use_count() << endl;
    cout << "sp5 use_count: " << sp5.use_count() << endl;
    cout << "sp3 and sp5 share the same object: " << (sp3.get() == sp5.get()) << endl;
    
    // 2.2 Copy assignment
    shared_ptr<int> sp6;
    sp6 = sp3;  // Copy assignment
    cout << "\nAfter copy assignment:" << endl;
    cout << "sp3 use_count: " << sp3.use_count() << endl;
    cout << "sp5 use_count: " << sp5.use_count() << endl;
    cout << "sp6 use_count: " << sp6.use_count() << endl;
    
    // ============================================
    // 3. MOVE CONSTRUCTION AND ASSIGNMENT
    // ============================================
    cout << "\n--- 3. MOVE CONSTRUCTION AND ASSIGNMENT ---" << endl;
    
    shared_ptr<int> sp7 = make_shared<int>(200);
    cout << "sp7 use_count before move: " << sp7.use_count() << endl;
    
    // 3.1 Move construction
    shared_ptr<int> sp8 = move(sp7);  // Move constructor
    cout << "After move construction:" << endl;
    cout << "sp7 is " << (sp7 ? "valid" : "empty") << endl;
    cout << "sp8 use_count: " << sp8.use_count() << endl;
    
    // 3.2 Move assignment
    shared_ptr<int> sp9;
    sp9 = move(sp8);
    cout << "\nAfter move assignment:" << endl;
    cout << "sp8 is " << (sp8 ? "valid" : "empty") << endl;
    cout << "sp9 use_count: " << sp9.use_count() << endl;
    
    // ============================================
    // 4. DEREFERENCE OPERATORS
    // ============================================
    cout << "\n--- 4. DEREFERENCE OPERATORS ---" << endl;
    
    // 4.1 operator* (dereference)
    *sp3 = 999;
    cout << "Modified *sp3 = 999, now *sp3 = " << *sp3 << endl;
    cout << "Since sp5 and sp6 share ownership, *sp5 = " << *sp5 << ", *sp6 = " << *sp6 << endl;
    
    // 4.2 operator-> (member access)
    shared_ptr<MyClass> sp10 = make_shared<MyClass>(50, "Object2");
    sp10->value = 75;  // Using operator->
    sp10->name = "ModifiedObject2";
    sp10->display();
    
    // ============================================
    // 5. MEMBER FUNCTIONS
    // ============================================
    cout << "\n--- 5. MEMBER FUNCTIONS ---" << endl;
    
    // 5.1 get() - get raw pointer (use with caution!)
    int* raw_ptr = sp3.get();
    cout << "Raw pointer from sp3.get(): " << raw_ptr << endl;
    cout << "Value via raw pointer: " << *raw_ptr << endl;
    cout << "Warning: Don't delete raw_ptr manually!" << endl;
    
    // 5.2 use_count() - reference count
    shared_ptr<int> sp11 = sp3;
    shared_ptr<int> sp12 = sp3;
    cout << "\nReference counts:" << endl;
    cout << "sp3 use_count: " << sp3.use_count() << endl;
    cout << "sp5 use_count: " << sp5.use_count() << endl;
    cout << "sp6 use_count: " << sp6.use_count() << endl;
    cout << "sp11 use_count: " << sp11.use_count() << endl;
    cout << "sp12 use_count: " << sp12.use_count() << endl;
    
    // 5.3 unique() - check if unique owner (deprecated in C++17, but still works)
    shared_ptr<int> sp13 = make_shared<int>(300);
    cout << "\nsp13 is unique: " << sp13.unique() << endl;
    shared_ptr<int> sp14 = sp13;
    cout << "After sharing, sp13 is unique: " << sp13.unique() << endl;
    
    // 5.4 bool conversion - check if valid
    shared_ptr<int> sp15;
    cout << "\nsp15 is " << (sp15 ? "valid" : "empty") << endl;
    sp15 = make_shared<int>(400);
    cout << "After assignment, sp15 is " << (sp15 ? "valid" : "empty") << endl;
    
    // ============================================
    // 6. RESET FUNCTIONALITY
    // ============================================
    cout << "\n--- 6. RESET FUNCTIONALITY ---" << endl;
    
    shared_ptr<int> sp16 = make_shared<int>(500);
    shared_ptr<int> sp17 = sp16;
    cout << "Before reset:" << endl;
    cout << "sp16 use_count: " << sp16.use_count() << endl;
    cout << "sp17 use_count: " << sp17.use_count() << endl;
    
    // 6.1 reset() - release ownership
    sp16.reset();
    cout << "\nAfter sp16.reset():" << endl;
    cout << "sp16 is " << (sp16 ? "valid" : "empty") << endl;
    cout << "sp17 use_count: " << sp17.use_count() << endl;
    
    // 6.2 reset(new_ptr) - reset with new pointer
    sp17.reset(new int(600));
    cout << "\nAfter sp17.reset(new int(600)):" << endl;
    cout << "sp17 use_count: " << sp17.use_count() << endl;
    cout << "*sp17 = " << *sp17 << endl;
    
    // ============================================
    // 7. SWAP FUNCTIONALITY
    // ============================================
    cout << "\n--- 7. SWAP FUNCTIONALITY ---" << endl;
    
    shared_ptr<int> sp18 = make_shared<int>(700);
    shared_ptr<int> sp19 = make_shared<int>(800);
    cout << "Before swap:" << endl;
    cout << "*sp18 = " << *sp18 << ", *sp19 = " << *sp19 << endl;
    
    sp18.swap(sp19);
    cout << "\nAfter swap:" << endl;
    cout << "*sp18 = " << *sp18 << ", *sp19 = " << *sp19 << endl;
    
    // ============================================
    // 8. CUSTOM DELETER
    // ============================================
    cout << "\n--- 8. CUSTOM DELETER ---" << endl;
    
    // Custom deleter function
    auto custom_deleter = [](int* p) {
        cout << "Custom deleter called for value: " << *p << endl;
        delete p;
    };
    
    shared_ptr<int> sp20(new int(900), custom_deleter);
    cout << "sp20 created with custom deleter, value: " << *sp20 << endl;
    cout << "sp20 use_count: " << sp20.use_count() << endl;
    
    // When sp20 goes out of scope, custom deleter will be called
    
    // ============================================
    // 9. SHARED OWNERSHIP DEMONSTRATION
    // ============================================
    cout << "\n--- 9. SHARED OWNERSHIP DEMONSTRATION ---" << endl;
    
    shared_ptr<MyClass> shared_obj = make_shared<MyClass>(100, "SharedObject");
    cout << "shared_obj use_count: " << shared_obj.use_count() << endl;
    
    {
        shared_ptr<MyClass> local_copy1 = shared_obj;
        shared_ptr<MyClass> local_copy2 = shared_obj;
        cout << "\nInside inner scope:" << endl;
        cout << "shared_obj use_count: " << shared_obj.use_count() << endl;
        cout << "local_copy1 use_count: " << local_copy1.use_count() << endl;
        cout << "local_copy2 use_count: " << local_copy2.use_count() << endl;
        
        // Modify through one reference
        local_copy1->value = 200;
        cout << "\nModified through local_copy1:" << endl;
        shared_obj->display();
    } // local_copy1 and local_copy2 go out of scope here
    
    cout << "\nAfter inner scope ends:" << endl;
    cout << "shared_obj use_count: " << shared_obj.use_count() << endl;
    shared_obj->display();
    
    // ============================================
    // 10. VECTOR OF SHARED_PTR
    // ============================================
    cout << "\n--- 10. VECTOR OF SHARED_PTR ---" << endl;
    
    vector<shared_ptr<MyClass>> vec;
    vec.push_back(make_shared<MyClass>(1, "VecObj1"));
    vec.push_back(make_shared<MyClass>(2, "VecObj2"));
    vec.push_back(make_shared<MyClass>(3, "VecObj3"));
    
    cout << "\nVector contains " << vec.size() << " shared_ptr objects" << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << "vec[" << i << "] use_count: " << vec[i].use_count() << endl;
    }
    
    // Share one element
    shared_ptr<MyClass> shared_from_vec = vec[1];
    cout << "\nAfter sharing vec[1]:" << endl;
    cout << "vec[1] use_count: " << vec[1].use_count() << endl;
    cout << "shared_from_vec use_count: " << shared_from_vec.use_count() << endl;
    
    // ============================================
    // 11. AUTOMATIC CLEANUP DEMONSTRATION
    // ============================================
    cout << "\n--- 11. AUTOMATIC CLEANUP DEMONSTRATION ---" << endl;
    cout << "Creating temporary shared_ptr objects..." << endl;
    
    {
        shared_ptr<MyClass> temp1 = make_shared<MyClass>(999, "Temp1");
        shared_ptr<MyClass> temp2 = temp1;
        shared_ptr<MyClass> temp3 = temp1;
        cout << "temp1 use_count: " << temp1.use_count() << endl;
    } // All temp objects go out of scope here, destructor called once
    
    cout << "\n=== End of Demonstration ===" << endl;
    cout << "\nNote: Destructors will be called for remaining objects when main() ends." << endl;
    
    return 0;
}

