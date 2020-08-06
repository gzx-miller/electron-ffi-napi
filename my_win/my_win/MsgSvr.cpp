#include "MsgSvr.h"

bool EvtSvr::Create(string name) {
    _hEvent = CreateEvent(NULL, FALSE, FALSE, name.c_str());
    return _hEvent != NULL;
}

bool EvtSvr::Wait(DWORD time) {
    return _hEvent && WAIT_OBJECT_0 == WaitForSingleObject(_hEvent, time);
}

bool EvtSvr::Signal() {
    if (_hEvent) SetEvent(_hEvent);
    return true;
}

bool EvtSvr::Uninit() {
    if (_hEvent) CloseHandle(_hEvent);
    return true;
}

bool MsgSvr::Listen(string name, int time) {
    string strGlobal = "Local\\";
    _evtSvr.Create((strGlobal + name + string("_event")).c_str());
    if (!_evtSvr.Wait(time)) return false;

    _hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,
        (strGlobal + name).c_str());
    if (_hMapFile == NULL) return false;
    _pBuf = (LPTSTR)MapViewOfFile(_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, _bufSize);
    if (!_pBuf) return false;
    if (_onRcvMsg) _onRcvMsg(*((MsgStruct*)_pBuf));
    return true;
}

bool MsgSvr::Unint() {
    _evtSvr.Signal();
    if (_pBuf)  UnmapViewOfFile(_pBuf);
    if (_hMapFile) CloseHandle(_hMapFile);
    return true;
}

bool MsgSvr::WaitMsg(int time) {
    _evtSvr.Wait(time);
    if (_onRcvMsg) _onRcvMsg(*((MsgStruct*)_pBuf));
    return true;
}

bool MsgSvr::PostMsg(MsgStruct &msg) {
    if (_pBuf) memcpy_s((void*)_pBuf, _bufSize, &msg, sizeof(msg));
    _evtSvr.Signal();
    return true;
}
