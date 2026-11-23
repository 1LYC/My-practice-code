#pragma once
#include <functional>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <algorithm>
#include <tchar.h>// 用来兼容 ANSI / Unicode 双字符集
#include <time.h>// 获取系统时间、时间格式化。
#include <assert.h>
#include <GdiPlus.h>
#pragma comment(lib,"GdiPlus.lib")
using namespace Gdiplus;

#ifdef _WINDOWS_
	#include <WinSock.h>// 老版 Windows Socket，网络套接字；
	#include <Windows.h>// Windows 最核心 API，窗口、线程、消息等；
	#include <Shlwapi.h>// Shell 轻量工具库，常用路径处理函数
	#pragma comment(lib,"Shlwapi.lib")
#endif

// 以RALL风格封装GDI+构造，析构
class GdiplusToken
{
public:
	GdiplusToken(){
		GdiplusStartup(&token,&startup,NULL);
	}
	~GdiplusToken(){
		GdiplusShutdown(token);
	}
private:
	GdiplusStartupInput startup;// 启动参数结构体
	ULONG_PTR			token;// 类型unsigned long，令牌
};


// 定义占位符，多与std::bind使用
#define WTR1 std::tr1::placeholders::_1
#define WTR2 std::tr1::placeholders::_1,std::tr1::placeholders::_2
#define WTR3 std::tr1::placeholders::_1,std::tr1::placeholders::_2,std::tr1::placeholders::_3
#define WTR4 std::tr1::placeholders::_1,std::tr1::placeholders::_2,std::tr1::placeholders::_3,std::tr1::placeholders::_4
#define WTR5 std::tr1::placeholders::_1,std::tr1::placeholders::_2,std::tr1::placeholders::_3,std::tr1::placeholders::_4,std::tr1::placeholders::_5
#define WTR6 std::tr1::placeholders::_1,std::tr1::placeholders::_2,std::tr1::placeholders::_3,std::tr1::placeholders::_4,std::tr1::placeholders::_5,std::tr1::placeholders::_6

// 定义了一种遍历-迭代器
#undef WForStl2
#define WForStl2(s,it) for(auto it=s.begin();it != s.end();it++)

// 定义了一种遍历-数组下标
#define WForArr2(s,i) for(size_t i = 0;i != s.size();i++)

#define WForArr(s) WForArr2(s,i)
#define WForStl(s) WForStl2(s,it)

// 定义了一种反遍历
#define WForStlr(c) for(auto it = c.rbegin();it != c.rend();it++)

// 定义了一种安全释放指针-解决分支语法的bug
#define WSafeDelete(ptr)\
	do{\
		if(ptr){delete ptr;ptr=0;}\
	}while(0);

namespace WAFX
{
	typedef std::vector<std::string>	vec_strA;// 窄字节-多字节
	typedef std::vector<std::wstring>	vec_strW;// 宽字节-unicode
#ifdef _UNICODE
	typedef std::wstring tstr;
	typedef vec_strW vec_tstr;
#else
	// 多字节
	typedef std::string tstr;
	typedef vec_strA vec_tstr;
#endif

	typedef time_t			wtime_tt;// 时间戳类型,从 UTC 1970-01-01 00:00:00 开始的秒数
	typedef SYSTEMTIME		wtime_st;// Windows API 结构体,里面有年月日-中转
	typedef timeval			wtime_tv;// 高精度时间戳（秒 + 微秒）


	template<typename T>
	struct MyDelete{
		typedef T Type;
		static void Delete(Type &s){
			int a = 0;
		}
	};

	template<typename T>// 偏特化有这个模版参数，且只有类模板才有偏特化
	struct MyDelete<T*>{// 偏特化-指针
		typedef T* Type;
		static void Delete(Type &s){
			WSafeDelete(s);
		}
	};

	template<>
	struct MyDelete<HANDLE>{// 全特化-void *
		typedef HANDLE Type;
		static void Delete(Type &s){
			CloseHandle(s);
			s = NULL;
		}
	};

	template<>
	struct MyDelete<HANDLE*>{// 全特化-void **
		typedef HANDLE* Type;
		static void Delete(Type &s){
			CloseHandle(*s);
			*s = NULL;
			s = NULL;
		}
	};

	template<>
	struct MyDelete<SOCKET>{// 全特化-网络套接字
		typedef SOCKET Type;
		static void Delete(Type &s){
			closesocket(s);
			s = INVALID_SOCKET;
		}
	};

	template<>
	struct MyDelete<SOCKET*>{// 全特化-网络套接字
		typedef SOCKET* Type;
		static void Delete(Type &s){
			closesocket(*s);
			*s = INVALID_SOCKET;
			s = NULL;
		}
	};
	
	// begin-函数模版重载-begin
	template<typename T>
	void _WDelete(T&t)
	{
		MyDelete<T>::Delete(t);
	}

	template<typename T>
	void _WDelete(std::vector<T>&t)
	{
		t.clear()
	}

	template<typename T>
	void _WDelete(std::vector<T*>&t)
	{
		WForStl(t){
			_WDelete(*it);
		}
		t.clear();
	}

	template<typename T>
	void _WDelete(std::list<T>&t)
	{
		t.clear();
	}

	template<typename T>
	void _WDelete(std::list<T*>&t)
	{
		WForStl(t){
			_WDelete(*it);
		}
		t.clear();
	}

	template<typename key,typename val>
	void _WDelete(std::map<key,val>&t)
	{
		t.clear();
	}

	template<typename key,typename val>
	void _WDelete(std::map<key,val*>&t)
	{
		WForStl(t){
			_WDelete(it->second);
		}
		t.clear();
	}
	// end-函数模版重载-end

	// find_if
	template<typename T,typename _Fn>
	typename T::iterator WAFX_findif(T &v,_Fn && fn)
	{
		return std::find_if(v.begin(),v.end(),fn);
	}

	// begin-格式化输出-begin
	static std::string WAFX_format(const char* fmt,...)
	{// 窄字节-多字节
		va_list argList;
		va_start(argList,fmt);
		int len = _vscprintf(fmt,argList);// _vscprintf
		std::string s;
		s.resize(len);
		int ir = _vsnprintf(s._Myptr(),len,fmt,argList);//_vsnprintf
		va_end(argList);
		return s;
	}
	static std::wstring WAFX_formatw(const wchar_t* fmt,...)
	{// 宽字节-unicode
		va_list argList;
		va_start(argList,fmt);
		int len = _vscwprintf(fmt,argList);// _vscwprintf
		std::wstring s;
		s.resize(len);
		int ir = _vsnwprintf(s._Myptr(),len,fmt,argList);//_vscwprintf
		va_end(argList);
		return s;
	}
	#define WFmtA WAFX::WAFX_format
	#define WFmtW WAFX::WAFX_formatw

	#ifdef _UNICODE
		#define  WFmt	WFmtW
	#else
		#define WFmt	WFmtA
	#endif
	// end-格式化输出-end

	// begin-可执行文件路径-begin
	static tstr GetExeFilePath()
	{// 获取 exe 完整全路径，如：D:\test\bin\myapp.exe
		TCHAR buf[256] = {0};
		GetModuleFileName(0,buf,256);
		return buf;
	}

	static tstr GetExeDir(bool slash = false)
	{// 获取 exe 所在目录,如：D:\test\bin
		TCHAR buf[256] = {0};
		GetModuleFileName(0,buf,256);
		::PathRemoveFileSpec(buf);
		if(slash){
			_tcscat(buf,_T("\\"));// D:\test\bin
		}
		return buf;
	}
	
	static tstr GetExeName()
	{// 获取 exe 的名字，如：myapp
		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(NULL,path,MAX_PATH);
		TCHAR exe[MAX_PATH] = {0};
		_tcscpy_s(exe,path);
		PathStripPath(exe);
		PathRemoveExtension(exe);
		return exe;
	}

	inline tstr exename()
	{// 获取 exe 的名字，如：myapp.exe
		TCHAR path[MAX_PATH] = {0};
		::GetModuleFileName(NULL,path,MAX_PATH);
		tstr str = path;
		int n = str.rfind(_T('\\'));
		str.erase(str.begin(),str.begin()+n+1);
		return str;
	}

	inline tstr exepath(LPCTSTR cat = _T(""))
	{// 拿到 exe 目录的名字并拼接，如：D:\test\bin\XXXX.xml
		TCHAR path[MAX_PATH] = {0};
		::GetModuleFileName(NULL,path,MAX_PATH);
		tstr str = path;
		int n = str.rfind(_T('\\'));
		str.erase(str.begin()+n+1,str.end());
		str += cat;
		return str;
	}

	inline std::string PathFmtA(const char* fmt,...)
	{// 拿到当前exe所在目录，在拼接格式化子路径-窄字符-多字节
		TCHAR path[MAX_PATH] = {0};
		::GetModuleFileName(NULL,path,MAX_PATH);
		tstr str = path;
		int n = str.rfind(_T('\\'));
		str.erase(str.begin()+n+1,str.end());

		va_list argList;
		va_start(argList,fmt);
		int len = _vscprintf(fmt,argList);// _vscprintf
		std::string s;
		s.resize(len);
		int ir = _vsnprintf(s._Myptr(),len,fmt,argList);//_vsnprintf
		va_end(argList);
		if(!s.empty())
		{
			str += s;
		}

		return str;
	}

	inline std::wstring PathFmtW(const wchar_t* fmt,...)
	{// 拿到当前exe所在目录，在拼接格式化子路径-宽字符-unicode
		wchar_t path[MAX_PATH] = {0};
		::GetModuleFileNameW(NULL,path,MAX_PATH);
		std::wstring str = path;
		int n = str.rfind(_T('\\'));
		str.erase(str.begin()+n+1,str.end());

		va_list argList;
		va_start(argList,fmt);
		int len = _vscwprintf(fmt,argList);// _vscwprintf
		std::wstring s;
		s.resize(len);
		int ir = _vsnwprintf(s._Myptr(),len,fmt,argList);//_vsnwprintf
		va_end(argList);
		if(!s.empty())
		{
			str += s;
		}

		return str;
	}
	// end-可执行文件路径-end

	// begin-开机自启动-begin
	//static void SetAppStart(bool bAdd)
	//{
	//	tstr path = GetExeFilePath();
	//	tstr exe = GetExeName();
	//	// TCHAR key[MAX_PATH] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//	tstr key = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//	HKEY result;
	//	RegOpenKey(HKEY_CURRENT_USER,key.c_str(),&result);
	//	if(bAdd == false){
	//		RegDeleteValue(result,exe.c_str());// 删除注册表项 → 取消开机自启
	//	}else{
	//		RegSetValueEx(result,exe.c_str(),0,REG_SZ,(const BYTE*)path.c_str(),path.size());
	//	}
	//}

	// 添加/取消当前用户开机自启
	static bool SetAppStart(bool bAdd)
	{
		tstr path = GetExeFilePath();
		tstr exeName = GetExeName();
		const TCHAR* regPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

		HKEY hKey = NULL;
		LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, regPath, 0, KEY_WRITE, &hKey);
		if (ret != ERROR_SUCCESS)
			return false;

		bool bOk = true;
		if (!bAdd)
		{
			ret = RegDeleteValue(hKey, exeName.c_str());
			if (ret != ERROR_SUCCESS && ret != ERROR_FILE_NOT_FOUND)
				bOk = false;
		}
		else
		{
			ret = RegSetValueEx(hKey, exeName.c_str(), 0, REG_SZ,
				(const BYTE*)path.c_str(),
				(path.size() + 1) * sizeof(TCHAR));
			if (ret != ERROR_SUCCESS)
				bOk = false;
		}

		RegCloseKey(hKey);
		return bOk;
	}

	/*static bool IsSingleApp(const tstr & name)
	{
	HANDLE h = CreateMutex(NULL,TRUE,name.c_str());
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
	return false;
	}
	return true;
	}*/

	static HANDLE g_hSingleMutex = NULL;
	// 检测单实例，成功返回true（第一个实例），失败false（已经存在）
	static bool IsSingleApp(const tstr& name)
	{
		g_hSingleMutex = CreateMutex(NULL, TRUE, name.c_str());
		if (g_hSingleMutex == NULL)
			return false;

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(g_hSingleMutex);
			g_hSingleMutex = NULL;
			return false;
		}
		return true;
	}
	/*
	// 单实例检测
		if (!WAFX::IsSingleApp(_T("MyAppMutex001")))
		{
		MessageBox(NULL, _T("程序已经运行！"), _T("提示"), MB_OK);
		return 0;
		}

		// 开机自启
		WAFX::SetAppStart(true);
		// WAFX::SetAppStart(false);
	*/
	// end-开机自启动-end

	// begin-去除空格、制表符-begin
	inline int trimA(std::string &_string)
	{
		int count_trim = 0;
		for(std::string::iterator it = _string.begin();it!=_string.end();)
		{// 为了解决迭代器失效的问题，这for中的迭代器++，是自己控制的
			if(*it == '\t' || *it == ' ' || *it == 13 || *it == '\n' )
			{
				it == _string.erase(it);
				count_trim++;
			}else
			{
				it++;
			}
		}
		return count_trim;
	}
	
	inline int trimW(std::wstring &_string)
	{//去除空格、制表符
		int count_trim = 0;
		for(std::wstring::iterator it = _string.begin();it!=_string.end();)
		{// 为了解决迭代器失效的问题，这for中的迭代器++，是自己控制的
			if(*it == L'\t' || *it == L' ' || *it == 13 || *it == '\n' )
			{
				it == _string.erase(it);
				count_trim++;
			}else
			{
				it++;
			}
		}
		return count_trim;
	}
	// end-去除空格、制表符-end

	// begin-删除字符串--begin
	// 字符串开头出现，就删除
	inline int trimleftA(std::string &_string,const std::string &trim)
	{
		// find找到了返回起点下标，如果开头找到了，返回0
		int n = _string.find(trim.c_str());
		if(n == 0){
			_string.erase(0,trim.size());
		}
		return n;// 这个返回值有点bug
	}
	inline int trimleftW(std::wstring &_string,const std::wstring &trim)
	{
		int n = _string.find(trim.c_str());
		if(n == 0){
			_string.erase(_string.begin(),_string.begin()+trim.size());
		}
		return n;
	}

	// 字符串最右边出现，就删除
	inline int trimrightA(std::string &_string,const std::string &trim)
	{
		int n = _string.rfind(trim.c_str());
		if(-1 != n)
		{
			if(n + trim.size() == _string.size())
			{
				_string.erase(_string.begin()+n,_string.end());
			}
		}
		return n;
	}
	inline int trimrightW(std::wstring &_string,const std::wstring &trim)
	{
		int n = _string.rfind(trim.c_str());
		if(-1 != n)
		{
			if(n + trim.size() == _string.size())
			{
				_string.erase(_string.begin()+n,_string.end());
			}
		}
		return n;
	}
	// end-删除字符串-end

	// begin-字符串分割-begin
	template<class T>
	struct tagSep
	{
		// std::basic_string的后面2个参数不用管，等价于basic_string<T>
		typedef std::basic_string<T,std::char_traits<T>,std::allocator<T>>Type;
		typedef std::vector<Type> Vector;
		static void separate(const Type& lpSymbo,const Type& lpstr,Vector &vct)
		{
			separate_cb(lpSymbo,lpstr,[&](const Type& str)
			{
				vct.push_back(str);
			});
		}

		static void separate_cb(const Type& lpSymbo,const Type& lpstr,std::function<void(const Type&)>fn)
		{
			int nSymbo = lpSymbo.size();
			Type _temp(lpstr);// 生成副本
			_temp += lpSymbo;// 追加一个分割符合，方便下面计算
			auto it = _temp.begin();
			auto it_begin = _temp.begin();
			while(it != _temp.end())
			{
				if((it + nSymbo) == _temp.end() || !memcmp(&(*it),lpSymbo.c_str(),nSymbo))
				{
					if(it != it_begin)
					{// 避免空串（连续分隔符的时候，不产出空元素）
						Type ele;
						ele.resize(it-it_begin);
						copy(it_begin,it,ele.begin());// 把[it_begin,it)区间的字符复制到ele.begin()
						fn(ele);
					}
					it += nSymbo;// 找到了直接一次性跳过整个分隔符
					it_begin = it;// 重新记录起点
				}
				else
				{
					it++;
				}
			}

		}

		static int trim(Type&src,const Type& tag)
		{

			return 0;
		}

	};

	// 将一个字符串，以某个分割字符为基准，分割字符串并存入数组
	inline void separateA(char c,const char* lpstr,vec_strA & vct)
	{// 窄字节
		std::string _temp(lpstr);
		trimA(_temp);
		_temp += c;// 追加一个分割符合，方便以下计算
		std::string::iterator it = _temp.begin();
		std::string::iterator it_begin = _temp.begin();
		for(;it != _temp.end();it++)
		{
			if(*it == c || (it+1) == _temp.end())
			{
				if(it != it_begin)
				{// 如果起始指针不等于当前指针，说明这一段不是空内容
					std::string line;
					line.resize(it-it_begin);
					copy(it_begin,it,line.begin());// 把[it_begin,it)区间的字符复制到line
					vct.push_back(line);// 记录
				}
				it_begin = it + 1;// 重新标记起点，加1是为了跳过那个分隔符
			}
		}
	}

	inline void separateW(wchar_t c,const wchar_t* lpstr,vec_strW & vct)
	{// 宽字节
		std::wstring _temp(lpstr);
		trimW(_temp);
		_temp += c;// 追加一个分割符合，方便以下计算
		std::wstring::iterator it = _temp.begin();
		std::wstring::iterator it_begin = _temp.begin();
		for(;it != _temp.end();it++)
		{
			if(*it == c || (it+1) == _temp.end())
			{
				if(it != it_begin)
				{// 如果起始指针不等于当前指针，说明这一段不是空内容
					std::wstring line;
					line.resize(it-it_begin);
					copy(it_begin,it,line.begin());// 把[it_begin,it)区间的字符复制到line
					vct.push_back(line);// 记录
				}
				it_begin = it + 1;// 重新标记起点，加1是为了跳过那个分隔符
			}
		}
	}

	// 多字符字符串分割
	// 第一个参数是多个分割字符，第二个参数是源字符串，存放数组
	inline void separateExA(const char* lpSymbo,const char* lpstr,vec_strA &vct)
	{
		assert(lpSymbo && lpstr);
		if(!lpSymbo || !lpstr || !strlen(lpSymbo) || !strlen(lpstr))
		{
			return;
		}
		
		int nSymbo = strlen(lpSymbo);
		std::string _temp(lpstr);
		trimA(_temp);
		_temp += lpSymbo;// 追加一个分割符号，方便计算

		std::string::iterator it = _temp.begin();
		std::string::iterator it_begin = _temp.begin();// 记录起点
		while(it != _temp.end())
		{
			if((it + nSymbo) == _temp.end() || !strncmp(&(*it),lpSymbo,nSymbo))
			{ // 防止后面剩余字符长度不够一个分割符，strncmp越界访问内存
			  // 用strncmp拿从it开始往后nSymbo长度的一串字符和lpSymbo对比
				if(it != it_begin)
				{
					// 有效数据
					std::string line;
					line.resize(it-it_begin);
					copy(it_begin,it,line.begin());
					vct.push_back(line);
				}
				it+=nSymbo;
				it_begin = it;
			}
			else{
				it++;
			}
		}
	}
	inline void separateExW(const wchar_t* lpSymbo,const wchar_t* lpstr,vec_strW &vct)
	{
		assert(lpSymbo && lpstr);
		if(!lpSymbo || !lpstr || !wcslen(lpSymbo) || !wcslen(lpstr))
		{
			return;
		}

		int nSymbo = wcslen(lpSymbo);
		std::wstring _temp(lpstr);
		trimW(_temp);
		_temp += lpSymbo;// 追加一个分割符号，方便计算

		std::wstring::iterator it = _temp.begin();
		std::wstring::iterator it_begin = _temp.begin();// 记录起点
		while(it != _temp.end())
		{
			if((it + nSymbo) == _temp.end() || !wcsncmp(&(*it),lpSymbo,nSymbo))
			{ 
				if(it != it_begin)
				{
					// 有效数据
					std::wstring line;
					line.resize(it-it_begin);
					copy(it_begin,it,line.begin());
					vct.push_back(line);
				}
				it+=nSymbo;
				it_begin = it;
			}
			else{
				it++;
			}
		}
	}
	// end-字符串分割-end

	// begin-字符串过滤/删除-begin
	// 把字符串里所以匹配str的子串全部删除，返回一共删除了多少次
	inline int filterA(std::string &_string,const char* str)
	{
		int filter_count = 0;
		if(str && 0 != strlen(str))
		{
			while(true)
			{
				int n = _string.find(str);
				if(-1 == n)
					break;// 没找到退出
				_string.erase(_string.begin()+n,_string.begin()+n+strlen(str));
				filter_count++;
			}
		}

		return filter_count;
	}
	inline int filterW(std::wstring &_string,const wchar_t* str)
	{
		int filter_count = 0;
		if(str && 0 != wcslen(str))
		{
			while(true)
			{
				int n = _string.find(str);
				if(-1 == n)
					break;// 没找到退出
				_string.erase(_string.begin()+n,_string.begin()+n+wcslen(str));
				filter_count++;
			}
		}

		return filter_count;
	}
	// end-字符串过滤/删除-end

	// begin-字符串查找替换-begin
	// _string源字符，src查找子串，dst替换子串
	inline int replaceA(std::string &_string,const char* src,const char* dst)
	{
		int replace_count = 0;
		if(src && 0 != strlen(src) && dst)
		{
			std::string s_dst(dst);
			int offset = 0;
			while(true)
			{
				int n = _string.find(src,offset);// 子串，从什么地方开始
				if(-1 == n)
					break;// 没找到返回
				auto it = _string.erase(_string.begin()+n,_string.begin()+n+strlen(src));
				replace_count++;

				// 再插入
				if(s_dst.size())
				{
					_string.insert(it,s_dst.begin(),s_dst.end());// 位置，迭代器范围
					offset = n + s_dst.size();
				}
			}
		}

		return replace_count;
	}

	// pos表示从第几个开始替换，count表示最多替换多少个
	inline int replacePosA(std::string & _string,const char* src,const char* dst,int pos = 1,int count = 1000)
	{
		int replace_count = 0;

		if(src && 0 != strlen(src) && dst)
		{
			std::string s_dst(dst);
			int offset = 0;
			int find_count = 0;
			while(true)
			{
				int n = _string.find(src,offset);
				if(-1 == n)
					break;//没找到退出
				++find_count;
				if(find_count < pos)
				{
					offset+=strlen(src);
					continue;// 还没有到pos，开始替换
				}

				if(replace_count >= count)
				{
					break;// 超过最多替换
				}

				auto it = _string.erase(_string.begin()+n,_string.begin()+n+strlen(src));

				replace_count++;
				if(s_dst.size())
				{
					_string.insert(it,s_dst.begin(),s_dst.end());
					offset = n + s_dst.size();
				}

			}
		}

		return replace_count;
	}
	// _string源字符，src查找子串，dst替换子串
	inline int replaceW(std::wstring &_string,const wchar_t* src,const wchar_t* dst)
	{
		int replace_count = 0;
		if(src && 0 != wcslen(src) && dst)
		{
			std::wstring s_dst(dst);
			int offset = 0;
			while(true)
			{
				int n = _string.find(src,offset);// 子串，从什么地方开始
				if(-1 == n)
					break;// 没找到返回
				auto it = _string.erase(_string.begin()+n,_string.begin()+n+wcslen(src));
				replace_count++;

				// 再插入
				if(s_dst.size())
				{
					_string.insert(it,s_dst.begin(),s_dst.end());// 位置，迭代器范围
					offset = n + s_dst.size();
				}
			}
		}

		return replace_count;
	}
	// end-字符串查找替换-end

	// begin-字符串黑名单删除-begin
	// [单字符]把src里面出现在tag里的字符全部删除(tag是一个字符黑名单集合)
	inline void span_include(std::string &src,const std::string &tag)
	{
		for(auto it = tag.begin();it != tag.end();it++)
		{
			for(auto iy = src.begin();iy != src.end();)
			{
				if(*it == *iy)
					iy = src.erase(iy);
				else
					iy++;
			}
		}
	}
	inline void span_includeW(std::wstring &src,const std::wstring &tag)
	{
		for(auto it = tag.begin();it != tag.end();it++)
		{
			for(auto iy = src.begin();iy != src.end();)
			{
				if(*it == *iy)
					iy = src.erase(iy);
				else
					iy++;
			}
		}
	}
	// end-字符串黑名单删除-end

	// begin-宽窄字符转换-begin
	// ANSI 窄字符串 → UTF-16 宽字符串
	inline std::wstring AnsiToUnicode(const std::string& ansi)
	{
		std::wstring utf16;
		int len = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), ansi.size(), NULL, 0);
		if (len > 0)
		{
			utf16.resize(len);
			wchar_t* p = &*utf16.begin();
			MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), ansi.size(), p, len);
		}
		return utf16;
	}

	inline std::wstring AnsiToUnicode(LPCSTR lpstr)
	{
		if ( lpstr )
		{
			return AnsiToUnicode(std::string(lpstr));
		}
		return L"";
	}
	inline std::string UnicodeToAnsi(const std::wstring& utf16)
	{
		std::string ansi;
		int len= WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), utf16.size(), NULL, 0, NULL, NULL);
		if (len > 0)
		{
			ansi.resize(len);
			char* p = &*ansi.begin();
			WideCharToMultiByte ( CP_ACP, 0, utf16.c_str(), utf16.size(), p, len, NULL, NULL );
		}
		return ansi;
	}

	inline std::string UnicodeToAnsi(LPCWSTR lpstr)
	{
		if ( lpstr )
		{
			return UnicodeToAnsi(std::wstring(lpstr));
		}
		return "";
	}

	// end-宽窄字符转换-end

	// begin-数值转字符串-begin
	static tstr to_str(char val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(unsigned char val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(short val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(unsigned short val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(int val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(unsigned int val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(long val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(unsigned long val){
		return WFmt(_T("%d"),val);
	}
	static tstr to_str(float val){
		return WFmt(_T("%f"),val);
	}
	static tstr to_str(double val){
		return WFmt(_T("%g"),val);
	}
	// end-数值转字符串-end

	// begin-时间-begin
	static wtime_tt now_tt()
	{
		return time(0);
	}
	static wtime_st now_st()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		return st;
	}
	static wtime_tt to_time(const wtime_st &st)
	{
		tm t;
		t.tm_year = st.wYear - 1900;
		t.tm_mon  = st.wMonth - 1;
		t.tm_mday = st.wDay;
		t.tm_wday = st.wDayOfWeek;
		t.tm_hour = st.wHour;
		t.tm_min  = st.wMinute;
		t.tm_sec  = st.wSecond;
		t.tm_yday = 0;
		t.tm_isdst= 0;
		return mktime(&t);
	}

	static wtime_st to_time(wtime_tt tt)
	{
		tm t = *localtime(&tt);
		wtime_st st;
		st.wYear         = t.tm_year + 1900;
		st.wMonth        = t.tm_mon + 1;
		st.wDay          = t.tm_mday;
		st.wDayOfWeek    = t.tm_wday;
		st.wHour         = t.tm_hour;
		st.wMinute       = t.tm_min ;
		st.wSecond       = t.tm_sec ;
		st.wMilliseconds = t.tm_isdst;
		return st;
	}
	static wtime_st to_time(wtime_tv tv)
	{
		wtime_st st = to_time((wtime_tt)tv.tv_sec);
		st.wMilliseconds = tv.tv_usec/1000;
		return st;
	}
	// end-时间-end

	// begin-时间格式化输出-begin
	static tstr to_str( const wtime_st & st,LPCTSTR fmt=_T("yyyy-MM-dd HH:mm:ss"))
	{
		tstr str(fmt);
		if ( -1 != str.find(_T("yyyy")) )
		{
			// 把这个str中子字符串"yyyy"替换为st.wYear，也就是2027，2027-MM-dd HH:mm:ss
			replaceA(str,_T("yyyy"),to_str(st.wYear).c_str());
		}
		if ( -1 != str.find(_T("MM")) )
		{
			replaceA(str,_T("MM"),WFmt(_T("%02d"),st.wMonth).c_str());
		}
		if ( -1 != str.find(_T("dd")) )
		{
			replaceA(str,_T("dd"),WFmt(_T("%02d"),st.wDay).c_str());
		}
		if ( -1 != str.find(_T("HH")) )
		{
			replaceA(str,_T("HH"),WFmt(_T("%02d"),st.wHour).c_str());
		}
		if ( -1 != str.find(_T("mm")) )
		{
			replaceA(str,_T("mm"),WFmt(_T("%02d"),st.wMinute).c_str());
		}
		if ( -1 != str.find(_T("ss")) )
		{
			replaceA(str,_T("ss"),WFmt(_T("%02d"),st.wSecond).c_str());
		}
		return str;
	}
	static tstr to_str(const wtime_tt & tt,LPCTSTR fmt = _T("yyyy-MM-dd HH:mm:ss")){
		return to_str(to_time(tt),fmt);
	}
	static tstr sec3timestr(int sec)
	{
		return WFmt(_T("%02d:%02d:%02d"),sec/3600,sec/60%60,sec%60);// 10:23:01 时分秒
	}
	static wtime_tt to_tt( const char * fmt){
		wtime_tt tt;
		vec_tstr v1;

		tstr str(fmt);
		replaceA(str," ",",");
		separateA(',',str.c_str(),v1);

		if ( v1.size() != 2 )
		{
			assert(0);
		}else
		{
			vec_tstr v2,v3;
			separateA('-',v1[0].c_str(),v2);
			separateA(':',v1[1].c_str(),v3);

			if ( v2.size() != 3 || v3.size() != 3 )
			{
				assert(0);
			}else
			{
				wtime_st st = now_st();
				st.wYear = atoi(v2[0].c_str());
				st.wMonth = atoi(v2[1].c_str());
				st.wDay = atoi(v2[2].c_str());
				st.wHour = atoi(v3[0].c_str());
				st.wMinute = atoi(v3[1].c_str());
				st.wSecond = atoi(v3[2].c_str());
				return to_time(st);
			}
		}

		return time(0);
	}

	static wtime_st to_st( const char * fmt){

		return to_time(to_tt(fmt));
	}

	static wtime_st to_st( wtime_tt tt){

		return to_time(tt);
	}
	static UINT to_sec(wtime_st st)
	{
		return st.wHour*3600+st.wMinute*60+st.wSecond;
	}

	static UINT to_sec(wtime_tt tt)
	{
		return to_sec(to_time(tt));
	}


	static wtime_tt now_tt_ex(int hour,int min,int sec)
	{
		wtime_st st;
		GetLocalTime(&st);
		st.wHour = hour;
		st.wMinute = min;
		st.wSecond = sec;
		return to_time(st);
	}

	static wtime_tv now_tv()
	{
		wtime_st st;
		GetLocalTime(&st);
		wtime_tv tv;
		tv.tv_sec = (wtime_tt)to_time(st);
		tv.tv_usec = st.wMilliseconds * 1000;
		return tv;
	}

	static tstr time_str(wtime_st t)
	{
		return to_str(t,_T("HH:mm:ss"));
	}
	static tstr time_str(wtime_tt t)
	{
		return to_str(t,_T("HH:mm:ss"));
	}
	static tstr time_str(int t)
	{
		// 好像无用
		return to_str(t,_T("HH:mm:ss"));
	}
	// end-时间格式化输出-end

};

// decltype(*v.begin())表示推导这个数据的类型
// vector<Student>vecStu;
// 使用：auto it = WAFX_FINDIF(vecStu,s,return s.id == 1001);
// 等价于：auto it = std::find_if(vecStu.begin(),vecStu.end(),[](Student&s){return s.id == 1001;});
#define WAFX_FINDIF(v,ref,exc) \
	WAFX::WAFX_findif(v,[&](decltype(*v.begin()) &ref)->bool \
	{\
		exc;\
	})

#define WToStr WAFX::to_str										// 将任意int,char转为字符串
#define WDelete WAFX::_WDelete									// 资源删除器，模版特化
#define WRelease(p) if(p)p->Release();							// 调用对象的Release()成员函数

#define WTimeStr WAFX::time_str									// 将任意时间变为HH::mm::ss格式字符串
#define WNowTT	WAFX::now_tt()									// 拿到当前的UTC时间戳
#define WNowST	WAFX::now_st()									// Windows API 结构体,里面有年月日
#define WNowTimeStr WAFX::time_str(WNowTT).c_str()				// 拿到当前UTC时间戳，转为SYSTEMTIME结构体，再转为HH::mm::ss

#define WTT2Str(tt) WAFX::time_str(tt).c_str()					// 将UTC时间变为HH::mm::ss格式字符串
#define WST2Str(st) WAFX::time_str(st).c_str()					// 将Window的SYSTEMTIME结构体变为HH::mm::ss格式字符串

#define WStr WAFX::tstr  // 自动字符数组
#define WStrVct WAFX::vec_tstr // 自动字符数组
#define WStrA	std::string		// 窄字符串
#define WStrW	std::wstring	// 宽字符串
#define WStrVctA WAFX::vec_strA	// 窄字符数组
#define WStrVctW WAFX::vec_strW // 宽字符数组

#define WPath WAFX::exepath		// WPath(config.xml)拿到同当前exe文件夹下的config.xml

#define WTimeTT WAFX::wtime_tt	// UTC时间戳类型
#define WTimeST WAFX::wtime_st	// Window下的时间结构体类型
#define WT2T(t) WAFX::to_time(t)// 将UTC时间戳类型转为windows下的时间结构体类型

#define _TSTRCPY _tcscpy_s // 字符串拷贝：TCHAR*dst,size_t dstSize,const TCHAR*src
#define	_TSTRCAT _tcscat_s // 字符串追加：TCHAR*dst,size_t dstSize,const TCHAR*src

#ifndef _UNICODE
	#define  WFilter WAFX::filterA		// 字符串过滤	=删除指定子串
	#define WTrim WAFX::trimA // 去除空格，制表符
	#define WTrimLeft WAFX::trimleftA // 字符串最左边出现子串，就删除
	#define WTrimRight WAFX::trimrightA // 字符串最右边出现子串，就删除
	#define WReplace WAFX::replaceA	// 将字符串中某个子串，替换为另一个子串
	#define WReplacePos WAFX::replacePosA // 将字符串中某个子串(可以限制从第几个开始)，替换为另一个子串
	#define WSeparate WAFX::separateA	// 将一个字符串以某个分割字符为基准，分割字符串并存入数组，separateA(',',"123,456,678",v);
	#define WSeparateEx WAFX::separateExA	// 将一个字符串以某多个分割字符为基准，分割字符串并存入数组，separateExA('||',"123||456||678",v);
	#define WSpan		WAFX::span_include // 把一个字符串和另一个字符串相同的所有单个字符删除

	#define W_T2W(s)	WAFX::AnsiToUnicode(s) // ANSI窄字符串-转换-UTF16宽字节
	#define W_T2A(s)	(s)// 无用?
	#define W_A2T(s)	(s)// 无用?

	#define WPathFmt	WAFX::PathFmtA	// 拿到当前exe所在目录，再拼接格式化子路径(窄字符)
#else
	#define WFilter        WAFX::filterW
	#define WTrim          WAFX::trimW
	#define WTrimLeft      WAFX::trimleftW
	#define WTrimRight     WAFX::trimrightW
	#define WReplace       WAFX::replaceW
	#define WSeparate      WAFX::separateW
	#define WSeparateEx    WAFX::separateExW
	#define WSpan          WAFX::span_includeW
	#define W_T2W(s)       (s)
	#define W_T2A(s)       UnicodeToAnsi(s)
	#define W_A2T(s)       AnsiToUnicode(s)
	#define WPathFmt       WAFX::PathFmtW

#endif

#define W_A2W(s) UnicodeToAnsi(s) // 这个名字好像有问题??
#define W_W2A(s) AnsiToUnicode(s) // 这个名字好像有问题??