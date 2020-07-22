#define  DLL_EXPORTS
#include "my_win.h"
#include "windows.h"
#include "stdio.h"

#define WM_CREATE_WIN WM_USER + 1
#define WM_SET_WIN_POS WM_USER + 2
#define WM_SHOW_WIN WM_USER + 3

void sprintLog(const char *format, ...) {
    char strBuf[512] = { 0 };
    va_list ap;
    va_start(ap, format);
    _vsnprintf_s(strBuf, sizeof(strBuf) - 1, format, ap);
    va_end(ap);
    OutputDebugStringA(strBuf);
}

HWND g_hwnd = NULL;
int lastX = 0, lastY = 0, lastW = 0, lastH = 0;
DLL_API bool create_win(int x, int y, int w, int h) {
    if (g_hwnd == NULL) {
        g_hwnd = FindWindow("MyWinClass", "MyWinTitle");
        if (g_hwnd == NULL) return false;
    }
    
    lastX = x; lastY = y; lastW = w; lastH = h;
    PostMessage(g_hwnd, WM_SET_WIN_POS, 
        (lastX<<16) |lastY, (lastW<<16) | lastH);
    sprintLog("create_win: %d,%d,%d,%d \r\n", x, y, w, h);
    return true;
}
DLL_API bool set_win_pos(int x, int y) {
    if (g_hwnd == NULL) {
        g_hwnd = FindWindow("MyWinClass", "MyWinTitle");
        if (g_hwnd == NULL) return false;
    }
    lastX = x; lastY = y;
    PostMessage(g_hwnd, WM_SET_WIN_POS, 
        (lastX << 16) | lastY, (lastW << 16) | lastH);
    sprintLog("set_win_pos: %d,%d \r\n", x, y);
    return true;
}

DLL_API bool set_win_size(int w, int h) {
    if (g_hwnd == NULL) {
        g_hwnd = FindWindow("MyWinClass", "MyWinTitle");
        if (g_hwnd == NULL) return false;
    }
    lastW = w; lastH = h;
    PostMessage(g_hwnd, WM_SET_WIN_POS, 
        (lastX << 16) | lastY, (lastW << 16) | lastH);
    sprintLog("set_win_size: %d,%d \r\n", w, h);
    return true;
}

DLL_API bool show_win(int show) {
    if (g_hwnd == NULL) {
        g_hwnd = FindWindow("MyWinClass", "MyWinTitle");
        if (g_hwnd == NULL) return false;
    }
    PostMessage(g_hwnd, WM_SHOW_WIN, (WPARAM)show, 0);
    sprintLog("show_win: %d \r\n", show);
    return true;
}

DLL_API bool quit_win() {
    if (g_hwnd == NULL) {
        g_hwnd = FindWindow("MyWinClass", "MyWinTitle");
        if (g_hwnd == NULL) return false;
    }
    PostMessage(g_hwnd, WM_DESTROY, 0, 0);
    sprintLog("quit_win: \r\n");
    return true;
}