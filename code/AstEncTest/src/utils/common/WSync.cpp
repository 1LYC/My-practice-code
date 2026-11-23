#include "stdafx.h"
#include "WSync.h"
#include "WThread.h"

struct CWSync::Members
{
	CWTimer        s_timer;  // 定时器
	CWThreadPool   s_pools;  // 线程池
	CWEvent        s_event;  // 等待事件
};

CWSync::CWSync()
	: m_ptr(nullptr)
{
}

CWSync::~CWSync()
{
	Release();
}

void CWSync::Init(int nThread)
{
	auto n = abs(nThread);
	Release();
	if (!m_ptr)
	{
		m_ptr = new Members;
		m_ptr->s_timer.Start();
		m_ptr->s_pools.Start(n);
		m_ptr->s_event.Start(); // 事件: 控制子线程启停、触发任务
	}
}

void CWSync::Release()
{
	if (m_ptr)
	{
		m_ptr->s_event.Reset();
		m_ptr->s_timer.Close();
		m_ptr->s_pools.Close();
		m_ptr->s_event.Close();
		WDelete(m_ptr);
		//WAFX::_WDelete(m_ptr);
	}
}

void CWSync::Reset()
{
	if ( m_ptr )
	{
		static int icount = 0;
		++icount;
		TRACE("CWSync::Reset() start %d\n",icount);
		m_ptr->s_event.Reset();
		TRACE("CWSync::Reset() event %d\n",icount);
		m_ptr->s_timer.KillAllTimer();
		TRACE("CWSync::Reset() timer %d\n",icount);
		m_ptr->s_pools.Reset();
		TRACE("CWSync::Reset() end!! %d\n\n",icount);
	}
}

void CWSync::KillTimer(UINT_PTR id)
{
	m_ptr->s_timer.KillTimer(id);
}

bool CWSync::HasTimer(UINT_PTR id)
{
	return m_ptr->s_timer.HasTimer(id);
}

void CWSync::SetTimer(UINT_PTR id,int es,FnCallBack fn,bool bOnce)
{
	m_ptr->s_timer.SetTimer(id,es,fn,bOnce);
}

void CWSync::KillTimer(LPCTSTR id)
{
	m_ptr->s_timer.KillTimer(id);
}

bool CWSync::HasTimer(LPCTSTR id)
{
	return m_ptr->s_timer.HasTimer(id);
}

void CWSync::SetTimer(LPCTSTR id,int es,FnCallBackStr fn,bool bOnce)
{
	m_ptr->s_timer.SetTimer(id,es,fn,bOnce);
}

void CWSync::AddTask(FnTaskCall fn,LPCTSTR label)
{
	m_ptr->s_pools.AddTask(fn);
}

bool CWSync::Wait(int ms)
{
	auto ret = m_ptr->s_event.Wait(ms);
	return ret == WAIT_TIMEOUT;
}

// 下面2个都是与CWEvent有关
void * CWSync::CreateEvent()
{
	return m_ptr->s_event.GetIdle();
}

bool CWSync::WaitTimout(void* ptr,unsigned int ms)
{
	auto event = (CWEvent::EventItem*)ptr;
	auto rs = event->Wait(ms);
	return WAIT_TIMEOUT == rs;
}
