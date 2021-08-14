#pragma once
#include <Windows.h>
#include "F:\Code\object\Exception\MyExceptionWindows.h"
#include <string>
#include <vector>
namespace LXY
{
	struct m_Window
	{
		INT iPositonX =0 , iPositonY = 0;//窗口位置
		unsigned uHigh =0 , uVol = 0;//高度和宽度

		std::string strName;//窗口名
		std::string strClassName;//窗口类名


		DWORD dwProcessID = -1;//窗口所属的进程ID
		DWORD dwThreadID = -1;//窗口所属的线程ID

		HWND hWnd = NULL; //窗口句柄
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
			LXY::m_PrintMessageBox("窗口名：%s,窗口类名：%s\r\n	窗口进程：%d，窗口处理线程：%d\r\n \
				窗口位置:left<%d> right<%d> top<%d> bottom<%d>\r\n \
				窗口大小:长<%u>,宽<%u>\r\n",
				strName.c_str(), strClassName.c_str(), dwProcessID, dwThreadID,
				iPositonX, iPositonY, iPositonX + uVol, iPositonY + uHigh,
				uVol, uHigh);
		}
	};
	class m_WindowshWnd
	{
#define MAXHWNDSIZE 0x10
		/*友元声明以及using定义*/
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
		/*私有数据成员*/
		HWND hWnd = NULL;//精确查找到的窗口句柄
		HWND ahWnd[MAXHWNDSIZE] = {NULL};//如果窗口句柄不唯一 则将窗口句柄放入其中
		int ihWndSize = 0;

		std::string strName;//需要查找的窗口名字
		std::string strClassName;//需要查找的类名

		DWORD dwProcessID;//需要查找窗口所属的进程ID
		DWORD dwThreadID;//需要查找窗口所属的线程ID
		INT iPositonX, iPositonY;//窗口位置

		bool bVisible = true;

		m_Window m_window;
		std::vector<m_Window> m_vecWindow;

	public:
		/*公有数据成员*/

	public:
		/*构造函数与析构函数*/
		m_WindowshWnd() {
		}
		~m_WindowshWnd() {
		}
	public:
		/*导出函数*/
		//通过窗口名获取窗口句柄
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
		//通过窗口类名获取窗口句柄
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
		//通过窗口类名和窗口名获取窗口句柄
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
		//通过进程ID获取窗口句柄
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
		//通过鼠标位置获取窗口句柄
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
		/*其他非导出功能函数*/
		void m_clear() {
			hWnd = NULL;
			ihWndSize = 0;
			m_vecWindow.clear();
		}
		//通过窗口名获取窗口句柄
		void m_GetWindowhWndByName();
		void m_GetWindowhWndByNameAlmost();//部分查找

				//通过窗口名获取窗口句柄
		void m_GetWindowhWndByNameAll();
		void m_GetWindowhWndByNameAlmostAll();//部分查找
		//通过窗口类名获取窗口句柄
		void m_GetWindowhWndByClassName();
		//通过窗口类名和窗口名获取窗口句柄
		void m_GetWindowhWndByNameAndClassName();
		//通过进程ID获取窗口句柄
		void m_GetWindowhWndByProcessId();
		//通过线程ID获取窗口句柄
		void m_GetWindowhWndByThreadId();
		//通过坐标位置获取窗口句柄
		void m_GetWindowhWndByPosition();


	};
}


