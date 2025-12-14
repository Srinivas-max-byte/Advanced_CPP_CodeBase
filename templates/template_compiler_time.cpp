
// template_compiler_time.cpp
// Simple example: use a template non-type parameter for compile-time allocation
// and then use that compile-time value at runtime.
//
// Key idea: non-type template parameters (e.g. template<std::size_t N>) fix
// sizes at compile time; those constexpr values can then be used at runtime
// to size dynamic containers or drive logic.
//
// Compile (PowerShell):
//   g++ -std=c++17 template_compiler_time.cpp -O2 -o template_compiler_time
//   .\template_compiler_time

#include <array>
#include <iostream>
#include <vector>

// A constexpr helper function — its result can be used as a template argument
constexpr std::size_t compute_size(int x) {
	return static_cast<std::size_t>(x) * 2u; // simple compile-time computation
}

// Buffer sized at compile time using a non-type template parameter
template <std::size_t N>
struct Buffer {
	static constexpr std::size_t size = N;
	std::array<int, N> data{}; // storage with compile-time size
};

int main() {
	// compute_size(4) is constexpr and therefore can be used as a template arg
	constexpr std::size_t compile_size = compute_size(4); // equals 8

	// Instantiate Buffer with a compile-time size
	Buffer<compile_size> buf;
	std::cout << "Compile-time buffer size: " << Buffer<compile_size>::size << "\n";

	// Use compile-time constant to size a runtime container
	std::vector<int> runtime_vec(Buffer<compile_size>::size, 42);
	std::cout << "Runtime vector size (from compile-time value): " << runtime_vec.size() << "\n";

	// You can also use the constexpr value directly at runtime
	std::size_t runtime_copy = compile_size; // constexpr -> runtime
	std::cout << "Runtime copy of compile-time value: " << runtime_copy << "\n";

	return 0;
}
