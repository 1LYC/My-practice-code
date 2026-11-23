#include "StdAfx.h"
#include "WDBAide.h"
#include "WCharset.h"
#include "..\..\3rdpart\Hdb\hdb.h"

/////////////////////////////////////////////////////////////
//

struct CWDBAide::member
{
    member():h(0)
    {
        h = HDB_Create();// 创建
    }
    ~member()
    {
        HDB_Delete(h);// 删除
    }
    HDB_HANDLE h;// 数据库句柄
};


#define _H222_ ptr->h
CWDBAide::CWDBAide()
    : ptr(new member)
{

}

CWDBAide::~CWDBAide()
{
    delete ptr;
}


bool CWDBAide::Connect(CWDBAide::DBCon* ptr)
{
    return Connect(ptr->host.c_str(),ptr->user.c_str(),ptr->pswd.c_str(),ptr->name.c_str(),ptr->port);
}

bool CWDBAide::Connect(LPCTSTR host, LPCTSTR user, LPCTSTR pswd, LPCTSTR name, unsigned int port)
{
    auto rs = -1 != HDB_Connect(_H222_,STRINGtoUTF8(host).c_str() , STRINGtoUTF8(user).c_str(), STRINGtoUTF8(pswd).c_str(), STRINGtoUTF8(name).c_str(),port);
    return rs;
}

void CWDBAide::Encode(LPCTSTR code)
{
    HDB_Encode(_H222_,STRINGtoUTF8(code).c_str());
}

void CWDBAide::Select(LPCTSTR sql,std::function<bool(CWDBAide&)> fn)
{
    auto rs = true;
    HDB_SelectCallback(_H222_,STRINGtoUTF8(sql).c_str(),[&](HDB_HANDLE h)
    {
        if (rs)
        {
            if (fn)
            {
                rs = fn(*this);
            }
        }
    });
}

int CWDBAide::Update(LPCTSTR sql)
{
    return HDB_Update(_H222_,STRINGtoUTF8(sql).c_str());
}

int CWDBAide::Insert(LPCTSTR sql)
{
    return HDB_Insert(_H222_,STRINGtoUTF8(sql).c_str());
}

int CWDBAide::Execute(LPCTSTR sql)
{
    return HDB_Execute(_H222_,STRINGtoUTF8(sql).c_str());
}

int CWDBAide::Procedure(const char *proc, const char *param)
{
    /*
    va_list arglist;
    va_start(arglist, fmt);
    int len = _vsnprintf(fmt, arglist);
    std::string sql;
    sql.resize(len);
    _vsnprintf(sql._Myptr(), len,fmt, arglist);
    va_end(arglist);*/
    return HDB_ExecuteProcedure(_H222_,proc,STRINGtoUTF8(param).c_str());
}

bool CWDBAide::IsEnd()
{
    return HDB_IsEof(_H222_);
}

LPCTSTR CWDBAide::GetValue(LPCTSTR item)
{
    auto key = STRINGtoUTF8(item);
    return HDB_GetFiledValueByName(_H222_,key.c_str());
}

void CWDBAide::GetValue(LPCTSTR item,bool &val)
{
    val = !!_ttoi(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,char&val)
{
    val = _ttoi(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,unsigned char&val)
{
    val = _ttoi(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,int&val)
{
    val = _ttoi(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,unsigned int&val)
{
    val = _ttoi(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,long&val)
{
    val = _ttol(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,unsigned long&val)
{
    val = _ttol(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,float&val)
{
    val = _ttof(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,double&val)
{
    val = _ttof(GetValue(item));
}

void CWDBAide::GetValue(LPCTSTR item,CWDBAide::Sstr &val,int enc/*=0*/)
{
    switch(enc)
    {
    case 1:
        {
            val = GetValue(item);
        }break;
    default:
        {
            val = UTF8toSTRING(GetValue(item)).c_str();
        }break;
    }
}

void CWDBAide::GetValue(LPCTSTR item,CString &val,int enc/*=0*/)
{
    switch(enc)
    {
    case 1:
        {
        }break;
    default:
        {
            val = UTF8toSTRING(GetValue(item)).c_str();
        }break;
    }
}

void CWDBAide::GetValue(LPCTSTR item,SYSTEMTIME &val)
{

}

void CWDBAide::GetValue(LPCTSTR item,time_t &val)
{
    auto rs = true;
    std::string str = GetValue(item);
    for (auto i = 0; i < str.size(); i++)
    {
        if (!isdigit(str[i]))
        {
            rs = false;
        }
    }

    if (rs)
    {
        val = _ttol(GetValue(item));
    }
    else
    {
        SYSTEMTIME st;
    }
}

void CWDBAide::GetValue(LPCTSTR item,TCHAR * str,int size)
{
    if (-1 < 0)
    {
        _tcscpy(str,UTF8toSTRING(GetValue(item)).c_str());
    }else
    {
        _tcscpy_s(str,size,UTF8toSTRING(GetValue(item)).c_str());
    }
}

int CWDBAide::Query(const char* str)
{
    return HDB_Execute(_H222_,str);
}

bool CWDBAide::Autocommit(bool b)
{
    return HDB_autocommit(_H222_,b);
}

bool CWDBAide::BeginTrans()
{
    return HDB_begintrans(_H222_);
}

bool CWDBAide::Rollback()
{
    return HDB_rollback(_H222_);
}

bool CWDBAide::Commit()
{
    return HDB_commit(_H222_);
}
