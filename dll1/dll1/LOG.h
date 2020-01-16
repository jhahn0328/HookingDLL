#pragma once

#include <atlstr.h>
#include<iostream>
#include <fstream>

#define MYDEBUG(fmt, ...) { myOutputDebugString(fmt L" ___[%s:%d] %s ",##__VA_ARGS__, PathFindFileName(__FILEW__), __LINE__, __FUNCTIONW__); } 
//Test키를 찾아서 inject라는 키를 생성 
//수동으로 bit를 수정
//bit 값이 1인경우 디버그 출력
//bit 값이 없거나 0인 경우 디버그 출력X
//try catch 구문을 활용하여 오류처리
static const HMODULE GetCurrentModule()
{
	DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
	HMODULE hm = 0;
	::GetModuleHandleEx(flags, reinterpret_cast<LPCTSTR>(GetCurrentModule), &hm);
	return hm;
}

static void myOutputDebugString(const WCHAR *format, ...)
{
	HMODULE hm;
	hm = GetCurrentModule();
	WCHAR     szAppPath[MAX_PATH];
	CString strAppName;
	CString token;
	int pos = 0;
	WCHAR tmp[1024];

	::GetModuleFileName(hm, szAppPath, MAX_PATH);

	// 이름 추출
	strAppName = szAppPath;
	strAppName = strAppName.Mid(strAppName.ReverseFind('\\') + 1);
	token = strAppName.Tokenize(L".", pos);

	strAppName = token;

	WCHAR buf[1024];
	va_list vaList;
	va_start(vaList, format);
	_vsnwprintf_s(buf, sizeof(buf), format, vaList);
	va_end(vaList);
	swprintf(tmp, sizeof(tmp) / sizeof(WCHAR), L"[%s]%s", strAppName, buf);
	OutputDebugStringW(tmp);
}

