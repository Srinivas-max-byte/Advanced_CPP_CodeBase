
// class_templates.cpp
// Basic, no-functionality C++ boilerplate intended to be used as a starting
// point for classes or small programs. This file does not implement any
// program-specific logic; it only provides a minimal, clean structure.
//
// Compile (PowerShell):
//   g++ -std=c++17 class_templates.cpp -O2 -o class_templates
//   .\class_templates
//

#include <iostream>
#include <string>
using namespace std;
// Example: Define small empty class to demonstrate a file that is ready to
// be extended. You can remove this if you don't need a class skeleton.
template <class T1, class T2>
class Pair {
private:
    T1 first;
    T2 second;
public:
	Pair(T1 a, T2 b) : first(a), second(b) {

    }
    void print() {
        //This breaks like cout<<"Pair(" cout<<"first, " cout<<second cout<<")\n";
        std::cout << "Pair(" << first << ", " << second << ")\n";
    }
};

//Only deifference between struct and class is that struct members are public by default whereas class members are private by default.
struct Person
{
    std::string name;
    int age;

    // Define operator+ to allow summing ages
    Person operator+(const Person& other) const
    {
        return Person{name + " & " + other.name, age + other.age};
    }

    // Here we are using friend function to overload the << operator for easy printing because if we don't use friend
    // then we can have only one parameter as we are inside the class scope and the overload will occur for this class.
    // When we use friend it basically tells compiler that operator<< of ostream is allowed to access members of Person.
    // As in cout<<person_object : theleft operand is ostream object and right operand is Person object which is equivalent to
    // operator<<(ostream_object, person_object). Hence to achieve this we use friend for passing two parameter.
    friend ostream& operator<<(ostream& os, const Person& p)
    {
        os << p.name << " (" << p.age << ")";
        return os;
    }
};

int main() {
	// No program functionality here — minimal boilerplate.
    Pair<int,int> p1(1,2);
    Pair<int,string> p2(1,"sudgif");
    Person person1{"Alice", 30};
    Person person2{"Bob", 25};
    person1 = person1 + person2; // Using operator+
    Pair<int, Person> p3(12, person1);

    p1.print();
    p2.print();
    p3.print();
	return 0;
}
