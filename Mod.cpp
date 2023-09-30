#include "pch.h"
#include <Helpers.h>
#include <stdio.h>
#include <cstdint>
#include <SigScan.h>
#include <detours.h>
#include <string>
#include <map>
#include "Diva.h"
#include <sqlite3.h>

// MegaMix+ addresses
const uint64_t DivaScoreBaseAddress = 0x00000001412EF568;
const uint64_t DivaScoreCompletionRateAddress = 0x00000001412EF634;
const uint64_t DivaScoreWorstCounterAddress = 0x00000001416E2D40; // For whatever reason the "worst" counter is stored separately from the rest of the hit counters
const uint64_t DivaScoreGradeAddress = 0x00000001416E2D00;
const uint64_t DivaCurrentPVTitleAddress = 0x00000001412EF228;
const uint64_t DivaCurrentPVIdAddress = 0x00000001412C2340;

// Non-SongLimitPatch 1.02
const uint64_t DivaCurrentPVDifficultyAddress = 0x00000001412B634C;

// SongLimitPatch 1.02 ONLY
//const uint64_t DivaCurrentPVDifficultyAddress = 0x00000001423157AC;

const std::string ConfigFileName = "config.toml";
bool consoleEnabled = false;

void* DivaScoreTrigger = sigScan(
    "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC\x48\x83\xEC\x60\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xF8\x48\x8B\xF9\x80\xB9\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00",
    "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxx?????xx????"
);

HOOK(int, __fastcall, _PrintResult, DivaScoreTrigger, int a1) {
    printf("hello, joey is awesome and great\n");

    if (consoleEnabled)
		printf("console: hello, joey is awesome and great\n");

    DIVA_SCORE DivaScore = *(DIVA_SCORE*)DivaScoreBaseAddress;
    int DivaScoreWorst = *(int*)DivaScoreWorstCounterAddress;
    DIVA_STAT DivaStat = *(DIVA_STAT*)DivaScoreCompletionRateAddress;
    std::string& DivaTitle = *(std::string*)DivaCurrentPVTitleAddress;
    DIVA_PV_ID DivaPVId = *(DIVA_PV_ID*)DivaCurrentPVIdAddress;
    DIVA_DIFFICULTY DivaDif = *(_DIVA_DIFFICULTY*)DivaCurrentPVDifficultyAddress;
    DIVA_GRADE DivaGrade = *(_DIVA_GRADE*)DivaScoreGradeAddress;

    // Client-side processing of whether or not to send the results to ShareDiva bot
    bool postScore = false;

    if (consoleEnabled)
        printf("console: %d %s", DivaPVId.Id, DivaTitle.c_str());
    printf("%d %s", DivaPVId.Id, DivaTitle.c_str());

    if (consoleEnabled)
    {
        printf("score: Total: %d; Combo: %d; Cool: %d; Fine: %d; Safe: %d; Sad: %d; Worst: %d\n", DivaScore.TotalScore, DivaScore.Combo, DivaScore.Cool, DivaScore.Fine, DivaScore.Safe, DivaScore.Sad, DivaScore.Worst);
        printf("worst: %d\n", DivaScoreWorst);
        printf("completion rate: %f\n", DivaStat.CompletionRate);
        printf("ID: %d; Title: %s\n", DivaPVId.Id, DivaTitle.c_str());
        printf("difficulty: %d\n", DivaDif);
        printf("grade: %d\n", DivaGrade);
    }

    // post score is if you passed
    // TODO i dont think these completion values are correct
    // could use int in table, and do joins, but that would be complicated
    std::string difficulty = "";
    switch (DivaDif)
    {
    case Normal:
        difficulty = "normal";
        if (DivaStat.CompletionRate >= 50.0F)
            postScore = true;
        break;
    case Hard:
        difficulty = "hard";
        if (DivaStat.CompletionRate >= 55.0F)
            postScore = true;
        break;
    case Extreme:
        difficulty = "extreme";
        if (DivaStat.CompletionRate >= 65.0F)
            postScore = true;
        break;
    case ExExtreme:
        difficulty = "exextreme";
        if (DivaStat.CompletionRate >= 65.0F)
            postScore = true;
        break;
    case Easy:
        difficulty = "easy";
        break;
    default:
        difficulty = "unknown";
        break;
    }

    std::string grade = "";
    switch (DivaGrade)
    {
    case Failed:
        grade = "failed";
        break;
    case Cheap:
        grade = "cheap";
        break;
    case Standard:
        grade = "standard";
        break;
    case Great:
        grade = "great";
        break;
    case Excellent:
        grade = "excellent";
        break;
    case Perfect:
        grade = "perfect";
        break;
    default:
        grade = "unknown";
        break;
    }

    if (!postScore)
        return original_PrintResult(a1);
    if (consoleEnabled)
        printf("post: console: %d %s", DivaPVId.Id, DivaTitle.c_str());
    printf("%d %s", DivaPVId.Id, DivaTitle.c_str());

    return original_PrintResult(a1);
};

extern "C"
{
    namespace {
        volatile bool done{ false };
    }

    void __declspec(dllexport) Init()
    {
        if (GetConsoleWindow()) {
            if (GetConsoleOutputCP() != CP_UTF8) {
                SetConsoleOutputCP(CP_UTF8);
            }

            #pragma warning(suppress : 4996)
            consoleEnabled = freopen("CONOUT$", "w", stdout) != NULL;
        }

        INSTALL_HOOK(_PrintResult);
    }
}