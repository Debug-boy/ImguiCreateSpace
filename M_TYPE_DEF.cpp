#include "M_TYPE_DEF.h"

double AngToRad(double ang)
{
	return ang * (M_PI / 180.0f);
}

float AngToRad(float _x)
{
	return (float)AngToRad((double)_x);
}

CHAR* ToTextA(LPCSTR _Format, ...)
{
	static CHAR transString[1024];
	va_list list_param;
	va_start(list_param, _Format);
	vsprintf(transString, _Format, list_param);
	va_end(list_param);
	return transString;
}

WCHAR* ToTextW(LPCWSTR _Format, ...)
{
	static wchar_t transString[1024];
	va_list list_param;
	va_start(list_param, _Format);
	wvsprintfW(transString, _Format, list_param);
	va_end(list_param);
	return transString;
}

BOOL GetWindowClientAreaData(HWND hWndClient, LONG& cx, LONG& cy, LONG& cw, LONG& ch)
{
	RECT clienRect = { 0 };
	POINT clienPoint = { 0 };
	BOOL getState = false;
	M_RECT<long>clien;

	getState = GetClientRect(hWndClient, &clienRect) && ClientToScreen(hWndClient, &clienPoint);

	clien.x = clienPoint.x;
	clien.y = clienPoint.y;

	clien.w = clienRect.right - clienRect.left;
	clien.h = clienRect.bottom - clienRect.top;

	cx = clien.x;
	cy = clien.y;
	cw = clien.w;
	ch = clien.h;
	return getState;
}

DWORD GetProcessID(const char* processName)
{
	DWORD localPid = 0;
	BOOL getState;
	LPPROCESSENTRY32 lpProcessEntry32 = nullptr;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		lpProcessEntry32 = new PROCESSENTRY32;
		if (lpProcessEntry32)
		{
			lpProcessEntry32->dwSize = sizeof(PROCESSENTRY32);
			getState = Process32First(hSnapshot, lpProcessEntry32);
			while (getState)
			{
				if (!_tcscmp(lpProcessEntry32->szExeFile, TEXT(processName)))
				{
					OutputDebugString(lpProcessEntry32->szExeFile);
					OutputDebugString("\n");
					localPid = lpProcessEntry32->th32ProcessID;
					break;
				}
				getState = Process32Next(hSnapshot, lpProcessEntry32);
			}
		}
	}
	delete lpProcessEntry32;
	CloseHandle(hSnapshot);
	return localPid;
}

DWORD GetModuleBase(DWORD processId,const char* moduleName)
{
	DWORD localModule = 0;
	BOOL getState;
	LPMODULEENTRY32 lpModuleEntry32 = nullptr;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		lpModuleEntry32 = new MODULEENTRY32;
		if (lpModuleEntry32)
		{
			lpModuleEntry32->dwSize = sizeof(MODULEENTRY32);
			getState = Module32First(hSnapshot, lpModuleEntry32);
			while (getState)
			{
				//OutputDebugString(lpModuleEntry32->szModule);
				//OutputDebugString(ToText(" %X",lpModuleEntry32->hModule));
				//OutputDebugString("\n");
				if (!_tcscmp(lpModuleEntry32->szModule, TEXT(moduleName)))
				{
					localModule = (DWORD)lpModuleEntry32->hModule;
					break;
				}
				getState = Module32Next(hSnapshot, lpModuleEntry32);
			}
		}
	}
	delete lpModuleEntry32;
	CloseHandle(hSnapshot);
	return localModule;
}

void PrintVec3(M_VECTOR3<FLOAT> vec3)
{
	OutputDebugString(ToText("%f|%f|%f\n", vec3.x, vec3.y, vec3.z));
}
