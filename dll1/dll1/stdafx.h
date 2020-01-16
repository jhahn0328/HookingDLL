// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>


//flag변환을 위한 함수
TCHAR* tranDesiredAccess(DWORD dwDesiredAccess);
TCHAR* tranShareMode(DWORD dwShareMode);
TCHAR* tranCreationDisposition(DWORD dwCreationDisposition );
TCHAR* tranFlagsAndAttributes(DWORD dwFlagsAndAttributes);
TCHAR* tranMoveMethod(DWORD dwMoveMethod);
TCHAR* tranMoveFileFlag(DWORD dwFlags);
TCHAR* tranCopyFlag(DWORD dwCopyFlags);
BOOL isSupportDll(TCHAR* module_name,TCHAR* cmpModuleList);
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
