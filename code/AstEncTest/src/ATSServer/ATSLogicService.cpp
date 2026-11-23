#include "StdAfx.h"
#include "ATSLogicService.h"

void TestBeforInit();
void TestAfterInit();
bool OnPreCctv(void*lp);

CATSLogicService::CATSLogicService()
{

}

bool CATSLogicService::Init()
{
    TestBeforInit();
    LogMgr->Init();// CLog//日志文件夹
    ThrMgr->Init();// 线程管理-初始化
    TestAfterInit();
    return true;
}

void CATSLogicService::Release()
{
    ThrMgr->Release();
    LogMgr->Release();
}


void DoOCCLogin(LPCTSTR user);

void CATSLogicService::Reset()
{
    ThrMgr->Reset();
    LogMgr->Reset();

#ifdef _DEBUG
    // DoOCCLogin(_T("CC1101"));
#endif

}


void CATSLogicService::SetAtsTime(int h,int m,int s)
{

}

void CATSLogicService::LoadPlan(LPCTSTR plan,SYSTEMTIME st)
{
}

void TestBeforInit()
{


}

void TestAfterInit()
{

#ifdef _DEBUG
    // DoOCCLogin(_T("CC1101"));
#endif
}
