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
	String parent = ((data->GetParent())->GetParent())->GetName();
	bool persistantObject = false;
	if (data->GetInt(TEXT("Persistant")) && !(parent == TEXT("global sources"))){
		persistantObject = true;
		pyPlug->pImageSource->setPersistant(true);
	}

	
	
	//Check if object aleady exists as persistant
	//Add to persistant
	//
	if (persistantObject ){	
		PyObject *pyPerst = NULL;
		pyPerst = pyPlug->getPersistItem(sceneName, sourceName);
		if (pyPerst != NULL){
			//Python object already exists yay
			Log(TEXT("Persistant object detected and found"));

			((pyImageSource*)pyPerst)->cppImageSource = pyPlug->pImageSource;
			pyPlug->pImageSource->pyImgSrc = (PyObject*)pyPerst;
			return pyPlug->pImageSource;
		}
	}
	






	/*
	DWORD dwWaitResult = WaitForSingleObject(
		pyPlug->ghMutex,    // handle to mutex
		INFINITE);  // no time-out interval
	*/

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
		//Add to persistant
		if (persistantObject){
			pyPlug->setPersistItem(sceneName, sourceName, (PyObject*)pyImgSrc);
		}
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

	//Create mutex
	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex


	

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


	PyRun_SimpleString("import sys,os");
	PyRun_SimpleString("sys.path.append(\"./plugins/Python\")");
	PyRun_SimpleString("sys.stdout = open('./plugins/Python/pyOut.txt','w',0)");
	PyRun_SimpleString("sys.stderr = open('./plugins/Python/pyErr.txt','w',0)");
	
	

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

	//Cleanup persistant objects



	
	
	if (Py_IsInitialized()){
		Py_Finalize();
	}

	Log(TEXT("Python plugin Destroyed"));
	isDynamicLocale = false;
	
	ReleaseMutex(ghMutex);
	CloseHandle(ghMutex);
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

	for (std::map<unsigned long, PyObject*>::iterator it = pyPlug->hotkeyToCallable.begin(); it != pyPlug->hotkeyToCallable.end(); ++it){
		PyObject* callback = it->second;
		Py_DECREF(callback);
	}

	pyPlug->hotkeyToCallable.clear();



	pyPlug->clearPersistItems();

	




	PyGILState_Release(gstate);
	return;
}

void PythonPlugin::setPersistItem(String sceneName, String sourceName,PyObject *pyImgSrc){
	
		if (persistantPyObjects.count(sceneName)){
			//scene already exists
			//get source map
			source_map &sourceMap = persistantPyObjects[sceneName];
			if (sourceMap.count(sourceName)){
				//source already exists What?
			}
			else{
				sourceMap[sourceName] = (PyObject*)pyImgSrc;
			}

		}
		else{
			//add scene and source
			persistantPyObjects[sceneName][sourceName] = (PyObject*)pyImgSrc;
		}
}

PyObject * PythonPlugin::getPersistItem(String sceneName, String sourceName){

	if (persistantPyObjects.count(sceneName)){

		if (persistantPyObjects[sceneName].count(sourceName)){

			return persistantPyObjects[sceneName][sourceName];
		}
				
	}
	return NULL;
	

}

void PythonPlugin::clearPersistItems(){
	//Get persistant items that are in the current scene

	XElement* scene = OBSGetSceneElement();
	String skipScene = scene->GetName();







	for (scene_map::iterator i = persistantPyObjects.begin(); i != persistantPyObjects.end(); ++i){
		if (i->first == skipScene){
			continue;
		}
		source_map sourceMap = i->second;
		for (source_map::iterator j = sourceMap.begin(); j != sourceMap.end(); ++j){
			PyObject* pyImage = j->second;
			PyObject * destr;
			//call destructor
			if (pyImage != NULL){
				destr = PyObject_GetAttrString(pyImage, (char*) "destructor");
				if (PyCallable_Check(destr)){
					PyObject *argList = Py_BuildValue("()");
					PyObject_CallObject(destr, argList);
					Py_DECREF(argList);
				}
				Py_DECREF(destr);
			}
			
			Py_DECREF(pyImage);
		}
	}

	persistantPyObjects.clear();
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