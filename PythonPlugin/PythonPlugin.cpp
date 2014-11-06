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


/*Called when the python source is expeteced to show a config gui*/
bool STDCALL ConfigurePythonSource(XElement *element, bool bCreating)
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
		moduleName = TEXT("DefaultGUI");
		className = TEXT("gui");

	}
	else{
		file = dataElement->GetString(TEXT("PythonGUIFile"));
		className = dataElement->GetString(TEXT("PythonGUIClass"));
		moduleName = addToPythonPath(file);

	}


	bool reload = dataElement->GetInt(TEXT("Debug"));
	
	pyHasError();

	pName = CTSTRtoPyUnicode(moduleName);
	PyObject *dict = PyImport_GetModuleDict();
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

		if (pFunc && PyCallable_Check(pFunc)) {
			PyObject *argList = Py_BuildValue("");
			PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
			Py_XDECREF(argList);
			((PyXElement*)pyConfig)->element = dataElement;

			argList = Py_BuildValue("(O)", pyConfig);
			PyObject *ret = PyObject_CallObject(pFunc, argList);			

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
		
		//check if the elements have been added to config. if not then setup gui failed.
		String log = dataElement->GetString(TEXT("PythonGUIFile"));

		if (!dataElement->GetString(TEXT("PythonGUIFile")) || !dataElement->GetString(TEXT("PythonGUIClass"))
			|| !dataElement->GetString(TEXT("PythonMainFile")) || !dataElement->GetString(TEXT("PythonMainClass"))){
			//User canceled
			return false;
		}
		else{
			return ConfigurePythonSource(element, false);
		}
	}
	else{
		return true;
	}
}

/*Called when the element is created at stream start.*/
ImageSource* STDCALL CreatePythonSource(XElement *data)
{

	Log(TEXT("Python Create ImageSource"));

	XElement *thisSource = data->GetParent();
	XElement *sources = thisSource->GetParent();
	XElement *thisScene = sources->GetParent();
	
	String sourceName = thisSource->GetName();
	String sceneName = thisScene->GetName();



	//Get the plugin instance
	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));	
		return NULL;
	}	

	//Create Image source
	CppImageSource *pImageSource = new CppImageSource(data);
	pyPlug->tmpImgSrc = pImageSource;
	

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

	bool reload = data->GetInt(TEXT("Debug"));

	PyObject *dict = PyImport_GetModuleDict();
	if (PyDict_Contains(dict, pName) && reload){
		pModule = PyDict_GetItem(dict, pName);
		pModule = PyImport_ReloadModule(pModule);
		pyHasError();
	}
	else{
		pModule = PyImport_Import(pName);
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
				pImageSource->pyImgSrc = (PyObject*)pyImgSrc;
				//Clear pyPlug
				pyPlug->tmpImgSrc = NULL;
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

	if (pyObjectCreated){		
		return pImageSource;
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
		Log(TEXT("Python Plugin strings not found, dynamically loading %d strings"), sizeof(localizationStrings) / sizeof(CTSTR));
		for (int i = 0; i < localizationStringCount; i += 2) {
			locale->AddLookupString(localizationStrings[i], localizationStrings[i + 1]);
		}
		if (!locale->HasLookup(KEY("PluginName"))) {
			AppWarning(TEXT("Uh oh..., unable to dynamically add our localization keys"));
		}
	}
	

	//Register callbacks with obs
	OBSRegisterImageSourceClass(VIDEO_SOURCE_CLASS, STR("ClassName"), (OBSCREATEPROC)CreatePythonSource, (OBSCONFIGPROC)ConfigurePythonSource);


	
	Py_Initialize();
	PyEval_InitThreads();
	

	
	/*Must set arguments for guis to work*/
	char *argv[] = { "OBS", NULL };
	int argc = sizeof(argv) / sizeof(char*) - 1;
	PySys_SetArgv(argc, argv);
	

	//Load the OBS Extension
	initOBS();


	String path = OBSGetPluginDataPath();
	path.FindReplace(TEXT("\\"), TEXT("/"));
	path = path + String("/Python");


	String appPath(OBSGetAppPath());
	appPath.FindReplace(TEXT("\\"), TEXT("/"));
	appPath = appPath + String("/plugins/PythonPlugin");




	PythonRunString(String("import sys,os"));

	PythonRunString(String("os.makedirs('") + path + String("')"));
	PythonRunString(String("sys.path.append('") + path + String("')"));
	PythonRunString(String("sys.path.append('") + appPath + String("')"));


	PythonRunString(String("sys.stdout = open('") + path + String("/stdOut.txt','w',0)"));
	PythonRunString(String("sys.stderr = open('") + path + String("/stdErr.txt','w',0)"));


	//Release the GIL	
	PyThreadState *pts = PyGILState_GetThisThreadState();
	PyEval_ReleaseThread(pts);
}


  
PythonPlugin::~PythonPlugin()
{


	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	Log(TEXT("Python plugin Destroyed"));
	if (isDynamicLocale) {
		int localizationStringCount = sizeof(localizationStrings) / sizeof(CTSTR);
		Log(TEXT("Plugin instance deleted; removing dynamically loaded localization strings"));
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

	if (!isPythonInPath()){
		int ret = OBSMessageBox(OBSGetMainWindow(), TEXT("'Python27' not found in PATH environment variable! \n Python Plugin will instantly close OBS if Python can't be loaded.\n Attempt to load Python Plugin?\n"), TEXT("ERROR- 'Python27' NOT IN PATH"), 4);
		if (ret == 7){ // No
			AppWarning(TEXT("Python Plugin - User Aborted Loading!"));
			return false;
		}
		else if(ret == 6){ //yes
			AppWarning(TEXT("Python Plugin - User Ignored warning! Attempting to Load"));
		}
		else{
			return false;
		}
	}

	if (PythonPlugin::instance != NULL){
		return false;
	}
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