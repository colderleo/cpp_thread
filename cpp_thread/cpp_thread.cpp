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
	void first() {
		// 等待直至 main() 发送数据
		//std::unique_lock<std::mutex> lk(g_mutex);
		printf("print first.\n");
		// 通知前完成手动解锁，以避免等待线程才被唤醒就阻塞（细节见 notify_one ）
		counter++;
		cv1.notify_one();
	}

	void second() {
		std::unique_lock<std::mutex> lk(g_mutex);
		cv1.wait(lk, [this]() {return counter == 2; }); // 阻塞当前线程，直到条件变量被唤醒 
		printf("print second.\n");
		counter++;
		cv2.notify_one();
	}

	void third() {

		std::unique_lock<std::mutex> lk(g_mutex);
		cv2.wait(lk, [this]() {return counter == 3; });
		printf("print thrid.\n");
	}

};

mutex mutex1;

void test1() {
	unique_lock<mutex> lk(mutex1);
	for (int i = 0; i < 5; ++i) {
		this_thread::sleep_for(chrono::seconds(1));
		printf("%d\n", i);
	}
}

void test2() {
	unique_lock<mutex> lk(mutex1);
	
	for (int i = 0; i < 5; ++i) {
		this_thread::sleep_for(chrono::seconds(1));
		printf("%d\n", i);
	}
}


int main()
{
	thread t1(test1);
	thread t2(test1);
	t1.join();
	t2.join();
	return 0;

	//Foo f;
	//thread t3(&Foo::third, &f);
	//this_thread::sleep_for(chrono::seconds(1));
	//thread t2(&Foo::second, &f);
	//this_thread::sleep_for(chrono::seconds(1));
	//thread t1(&Foo::first, &f);
	//t2.join();
	//t1.join();
	//t3.join();
}

