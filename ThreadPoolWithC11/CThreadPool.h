#pragma once
#ifndef C_THREAD_POOL_H_
#define C_THREAD_POOL_H_
#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>
#include <iostream>
#include <assert.h>
#include <atomic>
#include <fstream>
#include <sstream>
#include <functional>
#include <condition_variable>
#include <future>
#include <queue>
#include <deque>
/* 
线程池的思路：
1：创建一个线程队列，线程队列都去执行一个Run 方法
2：定义一个任务队列，Run方法每次都去这个任务队列取一个任务出来执行，取出来之后就丢掉这个任务，任务队列的任务数
   减少一；
3：创建一个任务接口：提供用户给项任务队列添加任务
4：提供用户终止线程池的方法；
*/
//不要在有构造函数和析构函数的对象上使用memset（或者memcpy）
typedef struct ThreadInfo
{
	std::thread* WorkThread;
	bool  Isbusy;

} THREADINFO ;

typedef std::map < std::string, THREADINFO>  THREAD_MAP;

class CThreadPool
{	
public:
	CThreadPool(size_t ThreadNum = 100);
	virtual ~CThreadPool();
	int  GetThreadNum() const;
	bool IsVaild() const;
	bool ColseThreadPool();
private:
	int Run();
	int IncreaseThread();
	
private:
	bool m_bVailble;
	std::vector<std::thread*>  m_ThreadList;
	std::atomic<bool> m_IsClose;
	std::queue<std::function<void(void)>> m_Task_List;
	std::atomic<int> m_TaskNum;
	std::atomic<int> m_VislbleNum;
	std::mutex m_accessmutex;
	std::mutex m_runaccesmt;
	std::condition_variable _Con;
public:
	/*
	@bref:提供给用户添加任务的接口
	@Param: fn,传入的函数指针
	@Param: arg,函数的参数，此处参数类型和个数都可变；
	@retrun 传入函数的返回值
	*/
	template <class T, class ...Arg>
	auto  InsertWork(T&& fn, Arg &&...arg)->std::future<decltype(fn(arg...))>
	{

		using RetType = decltype(fn(arg...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
		auto task = std::make_shared<std::packaged_task<RetType()> >(
			std::bind(std::forward <T>(fn), std::forward<Arg>(arg)...)); 
		std::future<RetType> returns = task->get_future();
	    {    // 添加任务到队列
		     std::lock_guard<std::mutex> lock{ m_accessmutex };//对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
			 m_Task_List.emplace([task]() {(*task)(); });
		 }
		_Con.notify_one();
		m_TaskNum++;
		if (m_TaskNum>m_VislbleNum)
		{
			IncreaseThread();

		}
		
		return returns;

	}
};
#endif // !C_THREAD_POOL_H_
