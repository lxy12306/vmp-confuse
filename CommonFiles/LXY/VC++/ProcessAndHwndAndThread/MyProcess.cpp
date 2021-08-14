#include "F:\Code\object\ProcessAndHwndAndThread\MyProcess.h"
#include <Tlhelp32.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

namespace LXY
{
	using std::wstring;
	BOOL SingleProcess::m_GetProcessIdByName(LPCTSTR lpszProcessName, PDWORD pdwProcessId)
	{
		*pdwProcessId = 0;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		//打开进程快照句柄
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
#if _DEBUG
			DebugBreak();
#endif // _DEBUG
			return FALSE;
		}

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof pe32;
		if (Process32First(hSnapshot, &pe32))
		{
			do {
				if (lstrcmpiW(lpszProcessName, pe32.szExeFile) == 0)
				{
					CloseHandle(hSnapshot);
					*pdwProcessId = pe32.th32ProcessID;
					return TRUE;
				}
			} while (Process32Next(hSnapshot, &pe32));
		}
		//遍历经常快照 找到进程ID  如果找到则返回NULL
		CloseHandle(hSnapshot);
#if _DEBUG
		DebugBreak();
#endif // _DEBUG
		return FALSE;
	}
	BOOL SingleProcess::m_GetProcessHandleById(DWORD dwProcessId, PHANDLE phProcess)
	{
		*phProcess = NULL;

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		//以读写 展示信息 创建线程的权限打开进程句柄
		if (!hProcess)
		{
#if _DEBUG
			DebugBreak();
#endif // _DEBUG
			return FALSE;
		}

		*phProcess = hProcess;
		return TRUE;
	}
	BOOL SingleProcess::m_GetProcessNameByHanlde(HANDLE hProcess, LPTSTR lpszProcessName)
	{

		DWORD dwRet = GetModuleFileNameEx(
			hProcess,
			NULL,
			lpszProcessName,
			MAX_PATH
		);
		if (dwRet < MAX_PATH)
			return TRUE;
		else
		{
#if _DEBUG
			DebugBreak();
#endif // _DEBUG
			return FALSE;
		}
	}

}