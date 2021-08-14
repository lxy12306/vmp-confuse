#pragma once
#include <Windows.h>
#include <string>
#include "F:/Code/object/Exception/MyExceptionWindows.h"

namespace LXY
{
	class SingleProcess
	{
	
		/*��Ԫ�����Լ�using����*/
#define MD_Throw_Windows(szError) throw LXY::m_WindowsException(__FILE__,__LINE__,#szError, GetLastError())
	protected:
		/*������˽�����ݳ�Ա*/
		std::wstring wstrProcessName; //��Ҫ���ҵĽ�����
		std::wstring wstrExeName; //��Ҫ���ҵĽ�����
		DWORD dwProcessId; ///��Ҫ���ҵĽ���ID
		HANDLE hProcess; ///��Ҫ���ҵĽ��̵ľ��
	public:
		/*�������ݳ�Ա*/
	public:
		/*���캯������������*/
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
		/*��������*/
	private:
		/*�����ǵ������ܺ���*/
	
		/// <summary>
		/// ͨ����������ȡ����ID
		/// </summary>
		/// <param name="lpszProcessName">������</param>
		/// <param name="pdwProcessId">����PID</param>
		/// <returns>�Ƿ�ɹ�</returns>
		BOOL m_GetProcessIdByName(LPCTSTR lpszProcessName, PDWORD pdwProcessId);
		/// <summary>
		/// ͨ������ID��ȡ���̾��
		/// </summary>
		/// <param name="dwProcessId"></param>
		/// <param name="phProcess"></param>
		/// <returns></returns>
		BOOL m_GetProcessHandleById(DWORD dwProcessId, PHANDLE phProcess);
		/// <summary>
		/// ͨ�����̾����ȡ������
		/// </summary>
		/// <param name="hProcess"></param>
		/// <param name="lpszProcessName"></param>
		/// <returns></returns>
		BOOL m_GetProcessNameByHanlde(HANDLE hProcess, LPTSTR lpszProcessName);
	};
}

