/*
	Trailing Return Type Syntax in C++ Templates
	This example demonstrates how to use trailing return type syntax in a function template.
*/

#include <iostream>
#include <string>

using namespace std;

// According to C++11 and later standards, you can use trailing return type syntax
// to specify the return type of a function template based on its parameters.
template <class T1, class T2>
auto add1(T1 a, T2 b) -> decltype(a + b) // trailing return type syntax
{
	return a + b;
}

// According to C++17 standard, we can directly use 'auto' as the return type without needing 'decltype' here.
template <class T1, class T2>
auto add2(T1 a, T2 b)
{
	return a + b;
}

//Template specialization


int main()
{
	// Minimal demo: call `add1()` with several type variations (trailing-return-type),
	// then call `add2()` (auto return) with the same set so you can compare both.
	std::cout << "add1(int, int)       -> " << add1(3, 4) << '\n';
	std::cout << "add1(double, double) -> " << add1(2.5, 3.75) << '\n';
	std::cout << "add1(int, double)    -> " << add1(3, 4.5) << '\n';

	// std::string concatenation works because operator+ is defined for strings
	std::string a = "Hello, ";
	std::string b = "world!";
	std::cout << "add1(string, string) -> " << add1(a, b) << '\n';

	// explicit template arguments (unnecessary here, but valid)
	std::cout << "add1<int,double>(3, 4.5) -> " << add1<int, double>(3, 4.5) << '\n';

	// Repeat same examples using add2 (auto return type)
	std::cout << "add2(int, int)       -> " << add2(3, 4) << '\n';
	std::cout << "add2(double, double) -> " << add2(2.5, 3.75) << '\n';
	std::cout << "add2(int, double)    -> " << add2(3, 4.5) << '\n';
	std::cout << "add2(string, string) -> " << add2(a, b) << '\n';
	std::cout << "add2<int,double>(3, 4.5) -> " << add2<int, double>(3, 4.5) << '\n';
	return 0;
}

/*
 * Follow up questions:
 * Suppose we need to ship the library to custoer we will include only header file and compiled code to the customer,
 * as we want to hide the implementation details of the `add` function. How can we achieve this in C++ with templates
 * as it needs to be recompiled for each type used?
 *
 * Answer:
 * In C++, templates are typically defined in header files because the compiler needs to see the template definition
 * to generate code for each type used. However, if you want to hide the implementation details, you can use the following approaches:
 * 1. Explicit Instantiation: You can explicitly instantiate the template for the specific types you want to support in a source file (.cpp).
 *    This way, you can compile the source file and provide only the compiled object code to the customer, while keeping the template definition hidden.
 * 2. Pimpl Idiom: You can use the Pointer to Implementation (Pimpl) idiom to hide the implementation details behind a pointer to an opaque type.
 *    This way, the customer only sees the interface and not the implementation.
 * 3. Type Erasure: You can use type erasure techniques to create a non-template interface that internally uses templates. This way, the customer interacts with a non-template interface, while the implementation can use templates internally.
 * 4. Precompiled Headers: You can precompile the template code for specific types and provide the precompiled headers to the customer. This way, the customer can use the precompiled code without needing to see the template definition.
 * Each of these approaches has its own trade-offs and considerations, so the choice depends on the specific requirements and constraints of your project.
 * 5. Obfuscation Tools: There are tools available that can obfuscate C++ code, making it difficult to read while still being compilable. This can be used to hide implementation details in header files.
 *
 */
