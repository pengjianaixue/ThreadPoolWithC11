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
�̳߳ص�˼·��
1������һ���̶߳��У��̶߳��ж�ȥִ��һ��Run ����
2������һ��������У�Run����ÿ�ζ�ȥ����������ȡһ���������ִ�У�ȡ����֮��Ͷ����������������е�������
   ����һ��
3������һ������ӿڣ��ṩ�û�������������������
4���ṩ�û���ֹ�̳߳صķ�����
*/
//��Ҫ���й��캯�������������Ķ�����ʹ��memset������memcpy��
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
	@bref:�ṩ���û��������Ľӿ�
	@Param: fn,����ĺ���ָ��
	@Param: arg,�����Ĳ������˴��������ͺ͸������ɱ䣻
	@retrun ���뺯���ķ���ֵ
	*/
	template <class T, class ...Arg>
	auto  InsertWork(T&& fn, Arg &&...arg)->std::future<decltype(fn(arg...))>
	{

		using RetType = decltype(fn(arg...)); // typename std::result_of<F(Args...)>::type, ���� f �ķ���ֵ����
		auto task = std::make_shared<std::packaged_task<RetType()> >(
			std::bind(std::forward <T>(fn), std::forward<Arg>(arg)...)); 
		std::future<RetType> returns = task->get_future();
	    {    // ������񵽶���
		     std::lock_guard<std::mutex> lock{ m_accessmutex };//�Ե�ǰ���������  lock_guard �� mutex �� stack ��װ�࣬�����ʱ�� lock()��������ʱ�� unlock()
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
