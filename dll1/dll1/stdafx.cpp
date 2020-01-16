// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// dll1.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include <tchar.h>
#define IS_BIT_SET(bFlag,b) (((bFlag)&(b))==(bFlag))
// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.


//flag변환을 위한 함수
TCHAR* tranDesiredAccess(DWORD dwDesiredAccess){
	TCHAR result[128]=L"";
	if (IS_BIT_SET(dwDesiredAccess,GENERIC_ALL)){
		wsprintf(result, L"ALL|");
	}
	if (IS_BIT_SET(dwDesiredAccess,GENERIC_EXECUTE)){
		wsprintf(result, L"%sEXEC|",result);
	}
	if (IS_BIT_SET(dwDesiredAccess, GENERIC_WRITE)){
		wsprintf(result, L"%sWRITE|", result);
	}
	if (IS_BIT_SET(dwDesiredAccess, GENERIC_READ)){
		wsprintf(result, L"%sREAD|", result);
	}
	if (IS_BIT_SET(dwDesiredAccess, DELETE)){
		wsprintf(result, L"%sDEL|", result);
	}
	if (IS_BIT_SET(dwDesiredAccess, READ_CONTROL)){
		wsprintf(result, L"%sRD_CON|", result);
	}
	if (IS_BIT_SET(dwDesiredAccess, WRITE_DAC)){
		wsprintf(result, L"%sWR_DAC|", result);
	}
	if (IS_BIT_SET(dwDesiredAccess, WRITE_OWNER)){
		wsprintf(result, L"%sWR_OWN|", result);
	}
	if (IS_BIT_SET(dwDesiredAccess, SYNCHRONIZE)){
		wsprintf(result, L"%sSYNCHRO|", result);
	}
	return result;
}

TCHAR* tranShareMode(DWORD dwShareMode){
	TCHAR result[128] = L"";
	if (IS_BIT_SET(dwShareMode,0)){
		wsprintf(result, L"0");
	}
	else if (IS_BIT_SET(dwShareMode, FILE_SHARE_READ)){
		wsprintf(result, L"READ");
	}
	else if (IS_BIT_SET(dwShareMode, FILE_SHARE_WRITE)){
		wsprintf(result, L"WRITE");
	}
	else if (IS_BIT_SET(dwShareMode, FILE_SHARE_DELETE)){
		wsprintf(result, L"DEL");
	}
	else{
		wsprintf(result, L"ETC");
	}
	return result;
}
TCHAR* tranCreationDisposition(DWORD dwCreationDisposition){
	TCHAR result[128] = L"";
	if (IS_BIT_SET(dwCreationDisposition,CREATE_NEW)){
		wsprintf(result, L"CRT_NEW");
	}
	if (IS_BIT_SET(dwCreationDisposition, CREATE_ALWAYS)){
		wsprintf(result, L"CRT_AL");
	}
	if (IS_BIT_SET(dwCreationDisposition, OPEN_EXISTING)){
		wsprintf(result, L"OPEN_EX");
	}
	if (IS_BIT_SET(dwCreationDisposition, OPEN_ALWAYS)){
		wsprintf(result, L"OPEN_AL");
	}
	if (IS_BIT_SET(dwCreationDisposition, TRUNCATE_EXISTING)){
		wsprintf(result, L"TRUN_EX");
	}
	return result;
}

TCHAR* tranFlagsAndAttributes(DWORD dwFlagsAndAttributes){
	TCHAR result[128] = L"";
	if (IS_BIT_SET(dwFlagsAndAttributes,FILE_ATTRIBUTE_READONLY)/*(dwFlagsAndAttributes & 1) == 1*/){
		wsprintf(result, L"READONLY|");
	}
	if (IS_BIT_SET(dwFlagsAndAttributes, FILE_ATTRIBUTE_HIDDEN)/*(dwFlagsAndAttributes & 2) == 2*/){
		wsprintf(result, L"%sHIDDEN|",result);
	}
	if (IS_BIT_SET(dwFlagsAndAttributes,FILE_ATTRIBUTE_SYSTEM)/*(dwFlagsAndAttributes & 4) == 4*/){
		wsprintf(result, L"%sSYSTEM|", result);
	}
	if (IS_BIT_SET(dwFlagsAndAttributes,FILE_ATTRIBUTE_ARCHIVE)/*(dwFlagsAndAttributes & 32) == 32*/){
		wsprintf(result, L"%sARCHIVE|", result);
	}
	if (IS_BIT_SET(dwFlagsAndAttributes, FILE_ATTRIBUTE_NORMAL)/*(dwFlagsAndAttributes & 128) == 128*/){
		wsprintf(result, L"%sNORMAL|", result);
	}
	if (IS_BIT_SET(dwFlagsAndAttributes, FILE_ATTRIBUTE_TEMPORARY)/*(dwFlagsAndAttributes & 256) == 256*/){
		wsprintf(result, L"%sTEMPORARY|", result);
	}
	if (IS_BIT_SET(dwFlagsAndAttributes, FILE_ATTRIBUTE_OFFLINE)/*(dwFlagsAndAttributes & 4096) == 4096*/){
		wsprintf(result, L"%sOFFLINE|", result);
	}
	if (IS_BIT_SET(dwFlagsAndAttributes, FILE_ATTRIBUTE_ENCRYPTED)/*(dwFlagsAndAttributes & 16384) == 16384*/){
		wsprintf(result, L"%sENCRYPTED|", result);
	}
	return result;
}

TCHAR* tranMoveMethod(DWORD dwMoveMethod){
	TCHAR result[128] = L"";
	if (IS_BIT_SET(dwMoveMethod, FILE_BEGIN)/*(dwFlagsAndAttributes & 1) == 1*/){
		wsprintf(result, L"BEGIN");
	}
	else if (IS_BIT_SET(dwMoveMethod, FILE_CURRENT)){
		wsprintf(result, L"CURRENT");
	}
	else if (IS_BIT_SET(dwMoveMethod, FILE_END)){
		wsprintf(result, L"END");
	}
	else{
		wsprintf(result, L"ETC");
	}
	return result;
}

TCHAR* tranMoveFileFlag(DWORD dwFlags){
	TCHAR result[1024] = L"";
	if (IS_BIT_SET(dwFlags, MOVEFILE_COPY_ALLOWED)){
		wsprintf(result, L"COPY_ALLOWED");
	}
	if(IS_BIT_SET(dwFlags, MOVEFILE_CREATE_HARDLINK)){
		wsprintf(result, L"CREATE_HARDLINK");
	}
	if (IS_BIT_SET(dwFlags, MOVEFILE_DELAY_UNTIL_REBOOT)){
		wsprintf(result, L"DELAY_UNTIL_REBOOT");
	}
	if (IS_BIT_SET(dwFlags, MOVEFILE_FAIL_IF_NOT_TRACKABLE)){
		wsprintf(result, L"FAIL_IF_NOT_TRACKABLE");
	}
	if (IS_BIT_SET(dwFlags, MOVEFILE_REPLACE_EXISTING)){
		wsprintf(result, L"REPLACE_EXISTING");
	}
	if (IS_BIT_SET(dwFlags, MOVEFILE_WRITE_THROUGH)){
		wsprintf(result, L"WRITE_THROUGH");
	}
	return result;
}

TCHAR* tranCopyFlag(DWORD dwCopyFlags){
	TCHAR result[1024] = L"";
	if (IS_BIT_SET(dwCopyFlags, COPY_FILE_ALLOW_DECRYPTED_DESTINATION)){
		wsprintf(result, L"ALLOW_DECRYPTED_DESTINATION");
	}
	if (IS_BIT_SET(dwCopyFlags, COPY_FILE_COPY_SYMLINK)){
		wsprintf(result, L"COPY_SYMLINK");
	}
	if (IS_BIT_SET(dwCopyFlags, COPY_FILE_FAIL_IF_EXISTS)){
		wsprintf(result, L"FAIL_IF_EXISTS");
	}
	if (IS_BIT_SET(dwCopyFlags, COPY_FILE_NO_BUFFERING)){
		wsprintf(result, L"NO_BUFFERING");
	}
	if (IS_BIT_SET(dwCopyFlags, COPY_FILE_OPEN_SOURCE_FOR_WRITE)){
		wsprintf(result, L"OPEN_SOURCE_FOR_WRITE");
	}
	if (IS_BIT_SET(dwCopyFlags, COPY_FILE_RESTARTABLE)){
		wsprintf(result, L"RESTARTABLE");
	}
	return result;
}

//ini리스트에 있는 모듈과 비교함
BOOL isSupportDll(TCHAR* module_name, TCHAR* cmpModuleList){
	TCHAR seps[] = _T("|");
	TCHAR* token;
	TCHAR tmp[128];
	swprintf(tmp, L"%s", cmpModuleList);
	token = _tcstok(tmp, seps);
	while (token != NULL){
		if (_tcscmp(module_name, token) == 0){
			return TRUE;
		}
		token = _tcstok(NULL, seps);
	}
	return FALSE;
}