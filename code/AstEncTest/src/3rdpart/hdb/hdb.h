#ifndef _HDB_HEAD_
#define _HDB_HEAD_
#pragma once

#ifndef HDB_STATIC
#  ifdef HDB_DLL
#    define HDB_CLASS __declspec(dllexport)
#  else
#    define HDB_CLASS __declspec(dllimport)
#  endif
#else
#  define HDB_CLASS
#endif

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif

#define HDB_API EXTERN_C HDB_CLASS

typedef void * HDB_HANDLE;

HDB_API int          HDB_Init();
HDB_API void         HDB_Release();

HDB_API HDB_HANDLE   HDB_Create();
HDB_API void         HDB_Delete(HDB_HANDLE&);
HDB_API void         HDB_Encode(HDB_HANDLE,const char* charset);
HDB_API int          HDB_query(HDB_HANDLE,const char*);

//设置事务方式
HDB_API bool         HDB_autocommit(HDB_HANDLE,bool);
//启动事务
HDB_API bool         HDB_begintrans(HDB_HANDLE);
//回滚事务
HDB_API bool         HDB_rollback(HDB_HANDLE);
//提交事务
HDB_API bool         HDB_commit(HDB_HANDLE);

//成功0，失败-1
HDB_API int          HDB_Connect(HDB_HANDLE,const char *host, const char *user, const char * passwd, const char * db, unsigned int port);
HDB_API int          HDB_Execute(HDB_HANDLE,const char *parameters, ...);
HDB_API int          HDB_ExecuteProcedure(HDB_HANDLE,const char *proc, const char *parameters, ...);
HDB_API int          HDB_Insert(HDB_HANDLE,const char *parameters, ...);
HDB_API int          HDB_Update(HDB_HANDLE,const char *parameters, ...);
HDB_API int          HDB_Select(HDB_HANDLE,const char *parameters, ...);


HDB_API int          HDB_DeleteSQL(HDB_HANDLE,const char *parameters, ...);
HDB_API int          HDB_QueryResult(HDB_HANDLE);
HDB_API int          HDB_Count(HDB_HANDLE);
HDB_API int          HDB_IsEof(HDB_HANDLE);
HDB_API int          HDB_MoveNext(HDB_HANDLE);
HDB_API int          HDB_GetFieldCount(HDB_HANDLE);
HDB_API char*        HDB_GetFieldName(HDB_HANDLE,int index);
HDB_API char*        HDB_GetFiledValueByName(HDB_HANDLE,const char *name);
HDB_API int          HDB_FindFieldName(HDB_HANDLE,const char *name);
HDB_API char *       HDB_GetFiledValue(HDB_HANDLE,int index);

#ifdef __cplusplus

#include <functional>
HDB_API int          HDB_SelectCallback(HDB_HANDLE,const char *sql,std::function<void(HDB_HANDLE)>);

#endif

#endif
