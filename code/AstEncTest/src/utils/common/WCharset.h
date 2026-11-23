#ifndef CHARSETSWITCH_H__
#define CHARSETSWITCH_H__

#include <string>

#ifdef _UNICODE
#define UTF8toSTRING    UTF8toUTF16
#define STRINGtoUTF8    UTF16toUTF8
#define STRINGtoANSI    UTF16toAnsi
#define ANSItoSTRING    AnsitoUTF16
#define UTF8toANSI      UTF8toAnsi
#else
#define UTF8toSTRING    UTF8toAnsi
#define STRINGtoUTF8    AnsitoUTF8
#define STRINGtoANSI
#define ANSItoSTRING
#define UTF8toANSI
#endif
// - GBK (ANSI) 数据 → 一般放在 `std::string` (char, 窄字节)
// - UTF8 数据 → 也一般放在 `std::string` (char, 窄字节)
// - UTF16 → 放在 `std::wstring` (wchar_t, 宽字节)

// - `char*` / `std::string` = 窄字符串 (多字节载体)
// - `wchar_t*` / `std::wstring` = 宽字符串 (UTF16 载体)
// - `LPSTR` = char*; `LPWSTR` = wchar_t*; `LPCTSTR` 自动切换
// - 非 UNICODE: `LPCTSTR` → `const char*` (ANSI/GBK)
// - UNICODE: `LPCTSTR` → `const wchar_t*` (UTF16)
size_t Bin2Hex(const void* data, size_t size, char* digest, bool upcase = false);
void Bin2Hex(const void* data, size_t size, std::string& digest, bool upcase = false);
std::string Bin2Hex(const void* data, size_t size, bool upcase = false);

size_t Hex2Bin(const char* hex, size_t size, void* digest);
size_t Hex2Bin(const std::string& hex, void* digest);

std::string UTF16toUTF8(const std::wstring& utf16);
std::wstring UTF8toUTF16(const std::string& utf8);

std::wstring AnsitoUTF16(const std::string& ansi);
std::string UTF16toAnsi(const std::wstring& utf16);

std::string UTF8toAnsi(const std::string& utf8);
std::string AnsitoUTF8(const std::string& ansi);

bool isHaveChinese(const std::string& strV);

#endif // CHARSETSWITCH_H__