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

DLL_API bool create_win(int x, int y, int w, int h) {
    sprintLog("create_win: %d,%d,%d,%d \r\n", x, y, w, h);
    return true;
}

DLL_API bool set_win_pos(int x, int y) {
    sprintLog("set_win_pos: %d,%d,%d,%d \r\n", x, y);
    return true;
}

DLL_API bool set_win_size(int w, int h) {
    sprintLog("set_win_size: %d,%d \r\n", w, h);
    return true;
}

DLL_API bool quit_win() {
    sprintLog("quit_win: \r\n");
    return true;
}