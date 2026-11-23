#pragma once

/*
    ×ÜÈë¿Ú
*/

class CATSLogicService:public StaticSingleton<CATSLogicService>
{
public:
    CATSLogicService();
    bool          Init();
    void          Release();
    void          Reset();
public:
    void          SetAtsTime(int h,int m,int s);
    void          LoadPlan(LPCTSTR plan,SYSTEMTIME st);
};

#define ATSSrv SSLT(CATSLogicService)
