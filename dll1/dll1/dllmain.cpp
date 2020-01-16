// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include "detours.h"
#include "LOG.h"
#include "NewLog.h"
//후킹될 함수
typedef HANDLE(WINAPI *PFCreateFileW)(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef BOOL(WINAPI *PFReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
typedef BOOL(WINAPI *PFWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
typedef int(WINAPI *PFStartDocW)(HDC hdc,const DOCINFOW *lpdi);
typedef int(WINAPI *PFStartDocA)(HDC hdc, const DOCINFOA *lpdi);
typedef int(WINAPI *PFStartPage)(HDC hdc);
typedef int(WINAPI *PFEndDoc)(HDC hdc);
typedef BOOL(WINAPI *PFCloseHandle)(HANDLE hObject);
typedef DWORD(WINAPI *PFSetFilePointer)(HANDLE hFile,LONG IDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod);
typedef BOOL(WINAPI *PFSetFilePointerEx)(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
typedef BOOL(WINAPI *PFMoveFile)(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName);
typedef BOOL(WINAPI *PFMoveFileEx)(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, DWORD dwFlags);
typedef DWORD(WINAPI *PFGetFileSize)(HANDLE hFile, LPDWORD lpFileSizeHigh);
typedef BOOL(WINAPI *PFGetFileSizeEx)(HANDLE hFile, PLARGE_INTEGER lpFileSize);
typedef BOOL(WINAPI *PFCopyFile)(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists);
typedef BOOL(WINAPI *PFCopyFileEx)(LPCWSTR lpExistingFileName,LPCWSTR lpNewFileName,LPPROGRESS_ROUTINE lpProgressRoutine,LPVOID lpData,LPBOOL pbCancel,DWORD dwCopyFlags);
typedef HWND(WINAPI *PFCreateWindowExW)(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
typedef HWND(WINAPI *PFCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
typedef HDC(WINAPI *PFCreateDCW)(LPCTSTR lpszDriver,_In_ LPCTSTR lpszDevice,LPCTSTR lpszOutput,_In_ const DEVMODE *lpInitData);
typedef HDC(WINAPI *PFCreateDCA)(LPCSTR pwszDriver, LPCSTR pwszDevice, LPCSTR pszPort, const DEVMODEA *pdm);
//원본함수를 저장할 변수
PFCreateFileW g_pOriginFunc = NULL;
PFReadFile g_pOriginFunc2 = NULL;
PFWriteFile g_pOriginFunc3 = NULL;
PFStartDocW g_pOriginFunc4 = NULL;
PFStartPage g_pOriginFunc5 = NULL;
PFEndDoc g_pOriginFunc6 = NULL;
PFCloseHandle g_pOriginFunc7 = NULL;
PFSetFilePointer g_pOriginFunc8 = NULL;
PFSetFilePointerEx g_pOriginFunc9 = NULL;
PFMoveFile g_pOriginFunc10 = NULL;
PFMoveFileEx g_pOriginFunc11 = NULL;
PFGetFileSize g_pOriginFunc12 = NULL;
PFGetFileSizeEx g_pOriginFunc13 = NULL;
PFCopyFile g_pOriginFunc14 = NULL;
PFCopyFileEx g_pOriginFunc15 = NULL;
PFStartDocA g_pOriginFunc16 = NULL;
PFCreateWindowExW g_pOriginFunc17 = NULL;
PFCreateWindowExA g_pOriginFunc18 = NULL;
PFCreateDCW g_pOriginFunc19 = NULL;
PFCreateDCA g_pOriginFunc20 = NULL;
//프린트 페이지 카운터
static int print_page_count;
//포함할 모듈이름
TCHAR inclusive_module[128];

HANDLE WINAPI MyCreateFileW(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile){
	USES_CONVERSION;
	//비트연산 변수
	TCHAR tDesiredAccess[128];
	TCHAR tShareMode[128];
	TCHAR tCreationDisposition[128];
	TCHAR tFlagsAndAttributes[128];
	//caller's info를 가져오기 위한 변수
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	//비트연산결과
	swprintf(tDesiredAccess, L"%s", tranDesiredAccess(dwDesiredAccess));
	swprintf(tShareMode, L"%s", tranShareMode(dwShareMode));
	swprintf(tCreationDisposition, L"%s", tranCreationDisposition(dwCreationDisposition));
	swprintf(tFlagsAndAttributes, L"%s", tranFlagsAndAttributes(dwFlagsAndAttributes));
	//호출한 파일 구하기
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(),&hModule );
	GetModuleFileName(hModule, module_name, 4096);
	CloseHandle(hModule);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"Before: MyCreateFile[%s](NAME:%s, ACS:%d, SH: %d, CD:%d, F&A:%d)",PathFindFileName(module_name) ,lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes);
	}
	HANDLE ret = g_pOriginFunc((LPCTSTR)lpFileName, (DWORD)dwDesiredAccess, (DWORD)dwShareMode, (LPSECURITY_ATTRIBUTES)pSecurityAttributes, (DWORD)dwCreationDisposition, (DWORD)dwFlagsAndAttributes, (HANDLE)hTemplateFile);
	//ini에서 모듈 찾아와서 비교 함수추가
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		//LogW(L"Caller's Name: %s", PathFindFileName(module_name));
		LogW(L"After: [0x%.8x] MyCreateFile[%s](NAME:%s, ACS:%s, SH: %s, CD:%s, F&A:%s)", ret, PathFindFileName(module_name), lpFileName, tDesiredAccess, tShareMode, tCreationDisposition, tFlagsAndAttributes);
		//LogW(L"Return Handle: 0x%.8x", ret);
		//LogW(L" MyCreateFile(NAME:%s, ACS:%s, SH: %s, CD:%s, F&A:%s)", lpFileName, tDesiredAccess, tShareMode, tCreationDisposition, tFlagsAndAttributes);
	}
	return ret;
}

BOOL WINAPI MyReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	//caller's info를 위한 변수
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	//호출한 파일정보 가져오기
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	//원본함수 호출
	BOOL ret = g_pOriginFunc2((HANDLE)hFile, (LPVOID)lpBuffer, (DWORD)nNumberOfBytesToRead, (LPDWORD)lpNumberOfBytesRead, (LPOVERLAPPED)lpOverlapped);
	//ini에서 모듈리스트 찾아와서 비교
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"[0x%.8x] MyReadFile[%s](Handle:0x%.8x, Buf:%p, ToRead:%d, Read:%d, %d)", hFile, PathFindFileName(module_name), hFile, lpBuffer, nNumberOfBytesToRead, *lpNumberOfBytesRead, lpOverlapped);
	}
	return ret;
}

BOOL WINAPI MyWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	//caller's info를 위한 변수
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	BOOL ret = g_pOriginFunc3(hFile, (LPCVOID)lpBuffer, (DWORD)nNumberOfBytesToWrite, (LPDWORD)lpNumberOfBytesWritten, (LPOVERLAPPED)lpOverlapped);
	//호출한 파일정보 가져오기
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	//ini에서 모듈리스트 찾아와서 비교
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"[0x%.8x] MyWriteFile[%s](Handle:0x%.8x, Buf:%p, ToWrite:%d, Write:%d, %d)", hFile, PathFindFileName(module_name), hFile, lpBuffer, nNumberOfBytesToWrite, *lpNumberOfBytesWritten, lpOverlapped);
	}
	return ret;
}

BOOL WINAPI MyCloseHandle(HANDLE hObject){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	//호출한 파일정보 가져오기
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	//ini에서 모듈리스트 찾아와서 비교
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"[0x%.8x] MyCloseHandle[%s](Handle:0x%.8x)", hObject, PathFindFileName(module_name), hObject);
	}
	return g_pOriginFunc7(hObject);
}

DWORD WINAPI MySetFilePointer(HANDLE hFile,LONG IDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	TCHAR tMoveMethod[1024];
	DWORD ret = g_pOriginFunc8(hFile, IDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
	swprintf(tMoveMethod, L"%s", tranMoveMethod(dwMoveMethod));
	//호출한 파일정보 가져오기
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	//ini에서 모듈리스트 찾아와서 비교
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"[0x%.8x]MySetFilePointer[%s](Handle:0x%.8x, Move:%ld, Move(4G):0x%p, Flag:%s)", hFile, PathFindFileName(module_name), hFile, IDistanceToMove, lpDistanceToMoveHigh, tMoveMethod);
	}
	return ret;
}

BOOL WINAPI MySetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	TCHAR tMoveMethod[128];
	BOOL ret = g_pOriginFunc9(hFile,liDistanceToMove,lpNewFilePointer,dwMoveMethod);
	swprintf(tMoveMethod, L"%s", tranMoveMethod(dwMoveMethod));
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	//ini에서 모듈리스트 찾아와서 비교
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"[0x%.8x]MySetFilePointerEx[%s](Handle:0x%.8x, Move:%lld, NewPnt:%lld, Flag:%s)", hFile, PathFindFileName(module_name), hFile, liDistanceToMove.QuadPart, lpNewFilePointer->QuadPart, tMoveMethod);
	}
	return ret;
}

BOOL WINAPI MyMoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	BOOL ret = g_pOriginFunc10(lpExistingFileName, lpNewFileName);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	//ini에서 모듈리스트 찾아와서 비교
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"MyMoveFile[%s](Origin:%s, New:%s)", PathFindFileName(module_name), lpExistingFileName, lpNewFileName);
	}
	return ret;
}

BOOL WINAPI MyMoveFileEx(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, DWORD dwFlags){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	TCHAR tFlags[1024];
	BOOL ret = g_pOriginFunc11(lpExistingFileName,lpNewFileName,dwFlags);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	swprintf(tFlags, L"%s", tranMoveFileFlag(dwFlags));
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"MyMoveFileEx[%s](Origin:%s, New:%s,Flag:%s)", PathFindFileName(module_name), lpExistingFileName, lpNewFileName, tFlags);//플래그 처리아직안함
	}
	return ret;
}

DWORD WINAPI MyGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	BOOL ret = g_pOriginFunc12(hFile, lpFileSizeHigh);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"[0x%.8x]MyGetFileSize[%s](Handle:0x%.8x, lpFileSizeHigh:0x%p)<Size:%d>", hFile, PathFindFileName(module_name), hFile, lpFileSizeHigh, ret);
	}
	return ret;
}

BOOL WINAPI MyGetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	BOOL ret = g_pOriginFunc13(hFile, lpFileSize);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"[0x%.8x]MyGetFileSizeEx[%s](Handle:0x%.8x, Size:%d)", hFile, PathFindFileName(module_name), hFile, lpFileSize->QuadPart);
	}
	return ret;
}

BOOL WINAPI MyCopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	BOOL ret = g_pOriginFunc14(lpExistingFileName, lpNewFileName, bFailIfExists);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"MyCopyFile[%s](Filename:%s, NewFile:%s, FailIfExt:%d)", PathFindFileName(module_name), lpExistingFileName, lpNewFileName, bFailIfExists);
	}
	return ret;
}

BOOL WINAPI MyCopyFileEx(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	TCHAR tCopyFlags[1024];
	BOOL ret = g_pOriginFunc15(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	swprintf(tCopyFlags, L"%s", tranCopyFlag(dwCopyFlags));
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"MyCopyFile[%s](Filename:%s, NewFile:%s, Flag:%s)", PathFindFileName(module_name), lpExistingFileName, lpNewFileName, tCopyFlags);//플래그 처리아직안함
	}
	return ret;
}

int WINAPI MyStartDocW(HDC hdc, const DOCINFOW* lpdi){
	/*SetLastError(0x0005);
	DWORD e = GetLastError();
	TCHAR* s = 0;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		0,
		e,
		0,
		(TCHAR*)&s,
		0,
		0);
	MessageBox(NULL, s, NULL, MB_ICONERROR);
	LocalFree(s);
	return 0;*/
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	int ret = g_pOriginFunc4(hdc, lpdi);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"MyStartDocW[%s](HDC:0x%.8x, DOCINFOW*:0x%.8x)",PathFindFileName(module_name),hdc,lpdi);
	}
	return ret;
}

int WINAPI MyStartDocA(HDC hdc, const DOCINFOA* lpdi){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	int ret = g_pOriginFunc16(hdc, lpdi);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"MyStartDocA[%s](HDC:0x%.8x, DOCINFOA*:0x%.8x)", PathFindFileName(module_name), hdc, lpdi);
	}
	return ret;
}

int WINAPI MyStartPage(HDC hdc){
	/*print_page_count++;
	LogW(L"MyStartPage(HDC:%p)", hdc);
	return ((PFStartPage)g_pOriginFunc5)((HDC)hdc);*/
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	int ret = g_pOriginFunc5(hdc);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"MyStartPage[%s](HDC:0x%.8x)", PathFindFileName(module_name), hdc);
	}
	return ret;
}

int WINAPI MyEndDoc(HDC hdc){
	/*LogW(L"MyEndDoc(HDC:%p)", hdc);
	LogW(L"PrintPage = %d",print_page_count);
	print_page_count = 0;
	return ((PFEndDoc)g_pOriginFunc6)((HDC)hdc);*/
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	int ret = g_pOriginFunc6(hdc);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"MyEndDoc[%s](HDC:0x%.8x)", PathFindFileName(module_name), hdc);
	}
	return ret;
}

HWND WINAPI MyCreateWindowExW(DWORD     dwExStyle,LPCWSTR   lpClassName,LPCWSTR   lpWindowName,DWORD     dwStyle,int       X,int       Y,int       nWidth,int       nHeight,HWND      hWndParent,HMENU     hMenu,HINSTANCE hInstance,LPVOID    lpParam){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	HWND ret = g_pOriginFunc17(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"[0x%.8x]MyCreateWindowExW[%s](ClassName:%s, WndName:%s, X:%d, Y:%d, Width:%d,Height:%d, pHandle:0x%.8x),", ret, PathFindFileName(module_name), lpClassName, lpWindowName, X, Y, nWidth, nHeight, hWndParent);
	}
	return ret;
}

HWND WINAPI MyCreateWindowExA(DWORD     dwExStyle, LPCSTR   lpClassName, LPCSTR   lpWindowName, DWORD     dwStyle, int       X, int       Y, int       nWidth, int       nHeight, HWND      hWndParent, HMENU     hMenu, HINSTANCE hInstance, LPVOID    lpParam){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	HWND ret = g_pOriginFunc18(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
		LogW(L"[0x%.8x]MyCreateindowExA[%s](ClassName:%s, WndName:%s, X:%d, Y:%d, Width:%d,Height:%d, pHandle:0x%.8x),", ret, PathFindFileName(module_name), lpClassName, lpWindowName, X, Y, nWidth, nHeight, hWndParent);
	}
	return ret;
}

HDC WINAPI MyCreateDCW(LPCTSTR lpszDriver, _In_ LPCTSTR lpszDevice, LPCTSTR lpszOutput, _In_ const DEVMODE *lpInitData){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	HDC ret = g_pOriginFunc19(lpszDriver, lpszDevice, lpszOutput, lpInitData);
	//if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"MyCreateDCW[%s](Driver:%s, Device:%s,Output:%s)", PathFindFileName(module_name), lpszDriver, lpszDevice, lpszOutput);
	//}
	return ret;
}

HDC WINAPI MyCreateDCA(LPCSTR pwszDriver, LPCSTR pwszDevice, LPCSTR pwszPort, const DEVMODEA *pdm){
	HMODULE hModule = NULL;
	TCHAR module_name[4096];
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)_ReturnAddress(), &hModule);
	GetModuleFileName(hModule, module_name, 4096);
	HDC ret = g_pOriginFunc20(pwszDriver, pwszDevice, pwszPort, pdm);
	//if (isSupportDll(PathFindFileName(module_name), inclusive_module)){
	LogW(L"MyCreateDCA[%s](Driver:%s, Device:%s,Output:%s)", PathFindFileName(module_name), pwszDriver, pwszDevice, pwszPort);
	//}
	return ret;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	//ini 결과 리턴 변수
	TCHAR szReturnString[1024] = { 0, };
	memset(szReturnString, NULL, 1024);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		LogW(L"ATTACH");
		//inclusive모듈가져오기
		GetPrivateProfileString(L"Module", L"Inclusive", L"", inclusive_module, 128, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		//원본함수 복사
		g_pOriginFunc = (PFCreateFileW)CreateFile;
		g_pOriginFunc2 = ReadFile;
		g_pOriginFunc3 = WriteFile;
		g_pOriginFunc4 = StartDocW;
		g_pOriginFunc5 = StartPage;
		g_pOriginFunc6 = EndDoc;
		g_pOriginFunc7 = CloseHandle;
		g_pOriginFunc8 = SetFilePointer;
		g_pOriginFunc9 = SetFilePointerEx;
		g_pOriginFunc10 = MoveFile;
		g_pOriginFunc11 = MoveFileEx;
		g_pOriginFunc12 = GetFileSize;
		g_pOriginFunc13 = GetFileSizeEx;
		g_pOriginFunc14 = CopyFile;
		g_pOriginFunc15 = CopyFileEx;
		g_pOriginFunc16 = StartDocA;
		g_pOriginFunc17 = CreateWindowEx;
		g_pOriginFunc18 = CreateWindowExA;
		g_pOriginFunc19 = CreateDC;
		g_pOriginFunc20 = CreateDCA;
		//detours hooking attach
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		GetPrivateProfileString(L"API", L"CreateFileW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc, (PROC)MyCreateFileW) != NO_ERROR){
				LogW(L"CreateFileW Attach Error!!");
			}
			else{
				LogW(L"CreateFileW Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"ReadFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc2, (PROC)MyReadFile) != NO_ERROR){
				LogW(L"ReadFile Attach Error!!");
			}
			else{
				LogW(L"ReadFile Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"WriteFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc3, (PROC)MyWriteFile) != NO_ERROR){
				LogW(L"WriteFile Attach Error!!");
			}
			else{
				LogW(L"WriteFile Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"StartDocW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc4, (PROC)MyStartDocW) != NO_ERROR){
				LogW(L"StartDocW Attach Error!!");
			}
			else{
				LogW(L"StartDocW Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"StartDocA", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc16, (PROC)MyStartDocA) != NO_ERROR){
				LogW(L"StartDocA Attach Error!!");
			}
			else{
				LogW(L"StartDocA Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"StartPage", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc5, (PROC)MyStartPage) != NO_ERROR){
				LogW(L"StartPage Attach Error!!");
			}
			else{
				LogW(L"StartPage Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"EndDoc", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc6, (PROC)MyEndDoc) != NO_ERROR){
				LogW(L"EndDoc Attach Error!!");
			}
			else{
				LogW(L"EndDoc Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CloseHandle", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc7, (PROC)MyCloseHandle) != NO_ERROR){
				LogW(L"CloseHandle Attach Error!!");
			}
			else{
				LogW(L"CloseHandle Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"SetFilePointer", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc8, (PROC)MySetFilePointer) != NO_ERROR){
				LogW(L"SetFilePointer Attach Error!!");
			}
			else{
				LogW(L"SetFilePointer Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"SetFilePointerEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc9, (PROC)MySetFilePointerEx) != NO_ERROR){
				LogW(L"SetFilePointerEx Attach Error!!");
			}
			else{
				LogW(L"SetFilePointerEx Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"MoveFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc10, (PROC)MyMoveFile) != NO_ERROR){
				LogW(L"MoveFile Attach Error!!");
			}
			else{
				LogW(L"MoveFile Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"MoveFileEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc11, (PROC)MyMoveFileEx) != NO_ERROR){
				LogW(L"MoveFileEx Attach Error!!");
			}
			else{
				LogW(L"MoveFileEx Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"GetFileSize", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc12, (PROC)MyGetFileSize) != NO_ERROR){
				LogW(L"GetFileSize Attach Error!!");
			}
			else{
				LogW(L"GetFileSize Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"GetFileSizeEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc13, (PROC)MyGetFileSizeEx) != NO_ERROR){
				LogW(L"GetFileSizeEx Attach Error!!");
			}
			else{
				LogW(L"GetFileSizeEx Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CopyFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc14, (PROC)MyCopyFile) != NO_ERROR){
				LogW(L"CopyFile Attach Error!!");
			}
			else{
				LogW(L"CopyFile Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CopyFileEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc15, (PROC)MyCopyFileEx) != NO_ERROR){
				LogW(L"CopyFileEx Attach Error!!");
			}
			else{
				LogW(L"CopyFileEx Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateWindowExW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc17, (PROC)MyCreateWindowExW) != NO_ERROR){
				LogW(L"CreateWindowExW Attach Error!!");
			}
			else{
				LogW(L"CreateWindowExW Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateWindowExA", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc18, (PROC)MyCreateWindowExA) != NO_ERROR){
				LogW(L"CreateWindowExA Attach Error!!");
			}
			else{
				LogW(L"CreateWindowExA Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateDCW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc19, (PROC)MyCreateDCW) != NO_ERROR){
				LogW(L"CreateDCW Attach Error!!");
			}
			else{
				LogW(L"CreateDCW Attach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateDCA", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourAttach(&(PVOID&)g_pOriginFunc20, (PROC)MyCreateDCA) != NO_ERROR){
				LogW(L"CreateDCA Attach Error!!");
			}
			else{
				LogW(L"CreateDCA Attach!!");
			}
		}
		if (DetourTransactionCommit() != NO_ERROR){
			LogW(L"Attach commit Error!!");
		}
		else{
			LogW(L"Attach commit!!");
		}
		break;
	case DLL_PROCESS_DETACH:
		LogW(L"Detach");
		//detours hooking detach
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		GetPrivateProfileString(L"API", L"CreateFileW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc, (PROC)MyCreateFileW) != NO_ERROR){
				LogW(L"CreateFileW Detach Error!!");
			}
			else{
				LogW(L"CreateFileW Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"ReadFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc2, (PROC)MyReadFile) != NO_ERROR){
				LogW(L"ReadFile Detach Error!!");
			}
			else{
				LogW(L"ReadFile Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"WriteFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc3, (PROC)MyWriteFile) != NO_ERROR){
				LogW(L"WriteFile Detach Error!!");
			}
			else{
				LogW(L"WriteFile Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"StartDocW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc4, (PROC)MyStartDocW) != NO_ERROR){
				LogW(L"StartDocW Detach Error!!");
			}
			else{
				LogW(L"StartDocW Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"StartDocA", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc16, (PROC)MyStartDocA) != NO_ERROR){
				LogW(L"StartDocA Detach Error!!");
			}
			else{
				LogW(L"StartDocA Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"StartPage", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc5, (PROC)MyStartPage) != NO_ERROR){
				LogW(L"StartPage Detach Error!!");
			}
			else{
				LogW(L"StartPage Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"EndDoc", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc6, (PROC)MyEndDoc) != NO_ERROR){
				LogW(L"EndDoc Detach Error!!");
			}
			else{
				LogW(L"EndDoc Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CloseHandle", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc7, (PROC)MyCloseHandle) != NO_ERROR){
				LogW(L"CloseHandle Detach Error!!");
			}
			else{
				LogW(L"CloseHandle Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"SetFilePointer", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc8, (PROC)MySetFilePointer) != NO_ERROR){
				LogW(L"SetFilePointer Detach Error!!");
			}
			else{
				LogW(L"SetFilePointer Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"SetFilePointerEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc9, (PROC)MySetFilePointerEx) != NO_ERROR){
				LogW(L"SetFilePointerEx Detach Error!!");
			}
			else{
				LogW(L"SetFilePointerEx Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"MoveFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc10, (PROC)MyMoveFile) != NO_ERROR){
				LogW(L"MoveFile Detach Error!!");
			}
			else{
				LogW(L"MoveFile Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"MoveFileEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc11, (PROC)MyMoveFileEx) != NO_ERROR){
				LogW(L"MoveFileEx Detach Error!!");
			}
			else{
				LogW(L"MoveFileEx Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"GetFileSize", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc12, (PROC)MyGetFileSize) != NO_ERROR){
				LogW(L"GetFileSize Detach Error!!");
			}
			else{
				LogW(L"GetFileSize Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"GetFileSizeEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc13, (PROC)MyGetFileSizeEx) != NO_ERROR){
				LogW(L"GetFileSize Detach Error!!");
			}
			else{
				LogW(L"GetFileSizeEx Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CopyFile", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc14, (PROC)MyCopyFile) != NO_ERROR){
				LogW(L"CopyFile Detach Error!!");
			}
			else{
				LogW(L"CopyFile Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CopyFileEx", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc15, (PROC)MyCopyFileEx) != NO_ERROR){
				LogW(L"CopyFile Detach Error!!");
			}
			else{
				LogW(L"CopyFile Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateWindowExW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc17, (PROC)MyCreateWindowExW) != NO_ERROR){
				LogW(L"CreateWindowExW Detach Error!!");
			}
			else{
				LogW(L"CreateWindowExW Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateWindowExA", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc18, (PROC)MyCreateWindowExA) != NO_ERROR){
				LogW(L"CreateWindowExA Detach Error!!");
			}
			else{
				LogW(L"CreateWindowExA Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateDCW", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc19, (PROC)MyCreateDCW) != NO_ERROR){
				LogW(L"CreateDCW Detach Error!!");
			}
			else{
				LogW(L"CreateDCW Detach!!");
			}
		}
		GetPrivateProfileString(L"API", L"CreateDCA", L"", szReturnString, 1024, L"C:\\dev\\API_Injector\\Debug\\Data.ini");
		if (_tcscmp(szReturnString, L"TRUE") == 0){
			if (DetourDetach(&(PVOID &)g_pOriginFunc20, (PROC)MyCreateDCA) != NO_ERROR){
				LogW(L"CreateDCA Detach Error!!");
			}
			else{
				LogW(L"CreateDCA Detach!!");
			}
		}
		if (DetourTransactionCommit() != NO_ERROR){
			LogW(L"Detach commit Error!!");
		}
		else{
			LogW(L"Detach commit!!");
		}
		break;
	}
	return TRUE;
}

