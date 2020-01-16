#include <atlstr.h>
#include<iostream>
#include <fstream>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAMEW__ (wcsrchr(__FILEW__, L'\\') ? wcsrchr(__FILEW__, L'\\') + 1 : __FILEW__)

extern LPCWSTR szModuleTag=L"[notepad++] ";
BOOL g_bDebugOut=TRUE;

#define LogW(format, ...)	\
		{ \
		if(g_bDebugOut) \
				{ \
			static const size_t max_format_message = 2048; \
			wchar_t message[max_format_message] = { 0 }; \
			wchar_t output[max_format_message] = { 0 }; \
			if (swprintf_s(message, format, __VA_ARGS__) > 0)	\
						{	\
				if (swprintf_s(output, L"%s %s___<%s::%d>", szModuleTag, message, __FILENAMEW__, __LINE__) > 0 ) \
								{ \
					OutputDebugStringW(output); \
								} \
								else OutputDebugStringW( L">>> dbg_format failure");	\
						}	\
						else OutputDebugStringW( L">>> dbg_format failure");	\
				} \
		}