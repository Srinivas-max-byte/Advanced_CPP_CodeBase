#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

class BoundedBuffer{
	private:
		queue<int> buffer;
		int capacity;

		mutex mtx;
		condition_variable cv_producer;
		condition_variable cv_consumer;

	public:
		BoundedBuffer(int cap) : capacity(cap) {}

		void produce(int item) {
			mtx.lock();
			cv_producer.wait(mtx, [this] {return buffer.size() < capacity; });
			buffer.push(item);
			mtx.unlock();
			cv_consumer.notify_one();
		}

		void consume() {
			mtx.lock();
			cv_consumer.wait(mtx, [this] {return !buffer.empty(); });
			int item = buffer.front();
			buffer.pop();
			mtx.unlock();
			cv_producer.notify_one();
		}
};

void producer(BoundedBuffer& buffer) {
	for (int i = 0; i < 10; ++i) {
		buffer.produce(i);
		cout << "Produced: " << i << endl;
	}
}

void consumer(BoundedBuffer& buffer) {
	for (int i = 0; i < 10; ++i) {
		buffer.consume();
		cout << "Consumed: " << i << endl;
	}
}

int main() {
	BoundedBuffer buffer(5);
	thread prod(producer, ref(buffer));
	thread cons(consumer, ref(buffer));
	prod.join();
	cons.join();
	return 0;
}