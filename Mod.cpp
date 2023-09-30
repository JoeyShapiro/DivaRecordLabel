#include "pch.h"
#include <Helpers.h>
#include <stdio.h>
#include <cstdint>
#include <SigScan.h>
#include <detours.h>
#include <string>
#include <map>
#include "Diva.h"

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

    switch (DivaDif)
    {
    case Normal:
        if (DivaStat.CompletionRate >= 50.0F)
            postScore = true;
        break;
    case Hard:
        if (DivaStat.CompletionRate >= 55.0F)
            postScore = true;
        break;
    case Extreme:
    case ExExtreme:
        if (DivaStat.CompletionRate >= 65.0F)
            postScore = true;
        break;
    case Easy:
    default:
        break;
    }

    if (!postScore)
        return original_PrintResult(a1);

    //// Create JSON with all results that will be sent to the bot
    //nlohmann::json results = {
    //    {"pvId", DivaPVId.Id},
    //    {"pvName", DivaTitle},
    //    {"pvDifficulty", DivaDif},
    //    {"totalScore", DivaScore.TotalScore},
    //    {"completionRate", DivaStat.CompletionRate},
    //    {"scoreGrade", DivaGrade},
    //    {"combo", DivaScore.Combo},
    //    {"cool", DivaScore.Cool},
    //    {"fine", DivaScore.Fine},
    //    {"safe", DivaScore.Safe},
    //    {"sad", DivaScore.Sad},
    //    {"worst", DivaScoreWorst}
    //};
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