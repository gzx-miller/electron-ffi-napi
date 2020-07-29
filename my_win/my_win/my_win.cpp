#define  DLL_EXPORTS
#include "my_win.h"
#include "windows.h"
#include "stdio.h"
#include <sstream>
#include "MsgSvr.h"
using namespace std;

#define WM_CREATE_WIN WM_USER + 1
#define WM_SET_WIN_POS WM_USER + 2
#define WM_SHOW_WIN WM_USER + 3

static void WriteToDebug(const char* buffer, size_t size) {
    static HANDLE hFile = INVALID_HANDLE_VALUE;
    if (hFile == INVALID_HANDLE_VALUE) {
        hFile = CreateFileA("./debug.log", GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return;
        // SetFilePointer(hFile, 0, NULL, FILE_END);
    }
    DWORD dwBytesWritten;
    if (!WriteFile(hFile, buffer, size, &dwBytesWritten, NULL)) return;
    FlushFileBuffers(hFile);
}

void sprintLog(const char *format, ...) {
    char strBuf[512] = { 0 };
    va_list ap;
    va_start(ap, format);
    _vsnprintf_s(strBuf, sizeof(strBuf) - 1, format, ap);
    va_end(ap);
    WriteToDebug(strBuf, strlen(strBuf));
    OutputDebugStringA(strBuf);
}

HWND g_hwnd = NULL;
bool onRcvMsg(MsgStruct & msg) {
    if (msg.type == win_handle) {
        g_hwnd = (HWND)msg.val;
    }
    sprintLog("MsgSvr onRcvMsg: %d,%x \r\n", msg.type, msg.val);
    return true;
}
MsgSvr msgSvr(sizeof(MsgStruct), onRcvMsg);
int lastX = 0, lastY = 0, lastW = 0, lastH = 0;
DLL_API bool create_win(int x, int y, int w, int h, int hwnd) {
    lastX = x; lastY = y; lastW = w; lastH = h;
    sprintLog("create_win: %d,%d,%d,%d,%x \r\n", x, y, w, h, hwnd);
    ostringstream ostr;
    ostr << x << "," << y << "," << w << "," << h << ',' << hwnd;

    SHELLEXECUTEINFO ShExecInfo;
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOASYNC;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = "open";
    ShExecInfo.lpFile = "little_win.exe";
    ShExecInfo.lpParameters = ostr.str().c_str();
    ShExecInfo.lpDirectory = "D:/DCode/electron/electron-ffi-napi/my_win/Release/";
    ShExecInfo.nShow = SW_NORMAL;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteEx(&ShExecInfo);

    msgSvr.Listen("player_win");    // Listen can rcv one time.
    // msgSvr.WaitMsg();
    return true;
}

DLL_API bool set_win_pos(int x, int y) {
    sprintLog("set_win_pos: %d,%d,%x \r\n", x, y, g_hwnd);
    if (!g_hwnd) return false;
    lastX = x; lastY = y;
    PostMessage(g_hwnd, WM_SET_WIN_POS, 
        (lastX << 16) | lastY, (lastW << 16) | lastH);
    return true;
}

DLL_API bool set_win_size(int w, int h) {
    sprintLog("set_win_size: %d,%d,%x \r\n", w, h, g_hwnd);
    if (!g_hwnd) return false;
    lastW = w; lastH = h;
    PostMessage(g_hwnd, WM_SET_WIN_POS, 
        (lastX << 16) | lastY, (lastW << 16) | lastH);
    return true;
}

DLL_API bool show_win(int show) {
    sprintLog("show_win: %d,%x \r\n", show, g_hwnd);
    if (!g_hwnd) return false;
    PostMessage(g_hwnd, WM_SHOW_WIN, (WPARAM)show, 0);
    return true;
}

DLL_API bool quit_win() {
    sprintLog("quit_win: \r\n");
    if (!g_hwnd) return false;
    PostMessage(g_hwnd, WM_DESTROY, 0, 0);
    return true;
}