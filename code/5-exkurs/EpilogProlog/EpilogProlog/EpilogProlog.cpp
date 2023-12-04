// EpilogProlog.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>


__declspec(naked) int __cdecl Test_cdecl_naked() {
    __asm {
        ret
    }
}

int __cdecl Test_cdecl_non_naked() {
    __asm {
        ret
    }
}

void stack() {
    // esp vorher notieren
    __asm {
        // esp reduzieren, Wert speichern in [esp]
        nop
        push dword ptr 1
        push dword ptr 2
        push dword ptr 3
        // esp resetten
        add esp, 12
        // nop nur für debugging zwecke
        nop
    }
    // esp nachher -> sollte gleich sei
}

int main()
{
    std::cout << "Hello World!\n";

    //Test_cdecl_naked();
    //Test_cdecl_non_naked();

    stack();
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
