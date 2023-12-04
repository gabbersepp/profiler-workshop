// StdCallConvention.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>

int __stdcall TestCpp(int number, int* result) {
    *result = number;
    return *result;
}

__declspec(naked) int __stdcall CallingCpp(int number, int* result) {
    __asm {
        push dword ptr [ESP+8] // IR after CALL (4 Bytes) + Last Argument [number] (4 bytes) 
        push dword ptr [ESP+8] // tricky: the PUSh before reduces ESP by four. So we are again 8 Bytes away from the first argument (number)
        CALL TestCpp
        ret 8 // stdcall has to clean it's stack on it's own
    }
}

// ESP points always to the last pushed value
// PUSH: decrease ESP and copy value to [ESP]
// write `input` to `output` and return it, too
// stdcall: push parameters from right to left
__declspec(naked) int __stdcall Test(int input, int* output) {
    __asm {
        // no need to push EAX as it is designed to be used within the function
        push EBX; // EBX is not callee safe!
        mov ECX, [ESP + 8]
        mov EBX, [ESP + 12]
        mov [EBX], ECX
        MOV EAX, ECX
        POP EBX
        ret 8
    }
}

// quite the same as stdcall but the caller cleans up the stack
__declspec(naked) int __cdecl Test_cdecl(int input, int* output) {
    __asm {
        // no need to push EAX as it is designed to be used within the function
        push EBX; // EBX is not callee safe!
        mov ECX, [ESP + 8]
        mov EBX, [ESP + 12]
        mov[EBX], ECX
        MOV EAX, ECX
        POP EBX
        ret
    }
}

int main()
{
    int output = 0;
    int retVal = CallingCpp(100, &output);
    std::cout << "retVal: " << retVal << ", output: " << output << "; hopefully both are the same";
}

// Programm ausführen: STRG+F5 oder Menüeintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
