#pragma once
#include <string>
#include <functional>


typedef const std::string cstr;

// 二进制文本文件生成器
// 由于全部都是static,则直接CATSFile::Save
class CATSFile
{
public:
    enum emFromType
    {
        from_file = 0,// 文件
        from_data = 1,// 已归档的内存块
        from_buff = 2,// 已归档的内存块
        from_excel = 3// 已归档的内存块
    };
public:
    typedef std::function<bool(cstr &name,cstr &format,cstr &userdata,cstr &data)> TypeCall;
public:
    CATSFile();
    //读取文件
    static int Read(cstr &file,std::string &data);
    //保存文件
    static void Save(cstr &file,cstr &buf);
    //保存文件
    static void Save(cstr &file,void * data,int size);
    //
    static void AddDb(cstr &file,void * data,int size,cstr &name,cstr &format,cstr &userdata,bool cover=false);
    //保存文件
    static void Save(cstr &file,cstr &from,int from_type,cstr &name,cstr &format,cstr &userdata,bool cover=false);
    //解析文件
    static void Parse(cstr &from,int from_type,TypeCall fn);
    //
    static void Delete(cstr &file,cstr &name,cstr &format);
    //保存文件
    static void Appand(cstr &file,cstr &buf);
    //
    static void Test();
    // 在输出窗口中, 打印相关信息
    static void Trace(cstr &file);
    //
    static bool Remove(std::string &data,cstr &name,cstr &format,int count=1);

    static bool Remove2(cstr &file,cstr &name,cstr &format,int count=1);

    static bool Rename(cstr &file,cstr &oldName,cstr &newName);
    //
    static void Serialize(CArchive&ar);// CArchive 是 MFC 的序列化类
};
