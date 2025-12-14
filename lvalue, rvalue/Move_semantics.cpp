#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

// ============================================================================
// 1. LVALUE vs RVALUE BASICS
// ============================================================================
/*
 * LVALUE: An object with a persistent memory address that you can take the
 *         address of using &. It can appear on the LEFT side of assignment.
 *         Examples: variables (x, y), named objects, return values by reference
 *
 * RVALUE: A temporary object without a persistent address. Cannot take address.
 *         It can only appear on the RIGHT side of assignment.
 *         Examples: literals (5, "hello"), temporary objects, return by value
 */

void lvalue_rvalue_demo() {
    cout << "\n=== LVALUE vs RVALUE ===" << endl;
    
    int x = 5;           // x is an LVALUE (has persistent memory address)
    int y = x;           // x on right side, but x is still an LVALUE
    int z = 10;          // 10 is an RVALUE (temporary literal)
    
    // int &ref = 10;    // ERROR: cannot bind lvalue reference to rvalue
    int &ref = x;        // OK: binding lvalue reference to lvalue
    
    cout << "x (lvalue): " << x << ", address: " << &x << endl;
    cout << "ref (reference to x): " << ref << endl;
}

// ============================================================================
// 2. LVALUE REFERENCES vs RVALUE REFERENCES
// ============================================================================
/*
 * LVALUE REFERENCE (&):  Binds to lvalues only. Cannot bind to temporaries.
 *                        Syntax: int &ref = x;
 *
 * RVALUE REFERENCE (&&): Binds to rvalues only. Allows you to capture
 *                        temporary objects before they are destroyed.
 *                        Syntax: int &&ref = 10;
 */

void reference_demo() {
    cout << "\n=== LVALUE REFERENCES vs RVALUE REFERENCES ===" << endl;
    
    int x = 5;
    
    // Lvalue reference
    int &lref = x;           // OK: lvalue ref binds to lvalue
    cout << "lvalue ref: " << lref << endl;
    
    // Rvalue reference
    int &&rref = 10;         // OK: rvalue ref binds to rvalue (temporary)
    cout << "rvalue ref: " << rref << endl;
    
    // You can modify rvalue references
    rref = 20;
    cout << "modified rvalue ref: " << rref << endl;
    
    // ERROR: int &&rref2 = x;  // rvalue ref cannot bind to lvalue
}

// ============================================================================
// 3. COPY vs MOVE SEMANTICS
// ============================================================================
/*
 * COPY SEMANTICS: Creates a deep copy. Expensive for large objects.
 *                 Uses copy constructor and copy assignment operator.
 *
 * MOVE SEMANTICS: Transfers ownership of resources (like pointers to data).
 *                 Cheap; avoids deep copying. Uses move constructor and
 *                 move assignment operator.
 */

class MyString {

public:
    char *data;
    int size;
    // Constructor
    MyString(const string &str = "") : size(str.length()) {
        data = new char[size + 1];
        strcpy(data, str.c_str());
        cout << "  [Constructor] Created MyString: " << data << endl;
    }

    // Copy Constructor (COPY SEMANTICS)
    MyString(const MyString &other) : size(other.size) {
        data = new char[size + 1];  // Allocate NEW memory
        strcpy(data, other.data);   // COPY data
        cout << "  [Copy Constructor] Copied: " << data << endl;
    }

    // Move Constructor (MOVE SEMANTICS)
    MyString(MyString &&other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;       // Leave source in empty state
        other.size = 0;
        cout << "  [Move Constructor] Moved: " << data << endl;
    }

    // Copy Assignment Operator
    MyString &operator=(const MyString &other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new char[size + 1];
            strcpy(data, other.data);
            cout << "  [Copy Assignment] Copied: " << data << endl;
        }
        return *this;
    }

    // Move Assignment Operator
    MyString &operator=(MyString &&other) noexcept {
        if (this != &other) {
            delete[] data;              // Clean up old resource
            data = other.data;          // STEAL resource from other
            size = other.size;
            other.data = nullptr;       // Leave source empty
            other.size = 0;
            cout << "  [Move Assignment] Moved: " << data << endl;
        }
        return *this;
    }

    ~MyString() {
        delete[] data;
        cout << "  [Destructor] Destroyed" << endl;
    }

    void print() const { cout << data; }
};

void copy_vs_move_demo() {
    cout << "\n=== COPY vs MOVE SEMANTICS ===" << endl;

    cout << "\n--- COPY (Expensive) ---" << endl;
    MyString s1("Hello");
    MyString s2 = s1;  // Calls copy constructor (deep copy)
    cout << "s1: "; s1.print(); cout << ", s2: "; s2.print(); cout << endl;

    cout << "\n--- MOVE (Cheap) ---" << endl;
    MyString s3("World");
    MyString s4 = move(s3);  // Calls move constructor (steals resources)
    cout << "s3: "; if(s3.data != nullptr) s3.print(); 
    cout << " (now empty), s4: "; s4.print(); cout << endl;
}

// ============================================================================
// 4. std::move() AND MOVE SEMANTICS
// ============================================================================
/*
 * std::move(): Converts an lvalue into an rvalue reference.
 *              Tells the compiler: "treat this as temporary, use move semantics"
 *              Does NOT move data itself; it just casts the type.
 */

void move_demo() {
    cout << "\n=== std::move() ===" << endl;

    MyString s1("MoveMe");
    cout << "\nWithout move (copy):" << endl;
    MyString s2 = s1;  // s1 is lvalue → copy constructor called

    MyString s3("MoveMe2");
    cout << "\nWith move (move):" << endl;
    MyString s4 = move(s3);  // move(s3) returns rvalue ref → move constructor called
}

// ============================================================================
// 5. PERFECT FORWARDING
// ============================================================================
/*
 * PERFECT FORWARDING: Passing arguments to another function while preserving
 *                     their lvalue/rvalue nature.
 *
 * Problem: If you pass arguments through a wrapper function, they become
 *          lvalues, losing their rvalue-ness.
 *
 * Solution: Use forwarding references (T&&) and std::forward<T>()
 */

// Version 1: WITHOUT perfect forwarding (loses rvalue info)
void process_copy(MyString s) {
    cout << "    Processing: "; s.print(); cout << endl;
}

void wrapper_bad(MyString s) {
    cout << "  [Wrapper] Received lvalue or rvalue" << endl;
    process_copy(s);  // s is lvalue here, even if original was rvalue
}

// Version 2: WITH perfect forwarding (preserves lvalue/rvalue)
template <typename T>
void process_fwd(T &&s) {
    cout << "    Processing: "; s.print(); cout << endl;
}

template <typename T>
void wrapper_good(T &&s) {
    cout << "  [Wrapper] Forwarding to process" << endl;
    process_fwd(forward<T>(s));  // std::forward preserves lvalue/rvalue nature
}

void perfect_forwarding_demo() {
    cout << "\n=== PERFECT FORWARDING ===" << endl;

    cout << "\n--- Without Perfect Forwarding (copy happens) ---" << endl;
    MyString s1("NoForward");
    cout << "Calling wrapper_bad with rvalue:" << endl;
    wrapper_bad(move(s1));  // Moves into wrapper, but wrapper_bad copies to process

    cout << "\n--- With Perfect Forwarding (move preserved) ---" << endl;
    MyString s2("WithForward");
    cout << "Calling wrapper_good with rvalue:" << endl;
    wrapper_good(move(s2));  // Move is preserved through the entire chain
}

// ============================================================================
// 6. UNIVERSAL REFERENCES / FORWARDING REFERENCES
// ============================================================================
/*
 * FORWARDING REFERENCE (T&&): In a template, T&& is NOT necessarily an rvalue ref.
 *                             It's a "universal reference" that can bind to BOTH
 *                             lvalues AND rvalues, depending on the context.
 *
 * Rules:
 * - T&& with lvalue → deduces T as T& (becomes int& && = int&)
 * - T&& with rvalue → deduces T as T (becomes int&& = int&&)
 */

template <typename T>
void show_type(T &&arg) {
    if (std::is_lvalue_reference_v<T>) {
        cout << "  Received as LVALUE (type is lvalue ref)" << endl;
    } else {
        cout << "  Received as RVALUE (type is rvalue ref)" << endl;
    }
}

void forwarding_reference_demo() {
    cout << "\n=== FORWARDING REFERENCES / UNIVERSAL REFERENCES ===" << endl;

    int x = 5;
    cout << "show_type(x) where x is lvalue:" << endl;
    show_type(x);  // T deduces as int&

    cout << "show_type(10) where 10 is rvalue:" << endl;
    show_type(10);  // T deduces as int
}

// ============================================================================
// 7. MOVE SEMANTICS WITH CONTAINERS
// ============================================================================

void container_move_demo() {
    cout << "\n=== MOVE SEMANTICS WITH CONTAINERS ===" << endl;

    cout << "\n--- Without std::move (copy) ---" << endl;
    vector<MyString> vec1;
    MyString s1("First");
    vec1.push_back(s1);  // Copy because s1 is lvalue

    cout << "\n--- With std::move (efficient) ---" << endl;
    vector<MyString> vec2;
    MyString s2("Second");
    vec2.push_back(move(s2));  // Move because move(s2) is rvalue
    /*
        Step 1: MyString s2("Second");
        s2 is a named object
        It owns some internal resource (say, a heap buffer holding "Second")
        At this point, everything is normal and stable.

        Step 2: std::move(s2)
        This does not move anything
        It casts s2 to MyString&& (an rvalue expression)
        You are telling vector:
        “You may steal from s2 if you know how.”

        Step 3: vec2.push_back(std::move(s2));
        std::vector::push_back has two relevant overloads:
        void push_back(const T&);  // copy
        void push_back(T&&);       // move

        Step 4: Inside push_back(T&&)
        The vector constructs a new element in its internal storage using:
        MyString(std::move(s2));  // move constructor

        If MyString has a move constructor:
        The internal buffer pointer is transferred from s2 to the vector element
        No deep copy happens
        This is fast
        If MyString does not have a move constructor:
        The copy constructor is used instead
        Performance drops, semantics stay safe.

        Step 5: State after the call
        vec2[0] now owns "Second"
        s2 is left in a valid but unspecified state
        Usually empty
        Must be safe to destroy or reassign
        s2 still exists, but you should not depend on its contents
    */

    cout << "\n--- Return value optimization (move elision) ---" << endl;
    auto create = []() {
        MyString temp("Created");
        return temp;  // Compiler optimizes away unnecessary copies
    };
    MyString s3 = create();  // Move constructor may be called
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    cout << "========================================" << endl;
    cout << "  MOVE SEMANTICS - COMPLETE TUTORIAL   " << endl;
    cout << "========================================" << endl;

    lvalue_rvalue_demo();
    reference_demo();
    copy_vs_move_demo();
    move_demo();
    perfect_forwarding_demo();
    forwarding_reference_demo();
    container_move_demo();

    cout << "\n========================================" << endl;
    cout << "  Tutorial Complete!" << endl;
    cout << "========================================" << endl;

    return 0;
}