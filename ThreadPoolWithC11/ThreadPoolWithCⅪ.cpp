// ThreadPoolWithC11.cpp : 定义控制台应用程序的入口点。
//
//线程池测试程序

#include "stdafx.h"
#include "CThreadPool.h"
#include <crtdbg.h>
int _stdcall add(int  x,int y )
{
	std::cout << x + y << std::endl;
	return x + y;


}
int _stdcall Returns(int x,std::string s)
{	
	std::cout << x <<s<< std::endl;
	return x;
} 
int _stdcall IncreseInt(int y)
{
	std::cout << y <<std::endl;
	return ++y;


}
int main()
{
	//int i, j;
	CThreadPool *Test;
	Test = new CThreadPool;
	Test->InsertWork(add,1,2);
	Test->InsertWork(Returns,1,"string");
	Test->InsertWork(IncreseInt,5);
	system("pause");
	std::cout << Test->GetThreadNum() << std::endl;
	Test->ColseThreadPool();
	delete Test;
	std::cout << "运行结束，释放资源已经完毕" << std::endl;
	system("pause");
	_CrtDumpMemoryLeaks();//内存泄漏检测
	return 0;
}
