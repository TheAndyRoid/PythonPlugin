#pragma once




#include <Python.h>


#include "OBSApi.h"
#include "Localization.h"
#include "utils.h"
#include "CppImageSource.h"






#include <map>


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
	std::map <unsigned long, PyObject* > hotkeyToCallable;
	
	 
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


static void STDCALL Hotkey(DWORD key, UPARAM *userData, bool isDown){

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();


	//userData is a pointer to a python callable function
	PyObject *pyFunc = (PyObject *)userData;
	if (pyFunc == NULL || !PyCallable_Check(pyFunc)){
		Log(TEXT("Hotkey callback is not callable"));
		return;
	}
	PyObject *pyIsDown;
	if (isDown){
		pyIsDown = Py_True;
	}
	else{
		pyIsDown = Py_False;
	}

	PyObject *pyKey = PyLong_FromUnsignedLong(key);
	
	PyObject *argList = Py_BuildValue("(OO)", pyKey, pyIsDown);
	PyObject *result = PyObject_CallObject(pyFunc, argList);
	pyHasError();
	PyGILState_Release(gstate);
	return;
}