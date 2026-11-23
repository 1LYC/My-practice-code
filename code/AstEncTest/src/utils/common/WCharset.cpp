#include "stdAfx.h"
#include "WCharset.h"
#include <assert.h>
#include <windows.h>
#include <string>
using namespace std;

// 二进制转十六进制字符串
// 源数据, 源数据的长度, 2倍源数据大小的缓冲区, 是否按大写字母编码, 返回编码后的长度
size_t Bin2Hex(const void* data, size_t size, char* digest, bool upcase /*= false*/)
{
    assert(data);
    assert(digest);
    if (size == 0){
        return 0;
    }

    static const char* szConvert1="0123456789ABCDEF";
    static const char* szConvert2="0123456789abcdef";
    const char* szConvert = upcase? szConvert1 : szConvert2;

    // 强制转成unsigned char, 避免符号位干扰移位运算
    unsigned char* src = (unsigned char*)data;
    for (size_t i=0; i<size; i++)
    {// 1110 0100
        digest[i*2] = szConvert[src[i] >> 4];// 1110 = 14 也就是E
        digest[i*2+1] = szConvert[src[i] & 0x0F];// 0100 = 4 也就是4
    }
    // 一个字节 8bit, 分成高 4bit、低 4bit
    // 每 1 个二进制字节, 产出 2 个 hex 字符, 总长度是size2
    return size*2;
}
void Bin2Hex(const void* data, size_t size, std::string& digest, bool upcase /*= false*/)
{
    digest.resize(size*2);
    Bin2Hex(data, size, const_cast<char*>(digest.c_str()), upcase);
}
std::string Bin2Hex(const void* data, size_t size, bool upcase /*= false*/)
{
    std::string digest;
    Bin2Hex(data, size, digest, upcase);
    return digest;
}

// 十六进制转二进制字符串
// 源数据, 源数据的长度, 1/2源数据大小的缓冲区, 返回编码后的长度
size_t Hex2Bin(const char* hex, size_t size, void* digest)
{
    // 自定义的ASCII码表转换, 数据-'0'的下标, 对应的数组值
    static const unsigned char nConvert[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
        -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,10,11,12,13,14,15};

    size_t j = 0;
    // 强制转成unsigned char, 避免符号位干扰移位运算
    unsigned char* dst = (unsigned char*)digest;
    for (size_t i=0; i<size; i++)
    {
        if (((hex[i] >= '0') && (hex[i] <= '9')) || ((hex[i] >= 'a') && (hex[i] <='f')) || ((hex[i] >= 'A') && (hex[i] <='F')))
        {// "E4"
            if (i%2)
            {  // i=1 (奇数): 字符`4`, 值 = 4, `dst[0] |= 0x04` -> `0xE4`; j++
                dst[j++] |= nConvert[hex[i]-'0'];// 放入低地址
            }else{
                // i=0 (偶数): 字符`E`, 值 = 14, 14 <<4,
                dst[j] = (nConvert[hex[i]-'0'] << 4);// 放入高地址
            }
        }else{
            break;
        }
    }
    return j;
}

// 十六进制转二进制字符串
size_t Hex2Bin(const std::string& hex, void* digest)
{
    return Hex2Bin(hex.c_str(), hex.length(), digest);
}

// 把UTF16编码的字符串转为UTF8编码
std::string UTF16toUTF8(const std::wstring& utf16)
{
    std::string utf8;
#ifdef _WIN32
    int len= WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), utf16.size(), NULL, 0, NULL, NULL);
    if (len > 0)
    {
        utf8.resize(len);
        char* p = &*utf8.begin();
        WideCharToMultiByte ( CP_UTF8, 0, utf16.c_str(), utf16.size(), p, len, NULL, NULL );
    }
#else
    // 这里需要补充linux下的转换代码
#endif
    return utf8;
}

// 把UTF8编码的字符串转为UTF16编码
std::wstring UTF8toUTF16(const std::string& utf8)
{
    std::wstring utf16;
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
    if (len > 0)
    {
        utf16.resize(len);
        wchar_t* p = &*utf16.begin();
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), p, len);
    }
#else
    // 这里需要补充linux下的转换代码
#endif
    return utf16;
}

// 把ANSI编码的字符串转为UTF16编码
std::wstring AnsitoUTF16(const std::string& ansi)
{
    std::wstring utf16;
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), ansi.size(), NULL, 0);
    if (len > 0)
    {
        utf16.resize(len);
        wchar_t* p = &*utf16.begin();
        MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), ansi.size(), p, len);
    }
#else
    // 这里需要补充linux下的转换代码
#endif
    return utf16;
}

// 把UTF16编码的字符串转为ANSI编码
std::string UTF16toAnsi(const std::wstring& utf16)
{
    std::string ansi;
#ifdef _WIN32
    int len= WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), utf16.size(), NULL, 0, NULL, NULL);
    if (len > 0)
    {
        ansi.resize(len);
        char* p = &*ansi.begin();
        WideCharToMultiByte ( CP_ACP, 0, utf16.c_str(), utf16.size(), p, len, NULL, NULL );
    }
#else
    // 这里需要补充linux下的转换代码
#endif
    return ansi;
}

// 把UTF8编码的字符串转为ANSI编码
std::string UTF8toAnsi(const std::string& utf8)
{
    return UTF16toAnsi(UTF8toUTF16(utf8));
}

// 把ANSI编码的字符串转为UTF8编码
std::string AnsitoUTF8(const std::string& ansi)
{
    return UTF16toUTF8(AnsitoUTF16(ansi));
}

// 判断是否有中文
bool isHaveChinese(const std::string& strName)
{
    // GBK 汉字: 2 字节
    // 两个字节的最高 bit 都是 1
    bool bRet = false;
    char szData[50] = {0};
    sprintf_s(szData, _countof(szData), "%s", strName.c_str());
    for (int i = 0; i < strlen(szData); i++)
    {
        if (szData[i] < 0)
        {// 而把0xD6 当作有符号 char, 最高bit是1, 是符号位, 是负数
            bRet = true;
            break;
        }
    }
    // 注: 不能用在 UTF8 场景, 也不是严谨的汉字校验函数
    return bRet;
}
