#pragma once
#include <Windows.h>
#include "F:\Code\object\Exception\MyExceptionWindows.h"
#include <string>
#include <vector>
namespace LXY
{
	struct m_Window
	{
		INT iPositonX =0 , iPositonY = 0;//����λ��
		unsigned uHigh =0 , uVol = 0;//�߶ȺͿ��

		std::string strName;//������
		std::string strClassName;//��������


		DWORD dwProcessID = -1;//���������Ľ���ID
		DWORD dwThreadID = -1;//�����������߳�ID

		HWND hWnd = NULL; //���ھ��
		m_Window() = default;
		m_Window(HWND hwnd):hWnd(hwnd)
		{
			CHAR szBuffer[MAX_PATH + 1];
			szBuffer[MAX_PATH] = 0;
			GetWindowTextA(hwnd, szBuffer, MAX_PATH);
			strName = szBuffer;
			RealGetWindowClassA(hwnd, szBuffer, MAX_PATH);
			strClassName = szBuffer;

			dwThreadID = GetWindowThreadProcessId(hwnd, &dwProcessID);

			RECT rect;
			GetWindowRect(hwnd, &rect);
			iPositonX = rect.left;
			iPositonY = rect.top;

			uVol = rect.right - rect.left;
			uHigh = rect.bottom - rect.top;
		}

		void e_show()
		{
			LXY::m_PrintMessageBox("��������%s,����������%s\r\n	���ڽ��̣�%d�����ڴ����̣߳�%d\r\n \
				����λ��:left<%d> right<%d> top<%d> bottom<%d>\r\n \
				���ڴ�С:��<%u>,��<%u>\r\n",
				strName.c_str(), strClassName.c_str(), dwProcessID, dwThreadID,
				iPositonX, iPositonY, iPositonX + uVol, iPositonY + uHigh,
				uVol, uHigh);
		}
	};
	class m_WindowshWnd
	{
#define MAXHWNDSIZE 0x10
		/*��Ԫ�����Լ�using����*/
		friend BOOL CALLBACK EnumWindowsNameProc(HWND hwnd, LPARAM lParam);
		friend BOOL CALLBACK EnumWindowsNameAlmostProc(HWND hwnd, LPARAM lParam);
		friend BOOL CALLBACK EnumWindowsNameProcAll(HWND hwnd, LPARAM lParam);
		friend BOOL CALLBACK EnumWindowsNameAlmostProcAll(HWND hwnd, LPARAM lParam);

		friend BOOL CALLBACK EnumWindowsClassNameProc(HWND hwnd, LPARAM lParam);
		friend BOOL CALLBACK EnumWindowsNameAndClassNameProc(HWND hwnd, LPARAM lParam);

		friend BOOL CALLBACK EnumWindowsProcessIDProc(HWND hwnd, LPARAM lParam);
		friend BOOL CALLBACK EnumWindowsThreadIDProc(HWND hwnd, LPARAM lParam);
		friend BOOL CALLBACK EnumWindowsPositionProc(HWND hwnd, LPARAM lParam);

	private:
		/*˽�����ݳ�Ա*/
		HWND hWnd = NULL;//��ȷ���ҵ��Ĵ��ھ��
		HWND ahWnd[MAXHWNDSIZE] = {NULL};//������ھ����Ψһ �򽫴��ھ����������
		int ihWndSize = 0;

		std::string strName;//��Ҫ���ҵĴ�������
		std::string strClassName;//��Ҫ���ҵ�����

		DWORD dwProcessID;//��Ҫ���Ҵ��������Ľ���ID
		DWORD dwThreadID;//��Ҫ���Ҵ����������߳�ID
		INT iPositonX, iPositonY;//����λ��

		bool bVisible = true;

		m_Window m_window;
		std::vector<m_Window> m_vecWindow;

	public:
		/*�������ݳ�Ա*/

	public:
		/*���캯������������*/
		m_WindowshWnd() {
		}
		~m_WindowshWnd() {
		}
	public:
		/*��������*/
		//ͨ����������ȡ���ھ��
		m_Window e_GetWindowhWndByName(CHAR const* szName,bool bWanZheng = TRUE)
		{
			strName = szName;
			try
			{
				if(bWanZheng)
					m_GetWindowhWndByName();
				else
					m_GetWindowhWndByNameAlmost();
			}
			catch (const LXY::m_HandleException& me)
			{
				me.e_ShowError();
				throw;
			}
			return m_window;
		}
		std::vector<m_Window> e_GetWindowhWndByNameAll(CHAR const* szName,bool bWanZheng = TRUE)
		{
			strName = szName;
			try
			{
				if (bWanZheng)
					m_GetWindowhWndByNameAll();
				else
					m_GetWindowhWndByNameAlmostAll();
			}
			catch (const LXY::m_HandleException& me)
			{
				me.e_ShowError();
				throw;
			}
			return m_vecWindow;
		}
		//ͨ������������ȡ���ھ��
		std::vector<m_Window> e_GetWindowhWndByClassName(CHAR const* szClassName)
		{
			strClassName = szClassName;
			try
			{
				m_GetWindowhWndByClassName();
			}
			catch (const LXY::m_HandleException& me)
			{
				me.e_ShowError();
				throw;
			}
			return m_vecWindow;
		}
		//ͨ�����������ʹ�������ȡ���ھ��
		m_Window e_GetWindowhWndByNameAndClassName(CHAR const* szName, CHAR const* szClassName)
		{
			strName = szName;
			strClassName = szClassName;
			try
			{
				m_GetWindowhWndByNameAndClassName();
			}
			catch (const LXY::m_HandleException& me)
			{
				me.e_ShowError();
				throw;
			}
			return m_window;

		}
		//ͨ������ID��ȡ���ھ��
		std::vector<m_Window> e_GetWindowhWndByProcessId(DWORD dwProcessId)
		{
			this->dwProcessID = dwProcessId;
			try
			{
				m_GetWindowhWndByProcessId();
			}
			catch (const LXY::m_HandleException& me)
			{
				me.e_ShowError();
				throw;
			}
			return m_vecWindow;
		}
		m_Window e_GetWindowhWndByThreadId(DWORD dwThreadID)
		{
			this->dwThreadID = dwThreadID;
			try
			{
				m_GetWindowhWndByThreadId();
			}
			catch (const LXY::m_HandleException& me)
			{
				me.e_ShowError();
				throw;
			}
			return m_window;
		}
		//ͨ�����λ�û�ȡ���ھ��
		std::vector<m_Window> e_GetWindowhWndByPosition(INT x, INT y)
		{
			iPositonX = x, iPositonY = y;
			try
			{
				m_GetWindowhWndByPosition();
			}
			catch (const LXY::m_HandleException& me)
			{
				me.e_ShowError();
				throw;
			}
			return m_vecWindow;
		}


	private:
		/*�����ǵ������ܺ���*/
		void m_clear() {
			hWnd = NULL;
			ihWndSize = 0;
			m_vecWindow.clear();
		}
		//ͨ����������ȡ���ھ��
		void m_GetWindowhWndByName();
		void m_GetWindowhWndByNameAlmost();//���ֲ���

				//ͨ����������ȡ���ھ��
		void m_GetWindowhWndByNameAll();
		void m_GetWindowhWndByNameAlmostAll();//���ֲ���
		//ͨ������������ȡ���ھ��
		void m_GetWindowhWndByClassName();
		//ͨ�����������ʹ�������ȡ���ھ��
		void m_GetWindowhWndByNameAndClassName();
		//ͨ������ID��ȡ���ھ��
		void m_GetWindowhWndByProcessId();
		//ͨ���߳�ID��ȡ���ھ��
		void m_GetWindowhWndByThreadId();
		//ͨ������λ�û�ȡ���ھ��
		void m_GetWindowhWndByPosition();


	};
}


