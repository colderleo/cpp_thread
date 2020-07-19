#pragma once 

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

mutex g1_mutex;
condition_variable cv;
int counter = 0;

void thread_fun1() {
	cout << "thread 1 start"<<endl;
	unique_lock<mutex> lck(g1_mutex, defer_lock);
	cv.wait(lck, []() {return (counter==1); });//这样写wait会lock，不知道为啥。然后就死在这里了。
	cout << "thread 1 end" << endl;

}
void thread_fun2() {
	cout << "thread 2 start" << endl;
	unique_lock<mutex> lck(g1_mutex, defer_lock);
	cv.wait(lck, []() {return (1); });
	counter += 1;
	cout << "thread 2 end" << endl;
}

void show_status() {
	for (int i = 0; i < 6; ++i) {
		cout <<"counter="<< counter << endl;
		//unique_lock<mutex> lck(g1_mutex);
		if (g1_mutex.try_lock()) {
			cout << "locak avalibale" << endl;
			g1_mutex.unlock();
		}
		else {
			cout << "lock unavaliable" << endl;
		}
		this_thread::sleep_for(chrono::seconds(1));
	}
}


//int test_cv_defer_lock() {
int main() {
	thread t1 = thread(thread_fun1);
	this_thread::sleep_for(chrono::seconds(1));
	thread t2 = thread(thread_fun2);

	thread watch = thread(show_status);

	cout << "test_cv_defer_lock"<<endl;
	t1.join();
	t2.join();
	watch.join();
	return 0;
}


