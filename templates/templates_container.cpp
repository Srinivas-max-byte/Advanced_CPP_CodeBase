/*
 * Implement a code that calls a template function defined for outputing the sum of elements in a container (like std::vector).
 * Also, the contents of container can be of any numeric type (int, double, etc.) or user-defined type that supports addition or even strings.
 */

#include <iostream>
#include <vector>
#include <list>
#include <string>
using namespace std;

template <typename Container>
typename Container::value_type sum(const Container& c)
{
    // Prefer value-initialization with braces: it zero-initializes built-in numeric
    // types, calls the default constructor for user-defined types, and avoids the
    // "most vexing parse" that can happen with ()-style initialization. It is
    // also safe for std::string and other types that properly implement operator+.
    typename Container::value_type total{};
    // auto total = decltype(*std::begin(c))(); or
    // typename Container::value_type total = typename Container::value_type{};

    for (auto& elem : c)
    {
        total += elem; // Accumulate the sum
    }
    return total;
}


int main()
{
    vector<int> v = {1, 2, 3, 4, 5};
    list<int> l = {1, 2, 3, 4, 5};
    vector<string> sv = {"Hello, ", "world", "!"};
    
    std::cout << "Sum of vector<int>: " << sum(v) << std::endl;
    std::cout << "Sum of list<int>: " << sum(l) << std::endl;
    std::cout << "Sum of vector<string>: " << sum(sv) << std::endl;//Why this works? Because while passing string container, we already have strings not pointers.
    return 0;
}
