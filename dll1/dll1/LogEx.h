#pragma once

using namespace ATL;
using namespace std;

#include <vector>
#include <strsafe.h>
#include <string>

#include <tchar.h>
#include <sstream>

extern LPCWSTR szModuleTag;
extern BOOL g_bDebugOut;

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAMEW__ (wcsrchr(__FILEW__, L'\\') ? wcsrchr(__FILEW__, L'\\') + 1 : __FILEW__)

/////////////////////////////////////////////////////////////////////////////////////////
/*
static void SetLog(LPCWSTR pAppName)
{
	USES_CONVERSION;
	try
	{
		if (wcslen(pAppName) <= 0)
		{
			return;
		}
		CRegKey debugkey;
		DWORD dwdebug = (DWORD)0;

		wstring strSubKey = L"SOFTWARE\\MarkAny\\Document Safer\\Debug\\";
		strSubKey += pAppName;

		LONG lRet = debugkey.Create(HKEY_LOCAL_MACHINE, strSubKey.c_str());
		if (lRet != ERROR_SUCCESS)
		{
			lRet = debugkey.Open(HKEY_LOCAL_MACHINE, strSubKey.c_str(), KEY_READ | KEY_QUERY_VALUE);
		}
		if (lRet != ERROR_SUCCESS)
		{
			return;
		}
		debugkey.QueryDWORDValue(L"Debug", dwdebug);

		TCHAR szTmp[MAX_PATH] = { 0 };
		if (dwdebug > 0)
		{
			g_bDebugOut = TRUE;
			_stprintf_s(szTmp, _countof(szTmp), _T("==== MarkAny Document SAFER [%s] Debug Setting!!! ===="), pAppName);
			OutputDebugString(szTmp);
		}
		else
		{
			debugkey.QueryDWORDValue(L"bit", dwdebug);
			if (dwdebug > 0)
			{
				g_bDebugOut = TRUE;
				_stprintf_s(szTmp, _countof(szTmp), _T("==== MarkAny Document SAFER [%s] Debug Setting!!! ===="), pAppName);
				OutputDebugString(szTmp);
			}
			else
			{
				g_bDebugOut = FALSE;
				_stprintf_s(szTmp, _countof(szTmp), _T("==== MarkAny Document SAFER [%s] Debug UnSetting!!! ===="), pAppName);
				OutputDebugString(szTmp);
			}
		}
	}
	catch (...)
	{
		g_bDebugOut = FALSE;
	}
}
*/
/////////////////////////////////////////////////////////////////////////////////////////
#define LogA(level, format, ...)	\
	{ \
		if(g_bDebugOut) \
		{ \
			USES_CONVERSION; \
			static const size_t max_format_message = 2048; \
			char message[max_format_message] = { 0 }; \
			char output[max_format_message] = { 0 }; \
			char szlevel[10] = { 0 }; \
			if(level == ds_debug::error ) {strcpy_s(szlevel,_countof(szlevel),"[ERR]");} else if(level == ds_debug::warning ){strcpy_s(szlevel,_countof(szlevel),"[WRN]");} else if(level == ds_debug::information ){strcpy_s(szlevel,_countof(szlevel),"[INF]");} else if(level == ds_debug::debug ){strcpy_s(szlevel,_countof(szlevel),"[DBG]");} else {strcpy_s(szlevel,_countof(szlevel),"[TRA]");}\
			if (sprintf_s(message, format, __VA_ARGS__) > 0)	\
			{	\
				if (sprintf_s(output, "%s %s___<%s::%d> %s", W2A(szModuleTag), message, __FILENAME__, __LINE__, szlevel) > 0 ) \
				{ \
					OutputDebugStringA(output); \
				} \
				else OutputDebugStringA( ">>> dbg_format failure");	\
			}	\
			else OutputDebugStringA( ">>> dbg_format failure");	\
		} \
	}

#define LogW(level, format, ...)	\
	{ \
		if(g_bDebugOut) \
		{ \
			static const size_t max_format_message = 2048; \
			wchar_t message[max_format_message] = { 0 }; \
			wchar_t output[max_format_message] = { 0 }; \
			wchar_t szlevel[10] = { 0 }; \
			if(level == ds_debug::error ) {wcscpy_s(szlevel,_countof(szlevel),L"[ERR]");} else if(level == ds_debug::warning ){wcscpy_s(szlevel,_countof(szlevel),L"[WRN]");} else if(level == ds_debug::information ){wcscpy_s(szlevel,_countof(szlevel),L"[INF]");} else if(level == ds_debug::debug ){wcscpy_s(szlevel,_countof(szlevel),L"[DBG]");} else {wcscpy_s(szlevel,_countof(szlevel),L"[TRA]");} \
			if (swprintf_s(message, format, __VA_ARGS__) > 0)	\
			{	\
				if (swprintf_s(output, L"%s %s___<%s::%d> %s", szModuleTag, message, __FILENAMEW__, __LINE__, szlevel) > 0 ) \
				{ \
					OutputDebugStringW(output); \
				} \
				else OutputDebugStringW( L">>> dbg_format failure");	\
			}	\
			else OutputDebugStringW( L">>> dbg_format failure");	\
		} \
	}

/////////////////////////////////////////////////////////////////////////////////////////
/** printf ��Ÿ�� ����� �α� (MBCS) */
#define dbg_formatA_cpp(level, format, ...)	\
	do {	\
		ds_debug::store_lasterror __ds_debug_store_lasterror__;	\
		if(ds_debug::debug_instance::prepare(ds_debug::level) == false) break;	\
		char message[ds_debug::max_format_message] = { 0 };	\
		if(sprintf_s(message, format, __VA_ARGS__) > 0)	\
		{	\
			sprintf_s(message, "%s___<%s::%d>", message, __FILENAME__, __LINE__); \
			ds_debug::debug_log(__FUNCTION__, __LINE__, ds_debug::level, ds_debug::ucs2_to_utf8(ds_debug::mbcs_to_ucs2(message)).c_str());	\
		}	\
		else ds_debug::debug_log(__FUNCTION__, __LINE__, ds_debug::error, "dbg_format failure");	\
	} while(false)

/** printf ��Ÿ�� ����� �α� (UCS-2) */
#define dbg_formatW_cpp(level, format, ...)	\
	do {	\
		ds_debug::store_lasterror __ds_debug_store_lasterror__;	\
		if(ds_debug::debug_instance::prepare(ds_debug::level) == false) break;	\
		wchar_t message[ds_debug::max_format_message] = { 0 };	\
		if (swprintf_s(message, format, __VA_ARGS__) > 0)	\
		{	\
			swprintf_s(message, L"%s___<%s::%d>", message, __FILENAMEW__, __LINE__); \
			ds_debug::debug_log(__FUNCTION__, __LINE__, ds_debug::level, ds_debug::ucs2_to_utf8(message).c_str());	\
		}	\
		else ds_debug::debug_log(__FUNCTION__, __LINE__, ds_debug::error, ">>> dbg_format failure");	\
	} while(false)

/////////////////////////////////////////////////////////////////////////////////////////
// DebugView�� ��������� Debug Log Viewer�� ����� ���� ����.
#define LOGA(fmt, ...)		{ if(g_bDebugOut) LogA( ds_debug::debug, fmt, __VA_ARGS__)		else dbg_formatA_cpp( trace, fmt, __VA_ARGS__); }		// debug ���� ���� ������ ǥ���ϴ� �޽���
#define LOGW(fmt, ...)		{ if(g_bDebugOut) LogW( ds_debug::debug, fmt, __VA_ARGS__)		else dbg_formatW_cpp( trace, fmt, __VA_ARGS__); }		// debug ���� ���� ������ ǥ���ϴ� �޽���
//#define LOGA(fmt, ...)		LogA( 4, fmt, __VA_ARGS__)
//#define LOGW(fmt, ...)		LogW( 4, fmt, __VA_ARGS__)

#define LOGA_TRA(fmt, ...)	{ if(g_bDebugOut) LogA( ds_debug::trace, fmt, __VA_ARGS__)			else dbg_formatA_cpp( trace, fmt, __VA_ARGS__); }		// debug ���� ���� ������ ǥ���ϴ� �޽��� ( �߿䵵�� �������� pass �α� ���� )(����׺� filter 20:ȸ��)
#define LOGA_DBG(fmt, ...)	{ if(g_bDebugOut) LogA( ds_debug::debug, fmt, __VA_ARGS__)			else dbg_formatA_cpp( debug, fmt, __VA_ARGS__); }		// ���α׷��� ������ϱ� ���� ���� �޽��� ( ����� �α�, ���� �߻� ���ɼ� )
#define LOGA_INF(fmt, ...)	{ if(g_bDebugOut) LogA( ds_debug::information, fmt, __VA_ARGS__)	else dbg_formatA_cpp( information, fmt, __VA_ARGS__); } // ���º���� ���� ������ �޽��� ( ���� �� ���� ����͸� �α� )(����׺� filter 3:���)
#define LOGA_WRN(fmt, ...)	{ if(g_bDebugOut) LogA( ds_debug::warning, fmt, __VA_ARGS__)		else dbg_formatA_cpp( warning, fmt, __VA_ARGS__); }		// ������ �߻��Ͽ�����, ���α׷� ������ ó�� ������ ������ ǥ���ϴ� �޽��� ( ���� �� ���� �α� )(����׺� filter 2:��Ȳ��)
#define LOGA_ERR(fmt, ...)	{ if(g_bDebugOut) LogA( ds_debug::error, fmt, __VA_ARGS__)			else dbg_formatA_cpp( error, fmt, __VA_ARGS__); }		// ó�� �Ұ����� ������ ���α׷��� �������� ���ᳪ ��ָ� �߻���Ű�� ������ ǥ���ϴ� �޽��� ( catch ���� �� �ɰ��� �α� )(����׺� filter 1:������)

#define LOGW_TRA(fmt, ...)	{ if(g_bDebugOut) LogW( ds_debug::trace, fmt, __VA_ARGS__)			else dbg_formatW_cpp( trace, fmt, __VA_ARGS__); }		// debug ���� ���� ������ ǥ���ϴ� �޽��� ( �߿䵵�� �������� pass �α� ���� )(����׺� filter 20:ȸ��)
#define LOGW_DBG(fmt, ...)	{ if(g_bDebugOut) LogW( ds_debug::debug, fmt, __VA_ARGS__)			else dbg_formatW_cpp( debug, fmt, __VA_ARGS__); }		// ���α׷��� ������ϱ� ���� ���� �޽��� ( ����� �α� �� ���� �߻� ���ɼ� )
#define LOGW_INF(fmt, ...)	{ if(g_bDebugOut) LogW( ds_debug::information, fmt, __VA_ARGS__)	else dbg_formatW_cpp( information, fmt, __VA_ARGS__); } // ���º���� ���� ������ �޽��� ( ���� �� ���� ����͸� �α� )(����׺� filter 3:���)
#define LOGW_WRN(fmt, ...)	{ if(g_bDebugOut) LogW( ds_debug::warning, fmt, __VA_ARGS__)		else dbg_formatW_cpp( warning, fmt, __VA_ARGS__); }		// ������ �߻��Ͽ�����, ���α׷� ������ ó�� ������ ������ ǥ���ϴ� �޽��� ( ���� �� ���� �α� )(����׺� filter 2:��Ȳ��)
#define LOGW_ERR(fmt, ...)	{ if(g_bDebugOut) LogW( ds_debug::error, fmt, __VA_ARGS__)			else dbg_formatW_cpp( error, fmt, __VA_ARGS__); }		// ó�� �Ұ����� ������ ���α׷��� �������� ���ᳪ ��ָ� �߻���Ű�� ������ ǥ���ϴ� �޽��� ( catch ���� �� �ɰ��� �α� )(����׺� filter 1:������)

//#define LOGOUT LOGW
//#define DBGOUT LOGOUT

/////////////////////////////////////////////////////////////////////////////////////////
static void GetLastErrorMsg(DWORD dwErr)
{
	LPTSTR lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	// Process any inserts in lpMsgBuf.

	LOGW(L"GetLastErrorMsg -- [ %s ] !!!", (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf); // Free the buffer.
}