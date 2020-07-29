#include "MsgClient.h"
using namespace std;

void WriteToDebug(const char* buffer, size_t size) {
    static HANDLE hFile = INVALID_HANDLE_VALUE;
    if (hFile == INVALID_HANDLE_VALUE) {
        hFile = CreateFileA("./debug.log", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
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

EvtClient::~EvtClient() {
    Uninit();
}

bool EvtClient::Connect(string name) {
    _hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, name.c_str());
    sprintLog("EvtClient::Connect: %x \r\n", _hEvent);
    return _hEvent != NULL;
}

bool EvtClient::Wait(DWORD time /*= INFINITE*/) {
    if (_hEvent) WaitForSingleObject(_hEvent, time);
    return true;
}

bool EvtClient::Signal() {
    sprintLog("EvtClient::Signal: %x \r\n", _hEvent);
    if (_hEvent) SetEvent(_hEvent);
    return true;
}

bool EvtClient::Uninit() {
    if (_hEvent) CloseHandle(_hEvent);
    return true;
}

bool MsgClient::Connect(string name) {
    string strGlobal = "Local\\";
    _evtClient.Connect((strGlobal + name + string("_event")).c_str());

    _hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
        PAGE_READWRITE, 0, _bufSize, (strGlobal + name).c_str());
    if (_hMapFile == NULL)  return false;
    _pBuf = (LPTSTR)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, _bufSize);
    return _pBuf != NULL;
}

bool MsgClient::Unint() {
    _evtClient.Signal();
    if (_pBuf) UnmapViewOfFile(_pBuf);
    if (_hMapFile) CloseHandle(_hMapFile);
    return true;
}

bool MsgClient::WaitMsg(int time /*= INFINITE*/) {
    _evtClient.Wait(time);
    if (_onRcvMsg && _pBuf) _onRcvMsg(*((MsgStruct*)_pBuf));
    return true;
}

bool MsgClient::PostMsg(MsgStruct &msg) {
    if (_pBuf) memcpy_s((void*)_pBuf, _bufSize, &msg, sizeof(msg));
    _evtClient.Signal();
    return true;
}
