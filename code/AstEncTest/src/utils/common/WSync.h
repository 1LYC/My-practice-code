#pragma once


/*
	异步调用服务,提供定时器、线程、等待事件的操作
*/

class CWSync
{
public:
	typedef std::function<bool(UINT_PTR)> FnCallBack;
	typedef std::function<bool(LPCTSTR id)> FnCallBackStr;
	typedef std::function<void(UINT_PTR, int, FnCallBack)> FnSet;
	typedef std::function<void(UINT_PTR)> FnKill;
	typedef std::function<void()> FnTaskCall;
	typedef std::function<void(FnTaskCall)> FnTaskSet;
	typedef std::function<bool(int sec)> FnWait;

public:
	CWSync();
	~CWSync();
	void        Init(int nThread=1);
	void        Release();
	void        Reset();

	void        SetTimer(UINT_PTR id,int es,FnCallBack f,bool bOnce=false);
	bool        HasTimer(UINT_PTR id);
	void        KillTimer(UINT_PTR id);

	void        SetTimer(LPCTSTR id,int es,FnCallBackStr f,bool bOnce=false);
	bool        HasTimer(LPCTSTR id);
	void        KillTimer(LPCTSTR id);

	void        AddTask(FnTaskCall fn,LPCTSTR label=_T(""));
	bool        Wait(int ms);
	void *      CreateEvent();
	bool        WaitTimout(void* ptr,unsigned int ms);

private:
	struct Members;
	Members * m_ptr;
};
