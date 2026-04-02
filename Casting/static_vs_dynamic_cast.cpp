/*
===============================================================================
              C++ TYPE CASTING - COMPREHENSIVE GUIDE
          static_cast vs dynamic_cast - Properties, Use Cases & Differences
===============================================================================

OVERVIEW OF C++ CASTS:
----------------------
C++ provides four type-casting operators (safer than C-style casts):
1. static_cast    - Compile-time type conversion (most common)
2. dynamic_cast   - Runtime type checking for polymorphic types
3. const_cast     - Add/remove const qualifier
4. reinterpret_cast - Low-level reinterpretation of bit patterns

This file focuses on static_cast and dynamic_cast.

===============================================================================
                            STATIC_CAST
===============================================================================

DEFINITION:
-----------
static_cast performs compile-time type conversions. It checks type compatibility
at compile time but does NO runtime checking.

SYNTAX:
-------
    static_cast<new_type>(expression)

PROPERTIES:
-----------
1. COMPILE-TIME: Conversion is resolved at compile time
2. NO RUNTIME OVERHEAD: Zero performance cost (no RTTI needed)
3. TYPE-SAFE: More type-safe than C-style casts
4. EXPLICIT: Makes programmer's intent clear
5. REVERSIBLE: Can convert back and forth (e.g., int ↔ float)
6. NO POLYMORPHIC CHECKING: Doesn't verify actual object type

WHEN TO USE static_cast:
------------------------
1. Primitive type conversions (int ↔ float, char ↔ int)
2. Upcasting in class hierarchy (Derived* → Base*)
3. Downcasting when you're CERTAIN of the type (not recommended for runtime)
4. Converting void* to typed pointer
5. Explicit conversions that would otherwise be implicit
6. Enum to int conversions

WHEN NOT TO USE:
----------------
1. When you need runtime type checking (use dynamic_cast)
2. Removing const (use const_cast)
3. Unrelated pointer conversions (use reinterpret_cast)

===============================================================================
                            DYNAMIC_CAST
===============================================================================

DEFINITION:
-----------
dynamic_cast performs runtime type checking for polymorphic types. It safely
converts pointers/references within an inheritance hierarchy.

SYNTAX:
-------
    dynamic_cast<new_type>(expression)

PROPERTIES:
-----------
1. RUNTIME: Type checking happens at runtime
2. REQUIRES RTTI: Needs Runtime Type Information (enabled by default)
3. REQUIRES POLYMORPHISM: Works only with classes having virtual functions
4. SAFE DOWNCASTING: Returns nullptr (pointer) or throws exception (reference)
5. PERFORMANCE COST: Slower than static_cast due to runtime checking
6. TYPE-SAFE: Guarantees safe conversions

WHEN TO USE dynamic_cast:
-------------------------
1. Safe downcasting in polymorphic hierarchies
2. When actual object type is unknown at compile time
3. Cross-casting (casting across inheritance branches)
4. Checking object type at runtime

WHEN NOT TO USE:
----------------
1. Non-polymorphic types (will cause compile error)
2. Performance-critical code (prefer static_cast if type is known)
3. Primitive type conversions (use static_cast)

===============================================================================
*/

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>

using namespace std;

// ============================================================================
// DEMONSTRATION 1: static_cast - PRIMITIVE TYPE CONVERSIONS
// ============================================================================

void static_cast_primitives() {
    cout << "\n=== 1. static_cast - PRIMITIVE TYPE CONVERSIONS ===" << endl;
    
    // Integer to float
    int i = 10;
    float f = static_cast<float>(i);
    cout << "int to float: " << i << " -> " << f << endl;
    
    // Float to int (truncation)
    float pi = 3.14159f;
    int truncated = static_cast<int>(pi);
    cout << "float to int (truncated): " << pi << " -> " << truncated << endl;
    
    // Char to int (ASCII value)
    char ch = 'A';
    int ascii = static_cast<int>(ch);
    cout << "char to int (ASCII): '" << ch << "' -> " << ascii << endl;
    
    // Double to int
    double large = 123.456789;
    int i_large = static_cast<int>(large);
    cout << "double to int: " << large << " -> " << i_large << endl;
    
    // Enum to int
    enum Color { RED = 1, GREEN = 2, BLUE = 3 };
    Color c = GREEN;
    int color_value = static_cast<int>(c);
    cout << "enum to int: GREEN -> " << color_value << endl;
    
    // Int to enum (unsafe but allowed)
    int value = 3;
    Color c2 = static_cast<Color>(value);
    cout << "int to enum: " << value << " -> " << c2 << " (BLUE)" << endl;
    
    /*
     * KEY POINTS:
     * -----------
     * - static_cast makes conversions explicit and clear
     * - Better than C-style casts: (int)pi or int(pi)
     * - Compiler checks if conversion is valid
     * - No runtime overhead
     */
}

// ============================================================================
// DEMONSTRATION 2: static_cast - POINTER CONVERSIONS (void*)
// ============================================================================

void static_cast_void_pointers() {
    cout << "\n=== 2. static_cast - VOID POINTER CONVERSIONS ===" << endl;
    
    int value = 42;
    
    // Convert int* to void*
    void* void_ptr = static_cast<void*>(&value);
    cout << "Converted int* to void*" << endl;
    
    // Convert void* back to int*
    int* int_ptr = static_cast<int*>(void_ptr);
    cout << "Converted void* back to int*" << endl;
    cout << "Value: " << *int_ptr << endl;
    
    /*
     * IMPORTANT:
     * ----------
     * - void* loses type information
     * - Programmer must ensure correct type when casting back
     * - No runtime checking - if you cast to wrong type, undefined behavior!
     * 
     * EXAMPLE OF DANGER:
     * void* ptr = &some_int;
     * double* d = static_cast<double*>(ptr); // WRONG! But compiles!
     * // Using d will cause undefined behavior
     */
}

// ============================================================================
// DEMONSTRATION 3: INHERITANCE HIERARCHY FOR CASTING DEMOS
// ============================================================================

class Animal {
public:
    string name;
    
    Animal(string n) : name(n) {
        cout << "  [Animal] " << name << " created" << endl;
    }
    
    virtual ~Animal() {  // Virtual destructor for polymorphism
        cout << "  [Animal] " << name << " destroyed" << endl;
    }
    
    virtual void makeSound() const {
        cout << "  [Animal] " << name << " makes a generic sound" << endl;
    }
    
    virtual void eat() const {
        cout << "  [Animal] " << name << " is eating" << endl;
    }
};

class Dog : public Animal {
public:
    string breed;
    
    Dog(string n, string b) : Animal(n), breed(b) {
        cout << "  [Dog] Breed: " << breed << endl;
    }
    
    ~Dog() {
        cout << "  [Dog] " << name << " destroyed" << endl;
    }
    
    void makeSound() const override {
        cout << "  [Dog] " << name << " barks: Woof! Woof!" << endl;
    }
    
    void fetch() const {
        cout << "  [Dog] " << name << " is fetching the ball!" << endl;
    }
};

class Cat : public Animal {
public:
    bool isIndoor;
    
    Cat(string n, bool indoor) : Animal(n), isIndoor(indoor) {
        cout << "  [Cat] Indoor: " << (isIndoor ? "Yes" : "No") << endl;
    }
    
    ~Cat() {
        cout << "  [Cat] " << name << " destroyed" << endl;
    }
    
    void makeSound() const override {
        cout << "  [Cat] " << name << " meows: Meow! Meow!" << endl;
    }
    
    void scratch() const {
        cout << "  [Cat] " << name << " is scratching the furniture!" << endl;
    }
};

class Bird : public Animal {
public:
    double wingSpan;
    
    Bird(string n, double ws) : Animal(n), wingSpan(ws) {
        cout << "  [Bird] Wingspan: " << wingSpan << " meters" << endl;
    }
    
    ~Bird() {
        cout << "  [Bird] " << name << " destroyed" << endl;
    }
    
    void makeSound() const override {
        cout << "  [Bird] " << name << " chirps: Tweet! Tweet!" << endl;
    }
    
    void fly() const {
        cout << "  [Bird] " << name << " is flying!" << endl;
    }
};

// ============================================================================
// DEMONSTRATION 4: static_cast - UPCASTING (Derived → Base)
// ============================================================================

void static_cast_upcasting() {
    cout << "\n=== 3. static_cast - UPCASTING (Derived -> Base) ===" << endl;
    
    Dog* dog = new Dog("Buddy", "Golden Retriever");
    
    // Upcast: Dog* → Animal* (ALWAYS SAFE)
    Animal* animal = static_cast<Animal*>(dog);
    cout << "\nAfter upcasting Dog* to Animal*:" << endl;
    animal->makeSound();  // Calls Dog::makeSound() (polymorphism works!)
    
    // Note: Upcasting is implicit, so static_cast is optional here
    // Animal* animal2 = dog;  // This also works
    
    delete dog;  // Proper cleanup (virtual destructor ensures Dog destructor called)
    
    /*
     * KEY POINTS:
     * -----------
     * - Upcasting is ALWAYS SAFE (derived "is-a" base)
     * - static_cast is not necessary (implicit conversion)
     * - Polymorphism works correctly (virtual functions)
     * - No runtime overhead
     */
}

// ============================================================================
// DEMONSTRATION 5: static_cast - DOWNCASTING (UNSAFE!)
// ============================================================================

void static_cast_downcasting() {
    cout << "\n=== 4. static_cast - DOWNCASTING (Base -> Derived) - UNSAFE! ===" << endl;
    
    // SCENARIO 1: Actual object IS a Dog (downcast succeeds)
    cout << "\nScenario 1: Actual object IS a Dog" << endl;
    Animal* animal1 = new Dog("Max", "Labrador");
    
    // Downcast: Animal* → Dog* (RISKY with static_cast!)
    Dog* dog1 = static_cast<Dog*>(animal1);
    cout << "Downcast successful (actual object was Dog)" << endl;
    dog1->makeSound();
    dog1->fetch();  // Works because object really is a Dog
    
    delete animal1;
    
    // SCENARIO 2: Actual object is NOT a Dog (downcast FAILS - UNDEFINED BEHAVIOR!)
    cout << "\nScenario 2: Actual object is Cat (DANGER!)" << endl;
    Animal* animal2 = new Cat("Whiskers", true);
    
    // Downcast: Animal* → Dog* (WRONG! But compiles!)
    Dog* dog2 = static_cast<Dog*>(animal2);
    cout << "Downcast 'succeeded' but object is actually a Cat!" << endl;
    dog2->makeSound();  // May appear to work (calls Cat::makeSound via vtable)
    
    // dog2->fetch();  // UNDEFINED BEHAVIOR! Cat doesn't have fetch()
    // Accessing Dog-specific members on a Cat object = CRASH or corruption!
    cout << "Breed (garbage data): " << dog2->breed << endl;
    
    delete animal2;
    
    /*
     * WARNING:
     * --------
     * - static_cast does NO runtime checking for downcasts!
     * - If you're wrong about the type, UNDEFINED BEHAVIOR occurs
     * - Use dynamic_cast for safe downcasting!
     * 
     * WHEN is static_cast downcast OK?
     * - Only when you're 100% certain of the type
     * - In performance-critical code where type is guaranteed
     * - Generally: AVOID! Use dynamic_cast instead!
     */
}

// ============================================================================
// DEMONSTRATION 6: dynamic_cast - SAFE DOWNCASTING (Pointers)
// ============================================================================

void dynamic_cast_safe_downcasting() {
    cout << "\n=== 5. dynamic_cast - SAFE DOWNCASTING (Pointers) ===" << endl;
    
    // SCENARIO 1: Actual object IS a Dog (downcast succeeds)
    cout << "\nScenario 1: Actual object IS a Dog" << endl;
    Animal* animal1 = new Dog("Rocky", "Bulldog");
    
    // Safe downcast: Animal* → Dog* with runtime checking
    Dog* dog1 = dynamic_cast<Dog*>(animal1);
    
    if (dog1 != nullptr) {
        cout << "dynamic_cast succeeded! Object is indeed a Dog" << endl;
        dog1->makeSound();
        dog1->fetch();
    } else {
        cout << "dynamic_cast failed! Object is not a Dog" << endl;
    }
    
    delete animal1;
    
    // SCENARIO 2: Actual object is NOT a Dog (downcast fails safely)
    cout << "\nScenario 2: Actual object is Cat (safe failure)" << endl;
    Animal* animal2 = new Cat("Mittens", false);
    
    // Safe downcast: Animal* → Dog* (will fail safely)
    Dog* dog2 = dynamic_cast<Dog*>(animal2);
    
    if (dog2 != nullptr) {
        cout << "dynamic_cast succeeded! Object is a Dog" << endl;
        dog2->fetch();
    } else {
        cout << "dynamic_cast failed! Object is NOT a Dog (returned nullptr)" << endl;
        cout << "No undefined behavior - we can safely handle this!" << endl;
    }
    
    // Let's try casting to the correct type
    Cat* cat = dynamic_cast<Cat*>(animal2);
    if (cat != nullptr) {
        cout << "\nSuccessfully cast to Cat*:" << endl;
        cat->makeSound();
        cat->scratch();
    }
    
    delete animal2;
    
    /*
     * KEY BENEFITS of dynamic_cast:
     * ------------------------------
     * - Returns nullptr if cast fails (for pointers)
     * - No undefined behavior - you can check the result
     * - Safe for runtime type identification
     * - Essential for polymorphic programming
     * 
     * COST:
     * -----
     * - Runtime overhead (RTTI lookup in vtable)
     * - Requires virtual functions in class hierarchy
     * - Slightly slower than static_cast
     */
}

// ============================================================================
// DEMONSTRATION 7: dynamic_cast - WITH REFERENCES (Throws Exception)
// ============================================================================

void dynamic_cast_references() {
    cout << "\n=== 6. dynamic_cast - WITH REFERENCES (Throws Exception) ===" << endl;
    
    Dog dog("Charlie", "Beagle");
    Animal& animal_ref = dog;
    
    // SCENARIO 1: Successful cast with reference
    cout << "\nScenario 1: Valid downcast (Animal& -> Dog&)" << endl;
    try {
        Dog& dog_ref = dynamic_cast<Dog&>(animal_ref);
        cout << "dynamic_cast succeeded!" << endl;
        dog_ref.makeSound();
        dog_ref.fetch();
    } catch (const bad_cast& e) {
        cout << "dynamic_cast failed: " << e.what() << endl;
    }
    
    // SCENARIO 2: Failed cast with reference (throws exception)
    cout << "\nScenario 2: Invalid downcast (Animal& -> Cat&)" << endl;
    Cat cat("Shadow", true);
    Animal& animal_ref2 = cat;
    
    try {
        Dog& dog_ref2 = dynamic_cast<Dog&>(animal_ref2);  // Will throw!
        cout << "This won't print - exception thrown!" << endl;
        dog_ref2.fetch();
    } catch (const bad_cast& e) {
        cout << "dynamic_cast failed with exception: " << e.what() << endl;
        cout << "Caught and handled safely!" << endl;
    }
    
    /*
     * DIFFERENCE: Pointer vs Reference dynamic_cast
     * ----------------------------------------------
     * POINTER:
     *   - Returns nullptr on failure
     *   - Check with: if (ptr != nullptr)
     *   - No exception thrown
     * 
     * REFERENCE:
     *   - Throws std::bad_cast on failure
     *   - Must use try-catch to handle
     *   - Cannot return "null reference"
     */
}

// ============================================================================
// DEMONSTRATION 8: dynamic_cast - CROSS-CASTING
// ============================================================================

// Multiple inheritance hierarchy for cross-casting demo
class Flyable {
public:
    virtual ~Flyable() {}
    virtual void fly() const {
        cout << "  Flying in the air!" << endl;
    }
};

class Swimmable {
public:
    virtual ~Swimmable() {}
    virtual void swim() const {
        cout << "  Swimming in the water!" << endl;
    }
};

class Duck : public Animal, public Flyable, public Swimmable {
public:
    Duck(string n) : Animal(n) {
        cout << "  [Duck] Created" << endl;
    }
    
    ~Duck() {
        cout << "  [Duck] Destroyed" << endl;
    }
    
    void makeSound() const override {
        cout << "  [Duck] " << name << " quacks: Quack! Quack!" << endl;
    }
    
    void fly() const override {
        cout << "  [Duck] " << name << " is flying!" << endl;
    }
    
    void swim() const override {
        cout << "  [Duck] " << name << " is swimming!" << endl;
    }
};

void dynamic_cast_cross_casting() {
    cout << "\n=== 7. dynamic_cast - CROSS-CASTING (Multiple Inheritance) ===" << endl;
    
    Duck* duck = new Duck("Donald");
    
    // We have a Flyable pointer
    Flyable* flyable = duck;
    cout << "\nWe have a Flyable* pointer:" << endl;
    flyable->fly();
    
    // Cross-cast: Flyable* → Swimmable* (different inheritance branch!)
    cout << "\nCross-casting Flyable* to Swimmable*:" << endl;
    Swimmable* swimmable = dynamic_cast<Swimmable*>(flyable);
    
    if (swimmable != nullptr) {
        cout << "Cross-cast succeeded!" << endl;
        swimmable->swim();
    } else {
        cout << "Cross-cast failed!" << endl;
    }
    
    // Cross-cast: Swimmable* → Animal*
    cout << "\nCross-casting Swimmable* to Animal*:" << endl;
    Animal* animal = dynamic_cast<Animal*>(swimmable);
    
    if (animal != nullptr) {
        cout << "Cross-cast succeeded!" << endl;
        animal->makeSound();
    } else {
        cout << "Cross-cast failed!" << endl;
    }
    
    delete duck;
    
    /*
     * CROSS-CASTING:
     * --------------
     * - Casting between sibling classes in multiple inheritance
     * - Only possible with dynamic_cast (requires RTTI)
     * - static_cast cannot do this!
     * - Very useful in complex inheritance hierarchies
     */
}

// ============================================================================
// DEMONSTRATION 9: PERFORMANCE COMPARISON
// ============================================================================

void performance_comparison() {
    cout << "\n=== 8. PERFORMANCE COMPARISON ===" << endl;
    
    const int ITERATIONS = 1000000;
    
    Dog* dog = new Dog("Speedy", "Greyhound");
    Animal* animal = dog;
    
    // Measure static_cast performance
    cout << "\nTesting static_cast performance (" << ITERATIONS << " iterations)..." << endl;
    auto start1 = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ITERATIONS; ++i) {
        Dog* d = static_cast<Dog*>(animal);
        (void)d;  // Prevent optimization
    }
    
    auto end1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::microseconds>(end1 - start1).count();
    
    // Measure dynamic_cast performance
    cout << "Testing dynamic_cast performance (" << ITERATIONS << " iterations)..." << endl;
    auto start2 = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ITERATIONS; ++i) {
        Dog* d = dynamic_cast<Dog*>(animal);
        (void)d;  // Prevent optimization
    }
    
    auto end2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::microseconds>(end2 - start2).count();
    
    cout << "\nResults:" << endl;
    cout << "static_cast:  " << duration1 << " microseconds" << endl;
    cout << "dynamic_cast: " << duration2 << " microseconds" << endl;
    cout << "Difference:   " << (duration2 - duration1) << " microseconds" << endl;
    cout << "dynamic_cast is approximately " 
         << (float)duration2 / duration1 << "x slower" << endl;
    
    delete dog;
    
    /*
     * PERFORMANCE NOTES:
     * ------------------
     * - static_cast: Compile-time, zero runtime cost
     * - dynamic_cast: Runtime RTTI lookup, slight overhead
     * - In most applications, the safety is worth the cost
     * - Use static_cast only when performance is critical AND type is guaranteed
     */
}

// ============================================================================
// DEMONSTRATION 10: WHEN TO USE WHICH CAST
// ============================================================================

void casting_guidelines() {
    cout << "\n=== 9. CASTING GUIDELINES - WHEN TO USE WHICH ===" << endl;
    
    cout << "\nUSE static_cast WHEN:" << endl;
    cout << "1. Converting primitive types (int, float, char, etc.)" << endl;
    cout << "2. Upcasting (Derived* -> Base*) - though implicit is fine" << endl;
    cout << "3. You're 100% certain of the type (downcasting in controlled scenarios)" << endl;
    cout << "4. Converting void* to typed pointer (and you know the type)" << endl;
    cout << "5. Performance is critical and type is guaranteed by design" << endl;
    cout << "6. Converting between related types (enum <-> int)" << endl;
    
    cout << "\nUSE dynamic_cast WHEN:" << endl;
    cout << "1. Safe downcasting in polymorphic hierarchies" << endl;
    cout << "2. Type is unknown at compile time" << endl;
    cout << "3. You need to verify the type at runtime" << endl;
    cout << "4. Implementing type-safe polymorphic operations" << endl;
    cout << "5. Cross-casting in multiple inheritance" << endl;
    cout << "6. Safety is more important than performance" << endl;
    
    cout << "\nAVOID static_cast WHEN:" << endl;
    cout << "1. Downcasting and type is not guaranteed (use dynamic_cast)" << endl;
    cout << "2. Removing const (use const_cast)" << endl;
    cout << "3. Converting between unrelated pointer types (use reinterpret_cast)" << endl;
    
    cout << "\nAVOID dynamic_cast WHEN:" << endl;
    cout << "1. Class hierarchy is not polymorphic (no virtual functions)" << endl;
    cout << "2. Converting primitive types (use static_cast)" << endl;
    cout << "3. In extremely performance-critical code (consider design alternatives)" << endl;
}

// ============================================================================
// DEMONSTRATION 11: COMPARISON TABLE
// ============================================================================

void comparison_table() {
    cout << "\n=== 10. static_cast vs dynamic_cast - COMPARISON TABLE ===" << endl;
    
    cout << "\n┌─────────────────────────┬────────────────────────────┬────────────────────────────┐" << endl;
    cout << "│ FEATURE                 │ static_cast                │ dynamic_cast               │" << endl;
    cout << "├─────────────────────────┼────────────────────────────┼────────────────────────────┤" << endl;
    cout << "│ Timing                  │ Compile-time               │ Runtime                    │" << endl;
    cout << "│ Type Checking           │ Compile-time only          │ Runtime checking           │" << endl;
    cout << "│ Performance             │ Fast (no overhead)         │ Slower (RTTI lookup)       │" << endl;
    cout << "│ Safety                  │ Less safe (no runtime chk) │ Very safe                  │" << endl;
    cout << "│ Requires Polymorphism   │ No                         │ Yes (virtual functions)    │" << endl;
    cout << "│ Requires RTTI           │ No                         │ Yes                        │" << endl;
    cout << "│ Primitive Conversions   │ Yes                        │ No                         │" << endl;
    cout << "│ Upcasting               │ Yes                        │ Yes                        │" << endl;
    cout << "│ Safe Downcasting        │ No (undefined if wrong)    │ Yes (nullptr/exception)    │" << endl;
    cout << "│ Cross-casting           │ No                         │ Yes                        │" << endl;
    cout << "│ Pointer Failure Result  │ Undefined behavior         │ Returns nullptr            │" << endl;
    cout << "│ Reference Failure       │ Undefined behavior         │ Throws bad_cast            │" << endl;
    cout << "│ Use in Production       │ When type is certain       │ When type is uncertain     │" << endl;
    cout << "└─────────────────────────┴────────────────────────────┴────────────────────────────┘" << endl;
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    cout << "========================================" << endl;
    cout << "     C++ TYPE CASTING - COMPLETE GUIDE " << endl;
    cout << "========================================" << endl;
    
    static_cast_primitives();
    static_cast_void_pointers();
    static_cast_upcasting();
    static_cast_downcasting();
    dynamic_cast_safe_downcasting();
    dynamic_cast_references();
    dynamic_cast_cross_casting();
    performance_comparison();
    casting_guidelines();
    comparison_table();
    
    cout << "\n========================================" << endl;
    cout << "         Tutorial Complete!             " << endl;
    cout << "========================================" << endl;
    
    /*
     * FINAL SUMMARY:
     * ==============
     * 
     * STATIC_CAST:
     * ------------
     * - Compile-time type conversion
     * - Fast, no runtime overhead
     * - Use for: primitives, void*, upcasting, known-safe downcasts
     * - Risk: No runtime checking - wrong type = undefined behavior
     * 
     * DYNAMIC_CAST:
     * -------------
     * - Runtime type checking with RTTI
     * - Safe downcasting (returns nullptr or throws exception)
     * - Use for: unknown types, safe downcasting, cross-casting
     * - Cost: Slight performance overhead
     * 
     * GOLDEN RULE:
     * ------------
     * "Use static_cast when you KNOW the type is correct.
     *  Use dynamic_cast when you need to CHECK if the type is correct."
     * 
     * BEST PRACTICE:
     * --------------
     * 1. Prefer dynamic_cast for downcasting in polymorphic code
     * 2. Use static_cast for primitive conversions
     * 3. Design your code to minimize casting needs
     * 4. If you find yourself casting a lot, reconsider your design!
     * 
     * MODERN C++ ALTERNATIVE:
     * -----------------------
     * Consider using std::variant, std::any, or redesigning with
     * virtual functions instead of heavy casting.
     */
    
    return 0;
}
