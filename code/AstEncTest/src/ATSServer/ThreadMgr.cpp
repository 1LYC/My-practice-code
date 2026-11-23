#include "stdafx.h"
#include "ThreadMgr.h"

CThreadMgr::CThreadMgr()
	: m_pSync(new CWSync)
{

}

bool CThreadMgr::Init()
{
	m_pSync->Init(10);
	return true;
}

void CThreadMgr::Release()
{
	WSafeDelete(m_pSync);
}

void CThreadMgr::Reset()
{
	static int i = 0;
	i++;
	auto tmr = WAFX::to_str(WNowTT,_T("yyyy-MM-dd HH:mm:ss"));
	auto str = WFmt(_T("CLog\\Reset.txt"));
	LogMgr->Add(str.c_str(),true,WFmt(_T("\n\n%s: \t¡÷.Reset: %d\n"),tmr.c_str(),i).c_str() );
	SycMgr->Reset();
	LogMgr->Add(str.c_str(),true,WFmt(_T("%s: \t¡ø.Reset: %d\n"),tmr.c_str(),i).c_str() );
}

CWSync * CThreadMgr::GetSync()
{
	return m_pSync;
}
