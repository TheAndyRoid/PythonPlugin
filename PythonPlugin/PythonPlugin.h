#pragma once




#include <Python.h>


#include "OBSApi.h"
#include "Localization.h"
#include "CppImageSource.h"







#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

class PythonPlugin 
{
public:
	static PythonPlugin *instance;
	HANDLE ghMutex;
	PyThreadState* renderIntepThread;
	PyInterpreterState* interpreterState;
	void initPython();
	void finPython();
public:
	PythonPlugin();
	~PythonPlugin();
	bool ConfigGUIActive;
	 
	CppImageSource *pImageSource;
private:
	bool isDynamicLocale;

private:
	
};

EXTERN_DLL_EXPORT bool LoadPlugin();
EXTERN_DLL_EXPORT void UnloadPlugin();
EXTERN_DLL_EXPORT void OnStartStream();
EXTERN_DLL_EXPORT void OnStopStream();
EXTERN_DLL_EXPORT CTSTR GetPluginName();
EXTERN_DLL_EXPORT CTSTR GetPluginDescription();