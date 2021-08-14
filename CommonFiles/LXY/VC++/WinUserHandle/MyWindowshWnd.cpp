#include "Mym_WindowshWnd.h"
namespace LXY
{
#define MD_Throw_Handle(szError) throw LXY::m_HandleException(__FILE__,__LINE__,#szError, GetLastError())

#if _DEBUG
	#define ERROR_MESSEGE(FuncName) \
		LXY::m_PrintDebug("%s is Error,Error Code = %d",#FuncName, GetLastError())
#elif
	#define ERROR_MESSEGE(FuncName)
#endif

	BOOL CALLBACK EnumWindowsNameProc(HWND hwnd, LPARAM lParam)
	{
		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;
		CHAR szName[0x200 + 1];
		szName[0x200] = 0;
		if (GetWindowTextA(hwnd, szName, 0x200) == 0 && GetLastError() != 0)
		{
			ERROR_MESSEGE(GetWindowTextA);//无法获取窗口名 或者标题为NULL
			return true;//继续查找
		}
		if (strcmp(szName, pClass->strName.c_str()) == 0)
		{
			pClass->hWnd = hwnd;
			return false;
		}
		return true;//继续查找
	}
	BOOL CALLBACK EnumWindowsNameAlmostProc(HWND hwnd, LPARAM lParam)
	{
		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;
		CHAR szName[0x200 + 1];
		szName[0x200] = 0;
		if (GetWindowTextA(hwnd, szName, 0x200) == 0 && GetLastError() != 0)
		{
			ERROR_MESSEGE(GetWindowTextA);//无法获取窗口名 或者标题为NULL
			return true;//继续查找
		}
		if (strstr(szName, pClass->strName.c_str()) != NULL)
		{
			pClass->hWnd = hwnd;
			return false;
		}
		return true;//继续查找
	}
	BOOL CALLBACK EnumWindowsNameProcAll(HWND hwnd, LPARAM lParam)
	{
		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;
		CHAR szName[0x200 + 1];
		szName[0x200] = 0;
		if (GetWindowTextA(hwnd, szName, 0x200) == 0 && GetLastError() != 0)
		{
			ERROR_MESSEGE(GetWindowTextA);//无法获取窗口名 或者标题为NULL
			return true;//继续查找
		}
		if (strcmp(szName, pClass->strName.c_str()) == 0)
		{
			pClass->ahWnd[pClass->ihWndSize++] = hwnd;
			if (pClass->ihWndSize == MAXHWNDSIZE)
				return false;
			else
				return true;
		}
		return true;//继续查找
	}
	BOOL CALLBACK EnumWindowsNameAlmostProcAll(HWND hwnd, LPARAM lParam)
	{
		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;
		CHAR szName[0x200 + 1];
		szName[0x200] = 0;
		if (GetWindowTextA(hwnd, szName, 0x200) == 0 && GetLastError() != 0)
		{
			ERROR_MESSEGE(GetWindowTextA);//无法获取窗口名 或者标题为NULL
			return true;//继续查找
		}
		if (strstr(szName, pClass->strName.c_str()) != NULL)
		{
			pClass->ahWnd[pClass->ihWndSize++] = hwnd;
			if (pClass->ihWndSize == MAXHWNDSIZE)
				return false;
			else
				return true;
			return false;
		}
		return true;//继续查找
	}
	BOOL CALLBACK EnumWindowsClassNameProc(HWND hwnd, LPARAM lParam)
	{

		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;
		CHAR szClassName[0x200 + 1];
		szClassName[0x200] = 0;
		if (RealGetWindowClassA(hwnd, szClassName, 0x200) == 0 && GetLastError() != 0)
		{
			ERROR_MESSEGE(RealGetWindowClassA);//无法获取窗口名 或者标题为NULL
			return true;
		}
		if (strcmp(szClassName, pClass->strClassName.c_str()) == 0)
		{
			pClass->ahWnd[pClass->ihWndSize++] = hwnd;
			if (pClass->ihWndSize == MAXHWNDSIZE)
				return false;
			else
				return true;
		}
		return true;//继续查找
	}
	BOOL CALLBACK EnumWindowsNameAndClassNameProc(HWND hwnd, LPARAM lParam)
	{
		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;

		CHAR szName[0x200 + 1];
		szName[0x200] = 0;
		CHAR szClassName[0x200 + 1];
		szClassName[0x200] = 0;

		if (GetWindowTextA(hwnd, szName, 0x200) == 0 && GetLastError() != 0)
		{
			ERROR_MESSEGE(GetWindowTextA);//无法获取窗口名 或者标题为NULL
			return true;
		}
		if (RealGetWindowClassA(hwnd, szClassName, 0x200) == 0 && GetLastError() != 0)
		{
			ERROR_MESSEGE(RealGetWindowClassA);//无法获取窗口名 或者标题为NULL
			return true;
		}

		if (strcmp(szName, pClass->strName.c_str()) == 0)
		{
			if (strcmp(szClassName, pClass->strClassName.c_str()) == 0)
			{
				pClass->hWnd = hwnd;
				return false;
			}
		}
		return true;//继续查找
	}

	BOOL CALLBACK EnumWindowsProcessIDProc(HWND hwnd, LPARAM lParam)
	{
		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;

		DWORD dwProcessID = 0;
		DWORD dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessID);

		if (pClass->dwProcessID == dwProcessID)
		{
			pClass->ahWnd[pClass->ihWndSize++] = hwnd;
			if (pClass->ihWndSize == MAXHWNDSIZE)
				return false;
			else
				return true;
		}

		return true;//继续查找
	}
	BOOL CALLBACK EnumWindowsThreadIDProc(HWND hwnd, LPARAM lParam)
	{
		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;

		DWORD dwProcessID = 0;
		DWORD dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessID);

		if (pClass->dwThreadID == dwThreadId)
		{
			pClass->hWnd = hwnd;
				return false;
		}

		return true;//继续查找
	}

	BOOL CALLBACK EnumWindowsPositionProc(HWND hwnd, LPARAM lParam)
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);

		//显式可见的窗口 当窗口最小化之后 它的左或者上接近0
		if (rect.left <= 10 && rect.left >= -10)
			return true;
		if (rect.top <= 10 && rect.top >= -10)
			return true;

		m_WindowshWnd* pClass = (m_WindowshWnd*)lParam;
		if (pClass->iPositonX<rect.left || pClass->iPositonX > rect.right || pClass->iPositonY < rect.top || pClass->iPositonY > rect.bottom)
		{
			return true;
		}
		if (pClass->bVisible && !IsWindowVisible(hwnd))  //判断是否顶层窗口并且可见  
		{
			return true;
		}

		pClass->ahWnd[pClass->ihWndSize++] = hwnd;
		if (pClass->ihWndSize == MAXHWNDSIZE)
			return false;
		else
			return true;

	}

	void m_m_WindowshWnd::
		m_GetWindowhWndByName()
	{
		m_clear();
		EnumWindows(EnumWindowsNameProc, (LPARAM)this);
		if (hWnd == NULL)
		{
			MD_Throw_Handle(m_GetWindowhWndByName);
		}
		m_window = m_Window(hWnd);
	}
	void m_WindowshWnd::
		m_GetWindowhWndByNameAlmost()
	{
		m_clear();
		EnumWindows(EnumWindowsNameAlmostProc, (LPARAM)this);
		if (hWnd == NULL)
		{
			MD_Throw_Handle(m_GetWindowhWndByNameAlmost);
		}
		m_window = m_Window(hWnd);
	}
	void m_WindowshWnd::
		m_GetWindowhWndByNameAll()
	{
		m_clear();
		EnumWindows(EnumWindowsNameProcAll, (LPARAM)this);
		if (ihWndSize == 0)
		{
			MD_Throw_Handle(m_GetWindowhWndByNameAll);
		}
		for (int i = 0; i < ihWndSize; ++i)
		{
			m_vecWindow.push_back(m_Window(ahWnd[i]));
		}
	}
	void m_WindowshWnd::
		m_GetWindowhWndByNameAlmostAll()
	{
		m_clear();
		EnumWindows(EnumWindowsNameAlmostProcAll, (LPARAM)this);
		if (ihWndSize == 0)
		{
			MD_Throw_Handle(m_GetWindowhWndByNameAlmostAll);
		}
		for (int i = 0; i < ihWndSize; ++i)
		{
			m_vecWindow.push_back(m_Window(ahWnd[i]));
		}
	}
	void m_WindowshWnd::
		m_GetWindowhWndByClassName()
	{
		m_clear();
		EnumWindows(EnumWindowsClassNameProc, (LPARAM)this);
		if (ihWndSize == 0)
		{
			MD_Throw_Handle(m_GetWindowhWndByClassName);
		}
		for (int i = 0; i < ihWndSize; ++i)
		{
			m_vecWindow.push_back(m_Window(ahWnd[i]));
		}
	}
	void m_WindowshWnd::
		m_GetWindowhWndByNameAndClassName()
	{
		m_clear();
		EnumWindows(EnumWindowsNameAndClassNameProc, (LPARAM)this);
		if (hWnd == NULL)
		{
			MD_Throw_Handle(m_GetWindowhWndByNameAndClassName);
		}
		m_window = m_Window(hWnd);
	}
	void m_WindowshWnd::
		m_GetWindowhWndByProcessId()
	{
		m_clear();
		EnumWindows(EnumWindowsProcessIDProc, (LPARAM)this);
		if (ihWndSize == 0)
		{
			MD_Throw_Handle(m_GetWindowhWndByProcessId);
		}
		for (int i = 0; i < ihWndSize; ++i)
		{
			m_vecWindow.push_back(m_Window(ahWnd[i]));
		}
	}
	void m_WindowshWnd::
		m_GetWindowhWndByThreadId()
	{
		m_clear();
		EnumWindows(EnumWindowsThreadIDProc, (LPARAM)this);
		if (hWnd == NULL)
		{
			MD_Throw_Handle(m_GetWindowhWndByThreadId);
		}
		m_window = m_Window(hWnd);
	}
	void m_WindowshWnd::
		m_GetWindowhWndByPosition()
	{
		m_clear();
		EnumWindows(EnumWindowsPositionProc, (LPARAM)this);
		if (ihWndSize == 0)
		{
			MD_Throw_Handle(m_GetWindowhWndByPosition);
		}
		for (int i = 0; i < ihWndSize; ++i)
		{
			m_vecWindow.push_back(m_Window(ahWnd[i]));
		}
	}

}