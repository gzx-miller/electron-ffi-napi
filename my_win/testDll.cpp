
#include <iostream>
#include <wtypes.h>

using namespace std;

typedef bool(*PF_XYWH)(int x, int y, int w, int h);
typedef bool(*PF_WH)(int w, int h);
typedef bool(*PF_quit)();

int main(int argc, char *argv[]) {
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

    pf_create_win(0, 0, 800, 600);
    Sleep(1000);
    pf_set_win_pos(100, 100);
    Sleep(1000);
    pf_set_win_size(1920, 1080);

    while (ch != 'q') {
        ch = getchar();
    }

    pf_quit_win();
    return -1;
}

