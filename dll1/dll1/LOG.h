#pragma once

#include <atlstr.h>
#include<iostream>
#include <fstream>

#define MYDEBUG(fmt, ...) { myOutputDebugString(fmt L" ___[%s:%d] %s ",##__VA_ARGS__, PathFindFileName(__FILEW__), __LINE__, __FUNCTIONW__); } 
//TestŰ�� ã�Ƽ� inject��� Ű�� ���� 
//�������� bit�� ����
//bit ���� 1�ΰ�� ����� ���
//bit ���� ���ų� 0�� ��� ����� ���X
//try catch ������ Ȱ���Ͽ� ����ó��
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

	// �̸� ����
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

