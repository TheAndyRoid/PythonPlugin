/********************************************************************************
Copyright (C) 2014 Andrew Skinner <obs@theandyroid.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/

#pragma once




#include <Python.h>


#include "OBSApi.h"
#include "Localization.h"
#include "utils.h"
#include "CppImageSource.h"


#include <map>

	

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

	struct StringComp
	{
		bool operator()(const String& lhs, const String& rhs)
		const {
			return scmp(lhs,rhs)>0;
		}
	};

	typedef std::map<String, PyObject*,StringComp> source_map;
	typedef std::map<String, source_map,StringComp> scene_map;
	typedef std::map<String, PyObject*, StringComp> shutdown_map;


class PythonPlugin 
{
public:
	static PythonPlugin *instance;
	HANDLE ghMutex;
	PyThreadState* renderIntepThread;
	PyInterpreterState* interpreterState;
	void initPython();
	void finPython();
	PyObject* getPersistItem(String sceneName, String sourceName);
	void setPersistItem(String sceneName, String sourceName,PyObject *imgSrc);
	void clearPersistItems();


	void addShutdownFunction(String filename, PyObject *function);
	void clearShutdownFunctions();

public:
	PythonPlugin();
	~PythonPlugin();
	bool ConfigGUIActive;
	std::map <unsigned long, PyObject* > hotkeyToCallable;

	scene_map persistantPyObjects;
	shutdown_map shutdownFunc;
	
	 
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