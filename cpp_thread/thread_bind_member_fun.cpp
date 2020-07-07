#include <thread>
#include <iostream>
#include <string>
using namespace std;


class X {
public:
	void printSomething(string str) {
		cout<< str;
	}
};

int rename_main() {
	X x;

	//方法一：传入类对象的指针。
	thread t1(&X::printSomething, &x, "aaa");
	t1.join();

	//用bind创建一个新函数，将类对象的指针绑定到其第一个参数上。
	auto fun_x_print = bind(&X::printSomething, &x, placeholders::_1);
	thread t2(fun_x_print, "bbb");
	t2.join();
	return 0;
}


