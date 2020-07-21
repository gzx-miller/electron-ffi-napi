#define  DLL_EXPORTS
#include "my_win.h"
#include "windows.h"
#include "stdio.h"

static void WriteToDebug(const char* buffer, size_t size) {
    static HANDLE hFile = NULL;
    hFile = CreateFileA("./debug.log", GENERIC_WRITE, FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;
    SetFilePointer(hFile, 0, NULL, FILE_END);
    DWORD dwBytesWritten;
    if (FALSE == WriteFile(hFile, buffer, size, &dwBytesWritten, NULL)) return;
    CloseHandle(hFile);
}

void sprintLog(const char *format, ...) {
    char strBuf[512] = { 0 };
    va_list ap;
    va_start(ap, format);
    _vsnprintf_s(strBuf, sizeof(strBuf) - 1, format, ap);
    va_end(ap);
    WriteToDebug(strBuf, strlen(strBuf));
}

HWND g_wnd = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
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

int lastX = 0, lastY = 0, lastW = 0, lastH = 0;
DLL_API bool create_win(int x, int y, int w, int h) {
    MyRegisterClass(NULL);
    g_wnd = CreateWindow("MyWinClass", "MyWinTitle", WS_OVERLAPPEDWINDOW,
        100, 100, 400, 300, nullptr, nullptr, NULL, nullptr);
    if (!g_wnd) return FALSE;
    lastX = x; lastY = y; lastW = w; lastH = h;
    ShowWindow(g_wnd, SW_SHOWNORMAL);
    UpdateWindow(g_wnd);

    sprintLog("create_win: %d,%d,%d,%d \r\n", x, y, w, h);
    return true;
}
DLL_API bool set_win_pos(int x, int y) {
    SetWindowPos(
        g_wnd,
        HWND_TOPMOST,
        x, y, lastW, lastH,
        SWP_SHOWWINDOW | SWP_DRAWFRAME
    );
    lastX = x; lastY = y;
    sprintLog("set_win_pos: %d,%d \r\n", x, y);
    return true;
}

DLL_API bool set_win_size(int w, int h) {
    SetWindowPos(
        g_wnd,
        HWND_TOPMOST,
        lastX, lastY, w, h,
        SWP_SHOWWINDOW | SWP_DRAWFRAME
    );
    lastW = w; lastH = h;
    sprintLog("set_win_size: %d,%d \r\n", w, h);
    return true;
}

DLL_API bool quit_win() {
    DestroyWindow(g_wnd);
    sprintLog("quit_win: \r\n");
    return true;
}