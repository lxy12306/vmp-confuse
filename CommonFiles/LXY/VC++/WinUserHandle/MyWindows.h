#pragma once
#include <Windows.h>

///////////////////////////����ͼ�����//////////////////////////

//����Dialogͼ��
inline void lxy_SETDLGICONS(HWND hWnd, int idi) {
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)
        LoadIcon((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            MAKEINTRESOURCE(idi)));
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)
        LoadIcon((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            MAKEINTRESOURCE(idi)));
}

///////////////////////////������Ϣ���//////////////////////////

#define lxy_HANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))
//�󶨴��ڴ����� �ʹ��� �Լ�������Ϣ