#include<thread>
#include<mutex>
#include<condition_variable>
#include<iostream>
#include<vector>
using namespace std;


int cargo = 0;
mutex g_mutex;
condition_variable cv_consumer, cv_producer;
bool consumer_exit = false;

void consumer(int thread_index) {
	this_thread::sleep_for(chrono::milliseconds(1000));
	while (true) {
		this_thread::sleep_for(chrono::milliseconds(rand() % 300)); // simulate porcessing goods.
		unique_lock<mutex> lck(g_mutex); //acquire lock befor modify cargo.
		cv_consumer.wait(lck, []() {return (cargo > 0 || consumer_exit); });
		cout << thread_index <<" get lock" << endl;
		if (consumer_exit) {
			cout <<"thread "<<thread_index <<" exit" << endl;
			lck.unlock();
			cv_consumer.notify_one();
			break;
		}
		cargo--;
		cout <<"thread " << thread_index << " consume 1 goods, current goods: " << cargo << endl;
		lck.unlock(); //release lock to let other thread run.
		cout << thread_index << " release lock" <<endl;
		if (!cargo)
			cv_producer.notify_one();
		else
			cv_consumer.notify_one();
		this_thread::sleep_for(chrono::milliseconds(200)); // simulate porcessing goods.
	}
	//cout << "thread " << thread_index << " end" << endl;
}

void deamon() {
	// in case all threads are sleep and notify is ignored, periodly notify to wake up.
	while (!consumer_exit) {
		this_thread::sleep_for(chrono::seconds(3));
		cv_consumer.notify_one();
		cv_producer.notify_one();
		cout << "current goods : " << cargo << endl;
		if (g_mutex.try_lock()) {
			cout << "mutex is avaliable" << endl;
			g_mutex.unlock();
		}
		else {
			cout << "mutex is unavaliable" << endl;
		}
	}
	this_thread::sleep_for(chrono::seconds(3));
	cv_consumer.notify_one();
	cv_producer.notify_one();
}

int conditon_variable_produce_main() {
	vector<thread> vec;

	thread t_deamon = thread(deamon);

	for (int i = 0; i < 3; ++i) {
		thread t = thread(consumer, i);
		vec.emplace_back(move(t));
	}

	for (int i = 0; i < 2; ++i) {
		unique_lock<mutex> lck(g_mutex);
		cv_producer.wait(lck, []() {return (cargo == 0); });
		cout << "main get lock" << endl;
		cargo += 5;
		cout << "produce goods " << (i+1)*5 <<"  current goods: "<< cargo << endl;
		lck.unlock();
		cout << "main release lock" << endl;
		cv_consumer.notify_one();
		this_thread::sleep_for(chrono::milliseconds(rand() % 2000)); //simulate producing.
	}

	//unique_lock<mutex> lck(g_mutex, defer_lock);
	//cv_producer.wait(lck, []() {return (cargo == 0); });//这样写wait会lock，不知道为啥。然后就死在这里了。
	while (cargo > 0) {
		this_thread::sleep_for(chrono::seconds(1));
	}
	consumer_exit = true;
	cout << "produce end" << endl;
	cv_consumer.notify_one();

	for (auto &t : vec) {
		t.join();
	}
	t_deamon.join();
	
	cout << "main end" << endl;
	return 0;
}
