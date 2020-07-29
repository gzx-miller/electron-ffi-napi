#pragma once
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <sstream>
// #pragma comment(lib, "user32.lib")

using namespace std;

enum MsgType {
    msg_client = 1,
    msg_svr = 2,
    win_handle = 3,
};
struct MsgStruct {
    MsgType type;
    int val;
};

class EvtSvr {
private:
    HANDLE _hEvent;
public:
    EvtSvr() {}
    ~EvtSvr() {
        Uninit();
    }
    bool Create(string name);
    bool Wait(DWORD time = INFINITE);
    bool Signal();
    bool Uninit();
};

typedef bool(*PFRcvMsg)(MsgStruct & msg);
class MsgSvr {
private:
    HANDLE _hMapFile;
    LPVOID _pBuf;
    int _bufSize;
    EvtSvr _evtSvr;
    PFRcvMsg _onRcvMsg;
public:
    MsgSvr(int bufSize, PFRcvMsg onRcvMsg) : _bufSize(bufSize),
        _onRcvMsg(onRcvMsg) { }
    ~MsgSvr() { Unint(); }
    bool Listen(string name);
    bool Unint();
    bool WaitMsg(int time = INFINITE);
    bool PostMsg(MsgStruct &msg);
};