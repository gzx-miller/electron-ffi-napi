// little_win.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include <string>
#include "little_win.h"
#include "MsgClient.h"

using namespace std;

#define WM_SET_WIN_POS WM_USER + 2
#define WM_SHOW_WIN WM_USER + 3

int lastX = 0, lastY = 0, lastW = 0, lastH = 0;

HWND g_hwnd;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SET_WIN_POS: {
        lastY = wParam & 0xffff;
        lastX = wParam >> 16;
        lastH = lParam & 0xffff;
        lastW = lParam >> 16;
        SetWindowPos(g_hwnd, HWND_TOP, lastX, lastY, lastW, lastH, SWP_SHOWWINDOW);
        sprintLog("rcv set_win_pos: %d,%d,%d,%d \r\n",
            lastX, lastY, lastW, lastH);
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
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
    wcex.hbrBackground = (HBRUSH)(COLOR_HIGHLIGHTTEXT);
    wcex.lpszClassName = "MyWinClass";
    wcex.hIcon = NULL;
    wcex.hIconSm = NULL;
    return RegisterClassEx(&wcex);
}

bool onRcvMsg(MsgStruct & msg) {
    sprintLog("MsgClient onRcvMsg: %d,%x \r\n", msg.type, msg.val);
    return true;
}
MsgClient msgClient(sizeof(MsgStruct), onRcvMsg);

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    char* cmdLine = GetCommandLine();
    int x = 0, y = 0, w = 400, h = 300;
    HWND hwnd_parent = (HWND)0;
    // sprintLog(cmdLine, strlen(cmdLine));
    string strCmd(cmdLine);
    int f = strCmd.find(' ');
    string strParams = strCmd.substr(f);
    sscanf(strParams.c_str(), "%d,%d,%d,%d,%d",
        &x, &y, &w, &h, &hwnd_parent);
    sprintLog("InitInstance: %d,%d,%d,%d,0x%x \r\n", x, y, w, h, hwnd_parent);
    
    // hwnd_parent = FindWindow("Chrome_WidgetWin_1", "my_electron_win");
    if (hwnd_parent == NULL) {
        g_hwnd = CreateWindowEx(WS_EX_TOPMOST,
            "MyWinClass", "MyWinTitle",
            WS_POPUPWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            x, y, w, h, hwnd_parent, nullptr, hInstance, nullptr);
    } else {
        long style = GetWindowLong(hwnd_parent, GWL_STYLE);
        style |= WS_CLIPCHILDREN;
        SetWindowLong(hwnd_parent, GWL_STYLE, style);
        g_hwnd = CreateWindowEx(WS_EX_TOPMOST,
            "MyWinClass", "MyWinTitle",
            WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            x, y, w, h, hwnd_parent, nullptr, hInstance, nullptr);
    }

    if (!g_hwnd) return FALSE;
    msgClient.Connect(string("player_win"));
    MsgStruct msg;
    msg.type = win_handle;
    msg.val = (int)g_hwnd;
    msgClient.PostMsg(msg);
    
    SetWindowPos(g_hwnd, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
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

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
