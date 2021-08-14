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

		//�򿪱����̵�Token���
		if (!LookupPrivilegeValue(NULL, SE_CREATE_PERMANENT_NAME, &sedebugnameValue)) {
			CloseHandle(hToken);
#if _DEBUG
			DebugBreak();
#endif // _DEBUG
			return -2;
		}
		//����Ȩ����Ϣ
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
		//����Ȩ��
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
		/*������Ȩ����Ȩ�޲��ܴ���Զ���̲߳�ע��*/

		LPVOID lpvAllocAddr = VirtualAllocEx(hProcess, NULL, strDll.size()+1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		/*��Ŀ����̷���һ���ڴ�ռ����ڴ��ݲ���*/
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
		/*����DLL·�����ֵ�Ŀ����̵��ڴ�*/

		HMODULE hModule = LoadLibraryA("KERNEL32.DLL");
		/*��ȡKERNEL32.DLLģ����(ÿ�����̶���KERNEL32.DLL ���������ڴ��ַ��ͬ)*/

		LPTHREAD_START_ROUTINE pStartFuncAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA");
		/*��ȡ����loadlibrary�����ĵ�ַ*/
		FreeLibrary(hModule);

		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pStartFuncAddress, lpvAllocAddr, 0, NULL);
		if (hRemoteThread == NULL)
		{
			VirtualFreeEx(hProcess, lpvAllocAddr, 0, MEM_RELEASE);
			MD_Throw_Windows(CreateRemoteThread);
		}
		/*����Զ���̲߳���ȡԶ���̵߳ľ��*/

		//�ȴ��߳��ں˶���ʧЧ ���߳�ִ�����
		WaitForSingleObject(hRemoteThread, INFINITE);
		VirtualFreeEx(hProcess, lpvAllocAddr, 0, MEM_RELEASE);
		if (!GetExitCodeThread(hRemoteThread, &dwError))
		{
			MD_Throw_Windows(GetExitCodeThread);
		}
		hModuleDll = (HMODULE)dwError;//�߳�ִ����ϵķ���ֵΪģ����

		CloseHandle(hRemoteThread);
	}
	void RemoteThreadInjection::m_UnInject(HANDLE hProcess, HMODULE hModuleDll)
	{
		if (hModuleDll == NULL)
			return;
		HMODULE hModule = LoadLibraryA("KERNEL32.DLL");
		/*��ȡKERNEL32.DLLģ����(ÿ�����̶���KERNEL32.DLL ���������ڴ��ַ��ͬ)*/

		LPTHREAD_START_ROUTINE pStartFuncAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
		/*��ȡ����loadlibrary�����ĵ�ַ*/
		FreeLibrary(hModule);
		 
		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pStartFuncAddress, hModuleDll, 0, NULL);
		if (hRemoteThread == NULL)
		{
			MD_Throw_Windows(CreateRemoteThread);
		}
		/*����Զ���߳�ж�����ǵ�DLL*/
		CloseHandle(hRemoteThread);
	}
}