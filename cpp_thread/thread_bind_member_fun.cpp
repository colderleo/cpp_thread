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

	//����һ������������ָ�롣
	thread t1(&X::printSomething, &x, "aaa");
	t1.join();

	//��bind����һ���º�������������ָ��󶨵����һ�������ϡ�
	auto fun_x_print = bind(&X::printSomething, &x, placeholders::_1);
	thread t2(fun_x_print, "bbb");
	t2.join();
	return 0;
}


