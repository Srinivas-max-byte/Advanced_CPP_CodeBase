#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Demo {
public:
    int value;
    Demo(int v) : value(v) {
        cout << "Demo(" << value << ") constructed\n";
    }
    ~Demo() {
        cout << "Demo(" << value << ") destructed\n";
    }
    void show() const {
        cout << "Demo value = " << value << endl;
    }
};

int main() {
    cout << "--- 1. Basic unique_ptr operations ---" << endl;
    // Default construction (empty unique_ptr)
    unique_ptr<int> up1;
    cout << "up1 is " << (up1 ? "not null" : "null") << endl;

    // std::unique_ptr<Car> p2 = p1; // ❌ compile error (no copying)

    // Construction from raw pointer
    unique_ptr<int> up2(new int(42));
    cout << "up2 points to " << *up2 << endl;

    // make_unique construction (recommended, exception-safe)
    auto up3 = make_unique<int>(99);
    cout << "up3 points to " << *up3 << endl;

    // unique_ptr with custom type
    unique_ptr<Demo> up4 = make_unique<Demo>(123);
    up4->show();

    cout << "\n--- 2. Transferring Ownership (move semantics) ---" << endl;
    // unique_ptr cannot be copied
    // unique_ptr<int> copy_up2 = up2; // ERROR: Not allowed
    // unique_ptr<int> copy_up3(up3);  // ERROR: Not allowed

    // Ownership transfer using move constructor
    unique_ptr<int> up5 = std::move(up2);
    cout << "up2 is " << (up2 ? "not null" : "null") << " after move" << endl;
    cout << "up5 points to " << (up5 ? std::to_string(*up5) : "null") << endl;

    // Ownership transfer using move assignment
    unique_ptr<int> up6;
    up6 = std::move(up5);
    cout << "up5 is " << (up5 ? "not null" : "null") << " after move assignment" << endl;
    cout << "up6 points to " << (up6 ? std::to_string(*up6) : "null") << endl;

    // unique_ptr can be reset and released
    cout << "\n--- 3. reset() and release() ---" << endl;
    up3.reset(); // Deletes the owned object, up3 becomes null
    cout << "up3 is " << (up3 ? "not null" : "null") << " after reset" << endl;

    up6.reset(new int(888)); // Deletes previous & takes ownership of new int
    cout << "up6 now points to " << *up6 << endl;

    int* raw = up6.release(); // Releases ownership to caller
    cout << "After release, up6 is " << (up6 ? "not null" : "null") << endl;
    cout << "raw points to " << *raw << " (remember to delete it manually!)" << endl;
    delete raw;

    // Comparison with nullptr
    if (!up6) {
        cout << "up6 is null" << endl;
    }

    cout << "\n--- 4. unique_ptr in STL containers ---" << endl;
    vector<unique_ptr<Demo>> demos;
    demos.push_back(make_unique<Demo>(10));
    demos.push_back(make_unique<Demo>(20));
    demos.emplace_back(new Demo(30));
    //unique_ptr is move-only (not copyable). Hence Iterate by reference (no copy):
    for (const auto& ptr : demos) {
        ptr->show();
    }

    cout << "\n--- 5. Custom deleter with unique_ptr ---" << endl;
    auto custom_deleter = [](int* p) {
        cout << "Custom deleter called for int: " << *p << endl;
        delete p;
    };
    unique_ptr<int, decltype(custom_deleter)> up7(new int(555), custom_deleter);
    cout << "up7 points to " << *up7 << endl;

    cout << "\n--- 6. What is NOT allowed? ---" << endl;
    unique_ptr<int> up8 = make_unique<int>(77);
    // unique_ptr<int> up9 = up8; // ERROR: Copying not allowed
    // up8 = up7; // ERROR: Copy assignment not allowed (type differs)

    // Example of manual resource management with unique_ptr
    up8.reset(); // deletes the resource; up8 becomes null

    cout << "--- End of unique_ptr demonstration ---" << endl;
    return 0;
}
