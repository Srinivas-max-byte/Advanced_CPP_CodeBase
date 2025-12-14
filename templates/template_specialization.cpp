// template_specialization.cpp
// Demonstrates the two main types of template specialization in C++:
// 1. Explicit (Full) Specialization
// 2. Partial Specialization
//

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ============================================================================
// PART 1: EXPLICIT (FULL) SPECIALIZATION
// ============================================================================
// Explicit specialization defines a completely concrete version of a template
// for specific template arguments. The compiler uses this instead of the
// generic template when the arguments match exactly.

// Primary (generic) template
template <typename T>
class Printer {
public:
    void print(const T& val) {
        cout << "Generic print: " << val << '\n';
    }
};

// Explicit specialization for const char* (C-style strings)
// This version has a completely different implementation than the generic.
template <>
class Printer<const char*> {
public:
    void print(const char* str) {
        cout << "Explicit specialization for const char*: \"" << str << "\"\n";
    }
};

// Explicit specialization for int
template <>
class Printer<int> {
public:
    void print(const int& val1, const int& val2) {
        cout << "Explicit specialization for int: value is " << val1 << val2 << " (int type)\n";
    }
};

// ============================================================================
// EXPLICIT (FULL) SPECIALIZATION WITH TWO TEMPLATE TYPES
// ============================================================================
// Unlike partial specialization, explicit specialization completely resolves
// all template parameters. The template <> is empty, and both types are concrete.

// Explicit specialization for (int, string)
template <>
class Pair<int, int> {
public:
    void describe(int a, int b) {
        cout << a << " " << b << endl;
    }
};

// Explicit specialization for (double, double)
template <>
class Pair<double, double> {
public:
    void describe() {
        cout << "Explicit specialization: (double, double) — both floats, special handling\n";
    }
};

// Explicit specialization for (int*, double*)
template <>
class Pair<int*, double*> {
public:
    void describe() {
        cout << "Explicit specialization: (int*, double*) — two specific pointer types\n";
    }
};

// ============================================================================
// PART 2: PARTIAL SPECIALIZATION
// ============================================================================
// Partial specialization refines a template for a subset of possible template
// arguments. For example, specializing for pointer types, or container types,
// or pairs of types. The template still has unresolved type parameters.

// Primary (generic) template for a pair of types
template <typename T, typename U>
class Pair {
public:
    void describe() {
        cout << "Generic Pair template (two unrelated types)\n";
    }
};

// Partial specialization: both types are the same
// template <typename T>
// class Pair<T, T> {
// public:
//     void describe() {
//         cout << "Partial specialization: both types are identical (T, T)\n";
//     }
// };

// Partial specialization: second type is pointer to first
template <typename T>
class Pair<T, T*> {
public:
    void describe() {
        cout << "Partial specialization: second is pointer to first (T, T*)\n";
    }
};

// Partial specialization: both types are pointers
template <typename T, typename U>
class Pair<T*, U*> {
public:
    void describe() {
        cout << "Partial specialization: both are pointers (T*, U*)\n";
    }
};

// Partial specialization: second is a vector of first
template <typename T>
class Pair<T, vector<T>> {
public:
    void describe() {
        cout << "Partial specialization: second is vector of first (T, vector<T>)\n";
    }
};


int main() {
    cout << "=== EXPLICIT (FULL) SPECIALIZATION ===" << '\n';
    
    Printer<double> p1;
    p1.print(3.14);  // Uses generic template
    
    Printer<int> p2;
    p2.print(42,30);  // Uses explicit specialization for int
    
    Printer<const char*> p3;
    p3.print("Hello");  // Uses explicit specialization for const char*
    
    cout << "\n=== PARTIAL SPECIALIZATION ===" << '\n';
    
    // Generic: two different types
    Pair<int, string> pair1;
    pair1.describe();
    
    // Partial: both types are the same
    Pair<int, int> pair2;
    pair2.describe(1,2);
    
    // Partial: second is pointer to first
    Pair<int, int*> pair3;
    pair3.describe();
    
    // Partial: both are pointers
    Pair<double*, int*> pair4;
    pair4.describe();
    
    // Partial: second is vector of first
    Pair<int, vector<int>> pair5;
    pair5.describe();
    
    cout << "\n=== EXPLICIT TWO-TYPE SPECIALIZATION ===" << '\n';
    
    // These use explicit (not partial) specializations
    Pair<int, int> explicit1;
    explicit1.describe(3,4);  // Explicit specialization (int, string)
    
    Pair<double, double> explicit2;
    explicit2.describe();  // Explicit specialization (double, double)
    
    Pair<int*, double*> explicit3;
    explicit3.describe();  // Explicit specialization (int*, double*)
    
    cout << "\n=== KEY DIFFERENCES ===" << '\n';
    cout << "Explicit: specific concrete version for exact template args (template <> class C<T1, T2>)\n";
    cout << "Partial:  refinement for pattern matching (still has templates: template <typename T> class C<...>)\n";
    
    return 0;
}
