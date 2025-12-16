#include <iostream>
using namespace std;


template <typename T>
class SharedPtr 
{
    private:
        T* ptr_ = nullptr;
        int* count_ = nullptr;
        void (*deleter_)(T*) = nullptr;  // Custom deleter function pointer

        void release() {
            if (count_ != nullptr) {
                (*count_)--;
                if (*count_ == 0) {
                    if (deleter_ != nullptr) {
                        deleter_(ptr_);  // Use custom deleter if provided
                    } else {
                        delete ptr_;  // Use default delete
                    }
                    delete count_;
                    ptr_ = nullptr;
                    count_ = nullptr;
                    deleter_ = nullptr;
                }
            }
        }

    public:
        // Constructor with optional custom deleter
        SharedPtr(T* ptr = nullptr, void (*deleter)(T*) = nullptr) {
            if (ptr != nullptr) {
                count_ = new int(1);
                ptr_ = ptr;
                deleter_ = deleter;
            }
        }

        SharedPtr(const SharedPtr& other) {
            ptr_ = other.ptr_;
            count_ = other.count_;
            deleter_ = other.deleter_;
            if (count_ != nullptr) {
                (*count_)++;
            }
        }
        // We return SharedPtr& here so that chained assignments (like a = b = c) work correctly.
        // The compiler interprets this by using the returned reference for further assignment operations.
        // For example, in 'a = b = c;', 'b = c' returns 'b' by reference, so then 'a = (b = c)' becomes 'a = b', etc.
        // This ensures assignment can be chained and works as expected per C++ assignment semantics.

        SharedPtr& operator=(const SharedPtr& other) {
            if (this != &other) {
                release();
                ptr_ = other.ptr_;
                count_ = other.count_;
                deleter_ = other.deleter_;
                if (count_ != nullptr) {
                    (*count_)++;
                }
            }
            return *this;
        }

        ~SharedPtr() {
            release();
        }

        // We return T& here so that expressions like *p work just like a normal reference.
        // When you write *p, the compiler calls p.operator*(), which gives a T& referring
        // to the managed object. This works for any T (int, struct, class, etc.).
        T& operator*() const {
            return *ptr_;
        }

        // We return T* here so that expressions like p->member work.
        // The compiler rewrites p->member as p.operator->()->member:
        // 1) calls this operator->(), getting a raw T*
        // 2) then applies the built-in -> on that T* to access 'member' as T can be a struct or a class apart from "int".
        T* operator->() const {
            return ptr_;
        }

        int count() const {
            return (count_ == nullptr) ? 0 : *count_;
        }

        // Reset function: release current ownership
        void reset(T* ptr = nullptr, void (*deleter)(T*) = nullptr) {
            release();  // Release current ownership
            if (ptr != nullptr) {
                count_ = new int(1);
                ptr_ = ptr;
                deleter_ = deleter;
            } else {
                ptr_ = nullptr;
                count_ = nullptr;
                deleter_ = nullptr;
            }
        }

        // Swap function: exchange contents with another SharedPtr
        void swap(SharedPtr& other) {
            // Swap pointers
            T* temp_ptr = ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = temp_ptr;

            // Swap reference counts
            int* temp_count = count_;
            count_ = other.count_;
            other.count_ = temp_count;

            // Swap deleters
            void (*temp_deleter)(T*) = deleter_;
            deleter_ = other.deleter_;
            other.deleter_ = temp_deleter;
        }
};

int main() 
{
    // 1. Default construction (tests constructor with nullptr)
    SharedPtr<int> pDefault;
    std::cout << "pDefault count: " << pDefault.count() << std::endl;

    // 2. Construction from raw pointer (tests main constructor)
    SharedPtr<int> p1(new int(10));
    std::cout << "p1 count after creation: " << p1.count() << std::endl;
    std::cout << "Value via *p1: " << *p1 << std::endl;  // uses operator*

    // 3. Copy construction (tests copy constructor and count increment)
    SharedPtr<int> p2 = p1;  // copy constructor
    std::cout << "p1 count after p2 copy-construct: " << p1.count() << std::endl;
    std::cout << "p2 count after p2 copy-construct: " << p2.count() << std::endl;

    // 4. Copy assignment (tests operator= and proper ref-count handling)
    SharedPtr<int> p3;
    p3 = p1;  // copy assignment
    std::cout << "p1 count after assigning to p3: " << p1.count() << std::endl;
    std::cout << "p3 count after assigning to p3: " << p3.count() << std::endl;

    // 5. Modify through one pointer, observe via others (tests shared ownership)
    *p1 = 99;  // operator*
    std::cout << "Value via *p1: " << *p1 << std::endl;
    std::cout << "Value via *p2: " << *p2 << std::endl;
    std::cout << "Value via *p3: " << *p3 << std::endl;

    // 6. Use operator-> with a simple struct
    struct Point {
        int x;
        int y;
    };

    SharedPtr<Point> pPoint(new Point{1, 2});
    std::cout << "pPoint count after creation: " << pPoint.count() << std::endl;
    std::cout << "Point via operator->: (" << pPoint->x << ", " << pPoint->y << ")" << std::endl;

    // Modify through operator->
    pPoint->x = 10;
    pPoint->y = 20;
    std::cout << "Modified Point via operator->: (" << pPoint->x << ", " << pPoint->y << ")" << std::endl;

    // 7. Scope block to demonstrate destructor and ref-count decrement
    {
        SharedPtr<int> p4 = p1;  // another copy
        std::cout << "p1 count inside inner scope (with p4): " << p1.count() << std::endl;
    } // p4 goes out of scope here, destructor called, ref-count decremented

    std::cout << "p1 count after inner scope ends: " << p1.count() << std::endl;

    // 8. Reset functionality demonstration
    std::cout << "\n--- 8. RESET FUNCTIONALITY ---" << std::endl;
    
    SharedPtr<int> p5(new int(500));
    SharedPtr<int> p6 = p5;
    std::cout << "Before reset:" << std::endl;
    std::cout << "p5 count: " << p5.count() << std::endl;
    std::cout << "p6 count: " << p6.count() << std::endl;
    
    // Reset p5 (release its ownership)
    p5.reset();
    std::cout << "\nAfter p5.reset():" << std::endl;
    std::cout << "p5 count: " << p5.count() << std::endl;
    std::cout << "p6 count: " << p6.count() << std::endl;
    
    // Reset with new pointer
    p5.reset(new int(600));
    std::cout << "\nAfter p5.reset(new int(600)):" << std::endl;
    std::cout << "p5 count: " << p5.count() << std::endl;
    std::cout << "*p5 = " << *p5 << std::endl;

    // 9. Swap functionality demonstration
    std::cout << "\n--- 9. SWAP FUNCTIONALITY ---" << std::endl;
    
    SharedPtr<int> p7(new int(700));
    SharedPtr<int> p8(new int(800));
    std::cout << "Before swap:" << std::endl;
    std::cout << "*p7 = " << *p7 << ", *p8 = " << *p8 << std::endl;
    std::cout << "p7 count: " << p7.count() << ", p8 count: " << p8.count() << std::endl;
    
    p7.swap(p8);
    std::cout << "\nAfter p7.swap(p8):" << std::endl;
    std::cout << "*p7 = " << *p7 << ", *p8 = " << *p8 << std::endl;
    std::cout << "p7 count: " << p7.count() << ", p8 count: " << p8.count() << std::endl;

    // 10. Custom deleter functionality demonstration
    std::cout << "\n--- 10. CUSTOM DELETER FUNCTIONALITY ---" << std::endl;
    
    // Define a custom deleter function
    auto custom_deleter = [](int* p) {
        std::cout << "Custom deleter called! Deleting value: " << *p << std::endl;
        delete p;
    };
    
    SharedPtr<int> p9(new int(900), custom_deleter);
    std::cout << "p9 created with custom deleter, value: " << *p9 << std::endl;
    std::cout << "p9 count: " << p9.count() << std::endl;
    
    // Create a copy to test that deleter is shared
    SharedPtr<int> p10 = p9;
    std::cout << "p10 created as copy of p9, p9 count: " << p9.count() << std::endl;
    
    // When p9 and p10 go out of scope, custom deleter will be called once
    // (when the last reference is destroyed)
    
    // Reset with custom deleter
    SharedPtr<int> p11(new int(1000));
    std::cout << "\np11 created without custom deleter" << std::endl;
    p11.reset(new int(1100), custom_deleter);
    std::cout << "After p11.reset(new int(1100), custom_deleter):" << std::endl;
    std::cout << "*p11 = " << *p11 << std::endl;

    // When main returns, destructors for p1, p2, p3, pPoint, pDefault, p5, p6, p7, p8, p9, p10, p11 are called,
    // demonstrating automatic cleanup when the last reference goes away.
    // Custom deleters will be called for p9, p10, and p11.

    return 0;
}