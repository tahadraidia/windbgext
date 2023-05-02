#ifndef _WINDBGEXT_H
#define _WINDBGEXT_H

#include <Windows.h>
#include <Dbgeng.h>
#include <string>
#include <vector>

#include "ntstuff.h"


class WinDBGExt {
private:
	IDebugClient* _pDebugClient = nullptr;
	IDebugControl* _pDebugControl = nullptr;
	IDebugSystemObjects* _pDebugSystemObjects = nullptr;
	DEBUG_VALUE _debug_value = { 0 };

public:
	WinDBGExt(IDebugClient* pDebugClient);
	VOID PrintOut(std::string const format, ...);
	HANDLE GetDebuggeeHandle();
	DWORD GetDebuggeePID();
	ULONG64 EvaluateArgAsINT64(PCSTR args);
	std::vector<SYSTEM_HANDLE> GetDebuggeeHandles();
};

#endif
