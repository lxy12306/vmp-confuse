#pragma once
#include "F:\Code\object\ProcessAndHwndAndThread\MyProcess.h"
namespace LXY
{


	class RemoteThreadInjection:public SingleProcess
	{
		/*��Ԫ�����Լ�using����*/
#define MD_Throw_Handle(szError) throw LXY::m_HandleException(__FILE__,__LINE__,#szError, GetLastError())
	private:
		/*˽�����ݳ�Ա*/
		std::string strDllName; //ע��DLL������
		HMODULE hModuleDll = NULL; //ע��DLL��ģ����

	public:
		/*�������ݳ�Ա*/

	public:
		/*���캯������������*/
		RemoteThreadInjection(std::wstring wstr,std::string strDll) :SingleProcess(wstr), strDllName(strDll){
			m_Inject(hProcess, strDllName);
		}

		~RemoteThreadInjection () override
		{
			m_UnInject(hProcess, hModuleDll);
		}
	public:
		/*��������*/
	private:
		/*�����ǵ������ܺ���*/

		/// <summary>
		/// ���ñ����̵�Ȩ��
		/// </summary>
		/// <returns>����̨</returns>
		DWORD m_SetPrivilege(void);
		/// <summary>
		/// Զ���߳�ע��DLL
		/// </summary>
		/// <param name="hProcess">ע��Ľ��̾��</param>
		/// <param name="strDll">ע���DLL·��</param>
		void m_Inject(HANDLE hProcess, std::string strDll);
		/// <summary>
		/// ж��ע��
		/// </summary>
		/// <param name="hProcess">ж��ע��Ľ��̾��</param>
		/// <param name="hModuleDll">ж��ע���DLLģ����</param>
		void m_UnInject(HANDLE hProcess, HMODULE hModuleDll);
	};

}

