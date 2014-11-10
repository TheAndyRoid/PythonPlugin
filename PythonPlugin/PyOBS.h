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
#include "structmember.h"
#include "OBSApi.h"


#include "PythonPlugin.h"


#include "PyVect2.h"
#include "PyImageSource.h"
#include "PyXElement.h"
#include "PyScene.h"






static PyObject *
py_OBSGetSceneName(PyObject *self, PyObject *args){
	CTSTR scenename = OBSGetSceneName();
	return PyUnicode_FromWideChar(scenename, wcslen(scenename));
}


static PyObject *
py_OBSGetScene(PyObject *self, PyObject *args){
	Scene* scene = OBSGetScene();
	PyObject *obj = PyObject_CallObject((PyObject *)&pySceneType, NULL);
	((PyScene*)obj)->scene = scene;
	return obj;	
}

static PyObject *
py_OBSGetAPIVersion(PyObject *self, PyObject *args){
	return PyLong_FromLong(OBSGetAPIVersion());
}


static PyObject *
py_OBSGetVersion(PyObject *self, PyObject *args){
	return PyLong_FromUnsignedLong(OBSGetVersion());
}


static PyObject *
py_OBSGetBaseSize(PyObject *self, PyObject *args){
	Vect2 size = OBSGetBaseSize();
	PyObject *argList = Py_BuildValue("dd", size.x, size.y);
	PyObject *obj = PyObject_CallObject((PyObject *)&PyVect2_Object, argList);
	Py_DECREF(argList);
	return obj;
}

static PyObject *
py_OBSGetRenderFrameSize(PyObject *self, PyObject *args){
	Vect2 size = OBSGetRenderFrameSize();
	PyObject *argList = Py_BuildValue("dd", size.x, size.y);
	PyObject *obj = PyObject_CallObject((PyObject *)&PyVect2_Object, argList);
	Py_DECREF(argList);
	return obj;
}

static PyObject *
py_OBSGetOutputSize(PyObject *self, PyObject *args){
	Vect2 size = OBSGetOutputSize();
	PyObject *argList = Py_BuildValue("dd", size.x, size.y);
	PyObject *obj = PyObject_CallObject((PyObject *)&PyVect2_Object, argList);
	Py_DECREF(argList);
	return obj;
}


static PyObject *
py_OBSGetMaxFPS(PyObject *self, PyObject *args){
	long maxfps = OBSGetMaxFPS();
	PyObject *obj = PyLong_FromLong(maxfps);	
	return obj;
}


static PyObject *
py_OBSGetCaptureFPS(PyObject *self, PyObject *args){
	long capfps = OBSGetCaptureFPS();
	PyObject *obj = PyLong_FromLong(capfps);
	return obj;
}


static PyObject *
py_OBSGetTotalFrames(PyObject *self, PyObject *args){
	long totalFrames = OBSGetTotalFrames();
	PyObject *obj = PyLong_FromLong(totalFrames);
	return obj;
}



static PyObject *
py_OBSGetFramesDropped(PyObject *self, PyObject *args){
	long droppedFrames = OBSGetFramesDropped();
	PyObject *obj = PyLong_FromLong(droppedFrames);
	return obj;
}


static PyObject *
py_OBSGetLanguage(PyObject *self, PyObject *args){
	CTSTR language = OBSGetLanguage();
	return PyUnicode_FromWideChar(language, wcslen(language));
}


static PyObject *
py_OBSGetMainWindow(PyObject *self, PyObject *args){
	return  PyLong_FromLong((long)OBSGetMainWindow());	
}


static PyObject *
py_OBSGetAppDataPath(PyObject *self, PyObject *args){
	CTSTR appdatapath = OBSGetAppDataPath();
	return PyUnicode_FromWideChar(appdatapath, wcslen(appdatapath));
}

static PyObject *
py_OBSGetPluginDataPath(PyObject *self, PyObject *args){
	String appdatapath = OBSGetPluginDataPath();		
	return PyUnicode_FromWideChar(TSTR(appdatapath), wcslen(TSTR(appdatapath)));
}


static PyObject *
py_OBSGetTotalStreamTime(PyObject *self, PyObject *args){
	long streamTime = OBSGetTotalStreamTime();
	PyObject *obj = PyLong_FromLong(streamTime);
	return obj;
}


static PyObject *
py_OBSGetBytesPerSec(PyObject *self, PyObject *args){
	PyObject *obj = PyLong_FromLong(OBSGetBytesPerSec());
	return obj;
}

static PyObject *
py_OBSGetAppPath(PyObject *self, PyObject *args){
	CTSTR apppath = OBSGetAppPath();
	return PyUnicode_FromWideChar(apppath, wcslen(apppath));
}

static PyObject *
py_OBSGetAudioTime(PyObject *self, PyObject *args){
	PyObject *obj = PyLong_FromLongLong(OBSGetAudioTime());
	return obj;
}


static PyObject *
py_OBSStartStopStream(PyObject *self, PyObject *args){
	OBSStartStopStream();
	return Py_BuildValue("");
}
static PyObject *
py_OBSStartStopPreview(PyObject *self, PyObject *args){
	OBSStartStopPreview();
	return Py_BuildValue("");
}

static PyObject *
py_OBSStartStopRecording(PyObject *self, PyObject *args){
	OBSStartStopRecording();
	return Py_BuildValue("");
}




static PyObject *
py_OBSLog(PyObject *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	PyUnicodeObject *str;

	if (!PyArg_ParseTuple(args, "U", &str)){
		return NULL;
	}

	wchar_t *wstr = pyObjectToWSTR(str);
	if (wstr == NULL){
		return NULL;
	}

	Log(TEXT("Python: %ls"),wstr);
	delete [] wstr;
	
	
	return Py_BuildValue("");
}








static PyObject *
py_OBSGetSceneListElement(PyObject *self, PyObject *args){
	PyObject *pyElement = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyElement)->element = OBSGetSceneListElement();
	return pyElement;
}
static PyObject *
py_OBSGetGlobalSourceListElement(PyObject *self, PyObject *args){
	PyObject *pyElement = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyElement)->element = OBSGetGlobalSourceListElement();
	return pyElement;
}

static PyObject *
py_OBSGetSceneElement(PyObject *self, PyObject *args){
	PyObject *pyElement = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyElement)->element = OBSGetSceneElement();

	return pyElement;
}
static PyObject *
py_OBSEnterSceneMutex(PyObject *self, PyObject *args){
	OBSEnterSceneMutex();
	return Py_BuildValue("");
}
static PyObject *
py_OBSLeaveSceneMutex(PyObject *self, PyObject *args){
	OBSLeaveSceneMutex();
	return Py_BuildValue("");
}
static PyObject *
py_OBSSetScene(PyObject *self, PyObject *args){

	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	PyUnicodeObject *name;
	PyObject *py_bPost;

	if (!PyArg_ParseTuple(args, "OO", &name, &py_bPost)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if ( wname == NULL){
		return NULL;
	}

	bool bPost;
	if (PyObject_IsTrue(py_bPost)){
		bPost = true;
	}
	else{
		bPost = false;
	}

	bool ret = OBSSetScene(wname, bPost);
	delete[] wname;

	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}

/*
static PyObject *
py_OBSGetIn1To1Mode(PyObject *self, PyObject *args){
	bool ret = OBSGetIn1To1Mode();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
*/
static PyObject *
py_OBSUseMultithreadedOptimizations(PyObject *self, PyObject *args){
	bool ret = OBSUseMultithreadedOptimizations();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *
py_OBSGetStreaming(PyObject *self, PyObject *args){
	bool ret = OBSGetStreaming();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *
py_OBSGetPreviewOnly(PyObject *self, PyObject *args){
	bool ret = OBSGetPreviewOnly();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *
py_OBSGetRecording (PyObject *self, PyObject *args){
	bool ret = OBSGetRecording();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *
py_OBSGetKeepRecording(PyObject *self, PyObject *args){
	bool ret = OBSGetKeepRecording();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *
py_OBSSetMicVolume(PyObject *self, PyObject *args){

	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	float value = 0.0;
	PyObject *pyFinal;

	if (!PyArg_ParseTuple(args, "fO", &value, &pyFinal)){
		return NULL;
	}

	bool finalValue;
	if (PyObject_IsTrue(pyFinal)){
		finalValue = true;
	}
	else{
		finalValue = false;
	}

	OBSSetMicVolume(value, finalValue);

	return Py_BuildValue("");
}
static PyObject *
py_OBSGetMicVolume(PyObject *self, PyObject *args){
	return PyFloat_FromDouble(OBSGetMicVolume());
}
static PyObject *
py_OBSToggleMicMute(PyObject *self, PyObject *args){
	OBSToggleMicMute();
	return Py_BuildValue("");
}
static PyObject *
py_OBSGetMicMuted(PyObject *self, PyObject *args){
	bool ret = OBSGetMicMuted();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *
py_OBSSetDesktopVolume(PyObject *self, PyObject *args){

	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	float value = 0.0;
	PyObject *pyFinal;

	if (!PyArg_ParseTuple(args, "fO", &value, &pyFinal)){
		return NULL;
	}

	bool finalValue;
	if (PyObject_IsTrue(pyFinal)){
		finalValue = true;
	}
	else{
		finalValue = false;
	}

	OBSSetDesktopVolume(value, finalValue);

	return Py_BuildValue("");
}
static PyObject *
py_OBSGetDesktopVolume(PyObject *self, PyObject *args){
	return PyFloat_FromDouble(OBSGetDesktopVolume());
}
static PyObject *
py_OBSToggleDesktopMute(PyObject *self, PyObject *args){
	OBSToggleDesktopMute();
	return Py_BuildValue("");
}
static PyObject *
py_OBSGetDesktopMuted(PyObject *self, PyObject *args){
	bool ret = OBSGetDesktopMuted();
	if (ret){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
py_OBSGetCurDesktopVolumeStats(PyObject *self, PyObject *args){
	float rms, max, peak;
	OBSGetCurDesktopVolumeStats(&rms,&max,&peak);
	PyObject *argList = Py_BuildValue("fff", rms, max,peak);
	return argList;

}


static PyObject *
py_OBSGetCurMicVolumeStats(PyObject *self, PyObject *args){
	float rms, max, peak;
	OBSGetCurMicVolumeStats(&rms, &max, &peak);
	PyObject *argList = Py_BuildValue("fff", rms, max, peak);
	return argList;

}

static PyObject *
py_OBSAddShutdownFunction(PyObject *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	PyObject *pyFunc;
	PyUnicodeObject *pyStr;

	if (!PyArg_ParseTuple(args, "OU", &pyFunc,&pyStr)){
		return NULL;
	}
	PythonPlugin *pyPlug = PythonPlugin::instance;
	
	

	wchar_t *wstr = pyObjectToWSTR(pyStr);
	if (wstr == NULL){
		return NULL;
	}

	Py_INCREF(pyFunc);
	pyPlug->addShutdownFunction(String(wstr), pyFunc);


	return Py_BuildValue("");
}







static PyMethodDef pyOBS_methods[] = {
		{ "GetAPIVersion", py_OBSGetAPIVersion, METH_VARARGS, "Gets OBS API version" },
		{ "GetSceneName", py_OBSGetSceneName, METH_VARARGS, "Gets OBS scene name" },
		{ "GetScene", py_OBSGetScene, METH_VARARGS, "Gets OBS scene name" },
		{ "GetBaseSize", py_OBSGetBaseSize, METH_VARARGS, "Get the base scene size " },
		{ "GetRenderFrameSize", py_OBSGetRenderFrameSize, METH_VARARGS, "Get the render frame size " },
		{ "GetOutputSize", py_OBSGetOutputSize, METH_VARARGS, "Get the stream output size " },
		{ "GetMaxFPS", py_OBSGetMaxFPS, METH_VARARGS, "Get the max FPS " },
		{ "GetCaptureFPS", py_OBSGetCaptureFPS, METH_VARARGS, "Get the capture FPS " },
		{ "GetTotalFrames", py_OBSGetTotalFrames, METH_VARARGS, "Get total number of frames " },
		{ "GetFramesDropped", py_OBSGetFramesDropped, METH_VARARGS, "Get number of dropped frames " },
		{ "GetLanguage", py_OBSGetLanguage, METH_VARARGS, "Get language of OBS" },
		{ "GetMainWindow", py_OBSGetMainWindow, METH_VARARGS, "Get HWNDID for OBS main window" },
		{ "GetAppDataPath", py_OBSGetAppDataPath, METH_VARARGS, "Get App data path" },
		{ "GetPluginDataPath", py_OBSGetPluginDataPath, METH_VARARGS, "Get Plugin data path" },
		{ "GetTotalStreamTime", py_OBSGetTotalStreamTime, METH_VARARGS, "Get stream time" },
		{ "GetBytesPerSec", py_OBSGetBytesPerSec, METH_VARARGS, "Get bytes per second" },
		{ "GetAppPath", py_OBSGetAppPath, METH_VARARGS, "Get application path" },
		{ "GetVersion", py_OBSGetVersion, METH_VARARGS, "Gets OBS version" },
		{ "GetAudioTime", py_OBSGetAudioTime, METH_VARARGS, "Gets Audio Time" },
		{ "StartStopStream", py_OBSStartStopStream, METH_VARARGS, "Starts or Stops Stream" },
		{ "StartStopPreview", py_OBSStartStopPreview, METH_VARARGS, "Starts or Stops Preview" },
		{ "StartStopRecording", py_OBSStartStopRecording, METH_VARARGS, "Starts or Stops Recording" },
		{ "GetSceneListElement", py_OBSGetSceneListElement, METH_VARARGS, "GetSceneListElement" },
		{ "GetGlobalSourceListElement", py_OBSGetGlobalSourceListElement, METH_VARARGS, "GetGlobalSourceListElement" },
		{ "GetSceneElement", py_OBSGetSceneElement, METH_VARARGS, "GetSceneElement" },
		{ "EnterSceneMutex", py_OBSEnterSceneMutex, METH_VARARGS, "EnterSceneMutex" },
		{ "LeaveSceneMutex", py_OBSLeaveSceneMutex, METH_VARARGS, "LeaveSceneMutex" },
		{ "SetScene", py_OBSSetScene, METH_VARARGS, "SetScene" },
		//{ "GetIn1To1Mode", py_OBSGetIn1To1Mode, METH_VARARGS, "GetIn1To1Mode" },
		{ "UseMultithreadedOptimizations", py_OBSUseMultithreadedOptimizations, METH_VARARGS, "UseMultithreadedOptimizations" },
		{ "GetStreaming", py_OBSGetStreaming, METH_VARARGS, "GetStreaming" },
		{ "GetPreviewOnly", py_OBSGetPreviewOnly, METH_VARARGS, "GetPreviewOnly" },
		{ "GetRecording", py_OBSGetRecording, METH_VARARGS, "GetRecording" },
		{ "GetKeepRecording", py_OBSGetKeepRecording, METH_VARARGS, "GetKeepRecording" },
		{ "SetMicVolume", py_OBSSetMicVolume, METH_VARARGS, "SetMicVolume" },
		{ "GetMicVolume", py_OBSGetMicVolume, METH_VARARGS, "GetMicVolume" },
		{ "ToggleMicMute", py_OBSToggleMicMute, METH_VARARGS, "ToggleMicMute" },
		{ "GetMicMuted", py_OBSGetMicMuted, METH_VARARGS, "GetMicMuted" },
		{ "SetDesktopVolume", py_OBSSetDesktopVolume, METH_VARARGS, "SetDesktopVolume" },
		{ "GetDesktopVolume", py_OBSGetDesktopVolume, METH_VARARGS, "GetDesktopVolume" },
		{ "ToggleDesktopMute", py_OBSToggleDesktopMute, METH_VARARGS, "ToggleDesktopMute" },
		{ "GetDesktopMuted", py_OBSGetDesktopMuted, METH_VARARGS, "GetDesktopMuted" },
		{ "GetCurDesktopVolumeStats", py_OBSGetCurDesktopVolumeStats, METH_VARARGS, "GetCurDesktopVolumeStats" },
		{ "GetCurMicVolumeStats", py_OBSGetCurMicVolumeStats, METH_VARARGS, "GetCurMicVolumeStats" },
		{ "Log", py_OBSLog, METH_VARARGS, "Writes to the OBS log" },
		{ "AddShutdownFunction", py_OBSAddShutdownFunction, METH_VARARGS, "Adds a function to be run at shutdown, limited to one per module" },

		{ NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC
initOBS(void)
{
	

	PyObject *m =  Py_InitModule("OBS", pyOBS_methods);
	if (m == NULL)
		return;

	if (PyType_Ready(&pyImageSourceType) < 0)
		return;
	Py_INCREF(&pyImageSourceType);
	PyModule_AddObject(m, "ImageSource", (PyObject *)&pyImageSourceType);

	if (PyType_Ready(&PyVect2_Object) < 0)
		return;
	Py_INCREF(&PyVect2_Object);
	PyModule_AddObject(m, "Vect2", (PyObject *)&PyVect2_Object);


	if (PyType_Ready(&PyVect4_Object) < 0)
		return;
	Py_INCREF(&PyVect4_Object);
	PyModule_AddObject(m, "Vect4", (PyObject *)&PyVect4_Object);


	if (PyType_Ready(&PyXElement_Object) < 0)
		return;
	Py_INCREF(&PyXElement_Object);
	PyModule_AddObject(m, "XElement", (PyObject *)&PyXElement_Object);


	if (PyType_Ready(&pySceneType) < 0)
		return;
	Py_INCREF(&pySceneType);
	PyModule_AddObject(m, "Scene", (PyObject *)&pySceneType);


	if (PyType_Ready(&pySceneItemType) < 0)
		return;
	Py_INCREF(&pySceneItemType);
	PyModule_AddObject(m, "SceneItem", (PyObject *)&pySceneItemType);

}



