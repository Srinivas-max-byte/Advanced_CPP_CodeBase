#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;


long long getSingleThreadResult(vector<int>& data) {
	long long result = 0;
	for (int i = 0; i < data.size(); ++i) {
		result += data[i];
	}
	return result;
}

long long getMultiThreadResult(vector<int>& data, int numThreads) {
	vector<thread> threads;
	vector<long long> partialResults(numThreads, 0LL);
	long long chunkSize = data.size() / numThreads;
	for (int t = 0; t < numThreads; ++t) {
		// emplace_back() constructs the thread object in-place inside the vector,
		// avoiding an unnecessary temporary copy. It directly creates the std::thread
		// with the given callable (lambda) and arguments in the vector's memory.
		// This is more efficient than creating a thread object separately and then
		// push_back()'ing it, which would involve a temporary object being moved.
		// Why we use it:
		// - Avoids temporary object creation
		// - More efficient than push_back (which would move the temporary thread)
		// - Cleaner syntax for constructing complex objects in containers
		// Alternative: threads.push_back(thread([t, chunkSize, &partialResults]() { ... }));
		// Alternative with std::move() for rvalue semantics:
		// thread worker([t, chunkSize, &partialResults, &data, numThreads]() { /* work */ });
		// threads.push_back(move(worker)); // std::move() converts lvalue 'worker' to rvalue, enabling efficient move semantics
		threads.emplace_back([t, chunkSize, &partialResults, &data, numThreads]() {
			int startIdx = t * chunkSize;
			int endIdx = (t == numThreads - 1) ? data.size() : startIdx + chunkSize;
			long long localSum = 0;
			for (int i = startIdx; i < endIdx; ++i) {
				localSum += data[i]; // since all elements are 1
			}
			partialResults[t] = localSum;
		});
	}
	for (auto& th : threads) {
		th.join();
	}
	long long totalResult = 0;
	for (const auto& partial : partialResults) {
		totalResult += partial;
	}
	return totalResult;
}

int main() {
	vector<int> data(10000000, 1);

	const int NUM_THREADS = thread::hardware_concurrency();

	auto start = chrono::high_resolution_clock::now();
	long long res = getSingleThreadResult(data);
	auto end = chrono::high_resolution_clock::now();

	cout << "Single thread result: " << res << "\n";
	cout << "Time taken (single thread): "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< " ms\n";

	start = chrono::high_resolution_clock::now();
	res = getMultiThreadResult(data, NUM_THREADS);
	end = chrono::high_resolution_clock::now();

	cout << "Multi thread result: " << res << "\n";
	cout << "Time taken (multi thread): "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< " ms\n";

	return 0;
}