#include <iostream>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;


class Foo {
private:
	int counter = 1;
	std::condition_variable cv1;
	std::condition_variable cv2;
	// 使用lock和unlock手动加锁
	std::mutex g_mutex;

public:
	Foo() {}

public:
	void first(function<void()> printFirst) {
		// 等待直至 main() 发送数据
		std::unique_lock<std::mutex> lk(g_mutex);
		// printFirst() outputs "first". Do not change or remove this line.
		printFirst();
		// 通知前完成手动解锁，以避免等待线程才被唤醒就阻塞（细节见 notify_one ）
		counter++;
		cv1.notify_one();
	}

	void second(function<void()> printSecond) {
		std::unique_lock<std::mutex> lk(g_mutex);
		cv1.wait(lk, [this]() {return counter == 2; }); // 阻塞当前线程，直到条件变量被唤醒 
		// printSecond() outputs "second". Do not change or remove this line.
		printSecond();
		counter++;
		cv2.notify_one();
	}

	void third(function<void()> printThird) {

		std::unique_lock<std::mutex> lk(g_mutex);
		cv2.wait(lk, [this]() {return counter == 3; });
		// printThird() outputs "third". Do not change or remove this line.
		printThird();
	}

};


void printFirst() {
	printf("print first.\n");
}

void printSecond() {
	printf("print second.\n");
}

void printThird() {
	printf("print thrid.\n");
}


int main()
{
	string name;

	Foo f;
	thread t2(&Foo::second, &f, printSecond);
	this_thread::sleep_for(chrono::seconds(3));
	thread t1(&Foo::first, &f, printFirst);
	thread t3(&Foo::third, &f, printThird);
	t2.join();
	t1.join();
	t3.join();
}

