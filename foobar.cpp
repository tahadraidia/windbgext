#include <windows.h>
#include <Dbgeng.h>
#include <stdio.h>
#include <string>
#include <psapi.h>

#include "windbgext.h"

#define EXT_MAJOR_VER  1
#define EXT_MINOR_VER  0

extern "C" __declspec(dllexport) HRESULT CALLBACK
foobar(IDebugClient* pDebugClient, PCSTR args) {

    WinDBGExt windbgExt(pDebugClient);
    const HANDLE handle = windbgExt.GetDebuggeeHandle();
    const DWORD pid = windbgExt.GetDebuggeePID();

    windbgExt.PrintOut(
            "Debuggee handle: 0x%x\nDebuggee PID: %ld\n",
            handle, pid);

    return S_OK;
}

extern "C" __declspec(dllexport) HRESULT CALLBACK
DebugExtensionInitialize(PULONG Version, PULONG Flags) {
    *Version = DEBUG_EXTENSION_VERSION(EXT_MAJOR_VER, EXT_MINOR_VER);
    *Flags = 0;
    return S_OK;
}
