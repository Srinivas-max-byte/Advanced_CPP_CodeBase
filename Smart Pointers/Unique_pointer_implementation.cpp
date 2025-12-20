#include <iostream>

// Minimal implementation of move semantics
template <typename T>
class my_unique_ptr {
    T* ptr;

    // Non-copyable: We explicitly delete copy constructor and copy assignment operator
    // This enforces unique ownership - prevents copying to ensure only one unique_ptr owns the resource
    // Copy constructor is deleted to prevent: my_unique_ptr<int> ptr2 = ptr1;
    my_unique_ptr(const my_unique_ptr& other) = delete;
    // Copy assignment operator is deleted to prevent: ptr2 = ptr1;
    my_unique_ptr& operator=(const my_unique_ptr& other) = delete;

public:
    // Default constructor
    my_unique_ptr() : ptr(nullptr) {}

    // Constructor from raw pointer.
    // Marked explicit to prevent implicit conversion from raw T* to my_unique_ptr<T>,
    // which could accidentally transfer ownership and lead to double-delete or logic bugs.
    explicit my_unique_ptr(T* p) : ptr(p) {}

    // Move constructor
    my_unique_ptr(my_unique_ptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    // Move assignment
    my_unique_ptr& operator=(my_unique_ptr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Destructor
    ~my_unique_ptr() {
        delete ptr;
    }

    // Dereference operators
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    // Bool operator
    operator bool() const { return ptr != nullptr; }

    // Release ownership
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // Reset pointer
    void reset(T* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }

    // Swap with another unique_ptr
    void swap(my_unique_ptr& other) {
        T* temp = ptr;
        ptr = other.ptr;
        other.ptr = temp;
    }
};

// Helper function for make_unique equivalent
// typename... Args: Variadic template (ellipsis) allows this function to accept any number of arguments
//                    of any types. This makes my_make_unique flexible - works with constructors that take
//                    0, 1, 2, or more parameters. Example: my_make_unique<Foo>() or my_make_unique<Foo>(10, "hello")
template <typename T, typename... Args>
// Args&&... args: Universal/forwarding references (r-value references in template context).
//                 The && here doesn't mean "r-value only" - it's a forwarding reference that preserves
//                 the value category (l-value or r-value) of the arguments passed in. This allows
//                 perfect forwarding - we can forward both temporary objects (r-values) and named
//                 variables (l-values) without unnecessary copies.
my_unique_ptr<T> my_make_unique(Args&&... args) {
    // std::forward<Args>(args)...: Perfect forwarding - forwards each argument with its original value category.
    //                               If an l-value was passed, it forwards as l-value; if r-value, forwards as r-value.
    //                               This prevents unnecessary copies and enables move semantics when possible.
    //                               The ... expands the pack, applying forward to each argument individually.
    return my_unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Demo usage
struct Foo {
    int x;
    Foo(int v) : x(v) { std::cout << "Foo(" << x << ") created\n"; }
    ~Foo() { std::cout << "Foo(" << x << ") destroyed\n"; }
    void show() const { std::cout << "Foo: " << x << std::endl; }
};

int main() {
    std::cout << "=== my_unique_ptr demo ===\n";

    my_unique_ptr<int> up1;
    std::cout << "up1 is " << (up1 ? "not null" : "null") << std::endl;
    my_unique_ptr<int> up2(new int(42));
    std::cout << "*up2 = " << *up2 << std::endl;
    
    my_unique_ptr<int> up3 = my_make_unique<int>(1234);
    std::cout << "*up3 = " << *up3 << std::endl;

    my_unique_ptr<Foo> up4 = my_make_unique<Foo>(99);
    up4->show();

    std::cout << "--- Move ownership ---\n";
    my_unique_ptr<Foo> up5 = std::move(up4);
    if (!up4) std::cout << "up4 is null after move\n";
    up5->show();

    std::cout << "--- Release pointer ---\n";
    Foo* raw = up5.release();
    if (!up5) std::cout << "up5 is null after release\n";
    raw->show();
    delete raw;

    std::cout << "--- Resetting ---\n";
    my_unique_ptr<int> up6(new int(88));
    up6.reset(new int(999));
    std::cout << "*up6 = " << *up6 << std::endl;
    up6.reset();
    std::cout << "up6 after reset is " << (up6 ? "not null" : "null") << std::endl;

    std::cout << "--- End of demo ---\n";
    return 0;
}
