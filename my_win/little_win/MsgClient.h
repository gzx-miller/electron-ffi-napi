#pragma once
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <sstream>

using namespace std;

static void WriteToDebug(const char* buffer, size_t size);
void sprintLog(const char *format, ...);

class EvtClient {
private:
    HANDLE _hEvent;
public:
    EvtClient() {}
    ~EvtClient();
    bool Connect(string name);
    bool Wait(DWORD time = INFINITE);
    bool Signal();
    bool Uninit();
};
enum MsgType {
    set_win_hwnd = 1,
    set_new_win_pos = 2,
    msg_exit = 3
};
struct MsgStruct {
    MsgType type;
    int x;
    int y;
    int w;
    int h;
};

typedef bool(*PFRcvMsg)(MsgStruct & msg);
class MsgClient {
private:
    HANDLE _hMapFile;
    LPVOID _pBuf;
    int _bufSize;
    EvtClient _evtClient;
    PFRcvMsg _onRcvMsg;
public:
    MsgClient(int bufSize, PFRcvMsg onRcvMsg) : _bufSize(bufSize),
        _onRcvMsg(onRcvMsg) { }
    ~MsgClient() { Unint(); }
    bool Connect(string name);
    bool Unint();
    bool WaitMsg(int time = INFINITE);
    bool PostMsg(MsgStruct &msg);
};

