// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>


//flag��ȯ�� ���� �Լ�
TCHAR* tranDesiredAccess(DWORD dwDesiredAccess);
TCHAR* tranShareMode(DWORD dwShareMode);
TCHAR* tranCreationDisposition(DWORD dwCreationDisposition );
TCHAR* tranFlagsAndAttributes(DWORD dwFlagsAndAttributes);
TCHAR* tranMoveMethod(DWORD dwMoveMethod);
TCHAR* tranMoveFileFlag(DWORD dwFlags);
TCHAR* tranCopyFlag(DWORD dwCopyFlags);
BOOL isSupportDll(TCHAR* module_name,TCHAR* cmpModuleList);
// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
