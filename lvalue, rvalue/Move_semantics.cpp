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
        cout << " [Move Constructor] Moved: " << data << endl;
    }

    // ========================================================================
    // COPY ASSIGNMENT OPERATOR - DETAILED EXPLANATION
    // ========================================================================
    /*
     * SIGNATURE: MyString& operator=(const MyString &other)
     * 
     * PURPOSE:
     * - Assigns one EXISTING object to another EXISTING object
     * - Performs DEEP COPY of resources (creates independent copy)
     * - Called when: obj1 = obj2; (both obj1 and obj2 already constructed)
     * 
     * WHEN IS IT CALLED?
     * Example:
     *     MyString s1("Hello");  // Constructor called
     *     MyString s2("World");  // Constructor called
     *     s1 = s2;               // Copy Assignment Operator called HERE
     *     // After: s1 contains "World", s2 still contains "World"
     *     // Both have separate, independent memory allocations
     * 
     * LOGIC FLOW (Step-by-Step):
     * 
     * 1. SELF-ASSIGNMENT CHECK: if (this != &other)
     *    - Prevents disaster when doing: obj = obj;
     *    - If we don't check, we'd delete our own data before copying it!
     *    - Example: s1 = s1; would cause undefined behavior without this check
     * 
     * 2. CLEAN UP OLD RESOURCE: delete[] data;
     *    - Before getting new data, we must free existing memory
     *    - Prevents memory leak (losing pointer to old allocation)
     *    - Example: If s1 had "Hello" (5 bytes), we free that memory first
     * 
     * 3. COPY METADATA: size = other.size;
     *    - Copy the size of the string we're copying from
     *    - Needed to allocate correct amount of memory
     * 
     * 4. ALLOCATE NEW MEMORY: data = new char[size + 1];
     *    - Create NEW, INDEPENDENT memory allocation
     *    - +1 for null terminator '\0'
     *    - This is DEEP COPY (not sharing pointers)
     * 
     * 5. COPY DATA: strcpy(data, other.data);
     *    - Actually copy the string content byte-by-byte
     *    - Now we have two identical but independent strings
     * 
     * 6. RETURN *this: return *this;
     *    - Returns reference to current object
     *    - Allows chaining: a = b = c;
     *    - Evaluates right-to-left: a = (b = c);
     * 
     * PERFORMANCE:
     * - Time: O(n) where n = string length (due to strcpy)
     * - Space: O(n) new allocation
     * - Expensive for large strings!
     * 
     * MEMORY DIAGRAM:
     * Before: s1 = s2;
     *     s1.data → [H][e][l][l][o][\0]  (memory address 0x1000)
     *     s2.data → [W][o][r][l][d][\0]  (memory address 0x2000)
     * 
     * After: s1 = s2;
     *     s1.data → [W][o][r][l][d][\0]  (memory address 0x3000) NEW!
     *     s2.data → [W][o][r][l][d][\0]  (memory address 0x2000) unchanged
     *     (0x1000 was freed by delete[])
     */
    MyString &operator=(const MyString &other) {
        if (this != &other) {           // Step 1: Self-assignment check
            delete[] data;               // Step 2: Clean up old resource
            size = other.size;           // Step 3: Copy metadata
            data = new char[size + 1];   // Step 4: Allocate NEW memory
            strcpy(data, other.data);    // Step 5: Deep copy data
            cout << "  [Copy Assignment] Copied: " << data << endl;
        }
        return *this;                    // Step 6: Return reference
    }

    // ========================================================================
    // MOVE ASSIGNMENT OPERATOR - DETAILED EXPLANATION
    // ========================================================================
    /*
     * SIGNATURE: MyString& operator=(MyString &&other) noexcept
     * 
     * PURPOSE:
     * - Transfers ownership of resources from source to destination
     * - NO deep copy - just steals the pointer!
     * - Called when: obj1 = std::move(obj2); or obj1 = temp_object();
     * 
     * WHEN IS IT CALLED?
     * Example:
     *     MyString s1("Hello");          // Constructor called
     *     MyString s2("World");          // Constructor called
     *     s1 = std::move(s2);            // Move Assignment Operator called HERE
     *     // After: s1 contains "World", s2 is empty (data = nullptr)
     *     // Only ONE memory allocation exists (transferred from s2 to s1)
     * 
     * LOGIC FLOW (Step-by-Step):
     * 
     * 1. SELF-ASSIGNMENT CHECK: if (this != &other)
     *    - Prevents moving object to itself
     *    - Example: s1 = std::move(s1); would be problematic
     *    - Less critical than copy, but still good practice
     * 
     * 2. CLEAN UP OLD RESOURCE: delete[] data;
     *    - Free memory that 'this' currently owns
     *    - Example: If s1 had "Hello", we free those 5 bytes
     *    - Prevents memory leak
     * 
     * 3. STEAL THE POINTER: data = other.data;
     *    - Copy the POINTER value (not the data itself!)
     *    - This is just copying an address (8 bytes on 64-bit)
     *    - FAST operation - O(1), no matter how large the string!
     * 
     * 4. STEAL THE SIZE: size = other.size;
     *    - Copy metadata
     *    - Also O(1) operation
     * 
     * 5. NULLIFY SOURCE: other.data = nullptr; other.size = 0;
     *    - Leave source in a VALID but EMPTY state
     *    - Critical! Prevents double-delete when source destructor runs
     *    - Source can still be destroyed or assigned to safely
     * 
     * 6. RETURN *this: return *this;
     *    - Returns reference to current object
     *    - Allows chaining
     * 
     * WHY noexcept?
     * - Promises this function won't throw exceptions
     * - Critical for std::vector optimization
     * - If not noexcept, vector uses copy instead of move during reallocation!
     * 
     * PERFORMANCE COMPARISON:
     * Copy Assignment:
     *     - Allocates new memory: ~1000 cycles (if size = 100 chars)
     *     - Copies 100 bytes: ~100 cycles
     *     - Total: ~1100 cycles
     * 
     * Move Assignment:
     *     - Copy pointer: ~1 cycle
     *     - Copy size: ~1 cycle
     *     - Nullify source: ~2 cycles
     *     - Total: ~4 cycles
     *     - **275x faster for 100-char string!**
     * 
     * MEMORY DIAGRAM:
     * Before: s1 = std::move(s2);
     *     s1.data → [H][e][l][l][o][\0]  (memory address 0x1000)
     *     s2.data → [W][o][r][l][d][\0]  (memory address 0x2000)
     * 
     * After: s1 = std::move(s2);
     *     s1.data → [W][o][r][l][d][\0]  (memory address 0x2000) STOLEN!
     *     s2.data → nullptr               (no longer owns anything)
     *     (0x1000 was freed by delete[])
     *     (0x2000 ownership transferred, not copied!)
     * 
     * IMPORTANT NOTES:
     * - Source object (other) is left in "valid but unspecified" state
     * - You can still call destructor on source (it's safe, checks nullptr)
     * - You can reassign source: s2 = MyString("New"); would work fine
     * - Don't use source after move unless you reassign it!
     * 
     * REAL-WORLD ANALOGY:
     * Copy Assignment = Photocopying a document (slow, creates duplicate)
     * Move Assignment = Taking ownership of a document (fast, transfer only)
     */
    MyString &operator=(MyString &&other) noexcept {
        if (this != &other) {            // Step 1: Self-assignment check
            delete[] data;               // Step 2: Clean up old resource
            data = other.data;           // Step 3: STEAL the pointer (not data!)
            size = other.size;           // Step 4: Steal metadata
            other.data = nullptr;        // Step 5: Nullify source
            other.size = 0;              // Step 5: (continued)
            cout << "  [Move Assignment] Moved: " << data << endl;
        }
        return *this;                    // Step 6: Return reference
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
// 3A. ASSIGNMENT OPERATORS DEMONSTRATION
// ============================================================================
/*
 * This section demonstrates the difference between:
 * - Copy Assignment vs Move Assignment
 * - Constructor vs Assignment Operator
 */

void assignment_operators_demo() {
    cout << "\n=== ASSIGNMENT OPERATORS IN ACTION ===" << endl;

    // ========================================================================
    // COPY ASSIGNMENT OPERATOR DEMO
    // ========================================================================
    cout << "\n--- COPY ASSIGNMENT OPERATOR ---" << endl;
    cout << "Creating two objects:" << endl;
    MyString original("Original");
    MyString target("Target");
    
    cout << "\nBefore assignment:" << endl;
    cout << "original: "; original.print(); 
    cout << ", target: "; target.print(); cout << endl;
    
    cout << "\nExecuting: target = original; (Copy Assignment)" << endl;
    target = original;  // Copy Assignment Operator called
    
    cout << "\nAfter copy assignment:" << endl;
    cout << "original: "; original.print(); 
    cout << ", target: "; target.print(); cout << endl;
    cout << "Both objects are independent (changing one won't affect the other)" << endl;

    // ========================================================================
    // MOVE ASSIGNMENT OPERATOR DEMO
    // ========================================================================
    cout << "\n--- MOVE ASSIGNMENT OPERATOR ---" << endl;
    cout << "Creating two objects:" << endl;
    MyString source("Source");
    MyString destination("Destination");
    
    cout << "\nBefore move assignment:" << endl;
    cout << "source: "; source.print(); 
    cout << ", destination: "; destination.print(); cout << endl;
    
    cout << "\nExecuting: destination = std::move(source); (Move Assignment)" << endl;
    destination = move(source);  // Move Assignment Operator called
    
    cout << "\nAfter move assignment:" << endl;
    cout << "source: "; 
    if(source.data != nullptr) source.print(); 
    else cout << "(empty/nullptr)";
    cout << ", destination: "; destination.print(); cout << endl;
    cout << "Source is now empty, destination took ownership!" << endl;

    // ========================================================================
    // CONSTRUCTOR vs ASSIGNMENT OPERATOR
    // ========================================================================
    cout << "\n--- CONSTRUCTOR vs ASSIGNMENT ---" << endl;
    cout << "1. Copy Constructor (initialization):" << endl;
    MyString a("AAA");
    MyString b = a;  // This is COPY CONSTRUCTOR, not assignment!
    // Equivalent to: MyString b(a);
    
    cout << "\n2. Copy Assignment (assignment to existing object):" << endl;
    MyString c("CCC");
    c = a;  // This is COPY ASSIGNMENT OPERATOR
    
    cout << "\n3. Move Constructor (initialization with rvalue):" << endl;
    MyString d = move(a);  // This is MOVE CONSTRUCTOR
    
    cout << "\n4. Move Assignment (assignment with rvalue):" << endl;
    MyString e("EEE");
    e = move(c);  // This is MOVE ASSIGNMENT OPERATOR

    // ========================================================================
    // CHAINING ASSIGNMENTS
    // ========================================================================
    cout << "\n--- CHAINING ASSIGNMENTS ---" << endl;
    cout << "Creating three objects:" << endl;
    MyString x("XXX");
    MyString y("YYY");
    MyString z("ZZZ");
    
    cout << "\nExecuting: x = y = z; (chained assignment)" << endl;
    cout << "This evaluates right-to-left: x = (y = z);" << endl;
    x = y = z;  // First y=z executes, then x=y
    
    cout << "\nAfter chaining:" << endl;
    cout << "x: "; x.print(); 
    cout << ", y: "; y.print(); 
    cout << ", z: "; z.print(); cout << endl;
    cout << "All three contain 'ZZZ' (independent copies)" << endl;

    // ========================================================================
    // SELF-ASSIGNMENT
    // ========================================================================
    cout << "\n--- SELF-ASSIGNMENT (Protected by check) ---" << endl;
    MyString self("SelfTest");
    cout << "Executing: self = self; (self-assignment)" << endl;
    self = self;  // Should be safe due to if (this != &other) check
    cout << "After self-assignment: "; self.print(); 
    cout << " (no crash, data preserved!)" << endl;
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
    assignment_operators_demo();
    move_demo();
    perfect_forwarding_demo();
    forwarding_reference_demo();
    container_move_demo();

    cout << "\n========================================" << endl;
    cout << "  Tutorial Complete!" << endl;
    cout << "========================================" << endl;

    return 0;
}