﻿
#include <iostream>
#include <wtypes.h>

using namespace std;

typedef bool(*PF_XYWH)(int x, int y, int w, int h, int hwnd);
typedef bool(*PF_WH)(int w, int h);
typedef bool(*PF_quit)();

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow) {
//int main(int argc, char *argv[]) {
    HINSTANCE handle = (HINSTANCE)LoadLibraryA("my_win.dll");
    if (handle == NULL) return -1;

    PF_XYWH pf_create_win = (PF_XYWH)GetProcAddress(handle, "create_win");
    PF_WH pf_set_win_pos = (PF_WH)GetProcAddress(handle, "set_win_pos");
    PF_WH pf_set_win_size = (PF_WH)GetProcAddress(handle, "set_win_size");
    PF_quit pf_quit_win = (PF_quit)GetProcAddress(handle, "quit_win");

    if (pf_create_win == nullptr || pf_set_win_pos == nullptr ||
        pf_set_win_size == nullptr || pf_quit_win == nullptr) {
        return -1;
    }

    char ch = 'r';

    pf_create_win(0, 0, 800, 600, 0x400A00);
    //for(int i =0; i<1000; ++i) {
    //    //Sleep(1);
    //    pf_set_win_pos(1000-i, 1000-i);
    //    //Sleep(1);
    //    pf_set_win_size(100+i, 100+i);
    //}

    while (ch != 'q') {
        ch = getchar();
    }

    pf_quit_win();
    return -1;
}

