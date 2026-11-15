/*
	异步写日志
*/

#ifndef _WSLOGHEAD_H_
#define _WSLOGHEAD_H_
#pragma once

#ifndef WSL_STATIC
#	ifdef WSL_DLL
#		define WSL_CLASS __declspec(dllexport)
#	else 
#		define WSL_CLASS __declspec(dllimport)
#	endif
#else
#	define WSL_CLASS 
#endif


#ifndef EXTERN_C
#	ifdef __cplusplus
#		define EXTERN_C    extern "C"
#	else
#		define EXTERN_C    extern
#	endif
#endif

#define WSL_API EXTERN_C WSL_CLASS

typedef void * WSL_HANDLE;

//初始化
WSL_API int				WSL_Init();

//释放
WSL_API void			WSL_Release();

//创建
WSL_API WSL_HANDLE		WSL_Create(const char * filename);

//转换
WSL_API WSL_HANDLE		WSL_FromFileName(const char * filename);

//转换
WSL_API const char *	WSL_FromHandle(WSL_HANDLE);

//关闭
WSL_API void			WSL_Close(WSL_HANDLE&);

//关闭
WSL_API void			WSL_CloseAll();

//插入(bFlush为true时,输出到文件)
WSL_API int				WSL_Add(WSL_HANDLE ,bool bFlush,const char *fmt, ...);

//插入(bFlush为true时,输出到文件)
WSL_API int				WSL_Add2(const char * filename ,bool bFlush,const char *fmt, ...);

//输出到文件
WSL_API void			WSL_Flush(WSL_HANDLE);

//输出到文件
WSL_API void			WSL_Flush2(const char * filename);

#endif
