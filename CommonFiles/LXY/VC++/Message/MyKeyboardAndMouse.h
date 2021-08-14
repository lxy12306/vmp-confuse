#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "F:\Code\object\Exception\MyExceptionWindows.h"

#define WM_GLOBALKEYBOARD WM_APP+110
#define MD_Throw_Handle(szError) throw LXY::m_HandleException(__FILE__,__LINE__,#szError, GetLastError())
namespace LXY
{
	//鼠标点击相关（绑定一个窗口句柄）
	class MouseRelated
	{
#define STOPSTR "G"
		/*友元声明以及using定义*/
	private:
		/*私有数据成员*/
		HWND hWnd;		//所属句柄
		POINT p_mouse;	//鼠标
		RECT rect;		//句柄坐标句柄

		LONG lXiangDuiX, lXiangDuiY; //记录相对坐标
		LONG lJueDuiX, lJueDuiY;//记录绝对坐标

		DWORD dwFenBianLvX, dwFenBianLvY;//分辨率

	public:
		/*公有数据成员*/

	public:
		/*构造函数与析构函数*/
		MouseRelated(HWND hwnd):hWnd(hwnd) {
			dwFenBianLvX = GetSystemMetrics(SM_CXSCREEN);
			dwFenBianLvY = GetSystemMetrics(SM_CYSCREEN);
			//获取当前分辨率
		}
		~MouseRelated() {
		}
	public:
		/*导出函数*/
		//根据按键获取当前鼠标位置（STOPSTR键）
		void e_GetMousePointNow(std::string& strKeyBoard)
		{
			if (strKeyBoard == STOPSTR)
			{
				GetCursorPos(&p_mouse);
				lJueDuiX = p_mouse.x;
				lJueDuiY = p_mouse.y;
				lXiangDuiX = p_mouse.x - rect.left;
				lXiangDuiY = p_mouse.y - rect.top;
				e_ShowPoint();
			}
		}
		void e_ShowPoint()
		{
			m_PrintMessageBox("相对X:<%d> 相对Y:<%d> \r\n,m_ShuBiaoDanJi(%d, %d) \r\n", lXiangDuiX, lXiangDuiY, lJueDuiX, lJueDuiY);
		}
		
		/// <summary>
		/// 鼠标单击事件 相对于hWnd标志的窗口
		/// </summary>
		/// <param name="lX"></param>
		/// <param name="lY"></param>
		void e_MouseOneClick_XiangDui(LONG lX, LONG lY)
		{
			if (!GetWindowRect(hWnd, &rect))
			{
				MD_Throw_Handle(e_MouseOneClick_xiangdui::GetWindowRect);
			}
			m_RelativeToAbsolute(lX, lY);
			e_MouseOneClick_JueDui(lX, lY);
		}
		//绝对坐标鼠标点击
		void e_MouseOneClick_JueDui(LONG lX, LONG lY)
		{
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, lX * 65535 / dwFenBianLvX, lY * 65535 / dwFenBianLvY, 0, 0);
			Sleep(50);
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			Sleep(50);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}

		//鼠标滚轮滑动的事件
		void e_MouseHuaDong(int i = -WHEEL_DELTA/*i表示滚轮滑动的多少 WHEEL_DELTA表示向上滑动到顶 WHEEL_DELTA表示向下滑动到最小*/)
		{
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, i, 0);
		}

	private:
		/*其他非导出功能函数*/
		void 
			m_RelativeToAbsolute(LONG& lX, LONG& lY)
		{
			lXiangDuiX = lX, lXiangDuiY = lY;
			lX = lX + rect.left;
			lY = lY + rect.top;
			lJueDuiX = lX, lJueDuiY = lY;
		}
	};

	//键盘消息HOOK 获取键盘消息
	class KeyBoardMessage
	{
		/*友元声明以及using定义*/
	private:
		/*私有数据成员*/
		static DWORD ms_dwThreadID;
		static HHOOK ms_hKeyboardHook;
		static DWORD ms_dwLastKey;
	public:
		/*公有数据成员*/
		std::vector<std::string> vecstrVKCode;
	public:
		/*构造函数与析构函数*/
		//全局消息挂钩
		KeyBoardMessage() {
			m_InitvecstrVKCode();
			ms_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyboardProc, NULL, 0);
		}
		KeyBoardMessage(DWORD dwThreadId) {
			m_InitvecstrVKCode();
			ms_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyboardProc, NULL, dwThreadId);
		}
		~KeyBoardMessage() {
			UnhookWindowsHookEx(ms_hKeyboardHook);
		}
	public:
		/*导出函数*/
	private:
		/*其他非导出功能函数*/

		void m_InitvecstrVKCode();
		static LRESULT CALLBACK MyLowLevelKeyboardProc(
			_In_ int    nCode,
			_In_ WPARAM wParam,
			_In_ LPARAM lParam
		);
	};

	//键盘事件
	class MyKeyBoardEvent
	{
		/*友元声明以及using定义*/
	private:
		/*私有数据成员*/
		std::unordered_map<std::string, unsigned> umapstruAnJianDuiYing;
	public:
		/*公有数据成员*/
		
	public:
		/*构造函数与析构函数*/
		MyKeyBoardEvent() {
			m_InitAnJian();
		}
		~MyKeyBoardEvent() {
		}
	public:
		/*导出函数*/
		

		/// <summary>
		/// 字符串 单字符表示一个事件 表示多个单字符事件
		/// </summary>
		/// <param name="strCin">要运行得事件</param>
		void e_CharCin(const std::string& strCin);

		/// <summary>
		/// 组合键事件
		/// </summary>
		/// <param name="strCin">组合键字符串</param>
		void e_ZuHeJian(const std::string& strCin);
	private:
		/*其他非导出功能函数*/
		void m_InitAnJian();
		void m_kbEvent(DWORD dwVirtualKeyCode)
		{
			keybd_event(dwVirtualKeyCode,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), 0/*按下*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), KEYEVENTF_KEYUP/*弹出*/, 0);
		}
		void m_kbEvent2(DWORD dwVirtualKeyCode1, DWORD dwVirtualKeyCode2)
		{
			keybd_event(dwVirtualKeyCode1,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), 0/*按下*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), 0/*按下*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), KEYEVENTF_KEYUP/*弹出*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode1,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), KEYEVENTF_KEYUP/*弹出*/, 0);
		}
		void m_kbEvent3(DWORD dwVirtualKeyCode1, DWORD dwVirtualKeyCode2, DWORD dwVirtualKeyCode3)
		{
			keybd_event(dwVirtualKeyCode1,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), 0/*按下*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), 0/*按下*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode3,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode3, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), 0/*按下*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode3,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode3, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), KEYEVENTF_KEYUP/*弹出*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), KEYEVENTF_KEYUP/*弹出*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode1,/*(扫描码*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*虚拟码翻译为扫码嘛*/), KEYEVENTF_KEYUP/*弹出*/, 0);
		}
		bool m_CheckIsDaXie() //判断大写是否打开 返回true为打开
		{
			auto state = GetKeyState(VK_CAPITAL)&1;
			return state == 1; 
		}
		
		//对c进行处理 获取事件并发出事件
		void m_DealChar(char c);
	};
}

