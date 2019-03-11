#include "stdafx.h"
#include "CThreadPool.h"

/*
@bref:构造函数
@Param: ThreadNum，创建的线程数
@retrun: NULL
*/
CThreadPool::CThreadPool(size_t ThreadNum)
{
	m_bVailble = false;
	m_TaskNum = 0;
	m_IsClose = false;
	const int  FunctionNum = ThreadNum ;
	try
	{	
		int ProcessorsNum;
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		ProcessorsNum = systemInfo.dwNumberOfProcessors;
		for (size_t i = 0; i < static_cast<size_t> (ProcessorsNum); ++i)
		{

			std::thread *t1 = new  std::thread(&CThreadPool::Run,this);

			m_ThreadList.push_back(t1);
		}
		m_VislbleNum = ProcessorsNum;
		
	}
	catch (const std::exception& ThreadCreate)
	{	
		std::cout << ThreadCreate.what() << std::endl;
		for (auto &FreeList : m_ThreadList)
		{	

			if (FreeList->joinable())
			{
				FreeList->join();
			}
			else
			{
				FreeList->~thread();

			}
			if (FreeList  != nullptr)
			{
				delete FreeList;

			}
		}
		
		
	}
	
	
}/*
	@bref:析构函数，释放线程资源
	@Param: 
	@retrun 
	*/
CThreadPool::~CThreadPool()
{
	for (auto &FreeList : m_ThreadList)
	{

		if (FreeList->joinable())
		{
			FreeList->join();
		}
		else
		{
			FreeList->~thread();

		}
		if (FreeList != nullptr)
		{
			delete FreeList;

		}
	}

	
}
/*
@bref:保留函数，未实现
@Param: 
@Param: 
@retrun 
*/
int CThreadPool::GetThreadNum() const 
{	
	
	return m_VislbleNum;
}

inline bool CThreadPool::IsVaild() const
{
	
	return m_bVailble;
}
/*
@bref:关闭线程池(先运行这个函数，再析构类)
@Param: 
@Param: 
@retrun 
*/
bool CThreadPool::ColseThreadPool()
{	
	m_IsClose = true;
	return m_IsClose;
}

int CThreadPool::Run()
{	
	while (!m_IsClose)
	{
		m_runaccesmt.lock();
		std::function<void(void)> task;
		if (!m_Task_List.empty())
		{
#ifdef _DEBUG
			std::cout << "Tast" << m_Task_List.size() << std::endl;
#endif // DEBUG
			task = std::move(m_Task_List.front());
			task();
			m_Task_List.pop();
			m_VislbleNum--;
		}
		else
		{
			Sleep(100);
		}
		m_runaccesmt.unlock();

	}
	return 0;
}

int CThreadPool::IncreaseThread()
{	
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int ProcessorsNum = systemInfo.dwNumberOfProcessors;
	for (size_t i = 0; i < static_cast<size_t> (ProcessorsNum); ++i)
	{
		
		std::thread *t1 = new  std::thread(&CThreadPool::Run, this);
		m_ThreadList.push_back(t1);
	}
	m_VislbleNum += ProcessorsNum;
	return 0;
}

