#include "pch.h"
#include <Windows.h>
#include <SigScan.h>
#include <Psapi.h>
#include <stdio.h>
bool sigValid = true;

MODULEINFO moduleInfo;
PROCESS_MEMORY_COUNTERS_EX processMemory;

const MODULEINFO& getModuleInfo()
{
    if (moduleInfo.SizeOfImage)
        return moduleInfo;

    ZeroMemory(&moduleInfo, sizeof(MODULEINFO));
    printf("id: %d\n", GetCurrentProcessId())
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO));
    GetProcessMemoryInfo(GetCurrentProcess(), &processMemory, sizeof(PROCESS_MEMORY_COUNTERS_EX));

    printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
    printf( "\tPeakWorkingSetSize: 0x%08X\n", 
                pmc.PeakWorkingSetSize );
    // i think i want to watch this one
    printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
    printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n", 
                pmc.QuotaPeakPagedPoolUsage );
    printf( "\tQuotaPagedPoolUsage: 0x%08X\n", 
                pmc.QuotaPagedPoolUsage );
    printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", 
                pmc.QuotaPeakNonPagedPoolUsage );
    printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n", 
                pmc.QuotaNonPagedPoolUsage );
    printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage ); 
    printf( "\tPeakPagefileUsage: 0x%08X\n", 
                pmc.PeakPagefileUsage );

    return moduleInfo;
}

void* sigScan(const char* signature, const char* mask)
{
    printf("hello\n");
    const MODULEINFO& info = getModuleInfo();
    const size_t length = strlen(mask);
    printf("%d\n", info.SizeOfImage);

    for (size_t i = 0; i < info.SizeOfImage; i++)
    {
        char* memory = (char*)info.lpBaseOfDll + i;

        size_t j;
        for (j = 0; j < length; j++)
        {
            if (mask[j] != '?' && signature[j] != memory[j])
                break;
        }

        if (j == length)
            return memory;
    }

    printf("failed\n");
    return nullptr;
}



#define SIG_SCAN(x, ...) \
    void* x(); \
    void* x##Addr = x(); \
    void* x() \
    { \
        static const char* x##Data[] = { __VA_ARGS__ }; \
        if (!x##Addr) \
        { \
            for (int i = 0; i < _countof(x##Data); i += 2) \
            { \
                x##Addr = sigScan(x##Data[i], x##Data[i + 1]); \
                if (x##Addr) \
                    return x##Addr; \
            } \
            sigValid = false; \
        } \
        return x##Addr; \
    }