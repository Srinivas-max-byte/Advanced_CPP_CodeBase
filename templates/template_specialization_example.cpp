#include "iostream"
#include "vector"
#include "string"
using namespace std;

template <typename T>
class DynamicArray {
    private:
        vector<T> data;
    public:
        void add(const T& value) {
            data.push_back(value);
        }
        void print() const {
            for (const auto& item : data) {
                cout << item << " ";
            }
            cout << endl;
        }
};

template <>
class DynamicArray<char> {
    private:
        string data; // Use string for char specialization
    public:
        void add(const char& value) {
            data += value; // Append char to string
        }
        void print() const {
            cout << data << endl; // Print the entire string
        }
};

int main() {
    DynamicArray<int> intArray;
    intArray.add(1);
    intArray.add(2);
    intArray.add(3);
    cout << "Integer Array: ";
    intArray.print();

    DynamicArray<string> stringArray;
    stringArray.add("Hello");
    stringArray.add("World");
    cout << "String Array: ";
    stringArray.print();

    DynamicArray<char> charArray;
    charArray.add('H');
    charArray.add('e');
    charArray.add('l');
    charArray.add('l');
    charArray.add('o');
    cout << "Character Array (as string): ";
    charArray.print();

    return 0;
}