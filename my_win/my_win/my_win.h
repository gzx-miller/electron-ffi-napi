#ifdef DLL_EXPORTS
#define DLL_API extern "C" __declspec(dllexport)
#else
#define DLL_API extern "C" __declspec(dllimport)
#endif

DLL_API bool create_win(int x, int y, int w, int h, int hwnd);
DLL_API bool set_win_pos(int x, int y);
DLL_API bool set_win_size(int w, int h);
DLL_API bool show_win(int show);
DLL_API bool quit_win();
