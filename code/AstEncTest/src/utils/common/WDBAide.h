#ifndef _HDBASSIST_H_
#define _HDBASSIST_H_
#pragma once
#include <string>
#include <functional>

/*
    数据库管理
*/

class CWDBAide
{
public:
#ifdef _UNICODE
    typedef std::wstring Sstr;
#else
    typedef std::string  Sstr;
#endif
    // 数据库连接配置
    struct DBCon
    {
        Sstr host;
        Sstr name;
        Sstr user;
        Sstr pswd;
        int  port;
    };
public:
    CWDBAide();
    ~CWDBAide();

    bool    Connect(DBCon*);
    bool    Connect(LPCTSTR host, LPCTSTR user, LPCTSTR pswd, LPCTSTR name, unsigned int port);

    void    Encode(LPCTSTR code);
    int     Query(const char*);
    bool    Autocommit(bool);  // 开启 / 关闭自动提交
    bool    BeginTrans();       // 开启事务
    bool    Rollback();         // 事务回滚 (出错撤销修改)
    bool    Commit();           // 事务提交 (确认修改落地到数据库)
    void    Select(LPCTSTR sql,std::function<bool(CWDBAide&)> fn=nullptr);
    int     Update(LPCTSTR sql);
    int     Insert(LPCTSTR sql);
    int     Execute(LPCTSTR sql);
    int     Procedure(const char *proc, const char *param);// 调用存储过程, 传入存储过程名和参数
    bool    IsEnd();            // 判断结果集是否读到末尾
    LPCTSTR GetValue(LPCTSTR item);
    void    GetValue(LPCTSTR item,bool &val);
    void    GetValue(LPCTSTR item,char &val);
    void    GetValue(LPCTSTR item,unsigned char &val);
    void    GetValue(LPCTSTR item,int &val);
    void    GetValue(LPCTSTR item,unsigned int &val);
    void    GetValue(LPCTSTR item,long &val);
    void    GetValue(LPCTSTR item,unsigned long &val);
    void    GetValue(LPCTSTR item,float &val);
    void    GetValue(LPCTSTR item,double &val);
    void    GetValue(LPCTSTR item,Sstr &val,int enc=0);
    void    GetValue(LPCTSTR item,CString &val,int enc=0);
    void    GetValue(LPCTSTR item,SYSTEMTIME &val);
    void    GetValue(LPCTSTR item,time_t &val);
    void    GetValue(LPCTSTR item,TCHAR * str,int size=-1);
private:
    struct member;
    member* ptr;
};

#endif
