#pragma once
#include <Windows.h>

///////////////////////////窗口图标相关//////////////////////////

//设置Dialog图标
inline void lxy_SETDLGICONS(HWND hWnd, int idi) {
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)
        LoadIcon((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            MAKEINTRESOURCE(idi)));
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)
        LoadIcon((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            MAKEINTRESOURCE(idi)));
}

///////////////////////////窗口消息相关//////////////////////////

#define lxy_HANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))
//绑定窗口处理函数 和窗口 以及窗口消息