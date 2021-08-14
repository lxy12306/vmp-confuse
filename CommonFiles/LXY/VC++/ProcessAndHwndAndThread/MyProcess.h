#pragma once
#include <Windows.h>
#include <string>
#include "F:/Code/object/Exception/MyExceptionWindows.h"

namespace LXY
{
	class SingleProcess
	{
	
		/*友元声明以及using定义*/
#define MD_Throw_Windows(szError) throw LXY::m_WindowsException(__FILE__,__LINE__,#szError, GetLastError())
	protected:
		/*保护的私有数据成员*/
		std::wstring wstrProcessName; //需要查找的进程名
		std::wstring wstrExeName; //需要查找的进程名
		DWORD dwProcessId; ///需要查找的进程ID
		HANDLE hProcess; ///需要查找的进程的句柄
	public:
		/*公有数据成员*/
	public:
		/*构造函数与析构函数*/
		SingleProcess(std::wstring wstr) :wstrProcessName(wstr) {
	
			if (!m_GetProcessIdByName(wstrProcessName.c_str(), &dwProcessId))
			{
				MD_Throw_Windows(m_GetProcessIdByName);
			}
			if (!m_GetProcessHandleById(dwProcessId, &hProcess))
			{
				MD_Throw_Windows(m_GetProcessHandleById);
			}
			TCHAR atcBuffer[MAX_PATH] = { 0 };
			if (!m_GetProcessNameByHanlde(hProcess, atcBuffer))
			{
				MD_Throw_Windows(m_GetProcessNameByHanlde);
			}
			wstrExeName = atcBuffer;
		}
		virtual ~SingleProcess() {
			CloseHandle(hProcess);
		}
	public:
		/*导出函数*/
	private:
		/*其他非导出功能函数*/
	
		/// <summary>
		/// 通过进程名获取进程ID
		/// </summary>
		/// <param name="lpszProcessName">进程名</param>
		/// <param name="pdwProcessId">进程PID</param>
		/// <returns>是否成功</returns>
		BOOL m_GetProcessIdByName(LPCTSTR lpszProcessName, PDWORD pdwProcessId);
		/// <summary>
		/// 通过进程ID获取进程句柄
		/// </summary>
		/// <param name="dwProcessId"></param>
		/// <param name="phProcess"></param>
		/// <returns></returns>
		BOOL m_GetProcessHandleById(DWORD dwProcessId, PHANDLE phProcess);
		/// <summary>
		/// 通过进程句柄获取进程名
		/// </summary>
		/// <param name="hProcess"></param>
		/// <param name="lpszProcessName"></param>
		/// <returns></returns>
		BOOL m_GetProcessNameByHanlde(HANDLE hProcess, LPTSTR lpszProcessName);
	};
}

