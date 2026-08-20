
#pragma warning(disable: 4996)

#include <algorithm>
#include <vector>
#include <string>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "glcInternal.h"

namespace glc
{
std::wstring ToWideString(const std::string& str)
{
	if(str.empty())
		return {};
	const char* text = str.c_str();
	UINT codePage = CP_UTF8;
	int len = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, text, -1, nullptr, 0);
	if(0>=len)
	{
		codePage = CP_ACP;
		len = MultiByteToWideChar(codePage, 0, text, -1, nullptr, 0);
	}
	if(0>=len)
		return {};
	std::wstring ret(static_cast<size_t>(len), L'\0');
	MultiByteToWideChar(codePage, 0, text, -1, ret.data(), len);
	if(!ret.empty() && ret.back() == L'\0')
		ret.pop_back();
	return ret;
}

int FormatLength(const char* format, va_list va)
{
	if(!format)
		return -1;
	va_list vc;
	va_copy(vc, va);
	int len = vsnprintf(nullptr, 0, format, vc);
	va_end(vc);
	return len;
}

std::string FormatString(const char* format, va_list va)
{
	if(!format)
		return {};
	int len = FormatLength(format, va);
	if(0>=len)
		return {};
	std::string ret(len, '\0');
	va_list vc;
	va_copy(vc, va);
	vsnprintf(ret.data(), len + 1, format, vc);
	va_end(vc);
	return ret;
}

} // namespace glc