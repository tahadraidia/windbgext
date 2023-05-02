#include "windbgext.h" 

WinDBGExt::WinDBGExt(IDebugClient* pDebugClient) {
    _pDebugClient = pDebugClient;
    _pDebugClient->QueryInterface(
	    __uuidof(IDebugSystemObjects),
	    reinterpret_cast<void **>(&_pDebugSystemObjects));
    _pDebugClient->QueryInterface(
	    __uuidof(IDebugControl),
	    reinterpret_cast<void **>(&_pDebugControl));
}

HANDLE WinDBGExt::GetDebuggeeHandle() {
    ULONG64 handle = 1337;
    _pDebugSystemObjects->GetCurrentProcessHandle(&handle);
    return (HANDLE)handle;
}

DWORD WinDBGExt::GetDebuggeePID() {
    ULONG pid = 1337;
    _pDebugSystemObjects->GetCurrentProcessSystemId(&pid);
    return (DWORD)pid;
}


ULONG64 WinDBGExt::EvaluateArgAsINT64(PCSTR args) {
    _pDebugControl->Evaluate(args,
			     DEBUG_VALUE_INT64,
			     &_debug_value,
    nullptr);
    return _debug_value.I64;
}

void WinDBGExt::PrintOut(std::string const format, ...) {
    va_list args;
    char buffer[MAX_PATH] = { 0 };
    va_start(args, format);
    vsprintf_s(buffer, sizeof(buffer), format.c_str(), args);
    _pDebugControl->Output(DEBUG_OUTPUT_NORMAL,
			   "%s\n",
    buffer);
    va_end(args);
}

std::vector<SYSTEM_HANDLE> WinDBGExt::GetDebuggeeHandles() {
    std::vector<SYSTEM_HANDLE> handles;
    NTSTATUS status = 0;
    HMODULE hNtDll = GetModuleHandle("ntdll.dll");
    _NtQuerySystemInformation pNtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(hNtDll, "NtQuerySystemInformation");
    _NtQueryObject pNtQueryObject = (_NtQueryObject)GetProcAddress(hNtDll, "NtQueryObject");

    // Get the system handle information
    ULONG ulSize;
    status = pNtQuerySystemInformation(SystemHandleInformation, nullptr, 0, &ulSize);

    if (status != STATUS_INFO_LENGTH_MISMATCH) {
	PrintOut("GetFilenameFromHandle::NtQuerySystemInformation() first round failed: 0x%x\n",status);
	return handles;
    }

    PSYSTEM_HANDLE_INFORMATION pBuffer = (PSYSTEM_HANDLE_INFORMATION)malloc(ulSize);

    if(!pBuffer) {
	PrintOut("GetFilenameFromHandle::Malloc() pBuffer failed: 0x%x\n",
		       ::GetLastError());
	return handles;
    }


    do {
	pBuffer = (PSYSTEM_HANDLE_INFORMATION)malloc(ulSize);
	if(!pBuffer){
	    PrintOut("GetFilenameFromHandle::Malloc() pBuffer failed: 0x%x\n",
			   ::GetLastError());
	    return handles;
	}
	status = pNtQuerySystemInformation(SystemHandleInformation, pBuffer, ulSize, &ulSize);
    }while(status == STATUS_INFO_LENGTH_MISMATCH);

    if(!NT_SUCCESS(status)){
	PrintOut("GetFilenameFromHandle::NtQuerySystemInformation() second round failed: 0x%x\n",
		       status);
	return handles;
    }


    // Find the handle in the system handle information
    PSYSTEM_HANDLE pHandleInfo = (PSYSTEM_HANDLE)pBuffer;

    for (ULONG i = 0; i <= pBuffer->HandleCount; i++) {
	const DWORD pid = GetDebuggeePID();
	if(pBuffer->Handles[i].ProcessId == pid) {
	    handles.push_back(pBuffer->Handles[i]);
	}
    }

    free(pBuffer);
    pBuffer = nullptr;

    return handles;
}
