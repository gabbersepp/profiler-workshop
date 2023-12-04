// dllmain.h : Deklaration der Modulklasse.

class CProfilerWorkshopModule : public ATL::CAtlDllModuleT< CProfilerWorkshopModule >
{
public :
	DECLARE_LIBID(LIBID_ProfilerWorkshopLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PROFILERWORKSHOP, "{f836278b-fb5c-410e-aa59-2d14563d7aea}")
};

extern class CProfilerWorkshopModule _AtlModule;
