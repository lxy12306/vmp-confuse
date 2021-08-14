#include "MyKeyboardAndMouse.h"
#include "F:/Code/object/Exception/MyExceptionWindows.h"
using namespace std;



namespace LXY
{

	/// <summary>
	/// string���и� 
	/// </summary>
	/// <param name="strCin">��Ҫ�и��string</param>
	/// <param name="iCFenGe">�и���ַ�</param>
	/// <returns>std::vector<std::string></returns>
	inline std::vector<std::string> stringSplitToString(std::string strCin, int iCFenGe)
	{
		std::vector<std::string> vecstrRet;
		size_t nBegin = 0;
		strCin += char(iCFenGe);
		auto nSize = strCin.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			if (strCin[i] == iCFenGe)
			{
				vecstrRet.emplace_back(strCin.substr(nBegin, i - nBegin));
				nBegin = i + 1;
			}
		}
		return vecstrRet;
	}


	DWORD KeyBoardMessage::ms_dwThreadID = GetCurrentThreadId();
	HHOOK KeyBoardMessage::ms_hKeyboardHook = NULL;
	DWORD KeyBoardMessage::ms_dwLastKey = 0;
	void KeyBoardMessage::m_InitvecstrVKCode()
	{
		vecstrVKCode.resize(0xFF, "");

		//���ⰴ��1
		vecstrVKCode[0x8] = "Back Space";
		vecstrVKCode[0xD] = "Enter";
		//���ְ���
		for (int i = 0x30; i < 0x3A; i++)
		{
			char cTemp = '0' + i - 0x30;
			vecstrVKCode[i] += cTemp;
		}
		//��ĸ����
		for (int i = 0x41; i < 0x5B; i++)
		{
			char cTemp = 'A' + i - 0x41;
			vecstrVKCode[i] += cTemp;
		}
		//С��������
		for (int i = 0x60; i < 0x6A; i++)
		{
			char cTemp = '0' + i - 0x60;
			string strTemp = "Numeric keypad 9 key";
			strTemp[15] = cTemp;
			vecstrVKCode[i] = strTemp;
		}

		//F����
		for (int i = 0x70; i < 0x88; i++)
		{
			char acTemp[3];
			sprintf_s(acTemp, "%-2d", i - 0x70+1);
			string strTemp = "F";
			strTemp += acTemp;
			vecstrVKCode[i] = strTemp;
		}
		//���ⰴ��2
		vecstrVKCode[0x5B] = "Win";
		vecstrVKCode[0xA0] = "Left Shift";
		vecstrVKCode[0xA1] = "Right Shift";
		vecstrVKCode[0xA2] = "Left Ctrl";
		vecstrVKCode[0xA3] = "Right Shift";

	}

	bool m_IsSpecialKey(DWORD dwKey)
	{
		if (dwKey == 0xA0/*��Shift*/|| dwKey == 0xA1/*��Shift*/ || dwKey == 0xA2/*��Ctrl*/ || dwKey == 0xA3/*��Ctrl*/)
			return true;
		return false;
	}
	
	LRESULT CALLBACK KeyBoardMessage::MyLowLevelKeyboardProc(
		_In_ int    nCode,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam)
	{
		if (WM_KEYDOWN == wParam)
		{
			KBDLLHOOKSTRUCT* pKHook = (KBDLLHOOKSTRUCT*)lParam;
			if (m_IsSpecialKey(ms_dwLastKey))
			{
				if(ms_dwLastKey == pKHook->vkCode)
					return CallNextHookEx(ms_hKeyboardHook, nCode, wParam, lParam);
			}
			PostThreadMessage(ms_dwThreadID, WM_GLOBALKEYBOARD, pKHook->vkCode, pKHook->flags);
			ms_dwLastKey = pKHook->vkCode;
			//vkCode ������ scanCode ������
			//flags ״̬��  80 ��ʾ����  81 ��ʾ������Alt��������
		}
	
		return CallNextHookEx(ms_hKeyboardHook, nCode, wParam, lParam);
	}

	void MyKeyBoardEvent::m_InitAnJian()
	{
		//���ⰴ��1
		umapstruAnJianDuiYing["Back Space"] = 0x8;
		umapstruAnJianDuiYing["Tab"] = 0x8;
		umapstruAnJianDuiYing["Enter"] = 0xD ;
		umapstruAnJianDuiYing[" "] = 0x20;
		umapstruAnJianDuiYing["Caps Lock"] = 0x14;

		//���ְ���
		for (int i = 0x30; i < 0x3A; i++)
		{
			char cTemp = '0' + i - 0x30;
			umapstruAnJianDuiYing[string(1,cTemp)] = i;
		}
		//��ĸ����
		for (int i = 0x41; i < 0x5B; i++)
		{
			char cTemp = 'A' + i - 0x41;
			umapstruAnJianDuiYing[string(1, cTemp)] = i;
		}

		//���ⰴ��
		umapstruAnJianDuiYing["Win"] = 0x5b;//Window��
		umapstruAnJianDuiYing["Shift"] = 0xA0;
		umapstruAnJianDuiYing["Ctrl"] = 0xA2;


		umapstruAnJianDuiYing["`"] = 0xc0;
		umapstruAnJianDuiYing["-"] = 0xbd;
		umapstruAnJianDuiYing["="] = 0xbb;
		umapstruAnJianDuiYing["["] = 0xdb;
		umapstruAnJianDuiYing["]"] = 0xdd;
		umapstruAnJianDuiYing["\\"] = 0xdc;
		umapstruAnJianDuiYing[";"] = 0xba;
		umapstruAnJianDuiYing["'"] = 0xde;
		umapstruAnJianDuiYing[","] = 0xbc;
		umapstruAnJianDuiYing["."] = 0xbe;
		umapstruAnJianDuiYing["/"] = 0xbf;
	}

	void MyKeyBoardEvent::m_DealChar(char c)
	{
		if (isalpha(c))
		{
			if (islower(c))
			{
				if (m_CheckIsDaXie())
				{
					m_kbEvent(0x14);//�ָ�Сд
					Sleep(20);
				}

				c = toupper(c);
			}
			else
			{
				if (!m_CheckIsDaXie())
				{
					m_kbEvent(0x14);//��ɴ�д
					Sleep(20);
				}
			}
		}
		if (umapstruAnJianDuiYing.find(string(1, c)) != umapstruAnJianDuiYing.end())
		{
			m_kbEvent(umapstruAnJianDuiYing[string(1, c)]);
			return;
		}
		else
		{
			string strZuHeJian = "Shift+";
			switch (c)
			{
			case '!': strZuHeJian += '1'; break;
			case '@': strZuHeJian += '2'; break;
			case '#': strZuHeJian += '3'; break;
			case '$': strZuHeJian += '4'; break;
			case '%': strZuHeJian += '5'; break;
			case '^': strZuHeJian += '6'; break;
			case '&': strZuHeJian += '7'; break;
			case '*': strZuHeJian += '8'; break;
			case '(': strZuHeJian += '9'; break;
			case ')': strZuHeJian += '0'; break;
			case '~': strZuHeJian += '`'; break;
			case '{': strZuHeJian += '['; break;
			case '}': strZuHeJian += ']'; break;
			case '|': strZuHeJian += '\\'; break;
			case ':': strZuHeJian += ';'; break;
			case '"': strZuHeJian += '\''; break;
			case '<': strZuHeJian += ','; break;
			case '>': strZuHeJian += '.'; break;
			case '?': strZuHeJian += '/'; break;
			default:
				break;
			}
			e_ZuHeJian(strZuHeJian);
		}

	}
	void MyKeyBoardEvent::e_CharCin(const std::string& strCin)
	{
		for (auto value : strCin)
		{
			m_DealChar(value);
			
			Sleep(20);
		}
	}
	void MyKeyBoardEvent::e_ZuHeJian(const std::string& strCin)
	{
		auto vecstr = stringSplitToString(strCin, '+');
		if (vecstr.size() == 1)
		{
			m_kbEvent(umapstruAnJianDuiYing[vecstr[0]]);
		}
		else if (vecstr.size() == 2)
		{
			m_kbEvent2(umapstruAnJianDuiYing[vecstr[0]], umapstruAnJianDuiYing[vecstr[1]]);
		}
		else if (vecstr.size() == 3)
		{
			m_kbEvent3(umapstruAnJianDuiYing[vecstr[0]], umapstruAnJianDuiYing[vecstr[1]], umapstruAnJianDuiYing[vecstr[2]]);
		}
		else
		{
			MD_Throw_Handle(MyKeyBoardEvent::e_ZuHeJian);
		}
	}
}