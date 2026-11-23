#pragma once

/*
	线程管理
*/
class CWSync;
class CThreadMgr : public StaticSingleton<CThreadMgr>
{
public:
	CThreadMgr();
	virtual bool        Init();
	virtual void        Release();
	virtual void        Reset();
	CWSync *            GetSync();
private:
	CWSync *            m_pSync;
};

#define ThrMgr  SSLT(CThreadMgr)
#define SycMgr  ThrMgr->GetSync()
