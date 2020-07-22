// little_win.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "little_win.h"

#define WM_SET_WIN_POS WM_USER + 2
#define WM_SHOW_WIN WM_USER + 3

int lastX = 0, lastY = 0, lastW = 0, lastH = 0;
void sprintLog(const char *format, ...) {
    char strBuf[512] = { 0 };
    va_list ap;
    va_start(ap, format);
    _vsnprintf_s(strBuf, sizeof(strBuf) - 1, format, ap);
    va_end(ap);
    // WriteToDebug(strBuf, strlen(strBuf));
    OutputDebugStringA(strBuf);
}

HWND g_hwnd;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SET_WIN_POS: {
        lastY = wParam & 0xffff;
        lastX = wParam >> 16;
        lastH = lParam & 0xffff;
        lastW = lParam >> 16;
        SetWindowPos(g_hwnd, HWND_TOPMOST,
            lastX, lastY, lastW, lastH, SWP_SHOWWINDOW);
        sprintLog("rcv set_win_pos: %d,%d,%d,%d \r\n",
            lastX, lastY, lastW, lastH);
        }
        break;
    case WM_SHOW_WIN: {
        BOOL show = (BOOL)wParam;
        ShowWindow(g_hwnd, show? SW_SHOW : SW_HIDE);
        sprintLog("rcv show_win: %d \r\n", show);
        }
        break;
    case WM_PAINT:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = "MyWinClass";
    wcex.hIcon = NULL;
    wcex.hIconSm = NULL;
    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    g_hwnd = CreateWindow("MyWinClass", "MyWinTitle", 
        WS_OVERLAPPEDWINDOW| WS_EX_TOPMOST,
        100, 100, 400, 300, nullptr, nullptr, hInstance, nullptr);
    if (!g_hwnd) return FALSE;
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    return TRUE;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    //for (int i = 0; i < 1000; ++i) {
    //    SetWindowPos(g_hwnd, HWND_TOPMOST, 1000 - i, 1000 - i, 100 + i, 100 + i, SWP_SHOWWINDOW);
    //}

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
