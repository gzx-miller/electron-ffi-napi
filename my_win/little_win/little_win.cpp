#define  DLL_EXPORTS
#include <stdio.h>
#include "little_win.h"

extern "C" {
#include "SDL.h"
}

SDL_Window * sdlWindow = nullptr;
SDL_Renderer* sdlRenderer = nullptr;
SDL_Texture* sdlTexture = nullptr;
SDL_Rect sdlRect = { 0 };

DLL_API bool create_win(int x, int y, int w, int h) {
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
    SDL_SetWindowPosition(sdlWindow, x, y);
    return true;
}

DLL_API bool set_win_size(int w, int h) {
    SDL_SetWindowSize(sdlWindow, w, h);
    SDL_RenderPresent(sdlRenderer);
    return true;
}

DLL_API bool quit_win() {
    SDL_Quit();
    return true;
}