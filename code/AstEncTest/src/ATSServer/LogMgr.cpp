#include "stdafx.h"
#include "LogMgr.h"

bool CLogMgr::Init()
{
	WSL_Init();
	NewPath();
	return true;
}

void CLogMgr::Release()
{
	WSL_Release();
}

void CLogMgr::Reset()
{
	WSL_CloseAll();
	NewPath();
}

void CLogMgr::NewPath()
{
	m_sPath = WFmt(_T("CLog\\%s"),WToStr(WNowTT,_T("yyyy年MM月dd日HH时mm分ss秒\\")).c_str());
}

LPCTSTR CLogMgr::GetPath()
{
	return m_sPath.c_str();
}

void CLogMgr::Add(LPCTSTR name,bool bFlush,LPCTSTR fmt,...)
{
	va_list argList;
	va_start(argList, fmt);
	int len = _vscprintf(fmt, argList);// 获取格式化字符串长度
	std::string s;
	s.resize(len);
	int ir = _vsnprintf(s._Myptr(), len,fmt, argList);// 写入数据到s中
	va_end(argList);// 可变参数收尾, 清理 argList
	WSL_Add2(name,bFlush,s.c_str());// 把模块名、是否刷盘、拼接好的日志字符串传给底层日志函数WSL_Add2
}

void CLogMgr::Close(LPCTSTR name)
{

}

void CLogMgr::CloseAll()
{
	WSL_CloseAll();
}

//void CLogMgr::Printf(DB_TTL*ttl)
//{
//	CString filename;
//	filename.Format(_T("%d实际图K%d%03d%02d(%d).txt"),GetPath(),ttl->ID3,ttl->ID,ttl->ID2,ttl->Color);
//
//	WStr tmstr;
//	auto type = 0;
//	auto way = ttl->Dir;
//	if(ttl->Arrival != 0)
//	{
//		tmstr = WFmt(_T("02d:%02d:%02d"),ttl->Arrival/10000,ttl->Arrival/100%100,ttl->Arrival%100);
//	}else
//	{
//		tmstr = WFmt(_T("02d:%02d:%02d"),ttl->Departure/10000,ttl->Departure/100%100,ttl->Departure%100);
//		type = 1;
//	}
//
//	Add(filename,true,WFmt(_T("%s:(%d,%d,%d,%d,\"%s\",%d,%d)\n"),WToStr(WNowST).c_str(),ttl->Color,ttl->ID));
//}

void CLogMgr::Printf(LPCTSTR label,LPCTSTR fmt)
{
	CString filename;
	filename.Format(_T("%s%s.txt"),GetPath(),label);
	Add(filename,true,fmt);
}

// 输出到日志中
void CLogMgr::PrintfCLog(LPCTSTR label,LPCTSTR fmt,...)
{
	va_list argList;
	va_start(argList, fmt);
	int len = _vscprintf(fmt, argList);//_vsctprintf
	std::string s;
	s.resize(len);
	int ir = _vsnprintf(s._Myptr(), len,fmt, argList);//_vsntprintf
	va_end(argList);

	//WAFX::exepath(WFmt("\\CLog\\%s",))

	auto file = WPathFmt(_T("\\CLog\\%s.txt"),label);

	WSL_Add2(file.c_str(),true,s.c_str());
}
