#include "StdAfx.h"
#include "ATSFile.h"
#include <iostream>
#include <fstream>



struct stAtsFile
{
    stAtsFile() {
        ZeroMemory(this,sizeof(*this));
    }
    int  file_size;// 文件大小-总字节长度
    int  head_size;// 头部大小
    int  data_size;// 数据大小-纯业务数据大小 |
    char format[16];
    char name[256];
    char userdata[128];
};



CATSFile::CATSFile()
{

}

// 把整个二进制文件一次性全部读入 std::string buf缓冲区, 返回到的字节长度
int CATSFile::Read(cstr &file,std::string &buf)
{
    auto size = 0;
    // 打开文件: ios::out 这里是【bug】, ifstream 读文件不该加 ios::out
    std::ifstream ifs(file.c_str(),std::ios::out|std::ios::binary|std::ios::ate);
    if (ifs.is_open())
    {
        size = (int)ifs.tellg();// ate模式打开, 文件指针直接在末尾, tellg拿到文件总字节数
        if ( size > 0 )
        {
            ifs.seekg (0, std::ios::beg);// 把文件指针挪回文件开头, 准备从头读
            buf.resize(size);          // 给string分配足够内存, 大小等于文件字节数
            ifs.read ((char*)buf._Myptr(), size);// 一次性读取全部内容到string内部缓冲区
            ifs.close();
        }
        return size;// 返回文件大小 (读到多少字节)
    }
}

// 自定义打包文件的保存函数
// 目标归档/保存文件路径, 数据源, 数据源的类型, 标识符, 格式标记, 自定义附加备注信息, 是否允许覆盖同名记录
// CATSFile::AddDb(db.c_str(),data,size,"Xioa_interlock","excel","base",true);

void CATSFile::Save(cstr &file,cstr &from,int from_type,cstr &name,cstr &format,cstr &userdata,bool cover)
{
    std::string org;

    // 先把整个目标归档文件全部读到org
    Read(file.c_str(),org);

    // 在内存里把旧文件中 name+format, Xioa_interlock+exec1匹配的老记录删掉
    if (Remove(org,name,format,100))
    {
        cover = true;
    }

    // std::ios::app追加
    auto flag = std::ios::app|std::ios::app|std::ios::binary;
    if (cover)
    {// 如果覆盖就清空
        flag = std::ios::binary|std::ios::out | std::ios::trunc;
    }


    std::string buf;
    auto size = 0;
    if (CATSFile::from_file == from_type)
    {// 文件路径
        Read(from.c_str(),buf);
        size = buf.size();
    }else
    {// 二进制业务数据
        size = from.size();
    }
    auto data = from_type==CATSFile::from_data? from.c_str():buf.c_str();

    std::ofstream out(file.c_str(),flag);
    if (out.is_open())
    {
        stAtsFile head;
        head.head_size = sizeof(stAtsFile);
        head.file_size = sizeof(stAtsFile)+size;
        head.data_size = size;
        strcpy_s(head.name,name.c_str());
        strcpy_s(head.format,format.c_str());
        strcpy_s(head.userdata,userdata.c_str());

        if (org.size())
        {// 剔除掉旧同名记录之后剩下的原有内容,写回去
            out.write(org.c_str(),org.size());
        }

        out.write((const char*)&head,sizeof(head));
        out.write(data,size);
        out.close();
    }
}


// 直接传入一块内存首地址 + 字节长度, 没有 "从文件读取" 这个分支
void CATSFile::AddDb(cstr &file,void * data,int size,cstr &name,cstr &format,cstr &userdata,bool cover)
{
    std::string org;

    //先去重, 只要是存在重复强制覆盖
    Read(file.c_str(),org);

    if (Remove(org,name,format,100))
    {
        cover = true;
    }


    auto flag = std::ios::app|std::ios::binary;

    if (cover)
    {
        flag = std::ios::binary|std::ios::out | std::ios::trunc;
    }

    std::ofstream out(file.c_str(),flag);
    if (out.is_open())
    {
        stAtsFile head;
        head.head_size = sizeof(stAtsFile);
        head.file_size = sizeof(stAtsFile)+size;
        head.data_size = size;
        strcpy_s(head.name,name.c_str());
        strcpy_s(head.format,format.c_str());
        strcpy_s(head.userdata,userdata.c_str());

        if (org.size())
        {
            out.write(org.c_str(),org.size());
        }

        out.write((const char*)&head,sizeof(head));
        out.write((const char*)data,size);
        out.close();
    }
}
void CATSFile::Save(cstr &file,cstr &buf)
{
    Save(file,(void*)buf.c_str(),buf.size());
}

// 二进制追加
void CATSFile::Appand(cstr &file,cstr &buf)
{
    std::ofstream out(file.c_str(),std::ios::app|std::ios::binary);
    if (out.is_open())
    {
        out.write(buf.c_str(),buf.size());
        out.close();
    }
}

// 纯二进制覆盖写入
void CATSFile::Save(cstr &file,void * data,int size)
{
    auto flag = std::ios::app|std::ios::binary;
    if (true)
    {
        flag = std::ios::binary|std::ios::out | std::ios::trunc;
    }

    std::ofstream out(file.c_str(),flag);
    if (out.is_open())
    {
        if (size > 0)
        {
            out.write((const char *)data,size);
        }
        out.close();
    }
}

/*
void CATSFile::Save(cstr &file)
{

    CFile cf;
    auto rs = cf.Open(file,CFile::modeRead|CFile::shareExclusive);
    if(rs)
    {
        CArchive ar(&cf, CArchive::load|CArchive::bNoFlushOnDelete);
        Serialize(ar);
        ar.Close();
        cf.Close();
    }

    CMemFile mem((LPBYTE)data,size);
    CArchive ar(&mem, CArchive::load|CArchive::bNoFlushOnDelete);
    Serialize(ar);
}
*/

// 解析拆包
// 文件路径, 文件类型, 回调函数
void CATSFile::Parse(cstr &from,int from_type,CATSFile::TypeCall fn)
{
    std::string buf;
    auto size = 0;
    if (CATSFile::from_file == from_type)
    {// 文件
        Read(from.c_str(),buf);
        size = buf.size();
    }else
    {// 归档二进制内存块
        size = from.size();
    }

    auto data = from_type==CATSFile::from_data? from.c_str():buf.c_str();
    auto i = 0;
    while( i < size)
    {
        if (size - i < sizeof(stAtsFile))
        {
            break;// 剩下的数据不够一个头部, 直接退出 (防止越界)
        }
        // 11.name = Xioa_interlock,format = excel      ,user = base      , size = 369999
        stAtsFile * head = (stAtsFile*)&data[i];
        std::string name = head->name;
        std::string format = head->format;
        std::string userdata = head->userdata;
        std::string filedata;
        filedata.resize(head->data_size);
        memcpy(filedata._Myptr(),(void*)&data[i+head->head_size],head->data_size);// 拷贝-真正业务数据
        //TRACE("name=%s,format=%s,size=%d\n",name.c_str(),fomat.c_str(),head->data_size);
        fn(name,format,userdata,filedata);
        i += head->file_size;
    }
}

// 只操作内存缓冲区, 不会直接写磁盘文件
bool CATSFile::Remove(std::string &data,cstr &name,cstr &format,int count)
{
    auto &org = data;
    auto times = 0;
    for (auto it = org.begin(); it != org.end();)
    {
        auto head = (stAtsFile*)&(*it);
        if (name == head->name && format==head->format)
        {// 匹配上了, erase删掉 [当前这条记录全部字节]
            it = org.erase(it,it+head->file_size);
            if (++times >= count)
            {
                break;// 已经删除count条, 直接退出循环
            }
        }else
        {// 不匹配, 迭代器向后跳一整条记录的长度
            it += head->file_size;
        }
    }

    return times > 0;
}

//// Rename: 直接修改归档文件内记录的 name 字段, 把所有name == oldName
//// 找到并修改成功返回 true, 没找到返回 false
bool CATSFile::Rename(cstr &file,cstr &oldName,cstr &newName)
{
    std::string buf;
    Read(file.c_str(),buf); // 把整个归档文件读到内存buf
    auto data = buf.c_str();
    auto size = buf.size();
    auto i = 0;
    auto rs = false;
    while( i < size)
    {
        if (size - i < sizeof(stAtsFile))
        {
            break;// 剩余字节不够头部, 退出 (和Parse一样做了越界保护
        }
        stAtsFile * head = (stAtsFile*)&data[i];
        if (oldName == head->name)
        {
            strcpy_s(head->name,newName.c_str());// 原地覆盖name
            rs = true;
        }
        i += head->file_size;// 跳到下一条记录
    }

    if ( rs )
    {
        Save(file,buf);// 有修改, 把更新后的buf写回磁盘文件
    }

    return rs;
}

bool CATSFile::Remove2(cstr &file,cstr &name,cstr &format,int count)
{
    std::string buf;
    Read(file.c_str(),buf); // 读整个归档文件进内存
    auto rs = Remove(buf,name,format,count);// 内存里删除 (就是刚才那个Remove函数)
    if ( rs )
    {
        Save(file,buf);   // 有删除, 写回磁盘覆盖原文件
    }
    return rs;
}

// 有可能是以前的半成品
void CATSFile::Delete(cstr &file,cstr &name,cstr &format)
{
    std::string buf;
    Read(file.c_str(),buf);
    Remove(buf,name,format,100);// 最多删掉 100 条符合 `name+format` 的记录
    // 这里缺失save,所以不会写回磁盘
}

void CATSFile::Serialize(CArchive&ar)
{

}


// 读取归档文件的记录信息
void CATSFile::Trace(cstr &file)
{

    TRACE(WFmt(_T("▲ATSFile List(%s)\n"),file.c_str()).c_str());
    TRACE("\t+=========================\n");
    int count = 0;
    Parse(file.c_str(),CATSFile::from_file,[&](cstr &name,cstr &format,cstr &userdata,cstr &data)->bool
    {
        TRACE("\t·%2d.name = %-12s,format = %-12s,user = %-12s, size = %-d\n",++count,name.c_str(),format.c_str(),userdata.c_str(),data.size());
        return true;
    });
    TRACE("\t+=========================\n");
    TRACE("▲ATSFile List\n");
}

void CATSFile::Test()
{

    int a= 0;
}
