#pragma once
#include "F:\Code\object\ProcessAndHwndAndThread\MyProcess.h"
namespace LXY
{


	class RemoteThreadInjection:public SingleProcess
	{
		/*友元声明以及using定义*/
#define MD_Throw_Handle(szError) throw LXY::m_HandleException(__FILE__,__LINE__,#szError, GetLastError())
	private:
		/*私有数据成员*/
		std::string strDllName; //注入DLL的名字
		HMODULE hModuleDll = NULL; //注入DLL的模块句柄

	public:
		/*公有数据成员*/

	public:
		/*构造函数与析构函数*/
		RemoteThreadInjection(std::wstring wstr,std::string strDll) :SingleProcess(wstr), strDllName(strDll){
			m_Inject(hProcess, strDllName);
		}

		~RemoteThreadInjection () override
		{
			m_UnInject(hProcess, hModuleDll);
		}
	public:
		/*导出函数*/
	private:
		/*其他非导出功能函数*/

		/// <summary>
		/// 设置本进程的权限
		/// </summary>
		/// <returns>错误窗台</returns>
		DWORD m_SetPrivilege(void);
		/// <summary>
		/// 远程线程注入DLL
		/// </summary>
		/// <param name="hProcess">注入的进程句柄</param>
		/// <param name="strDll">注入的DLL路径</param>
		void m_Inject(HANDLE hProcess, std::string strDll);
		/// <summary>
		/// 卸载注入
		/// </summary>
		/// <param name="hProcess">卸载注入的进程句柄</param>
		/// <param name="hModuleDll">卸载注入的DLL模块句柄</param>
		void m_UnInject(HANDLE hProcess, HMODULE hModuleDll);
	};

}

