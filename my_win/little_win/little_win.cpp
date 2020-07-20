#define  DLL_EXPORTS
#include <stdio.h>
#include "windows.h"
#include "little_win.h"

extern "C" {
#include "SDL.h"
}

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

SDL_Window * sdlWindow = nullptr;
SDL_Renderer* sdlRenderer = nullptr;
SDL_Texture* sdlTexture = nullptr;
SDL_Rect sdlRect = { 0 };

DLL_API bool create_win(int x, int y, int w, int h) {
    sprintLog("create_win: %d,%d,%d,%d \r\n", x, y, w, h);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        return false;
    }
    sdlWindow = SDL_CreateWindow("fd-player", x, y, w, h, SDL_WINDOW_ALWAYS_ON_TOP| SDL_WINDOW_SHOWN);
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_RenderPresent(sdlRenderer);
    return true;
}

DLL_API bool set_win_pos(int x, int y) {
    sprintLog("set_win_pos: %d,%d \r\n", x, y);
    SDL_SetWindowPosition(sdlWindow, x, y);
    return true;
}

DLL_API bool set_win_size(int w, int h) {
    sprintLog("set_win_size: %d,%d \r\n", w, h);
    SDL_SetWindowSize(sdlWindow, w, h);
    SDL_RenderSetLogicalSize(sdlRenderer, w, h);
    SDL_RenderPresent(sdlRenderer);
    return true;
}

DLL_API bool quit_win() {
    sprintLog("quit_win: \r\n");
    SDL_Quit();
    return true;
}