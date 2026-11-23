#pragma once

/*
    日志管理
*/

class CLogMgr : public StaticSingleton<CLogMgr>
{
public:
    virtual bool    Init();
    virtual void    Release();
    virtual void    Reset();
public:
    void            NewPath();
    LPCTSTR         GetPath();
    void            Add(LPCTSTR name,bool bFlush, LPCTSTR fmt,...);
    void            Flush(LPCTSTR name);
    void            Close(LPCTSTR name);
    void            CloseAll();
    void            Printf(LPCTSTR label,LPCTSTR fmt);
    //void            Printf(DB_TTL*);
    void            PrintfCLog(LPCTSTR label,LPCTSTR fmt,...);
protected:
    WStr            m_sPath;
};
#define LogMgr SSLT(CLogMgr)

#define PRINT_TRAIN LogMgr->PrintfTrain
