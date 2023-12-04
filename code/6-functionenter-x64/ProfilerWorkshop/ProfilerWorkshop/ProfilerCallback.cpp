#include "pch.h"
#include "ProfilerCallback.h"
#include<iostream>
#include <corprof.h>
bool GetClassNameByClassId(ClassID classId, char* output, ULONG outputLength);
bool GetClassNameByObjectId(ObjectID objectId, char* output, ULONG outputLength);
bool GetFunctionNameByFunctionId(FunctionID functionId, char* output, ULONG outputLength);

CComQIPtr<ICorProfilerInfo2> iCorProfilerInfo;

ProfilerCallback::ProfilerCallback() {
	std::cout << "constructor";
}

HRESULT ProfilerCallback::FinalConstruct()
{
	return S_OK;
}

void ProfilerCallback::FinalRelease()
{
}

// just add "extern C" to leave the function name as it is, otherwise it will be manipulated during compiling. Then we can not call it anymore from assembler
extern "C" void _stdcall EnterCallbackCpp(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func,
	COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo) {
	char* output = new char[1000];
	memset(output, 0, 1000);

	GetFunctionNameByFunctionId(funcId, output, 1000);

	std::cout << "Function enter: " << output << "\r\n";

	delete[] output;
}

#ifdef _WIN64
// those functions are defined in the assembler file

EXTERN_C void _fastcall FnEnterCallback(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func,
	COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo);

EXTERN_C void FnLeaveCallback(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func,
	COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo);

EXTERN_C void FnTailcallCallback(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func);
#else


void __declspec(naked) FnEnterCallback(
	FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func,
	COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo) {
	__asm {
		push dword ptr[ESP + 16]
		push dword ptr[ESP + 16]
		push dword ptr[ESP + 16]
		push dword ptr[ESP + 16]
		CALL EnterCallbackCpp

		ret 16
	}
}

void __declspec(naked) FnLeaveCallback(
	FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func,
	COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo) {
	__asm {
		ret 16
	}
}

void __declspec(naked) FnTailcallCallback(FunctionID funcId,
	UINT_PTR clientData,
	COR_PRF_FRAME_INFO func) {
	__asm {
		ret 8
	}
}

#endif

HRESULT __stdcall ProfilerCallback::Initialize(IUnknown* pICorProfilerInfoUnk)
{
	std::cout << "init";
	pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&iCorProfilerInfo);
	iCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_MONITOR_ENTERLEAVE);
	iCorProfilerInfo->SetEnterLeaveFunctionHooks2((FunctionEnter2*) &FnEnterCallback, (FunctionLeave2*) & FnLeaveCallback, (FunctionTailcall2*) & FnTailcallCallback);

	return S_OK;
}

bool GetFunctionNameByFunctionId(FunctionID functionId, char* output, ULONG outputLength) {
	IMetaDataImport* metadata;
	mdMethodDef methodToken;
	mdTypeDef typeDefToken;
	wchar_t* functionName = new wchar_t[1000];
	ULONG wcbCount;
	memset(functionName, 0, 1000);

	iCorProfilerInfo->GetTokenAndMetaDataFromFunction(functionId, IID_IMetaDataImport, (IUnknown**)&metadata, &methodToken);

	metadata->GetMethodProps(methodToken, &typeDefToken, functionName, 1000, &wcbCount, NULL, NULL, NULL, NULL, NULL);
	wcstombs(output, functionName, outputLength);
	metadata->Release();
	delete[] functionName;

	return true;
}

bool GetClassNameByClassId(ClassID classId, char* output, ULONG outputLength) {
	ModuleID moduleId;
	mdTypeDef typeDef;
	HRESULT hresult = iCorProfilerInfo->GetClassIDInfo(classId, &moduleId, &typeDef);
	IMetaDataImport* metadata;
	wchar_t* typeName = new wchar_t[outputLength];
	ULONG read = 0;

	if (hresult < 0 || moduleId == 0) {
		return false;
	}

	hresult = iCorProfilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport, (IUnknown**) & metadata);
	hresult = metadata->GetTypeDefProps(typeDef, typeName, outputLength, &read, NULL, NULL);
	metadata->Release();

	memset(output, 0, outputLength);
	wcstombs(output, typeName, outputLength);
	delete[] typeName;
	return true;
}

bool GetClassNameByObjectId(ObjectID objectId, char* output, ULONG outputLength) {
	ClassID classId;
	iCorProfilerInfo->GetClassFromObject(objectId, &classId);
	return GetClassNameByClassId(classId, output, outputLength);
}

HRESULT __stdcall ProfilerCallback::ExceptionThrown(ObjectID thrownObjectID)
{
	char* className = new char[100];
	GetClassNameByObjectId(thrownObjectID, className, 100);
	std::cout << "from profiler: \t\t\texception thrown:\r\n" << className;
	delete[] className;
	return S_OK;
}

#pragma region unused

HRESULT __stdcall ProfilerCallback::Shutdown()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AppDomainCreationStarted(AppDomainID appDomainID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AppDomainCreationFinished(AppDomainID appDomainID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AppDomainShutdownStarted(AppDomainID appDomainID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AppDomainShutdownFinished(AppDomainID appDomainID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AssemblyLoadStarted(AssemblyID assemblyID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AssemblyLoadFinished(AssemblyID assemblyID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AssemblyUnloadStarted(AssemblyID assemblyID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::AssemblyUnloadFinished(AssemblyID assemblyID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ModuleLoadStarted(ModuleID moduleID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ModuleLoadFinished(ModuleID moduleID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ModuleUnloadStarted(ModuleID moduleID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ModuleUnloadFinished(ModuleID moduleID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ModuleAttachedToAssembly(ModuleID moduleID, AssemblyID assemblyID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ClassLoadStarted(ClassID classID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ClassLoadFinished(ClassID classID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ClassUnloadStarted(ClassID classID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ClassUnloadFinished(ClassID classID, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::FunctionUnloadStarted(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::JITCompilationFinished(FunctionID functionID, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::JITCachedFunctionSearchStarted(FunctionID functionID, BOOL* pbUseCachedFunction)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::JITCachedFunctionSearchFinished(FunctionID functionID, COR_PRF_JIT_CACHE result)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::JITFunctionPitched(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::JITInlining(FunctionID callerID, FunctionID calleeID, BOOL* pfShouldInline)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::UnmanagedToManagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ManagedToUnmanagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ThreadCreated(ThreadID threadID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ThreadDestroyed(ThreadID threadID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ThreadAssignedToOSThread(ThreadID managedThreadID, DWORD osThreadID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingClientInvocationStarted()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingClientSendingMessage(GUID* pCookie, BOOL fIsAsync)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingClientReceivingReply(GUID* pCookie, BOOL fIsAsync)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingClientInvocationFinished()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingServerReceivingMessage(GUID* pCookie, BOOL fIsAsync)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingServerInvocationStarted()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingServerInvocationReturned()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RemotingServerSendingReply(GUID* pCookie, BOOL fIsAsync)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RuntimeSuspendFinished()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RuntimeSuspendAborted()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RuntimeResumeStarted()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RuntimeResumeFinished()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RuntimeThreadSuspended(ThreadID threadID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RuntimeThreadResumed(ThreadID threadID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::MovedReferences(ULONG cmovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ObjectAllocated(ObjectID objectID, ClassID classID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ObjectsAllocatedByClass(ULONG classCount, ClassID classIDs[], ULONG objects[])
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ObjectReferences(ObjectID objectID, ClassID classID, ULONG objectRefs, ObjectID objectRefIDs[])
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RootReferences(ULONG rootRefs, ObjectID rootRefIDs[])
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionUnwindFunctionEnter(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionUnwindFunctionLeave()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionSearchFunctionEnter(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionSearchFunctionLeave()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionSearchFilterEnter(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionSearchFilterLeave()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionSearchCatcherFound(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionCLRCatcherFound()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionCLRCatcherExecute()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionOSHandlerEnter(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionOSHandlerLeave(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionUnwindFinallyEnter(FunctionID functionID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionUnwindFinallyLeave()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionCatcherEnter(FunctionID functionID,
	ObjectID objectID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ExceptionCatcherLeave()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::COMClassicVTableCreated(ClassID wrappedClassID, REFGUID implementedIID, void* pVTable, ULONG cSlots)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::COMClassicVTableDestroyed(ClassID wrappedClassID, REFGUID implementedIID, void* pVTable)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::ThreadNameChanged(ThreadID threadID, ULONG cchName, WCHAR name[])
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::GarbageCollectionFinished()
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::RootReferences2(ULONG cRootRefs, ObjectID rootRefIDs[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIDs[])
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::HandleCreated(GCHandleID handleID, ObjectID initialObjectID)
{
	return S_OK;
}

HRESULT __stdcall ProfilerCallback::HandleDestroyed(GCHandleID handleID)
{
	return S_OK;
}

#pragma endregion unused

