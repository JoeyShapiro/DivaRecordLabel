#include "pch.h"
#include <Windows.h>
#include <SigScan.h>
#include <Psapi.h>
#include <stdio.h>
bool sigValid = true;

MODULEINFO moduleInfo;
PROCESS_MEMORY_COUNTERS pmc2;

const MODULEINFO& getModuleInfo()
{
    if (moduleInfo.SizeOfImage)
        return moduleInfo;

    ZeroMemory(&moduleInfo, sizeof(MODULEINFO));
    printf("id: %d\n", GetCurrentProcessId());
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO));
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc2, sizeof(PROCESS_MEMORY_COUNTERS));

    printf( "\tPageFaultCount: 0x%08X\n", pmc2.PageFaultCount );
    printf( "\tPeakWorkingSetSize: 0x%08X\n", 
                pmc2.PeakWorkingSetSize );
    // i think i want to watch this one
    printf( "\tWorkingSetSize: 0x%08X\n", pmc2.WorkingSetSize );
    printf("\tWorkingSetSize: %d\n", pmc2.WorkingSetSize);
    printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n", 
                pmc2.QuotaPeakPagedPoolUsage );
    printf( "\tQuotaPagedPoolUsage: 0x%08X\n", 
                pmc2.QuotaPagedPoolUsage );
    printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", 
                pmc2.QuotaPeakNonPagedPoolUsage );
    printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n", 
                pmc2.QuotaNonPagedPoolUsage );
    printf( "\tPagefileUsage: 0x%08X\n", pmc2.PagefileUsage ); 
    printf( "\tPeakPagefileUsage: 0x%08X\n", 
                pmc2.PeakPagefileUsage );

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