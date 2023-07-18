---
marp: true
---

# .NET (Core) Profiler Workshop

Josef Biehler

---

# Über mich
- biehler-josef.de
- kack.dev
- github.com/gabbersepp
- dev.to/gabbersepp
- gabbersepp@googlemail.com

---

# Agenda
- Aufbau des Startprojekts (damit man weis wie es geht)
- Basiswissen
- Einen Profiler debuggen
- Beispiel: Wann wird ein Objekt erzeugt
- FunctionEnter/FunctionLeave 32Bit
- FunctionEnter/FunctionLeave 64Bit
- Funktionsparameter auslesen 32 Bit
- Funktionsparameter auslesen 64 Bit
- Profiler in C#

---
# Startprojekt
- C++ in VS integrieren
![](./img/1/vs-setup.jpg)

---
# Startprojekt
- Neues Projekt `ATL` mit Standardeinstellungen
- Name: ProfilerWorkshop
![](./img/1/default.JPG)

---
# Startprojekt
- `ProfilerWorkshopPS` Projekt: Keine Ahnung für was das benutzt wird. Einfach löschen
- Kein automatisches Registrieren der DLL
- Zusätzliche Abhängigkeiten
- muss für x86 und x64 gemacht werden!
![](./img/1/linker.jpg)

---
# Startprojekt
Änderungen an Dateien:
- **framework.h**: Füge `using namespace ATL;` am Ende der Datei hinzu
- **Resource.h**: Füge `#define IDR_PROFILER 102` ans Ende hinzu, `NewLine` nicht vergessen!
- **ProfilerWorkshop.cpp**: Lösche die Funktion `STDAPI DllInstall(BOOL bInstall, _In_opt_ LPCWSTR pszCmdLine)`.
- **ProfilerWorkshop.def**: Ersetze `LIBRARY` durch `LIBRARY "ProfilerWorkshop.dll"` und entferne die Zeile `DllInstall PRIVATE`
---

# Startprojekt
Hinzufügen neuer Dateien:
- `ProfilerWorkshop.idl` (Ersetzt die vorherige komplett) Link:
- `ProfilerCallback.h` Link: 
- jetzt einmal kompilieren, es sollten keine Fehler mehr auftauchen
- `ProfilerCallback.cpp` anlegen. Link:
- Nochmal kompilieren :-)

---
# Startprojekt
- Anlegen eines Testprogramms
- Konsolenapplikation (dotnet core), Version sollte egal sein
- Name "TestApp"
```
    namespace TestApp
    {
        public class Program
        {
            public static void Main()
            {
                Console.WriteLine("Hello, World!");
                Console.ReadKey();
            }
        }
    }
```

---

# Startprojekt
- **Any Cpu**:
    `anycpu (default) compiles your assembly to run on any platform. Your application runs as a 64-bit process whenever possible and falls back to 32-bit when only that mode is available.`
- Ungünstig! Deswegen -> Definition von festen Targets in TestApp
![](./img/1/bit.jpg)

---

# Startprojekt
Profiler muss der TestApp bzw dotnet bekannt gemacht werden:
```
SET CORECLR_ENABLE_PROFILING=1
SET CORECLR_PROFILER={b45048d5-6f44-4fbe-ae88-b468a5e4927a}
SET CORECLR_PROFILER_PATH=ProfilerWorkshop/Debug/ProfilerWorkshop.dll
SET COMPLUS_ProfAPI_ProfilerCompatibilitySetting=EnableV2Profiler

START TestApp/TestApp/bin/x86/Debug/net6.0/TestApp.exe
```
Wichtig: Pfade anpassen, je nach `bitness`.
Und: Vorgehen für .Net Framework identisch, außer ENV Variablen

--- 

# Startprojekt
![](./img/1/testapp.jpg)

---

# Basiswissen

- Profilercallbacks (z.b. FunctionEnter) werden durch Thread aus Applikation aufgerufen
- Mehrere Threads in App -> parallele Aufrufe in Profiler -> Zugriffe müssen sicher sein!
- **ICorProfilerCallback**: Callbacks werden durch App CLR aufgerufen
- Lifecycle **Initialize()**: Konfigurieren des Profilers

---
# Basiswissen - Initialize()
- Instanz vom Typ `ICorProfilerInfo` anfordern
- Per Flag die Events konfigurieren
- Hooks installieren

---
# Basiswissen - Änderung an ProfilerWorkshop.cpp
- `include <corprof.h>`
- `CComQIPtr<ICorProfilerInfo2> iCorProfilerInfo;` 
- `pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&iCorProfilerInfo);`
