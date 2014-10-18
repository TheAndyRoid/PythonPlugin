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


#include "PythonPlugin.h"

#include <windowsx.h>
#include <cstdio>
#include <windows.h>
#include <cstdlib>
#include "PyOBS.h"
#include "PyXElement.h"





#define VIDEO_SOURCE_CLASS TEXT("PythonSource")

PythonPlugin *PythonPlugin::instance = NULL;



bool STDCALL ConfigureVideoSource(XElement *element, bool bCreating)
{

	String file;
	String className;
	String moduleName;
	Log(TEXT("Python Source Configure"));
	XElement *dataElement = element->GetElement(TEXT("data"));


	bool isMissingDataElement = true;
	if (isMissingDataElement = !dataElement) {
		dataElement = element->CreateElement(TEXT("data"));
	}
	else{
		isMissingDataElement = false;
	}

	

	/*Called when the element is created at stream start.*/

	//This is called when the source goes live


	//Get the plugin instance
	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
		return NULL;
	}
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	PyObject *pName, *pModule, *pFunc;

	if (isMissingDataElement){
		moduleName = TEXT("selectionGUI");
		className = TEXT("gui");

	}
	else{
		file = dataElement->GetString(TEXT("PythonGUIFile"));
		className = dataElement->GetString(TEXT("PythonGUIClass"));
		moduleName = addToPythonPath(file);

	}


	/*
	DWORD dwWaitResult = WaitForSingleObject(
		pyPlug->ghMutex,    // handle to mutex
		INFINITE);  // no time-out interval
	*/



	

	//Python
	


	//get correct files from config here

	
	//pName = PyString_FromString("baseGUI");
	pyHasError();

	pName = CTSTRtoPyUnicode(moduleName);
	PyObject *dict = PyImport_GetModuleDict();
	bool reload = false;
	if (PyDict_Contains(dict, pName) && reload){ //make a debug option to reload source	
		pModule = PyDict_GetItem(dict, pName);
		pModule = PyImport_ReloadModule(pModule);
		pyHasError();
	}
	else { 	
		pModule = PyImport_Import(pName);
		pyHasError();
	}


	
	if (pyHasError()){
		Py_XDECREF(pName);
		return false;
	}


	if (pModule != NULL) {
		pFunc = PyObject_GetAttr(pModule, CTSTRtoPyUnicode(className));
		//pFunc = PyObject_GetAttrString(pModule, (char*) "guimain");

		if (pFunc && PyCallable_Check(pFunc)) {
			PyObject *argList = Py_BuildValue("");
			PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
			Py_XDECREF(argList);
			((PyXElement*)pyConfig)->element = dataElement;

			argList = Py_BuildValue("(O)", pyConfig);
			PyObject_CallObject(pFunc, argList);			

			if (pyHasError()){
				Py_XDECREF(pModule);
				Py_XDECREF(pName);
				Py_XDECREF(pyConfig);
				PyGILState_Release(gstate);
				return false;
			}
			Py_XDECREF(pyConfig);


		}
		else {


		}
		
		Py_DECREF(pFunc);
	}
	else {
		PyErr_Print();


	}

	Py_XDECREF(pModule);
	Py_XDECREF(pName);
	

	PyGILState_Release(gstate);

	if (isMissingDataElement){
		//The setup gui has run now run the specified gui
		// check if the required element now exist and restart gui
		return ConfigureVideoSource(element, false);
	}
	else{
		return true;
	}
}

ImageSource* STDCALL CreatePythonSource(XElement *data)
{

	Log(TEXT("Python Create ImageSource"));

	XElement *thisSource = data->GetParent();
	XElement *sources = thisSource->GetParent();
	XElement *thisScene = sources->GetParent();
	
	String sourceName = thisSource->GetName();
	String sceneName = thisScene->GetName();

	/*Called when the element is created at stream start.*/

	//This is called when the source goes live


	//Get the plugin instance
	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));	
		return NULL;
	}	

	//Create Image source
	pyPlug->pImageSource = new CppImageSource(data);
	

	String file;
	String className;
	String moduleName;
	
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();	


	


	file = data->GetString(TEXT("PythonMainFile"));
	className = data->GetString(TEXT("PythonMainClass"));

	if (file == NULL || className == NULL){
		PyGILState_Release(gstate);
		return NULL;		
	}else{		
		moduleName = addToPythonPath(file);
	}


		
	
	

	
	//Python
	PyObject *pName, *pModule, *pFunc;
	pyImageSource *pyImgSrc = NULL;

	pName = CTSTRtoPyUnicode(moduleName);



	PyObject *dict = PyImport_GetModuleDict();
	if (!PyDict_Contains(dict, pName)){
		pModule = PyImport_Import(pName);
		pyHasError();
	}
	else{
		pModule = PyDict_GetItem(dict, pName);
		pModule = PyImport_ReloadModule(pModule);
		pyHasError();
	}

	bool pyObjectCreated = false;

	if (pModule != NULL) {
		pFunc = PyObject_GetAttr(pModule, CTSTRtoPyUnicode(className));
		pyHasError();
		// pFunc is a new reference 
		if (pFunc && PyCallable_Check(pFunc)) {
			//pass in config
			PyObject *argList = Py_BuildValue("");
			PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
			Py_XDECREF(argList);
			((PyXElement*)pyConfig)->element = data;
			argList = Py_BuildValue("(O)",pyConfig);
			pyImgSrc = (pyImageSource*)PyObject_CallObject(pFunc, argList);
			
			if (pyHasError()){
				Log(TEXT("Unable to create class %ws"),className);
			}
			else if (!isPyObjectBaseClass((PyObject*)pyImgSrc, &String(TEXT("ImageSource")))){
				Log(TEXT("PYTHON ERROR Class: %ws , is not derived from OBS.ImageSource"),className);
			}else{
				pyImgSrc->cppImageSource = pyPlug->pImageSource;
				pyPlug->pImageSource->pyImgSrc = (PyObject*)pyImgSrc;
				pyObjectCreated = true;
			}
			Py_XDECREF(argList);

		}
		else {			
		}
		
		Py_XDECREF(pFunc);
	}
	else {
		PyErr_Print();
	}

	
	Py_XDECREF(pModule);
	Py_XDECREF(pName);
	
	


	PyGILState_Release(gstate);

	//ReleaseMutex(pyPlug->ghMutex);
	if (pyObjectCreated){
		
		return pyPlug->pImageSource;
	}
	else{
		return NULL;
	}
	

}



PythonPlugin::PythonPlugin()
{
	isDynamicLocale = false;
	Log(TEXT("Python plugin loaded"));
	if (!locale->HasLookup(KEY("PluginName"))) {
		isDynamicLocale = true;
		int localizationStringCount = sizeof(localizationStrings) / sizeof(CTSTR);
		Log(TEXT("Video Source Plugin strings not found, dynamically loading %d strings"), sizeof(localizationStrings) / sizeof(CTSTR));
		for (int i = 0; i < localizationStringCount; i += 2) {
			locale->AddLookupString(localizationStrings[i], localizationStrings[i + 1]);
		}
		if (!locale->HasLookup(KEY("PluginName"))) {
			AppWarning(TEXT("Uh oh..., unable to dynamically add our localization keys"));
		}
	}
	

	//Register callbacks with obs
	OBSRegisterImageSourceClass(VIDEO_SOURCE_CLASS, STR("ClassName"), (OBSCREATEPROC)CreatePythonSource, (OBSCONFIGPROC)ConfigureVideoSource);


	Py_Initialize();
	PyEval_InitThreads();
	

	
	/*Must set arguments for gui to work*/
	char *argv[] = { "OBS", NULL };
	int argc = sizeof(argv) / sizeof(char*) - 1;
	PySys_SetArgv(argc, argv);
	

	//Load the OBS Extension
	initOBS();


	String path = OBSGetPluginDataPath();
	path.FindReplace(TEXT("\\"), TEXT("/"));
	path = path + String("/Python");







	PythonRunString(String("import sys,os"));

	PythonRunString(String("os.makedirs('") + path + String("')"));
	PythonRunString(String("sys.path.append('") + path + String("')"));


	PythonRunString(String("sys.stdout = open('") + path + String("/stdOut.txt','w',0)"));
	PythonRunString(String("sys.stderr = open('") + path + String("/stdErr.txt','w',0)"));



	
	

	/*PyRun_SimpleString("import OBS\n",
		"import sys\n"
		"from PySide import QtGui, QtCore\n"
		"import ctypes\n"
		"app = QtGui.QApplication('')\n"
	);*/
	
	
	PyThreadState *pts = PyGILState_GetThisThreadState();
	PyEval_ReleaseThread(pts);
}




void PythonPlugin::finPython(){
	
	
	if (Py_IsInitialized()){
		Py_Finalize();
	}



}
  
PythonPlugin::~PythonPlugin()
{
	/*
	DWORD dwWaitResult = WaitForSingleObject(
		ghMutex,    // handle to mutex
		INFINITE);  // no time-out interval
	*/

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	Log(TEXT("Python plugin Destroyed"));
	if (isDynamicLocale) {
		int localizationStringCount = sizeof(localizationStrings) / sizeof(CTSTR);
		Log(TEXT("Video Source Plugin instance deleted; removing dynamically loaded localization strings"));
		for (int i = 0; i < localizationStringCount; i += 2) {
			locale->RemoveLookupString(localizationStrings[i]);
		}
	}


	

	//Run Alocated cleanup functions
	//One cleanup per uinque file
	clearShutdownFunctions();





	
	
	if (Py_IsInitialized()){
		Py_Finalize();
	}

	Log(TEXT("Python plugin Destroyed"));
	isDynamicLocale = false;
	

}

CTSTR GetPluginName(){
	return STR("PluginName");
}

CTSTR GetPluginDescription()
{
	return STR("PluginDescription");
}

bool LoadPlugin(){
	if (PythonPlugin::instance != NULL)
		return false;
	PythonPlugin::instance = new PythonPlugin();
	return true;
}

void UnloadPlugin(){
	if (PythonPlugin::instance == NULL)
		return;
	delete PythonPlugin::instance;
	PythonPlugin::instance = NULL;
}


void OnStartStream()
{
}

void OnStopStream()
{

	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
	}


	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	



	PyGILState_Release(gstate);
	return;
}


void PythonPlugin::addShutdownFunction(String filename, PyObject *function){

	shutdownFunc[filename] = function;
	Py_INCREF(function);
}


void PythonPlugin::clearShutdownFunctions(){


	for (shutdown_map::iterator i = shutdownFunc.begin(); i != shutdownFunc.end(); ++i){

		PyObject* pyFunc = i->second;
		//call shutdown func
		if (pyFunc != NULL){
			PyObject *argList = Py_BuildValue("()");
			PyObject_CallObject(pyFunc, argList);
			Py_DECREF(argList);
		}
		Py_DECREF(pyFunc);
	}
	shutdownFunc.clear();

}