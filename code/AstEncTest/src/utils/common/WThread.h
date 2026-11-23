#ifndef _BASE_TIMER_H_
#define _BASE_TIMER_H_
#pragma once
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <queue>
#include <Windows.h>
#include <WinBase.h>
#include <process.h>
#include <string>

// 自适应宽窄字符
#ifdef _UNICODE
	typedef std::wstring thr_string;
#else
	typedef std::string thr_string;
#endif

// Windows 内核句柄的安全释放宏，专门用来释放HANDLE
#define DESTROYHANDLE(h)\
	do \
{\
	CloseHandle(h);\
	h = 0;\
} while (0);

typedef HANDLE WHANDLE;

// 自定义锁
class WLock{
public:
	class WSect{
	public:
		WSect(){::InitializeCriticalSection(&m_cs);}
		~WSect(){::DeleteCriticalSection(&m_cs);}
	private:
		void Lock(){::EnterCriticalSection(&m_cs);}
		void Unlock(){::LeaveCriticalSection(&m_cs);}
		friend class WLock;
		CRITICAL_SECTION m_cs;
	}&m_sect; // 结构体老式写法, 按理说应该是WLock& m_sect
	WLock(WSect& cs):m_sect(cs){m_sect.Lock();}
	~WLock(){m_sect.Unlock();}
};

#define WDECLARESECT(cs) WLock::WSect cs;
#define WAUTOLOCK(cs)    WLock lock(cs);

#define EXCEPTION_SEPARATE(esp) \
	do \
	{\
		esp;\
	} while (0);


/*
	对线程的封装，使用了互斥锁
*/
class CWThread
{
public:
	typedef std::function<void(CWThread*)> _MyCall;
public:
	CWThread::CWThread()
		: m_isRun(false)
		, m_onCreate(nullptr)
		, m_onRun(nullptr)
		, m_onDestroy(nullptr)
		, m_threadId(0)
		, m_hThread(0)
	{
		int a = 0;
	}

	CWThread::~CWThread()
	{
		Close();
	}

	bool Start(_MyCall _create = nullptr,_MyCall _run = nullptr,_MyCall _destroy = nullptr)
	{
		WAUTOLOCK(m_cs);
		if (!IsRun())
		{
			m_isRun = true;
			m_onCreate = _create;
			m_onRun = _run;
			m_onDestroy = _destroy;
			// 0=默认安全，0=使用默认栈大小，线程入口函数，参数，立刻运行，输出(拿回线程ID)
			m_hThread = (WHANDLE)_beginthreadex(0,0,Worker,this,0, &m_threadId);// 返回unsigned int，后强转为HANDLE
			if ( !m_hThread )
			{
				return false;
			}
			return true;
		}
		return false;
	}

	void Close(bool bWait = true)
	{
		int a = 0;
#if (_WIN32_WINNT >= 0x0502)
		auto curid = ::GetThreadId(GetCurrentThread());
		if ( curid == m_current_threadId)
		{// 如果是这个自己调用的, 那么这2个就会成立,
		}else
		{// 如果是外部调用, 那么这2个就会不相同
#endif
			WAUTOLOCK(m_cs);
			Exit();
			if ( m_hThread )
			{
				WaitForSingleObject(m_hThread, INFINITE);
				DESTROYHANDLE(m_hThread);
			}
#if (_WIN32_WINNT >= 0x0502)
		}
#endif
	}

	bool IsRun()
	{
		return m_isRun;
	}

	void Exit()
	{
		m_isRun = false;
	}

	unsigned GetThreadId()
	{
		return m_threadId;
	}

	void SetUserData(unsigned int userdata)
	{
		m_userdata = userdata;
	}

	unsigned int GetUserData()
	{
		return m_userdata;
	}

private:
	static unsigned __stdcall Worker(LPVOID Param)
	{
		return ((CWThread*)Param)->Worker();
	}

	// 线程入口函数
	unsigned Worker()
	{
		int a = 0;
#if (_WIN32_WINNT >= 0x0502)
		//拿到当前线程 ID 存入
		m_current_threadId = ::GetThreadId(GetCurrentThread());
#endif

		if ( m_onCreate ){
			m_onCreate(this);// 调用
		}
		if ( m_onRun ){
			m_onRun(this);// 调用
		}
		if ( m_onDestroy ){
			m_onDestroy(this);// 调用
		}
		return 0;
	}


private:
	// _MyCall是参数为CWThread*的回调函数
	WDECLARESECT(m_cs);					// 互斥锁
	_MyCall m_onCreate;					// 创建
	_MyCall m_onRun;					// 运行
	_MyCall m_onDestroy;				// 销毁
	WHANDLE	m_hThread;					// 线程句柄，也就是void*
	unsigned m_threadId;				// 线程ID
	bool	m_isRun;					// 是否运行
	unsigned m_userdata;				// 用户自定义数据
	unsigned m_current_threadId;		// 当前线程ID
};


/*
	定时器-ok
*/
class CWTimer
{
public:
	typedef UINT_PTR _MyID;// unsigned int
	typedef CWTimer _MyTy;
	typedef LPCTSTR _MyIDStr;
	typedef std::shared_ptr<_MyTy> _MySP;
	typedef std::function<bool(_MyID)> _MyCall;
	typedef std::function<bool(_MyIDStr)> _MyCallStr;
	typedef CWThread _MyThread;
	typedef _MySP SPHANDLE;

private:
	// 定时器条目
	struct _MyElement
	{
		_MyID       id;
		thr_string  idstr;
		_MyCall     fCall;      // 业务回调
		_MyCallStr  fCallStr;   // 业务回调
		int         elapse;     // 定时间隔(毫秒)
		DWORD       lastTick;   // 上一次触发回调的时间点
		bool        bDelete;    // 外部标记,标记这条定时器要被删掉
	};
	typedef std::shared_ptr<_MyElement> _MySpElement;
	typedef std::vector<_MySpElement> _MyMpElement;

public:
	static _MySP CreateTimer(_MyCall fn=nullptr)
	{
		return _MySP(new _MyTy);
	}

	bool Start()
	{
		if (!m_bState)
		{
			// 启动线程,绑定_MyTy::OnRun
			m_bState = m_thread.Start(nullptr,std::bind(&_MyTy::OnRun,this,std::tr1::placeholders::_1),nullptr);
		}
		return m_bState;
	}

	void Close()
	{
		if (m_bState)
		{
			m_thread.Close();
			m_bState = false;
		}
		m_elems.clear();
	}

	void SetCallback(_MyCall f)
	{
		m_def = f;
	}

	// elapse表示时间间隔，也就是隔多少s后触发
	bool SetTimer(_MyID id,int elapse,_MyCall f,bool bOnce=false)
	{
		if (0 == id)
			return false;

		Start();
		auto sp = GetElement(id);
		if (!sp)
		{
			sp = _MySpElement(new _MyElement);
			sp->id=0;
			WAUTOLOCK(m_cs);
			m_elems.push_back(sp);
		}

		sp->lastTick = GetTickCount();
		sp->fCall = f;
		sp->elapse = elapse;
		sp->bDelete = false;
		sp->id = id;
		if (bOnce)
			sp->fCall(id);
		return true;
	}

	bool SetTimer(_MyIDStr id,int elapse,_MyCallStr f,bool bOnce=false)
	{
		if (!id || !_tcsicmp(id,_T("")))
		{
			return false;
		}

		Start();
		auto sp = GetElement(id);
		if (!sp)
		{
			sp = _MySpElement(new _MyElement);
			WAUTOLOCK(m_cs);
			m_elems.push_back(sp);
		}
		sp->id=0;
		sp->lastTick = GetTickCount();
		sp->fCallStr = f;
		sp->elapse = elapse;
		sp->bDelete = false;
		sp->idstr = id;
		if (bOnce)
			sp->fCallStr(id);

		return true;
	}

	bool HasTimer(_MyID id)
	{
		return !!GetElement(id);
	}

	bool HasTimer(_MyIDStr id)
	{
		return !!GetElement(id);
	}

	bool KillTimer(_MyID id)
	{
		WAUTOLOCK(m_cs);
		auto sp = GetElement(id);
		if ( sp )
		{
			sp->bDelete = true;
			return true;
		}

		return false;
	}

	bool KillTimer(_MyIDStr id)
	{
		WAUTOLOCK(m_cs);
		auto sp = GetElement(id);
		if ( sp )
		{
			sp->bDelete = true;
			return true;
		}
		return false;
	}

	void KillAllTimer()
	{
		WAUTOLOCK(m_cs);
		for ( auto it = m_elems.begin(); it != m_elems.end();it++)
		{
			(*it)->bDelete = true;
		}
	}

	void Reset()
	{
		KillAllTimer();
	}

private:
	void OnRun(CWThread*pThread)
	{
		while(m_bState && pThread->IsRun())
		{
			Sleep(1);
			TraceTimer();
		}
	}

	void TraceTimer()
	{
		WAUTOLOCK(m_cs);
		DWORD tick = GetTickCount();
		for ( auto it = m_elems.begin(); it != m_elems.end(); )
		{
			if ( (*it)->bDelete )
			{
				// 标记待删除的定时器条目, 直接删掉
				it = m_elems.erase(it);
				continue;
			}

			if( (int)(tick-(*it)->lastTick) > (*it)->elapse )
			{
				// 判断这个定时器是否到时间了
				(*it)->lastTick = GetTickCount();
				bool rs = false;

				if ( (*it)->fCall )
				{
					rs = (*it)->fCall((*it)->id);
				}else if ( (*it)->fCallStr )
				{
					rs = (*it)->fCallStr((*it)->idstr.c_str());
				}
				else
				{
					// 两个回调都没设置, 走默认回调 m_def
					if ( m_def )
					{
						// 默认构造是CWTimer构造是的那个,
						rs = m_def((*it)->id);
					}
				}

				if ( !rs )
				{
					// 回调返回false → 删除这条定时器
					it = m_elems.erase(it);
					continue;
				}
			}
			it++;
		}
	}


	_MySpElement GetElement(_MyID id)
	{
		WAUTOLOCK(m_cs);
		for ( auto it = m_elems.begin(); it != m_elems.end();it++)
		{
			if ((*it)->id == id)
			{
				return *it;
			}
		}
		return _MySpElement();// 返回空智能指针
	}

	_MySpElement GetElement(const _MyIDStr &id)
	{
		WAUTOLOCK(m_cs);
		for ( auto it = m_elems.begin(); it != m_elems.end();it++)
		{
			if ((*it)->idstr == id)
			{
				return *it;
			}
		}
		return _MySpElement();// 返回空智能指针
	}

public:
	CWTimer(_MyCall def=nullptr):m_def(def),m_bState(false){}
	~CWTimer(){ Close();}

private:
	WDECLARESECT(m_cs);
	_MyMpElement m_elems;// 定时器条目
	_MyThread m_thread; // 线程
	_MyCall m_def;// 一个默认回调
	bool m_bState;

};

/*
	线程任务
*/
class CWTask
{
public:
	typedef std::shared_ptr<CWTask> _MySP;
	typedef std::function<void()> _MyCall;
	typedef CWThread _MyThread;

public:
	CWTask::CWTask()
		: m_fCall(nullptr)
		, m_event(0)
	{
	}

	~CWTask()
	{
		Close();
	}

	bool Start()
	{
		WAUTOLOCK(m_cs);
		if (!m_event ){
			m_event = CreateEvent(NULL, TRUE, FALSE, NULL);// 手动重置事件, 初始无信号
		}
		if ( m_event )// 启动线程并绑定
		{
			return m_thread.Start(nullptr,std::bind(&CWTask::OnRun,this,std::tr1::placeholders::_1),nullptr);
		}
		return false;
	}

	void Close()
	{
		WAUTOLOCK(m_cs);
		m_thread.Exit();
		SetEvent(m_event);
		DESTROYHANDLE(m_event);
		m_thread.Close();
	}

	bool IsIdle()
	{
		// 当前没有设置回调函数，表示空闲状态时，返回true
		return !m_fCall;
	}

	bool SetTask(_MyCall f,LPCTSTR label=_T(""))
	{
		WAUTOLOCK(m_cs);
		m_label = label;
		if ( f && IsIdle() )
		{
			m_fCall = f;
			SetEvent(m_event); // 唤醒线程，会走OnRun中的OnWorker函数
			return true;
		}
		return false;
	}

	void CWTask::Reset()
	{

	}

	void SetLabel(LPCTSTR label)
	{
		m_label = label;
	}

private:
	void OnRun(CWThread*pThread)
	{
		DWORD ret = WAIT_FAILED;
		while(WAIT_OBJECT_0 ==(ret=WaitForSingleObject(m_event,INFINITE)) )
		{
			// 无限等待，卡死在这里，直到 m_event被 SetEvent 变成有信号
			if ( !pThread->IsRun() )
			{
				// 如果线程没有运行，跳出
				break;
			}
			OnWorker();
		}
		int a = 0;
	}
	void OnWorker()
	{
		if ( m_fCall )// 判断是否有回调
		{
			m_fCall();// 调用回调函数
			m_fCall = nullptr;// 重新标记为空闲状态
		}
		::ResetEvent(m_event);// 事件复位，线程回到等待状态
	}

private:
	WDECLARESECT(m_cs);
	_MyThread m_thread;
	_MyCall m_fCall;
	thr_string m_label;
	WHANDLE m_event;

};


/*
	线程池
	Start 初始化时预设线程个数，当增加任务时没有空闲线程，将追加新线程
*/
class CWThreadPool
{
	typedef CWTask::_MySP    _MySP;
	typedef CWTask::_MyCall   _MyCall;
	typedef std::vector<_MySP> _MyTy;
public:
	CWThreadPool()
		: m_count(0)
		, m_isRun(false)
		, m_allow_idle(3)
	{
	}

	~CWThreadPool()
	{
		Close();
	}

	void Start(int nCount)
	{
		WAUTOLOCK(m_cs);
		if ( !m_isRun )
		{
			m_isRun = true;
			m_count = nCount;
			for ( int i = 0 ; i < nCount; i++ )
			{
				_MySP p(new CWTask);
				if ( p->Start() )
				{
					m_tasks.push_back(p);
				}
			}
		}
	}

	void Close()
	{
		WAUTOLOCK(m_cs);
		m_tasks.clear();
		m_temp_tasks.clear();
		m_isRun = false;
	}

	void SetIdleCount(int n)
	{
		WAUTOLOCK(m_cs);
		m_allow_idle = n;
	}

	bool AddTask(_MyCall f)
	{
		WAUTOLOCK(m_cs);
		_MySP spTask;

		//查询预先开辟
		if ( !spTask )
		{
			for (auto it = m_tasks.begin(); it != m_tasks.end();it++)
			{
				if ( (*it)->IsIdle())
				{ // 如果这个CWTask是空闲时
					spTask = *it;
					break;
				}
			}
		}

		//查询临时开辟, 没有就创建
		if ( !spTask )
		{
			for (auto it = m_temp_tasks.begin(); it != m_temp_tasks.end();it++)
			{
				if ( (*it)->IsIdle())
				{
					spTask = *it;
					break;
				}
			}
			if ( !spTask )
			{
				spTask = _MySP(new CWTask);
				if ( !spTask->Start())
				{
					return false;
				}
				m_temp_tasks.push_back(spTask);
			}
		}

		//保留m_allow_idle个空闲任务, 把其他空闲的都干掉
		int n = 0;
		for (auto it = m_temp_tasks.begin(); it != m_temp_tasks.end();)
		{
			if ( (*it)->IsIdle() && ++n > m_allow_idle )
			{
				it = m_temp_tasks.erase(it);
			}
			else
			{
				it++;
			}
		}

		// 最终还是调用的CWTask中的SetTask, 设置任务的回调
		bool rs = spTask->SetTask(f);
		return rs;
	}

	void Reset()
	{
		WAUTOLOCK(m_cs);
		for (auto it = m_tasks.begin(); it != m_tasks.end();it++)
		{
			(*it)->Reset();
		}

		for (auto it = m_temp_tasks.begin(); it != m_temp_tasks.end();it++)
		{
			(*it)->Reset();
		}
	}

private:
	WDECLARESECT(m_cs);
	_MyTy m_tasks;// 任务数组
	_MyTy m_temp_tasks;
	bool m_isRun;
	int m_count;
	int m_allow_idle;

};


/*
	事件，这个事件怎么理解呢
*/
class CWEvent
{
public:
	// 事件条目
	struct EventItem
	{
		EventItem()
		{
			hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
			bIdle = false;
		}
		~EventItem()
		{
			SetEvent(hEvent);
			CloseHandle(hEvent);// 关闭句柄
			hEvent = NULL;
		}
		bool IsIdle()
		{
			return bIdle;
		}
		void SetIdle(bool bSet)
		{
			bIdle = bSet;
		}
		DWORD Wait(DWORD msec)
		{
			ResetEvent(hEvent);// 先把事件置为【无信号】
			DWORD ret = WaitForSingleObject(hEvent,msec);// 阻塞等待,最多等待msec秒
			return ret;
		}
		void Active()
		{
			SetEvent(hEvent);
			bIdle = true;
		}

		HANDLE hEvent;
		bool bIdle;// 是否空闲?
	};
	typedef std::vector<EventItem*> DataType;

public:
	CWEvent(){}
	~CWEvent(){}
	void Start()
	{
		Close();// 清理事件数组??
	}
	void Close()
	{
		WAUTOLOCK(m_cs);
		for ( auto it = m_events.begin(); it != m_events.end(); )
		{
			if ( *it )
			{
				delete (*it);
				(*it) = nullptr;
			}
			it = m_events.erase(it);
		}
	}

	DWORD Wait(DWORD msec)
	{
		return GetIdle()->Wait(msec);
	}
	void Reset()
	{
		WAUTOLOCK(m_cs);
		for ( auto it = m_events.begin(); it != m_events.end(); it++ )
		{
			(*it)->Active();
		}
	}
	EventItem * GetIdle()
	{// 得到一个空闲事件条目
		WAUTOLOCK(m_cs);
		EventItem * pNode = nullptr;
		for ( auto it = m_events.begin(); it != m_events.end(); it++ )
		{
			if ( (*it)->IsIdle() )
			{
				pNode = *it;
				pNode->SetIdle(false);
				break;
			}
		}
		if ( !pNode )
		{
			pNode = new EventItem;
			m_events.push_back(pNode);
		}
		return pNode;
	}

private:
	WDECLARESECT(m_cs);// 定义锁内部的一个类
	DataType m_events;// 事件条目数组

};

#endif // !_BASE_TIMER_H_
