#define  DLL_EXPORTS
#include "my_win.h"
#include "windows.h"
#include "stdio.h"
#include <sstream>
#include <process.h>
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
    //WriteToDebug(strBuf, strlen(strBuf));
    OutputDebugStringA(strBuf);
}

PFCallback g_cb = nullptr;
DLL_API void set_callback(PFCallback cb) {
    sprintLog("[ele-ffi] set_callback: %x \r\n", cb);
    g_cb = cb;
}
void RunJsCallback() {
    sprintLog("[ele-ffi] callback begin: %x \r\n", g_cb);
    if (g_cb) {
        int ret = g_cb(123);
        sprintLog("[ele-ffi] callback ret: %d \r\n", ret);
        // g_cb = nullptr;
    }
}

HWND g_hwnd = NULL;
typedef LRESULT (* CALLBACK PFWndProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LONG g_oldWinProc = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        sprintLog("[ele-ffi] on rcv WM_DESTROY: %x,%x \r\n", wParam, lParam);
        break;
    case WM_MOVE:
        sprintLog("[ele-ffi] on rcv WM_MOVE: %x,%x \r\n", wParam, lParam);
        break;
    case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE) {
            sprintLog("[ele-ffi] on rcv SC_MINIMIZE: %x,%x \r\n", wParam, lParam);
        } else if (wParam == SC_MOVE){
            sprintLog("[ele-ffi] on rcv SC_MOVE: %x,%x \r\n", wParam, lParam);
        } else if (wParam == SC_RESTORE) {
            sprintLog("[ele-ffi] on rcv SC_RESTORE: %x,%x \r\n", wParam, lParam);
        }
        break;
    case WM_ACTIVATE:
        sprintLog("[ele-ffi] on rcv WM_ACTIVATE: %x,%x \r\n", wParam, lParam);
        break;
    case WM_SETFOCUS: 
        sprintLog("[ele-ffi] on rcv WM_SETFOCUS: %x,%x \r\n", wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool onRcvMsg(MsgStruct & msg) {
    if(msg.type == set_win_hwnd) {
        g_hwnd = (HWND)msg.x;
    }
    sprintLog("[ele-ffi] MsgSvr onRcvMsg: %d,%x \r\n", msg.type);
    return true;
}
MsgSvr msgSvr(sizeof(MsgStruct), onRcvMsg);
void __cdecl threadProc(void*) {
    if (!msgSvr.Listen("player_win", 2000)) return;
}

int lastWinX = 0, lastWinY = 0, lastWinH = 0;
int lastDomX = 0, lastDomY = 0;
int lastW = 0, lastH = 0;
int lastScrollTop = 0;
DLL_API bool create_win(int winX, int winY, int winH, int domX, int domY, int w, int h, int hwnd) {
	lastWinX = winX; lastWinY = winY; lastWinH = winH;
	lastDomX = domX; lastDomY = domY;
	lastW = w; lastH = h;
    sprintLog("[ele-ffi] create_win: (%d,%d,%d),(%d,%d),[%d,%d],%x \r\n",
		winX, winY, winH, domX, domY, w, h, hwnd);
    ostringstream ostr;
    ostr << winX << "," << winY << "," << winH << "," << domX << "," << domY << "," 
		 << w << "," << h << ',' << hwnd;

    SHELLEXECUTEINFO ShExecInfo;
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOASYNC;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = "open";
    ShExecInfo.lpFile = "little_win.exe";
    ShExecInfo.lpParameters = ostr.str().c_str();
    ShExecInfo.lpDirectory = "./my_win/Release/";
    ShExecInfo.nShow = SW_NORMAL;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteEx(&ShExecInfo);

    UINT32 threadID;
    HANDLE hThread = (HANDLE)_beginthread(threadProc, 0, 0);

    g_oldWinProc = GetWindowLong((HWND)hwnd, GWL_WNDPROC);
    return true;
}

DLL_API bool set_win_pos(int winX, int winY, int winH, int domX, int domY, int scrollTop) {
	lastWinX = winX; lastWinY = winY; lastWinH = winH;
	lastDomX = domX; lastDomY = domY;
	lastScrollTop = scrollTop;
	sprintLog("[ele-ffi] set_win_pos: (%d,%d,%d),(%d,%d),%d,%x \r\n",
		winX, winY, winH, domX, domY, scrollTop, g_hwnd);

    MsgStruct msg;
    msg.type = set_new_win_pos;
    msg.x = lastWinX; 
	msg.y = lastWinY;
	msg.h_ = lastWinH;
	msg.x_ = lastDomX;
	msg.y_ = lastDomY;
    msg.w = lastW;
    msg.h = lastH;
	msg.offset = lastScrollTop;
    msgSvr.PostMsg(msg);
	//    if (!g_hwnd) return false;
    //PostMessage(g_hwnd, WM_SET_WIN_POS, 
    //    (lastX << 16) | lastY, (lastW << 16) | lastH);
    //RunJsCallback();
    return true;
}

DLL_API bool set_win_size(int w, int h) {
    sprintLog("[ele-ffi] set_win_size: [%d,%d],%x \r\n", w, h, g_hwnd);
    //if (!g_hwnd) return false;
    lastW = w; lastH = h;
    MsgStruct msg;
    msg.type = set_new_win_pos;
	msg.x = lastWinX;
	msg.y = lastWinY;
	msg.h_ = lastWinH;
	msg.x_ = lastDomX;
	msg.y_ = lastDomY;
	msg.w = lastW;
	msg.h = lastH;
	msg.offset = lastScrollTop;
    msgSvr.PostMsg(msg);
    //PostMessage(g_hwnd, WM_SET_WIN_POS, 
    //    (lastX << 16) | lastY, (lastW << 16) | lastH);
    //RunJsCallback();
    return true;
}

DLL_API bool show_win(int show) {
	sprintLog("[ele-ffi] show_win: %d,%x \r\n", show, g_hwnd);
	if (show == 1) {
		MsgStruct msg;
		msg.type = set_show;
		msg.x = SW_MINIMIZE;
		msgSvr.PostMsg(msg);
	}
    // if (!g_hwnd) return false;
    // PostMessage(g_hwnd, WM_SHOW_WIN, (WPARAM)show, 0);
    return true;
}

DLL_API bool quit_win() {
	sprintLog("[ele-ffi] quit_win: \r\n");
    MsgStruct msg;
    msg.type = set_exit;
    msgSvr.PostMsg(msg);
	//if (!g_hwnd) return false;
    //PostMessage(g_hwnd, WM_DESTROY, 0, 0);
    return true;
}



