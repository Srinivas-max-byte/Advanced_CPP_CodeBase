#include <iostream>
using namespace std;


template <typename T>
class SharedPtr 
{
    private:
        T* ptr_ = nullptr;
        int* count_ = nullptr;

        void release() {
            if (count_ != nullptr) {
                (*count_)--;
                if (*count_ == 0) {
                    delete ptr_;
                    delete count_;
                    ptr_ = nullptr;
                    count_ = nullptr;
                }
            }
        }

    public:
        SharedPtr(T* ptr = nullptr) {
            if (ptr != nullptr) {
                count_ = new int(1);
                ptr_ = ptr;
            }
        }

        SharedPtr(const SharedPtr& other) {
            ptr_ = other.ptr_;
            count_ = other.count_;
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

    // When main returns, destructors for p1, p2, p3, pPoint, pDefault are called,
    // demonstrating automatic cleanup when the last reference goes away.

    return 0;
}