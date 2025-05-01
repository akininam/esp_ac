#pragma once
#include <Windows.h>
#include <TlHelp32.h>





class Memory
{
public:
  
  Memory(const char* processName);
  uintptr_t GetModuleBaseAddress(const char* moduleName);

    template<typename T>
    T Read(uintptr_t address)const 
    {
        T buffer;
        ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
        return buffer;
    }
    template <typename T>
    void Write(uintptr_t address, T value) const
    {
        WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(T), NULL);
    }
    bool IsValid() const 
    {
        return hProcess != NULL;

    }
    private:
HANDLE hProcess = NULL;
  DWORD pid = 0;

};
