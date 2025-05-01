

#include "Memory.h"
#include <stdexcept>
#include <iostream>


Memory::Memory(const char* processName)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize= sizeof(PROCESSENTRY32) ;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Failed to take snapshot");

    while (Process32Next(snapshot, &pe32))
    {
        if (!_stricmp(pe32.szExeFile, processName))
        {
            pid = pe32.th32ProcessID;

            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
            break;
        }
    }
    CloseHandle(snapshot);  
}

uintptr_t Memory:: GetModuleBaseAddress(const char* moduleName)
{
   MODULEENTRY32 mod;
    mod.dwSize= sizeof(mod);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

    uintptr_t moduleBaseAddr = 0;
    while (Module32Next(snapshot, &mod))
    {
        if (!_stricmp(mod.szModule, moduleName))
        {
            moduleBaseAddr = (uintptr_t)mod.modBaseAddr;
            break;
        }
    }
    
    CloseHandle(snapshot);
    return moduleBaseAddr;
}
