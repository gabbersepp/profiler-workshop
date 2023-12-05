// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) int __cdecl Add(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}

int __stdcall CallCppWithin(int returnValue, char* str, int* strLength) {
    *strLength = strlen(str);
    return returnValue;
}

// mit __declspec können wir besser kontrollieren, was in ESP drin ist
__declspec(naked) int __cdecl UseAsmCallCppWithin(int returnValue, char* str, int* strLength) {
    __asm {
        push dword ptr[ESP + 12]
        push dword ptr[ESP + 12]
        push dword ptr[ESP + 12]
        CALL CallCppWithin

        // wenn wir CallCppWithin als __stdcall definieren, müssen wir den Stack wieder aufräumen, d.h. ESP um 3*4 Bytes erhöhen
        // andernfalls wird ret nicht den richtigen IP finden auf dem Stack
        ret
    }
}

extern "C" __declspec(dllexport) int __cdecl CallCppUseAsmCallCppWithin(int returnValue, char* str, int* strLength) {
    return UseAsmCallCppWithin(returnValue, str, strLength);
}



