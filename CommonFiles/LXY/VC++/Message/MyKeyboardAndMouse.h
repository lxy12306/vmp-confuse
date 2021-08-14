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
	//�������أ���һ�����ھ����
	class MouseRelated
	{
#define STOPSTR "G"
		/*��Ԫ�����Լ�using����*/
	private:
		/*˽�����ݳ�Ա*/
		HWND hWnd;		//�������
		POINT p_mouse;	//���
		RECT rect;		//���������

		LONG lXiangDuiX, lXiangDuiY; //��¼�������
		LONG lJueDuiX, lJueDuiY;//��¼��������

		DWORD dwFenBianLvX, dwFenBianLvY;//�ֱ���

	public:
		/*�������ݳ�Ա*/

	public:
		/*���캯������������*/
		MouseRelated(HWND hwnd):hWnd(hwnd) {
			dwFenBianLvX = GetSystemMetrics(SM_CXSCREEN);
			dwFenBianLvY = GetSystemMetrics(SM_CYSCREEN);
			//��ȡ��ǰ�ֱ���
		}
		~MouseRelated() {
		}
	public:
		/*��������*/
		//���ݰ�����ȡ��ǰ���λ�ã�STOPSTR����
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
			m_PrintMessageBox("���X:<%d> ���Y:<%d> \r\n,m_ShuBiaoDanJi(%d, %d) \r\n", lXiangDuiX, lXiangDuiY, lJueDuiX, lJueDuiY);
		}
		
		/// <summary>
		/// ��굥���¼� �����hWnd��־�Ĵ���
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
		//�������������
		void e_MouseOneClick_JueDui(LONG lX, LONG lY)
		{
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, lX * 65535 / dwFenBianLvX, lY * 65535 / dwFenBianLvY, 0, 0);
			Sleep(50);
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			Sleep(50);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}

		//�����ֻ������¼�
		void e_MouseHuaDong(int i = -WHEEL_DELTA/*i��ʾ���ֻ����Ķ��� WHEEL_DELTA��ʾ���ϻ������� WHEEL_DELTA��ʾ���»�������С*/)
		{
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, i, 0);
		}

	private:
		/*�����ǵ������ܺ���*/
		void 
			m_RelativeToAbsolute(LONG& lX, LONG& lY)
		{
			lXiangDuiX = lX, lXiangDuiY = lY;
			lX = lX + rect.left;
			lY = lY + rect.top;
			lJueDuiX = lX, lJueDuiY = lY;
		}
	};

	//������ϢHOOK ��ȡ������Ϣ
	class KeyBoardMessage
	{
		/*��Ԫ�����Լ�using����*/
	private:
		/*˽�����ݳ�Ա*/
		static DWORD ms_dwThreadID;
		static HHOOK ms_hKeyboardHook;
		static DWORD ms_dwLastKey;
	public:
		/*�������ݳ�Ա*/
		std::vector<std::string> vecstrVKCode;
	public:
		/*���캯������������*/
		//ȫ����Ϣ�ҹ�
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
		/*��������*/
	private:
		/*�����ǵ������ܺ���*/

		void m_InitvecstrVKCode();
		static LRESULT CALLBACK MyLowLevelKeyboardProc(
			_In_ int    nCode,
			_In_ WPARAM wParam,
			_In_ LPARAM lParam
		);
	};

	//�����¼�
	class MyKeyBoardEvent
	{
		/*��Ԫ�����Լ�using����*/
	private:
		/*˽�����ݳ�Ա*/
		std::unordered_map<std::string, unsigned> umapstruAnJianDuiYing;
	public:
		/*�������ݳ�Ա*/
		
	public:
		/*���캯������������*/
		MyKeyBoardEvent() {
			m_InitAnJian();
		}
		~MyKeyBoardEvent() {
		}
	public:
		/*��������*/
		

		/// <summary>
		/// �ַ��� ���ַ���ʾһ���¼� ��ʾ������ַ��¼�
		/// </summary>
		/// <param name="strCin">Ҫ���е��¼�</param>
		void e_CharCin(const std::string& strCin);

		/// <summary>
		/// ��ϼ��¼�
		/// </summary>
		/// <param name="strCin">��ϼ��ַ���</param>
		void e_ZuHeJian(const std::string& strCin);
	private:
		/*�����ǵ������ܺ���*/
		void m_InitAnJian();
		void m_kbEvent(DWORD dwVirtualKeyCode)
		{
			keybd_event(dwVirtualKeyCode,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), 0/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), KEYEVENTF_KEYUP/*����*/, 0);
		}
		void m_kbEvent2(DWORD dwVirtualKeyCode1, DWORD dwVirtualKeyCode2)
		{
			keybd_event(dwVirtualKeyCode1,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), 0/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), 0/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), KEYEVENTF_KEYUP/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode1,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), KEYEVENTF_KEYUP/*����*/, 0);
		}
		void m_kbEvent3(DWORD dwVirtualKeyCode1, DWORD dwVirtualKeyCode2, DWORD dwVirtualKeyCode3)
		{
			keybd_event(dwVirtualKeyCode1,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), 0/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), 0/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode3,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode3, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), 0/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode3,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode3, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), KEYEVENTF_KEYUP/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode2,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode2, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), KEYEVENTF_KEYUP/*����*/, 0);
			Sleep(5);
			keybd_event(dwVirtualKeyCode1,/*(ɨ����*/MapVirtualKey(dwVirtualKeyCode1, MAPVK_VK_TO_VSC/*�����뷭��Ϊɨ����*/), KEYEVENTF_KEYUP/*����*/, 0);
		}
		bool m_CheckIsDaXie() //�жϴ�д�Ƿ�� ����trueΪ��
		{
			auto state = GetKeyState(VK_CAPITAL)&1;
			return state == 1; 
		}
		
		//��c���д��� ��ȡ�¼��������¼�
		void m_DealChar(char c);
	};
}

