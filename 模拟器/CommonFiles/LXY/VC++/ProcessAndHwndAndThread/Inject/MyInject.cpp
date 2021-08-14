#include "F:\Code\object\ProcessAndHwndAndThread\Inject\MyInject.h"
#include <Tlhelp32.h>


namespace LXY
{

	DWORD RemoteThreadInjection::m_SetPrivilege(void)
	{
		HANDLE hToken;
		LUID sedebugnameValue;
		TOKEN_PRIVILEGES tkp;

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
#if _DEBUG
			DebugBreak();
#endif // _DEBUG
			return -1;
		}

		//打开本进程的Token句柄
		if (!LookupPrivilegeValue(NULL, SE_CREATE_PERMANENT_NAME, &sedebugnameValue)) {
			CloseHandle(hToken);
#if _DEBUG
			DebugBreak();
#endif // _DEBUG
			return -2;
		}
		//查找权限信息
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = sedebugnameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
		{
			CloseHandle(hToken);
#if _DEBUG
			DebugBreak();
#endif // _DEBUG
			return -4;
		}
		//设置权限
		CloseHandle(hToken);
		return 0;
	}

	void RemoteThreadInjection::m_Inject(HANDLE hProcess, std::string strDll)
	{
		DWORD dwError = 0;
		if ((dwError = m_SetPrivilege()))
		{
			MD_Throw_Windows(m_SetPrivilege);
		}
		/*进程提权，有权限才能创建远程线程并注入*/

		LPVOID lpvAllocAddr = VirtualAllocEx(hProcess, NULL, strDll.size()+1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		/*在目标进程分配一段内存空间用于传递参数*/
		if (!lpvAllocAddr)
		{
			MD_Throw_Windows(VirtualAllocEx);
		}

		BOOL bRet = WriteProcessMemory(hProcess, lpvAllocAddr, strDll.c_str(), strDll.size() + 1, NULL);
		if (!bRet)
		{
			VirtualFreeEx(hProcess, lpvAllocAddr, 0, MEM_RELEASE);
			MD_Throw_Windows(WriteProcessMemory);
		}
		/*拷贝DLL路径名字到目标进程的内存*/

		HMODULE hModule = LoadLibraryA("KERNEL32.DLL");
		/*获取KERNEL32.DLL模块句柄(每个进程都有KERNEL32.DLL 并且虚拟内存地址相同)*/

		LPTHREAD_START_ROUTINE pStartFuncAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA");
		/*获取函数loadlibrary函数的地址*/
		FreeLibrary(hModule);

		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pStartFuncAddress, lpvAllocAddr, 0, NULL);
		if (hRemoteThread == NULL)
		{
			VirtualFreeEx(hProcess, lpvAllocAddr, 0, MEM_RELEASE);
			MD_Throw_Windows(CreateRemoteThread);
		}
		/*创建远程线程并获取远程线程的句柄*/

		//等待线程内核对象失效 即线程执行完毕
		WaitForSingleObject(hRemoteThread, INFINITE);
		VirtualFreeEx(hProcess, lpvAllocAddr, 0, MEM_RELEASE);
		if (!GetExitCodeThread(hRemoteThread, &dwError))
		{
			MD_Throw_Windows(GetExitCodeThread);
		}
		hModuleDll = (HMODULE)dwError;//线程执行完毕的返回值为模块句柄

		CloseHandle(hRemoteThread);
	}
	void RemoteThreadInjection::m_UnInject(HANDLE hProcess, HMODULE hModuleDll)
	{
		if (hModuleDll == NULL)
			return;
		HMODULE hModule = LoadLibraryA("KERNEL32.DLL");
		/*获取KERNEL32.DLL模块句柄(每个进程都有KERNEL32.DLL 并且虚拟内存地址相同)*/

		LPTHREAD_START_ROUTINE pStartFuncAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
		/*获取函数loadlibrary函数的地址*/
		FreeLibrary(hModule);
		 
		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pStartFuncAddress, hModuleDll, 0, NULL);
		if (hRemoteThread == NULL)
		{
			MD_Throw_Windows(CreateRemoteThread);
		}
		/*创建远程线程卸载我们的DLL*/
		CloseHandle(hRemoteThread);
	}
}