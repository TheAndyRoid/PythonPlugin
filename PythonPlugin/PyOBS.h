#pragma once


#include <Python.h>
#include "structmember.h"
#include "OBSApi.h"


#include "PythonPlugin.h"


#include "PyVect2.h"
#include "PyImageSource.h"





static PyObject *
py_OBSGetSceneName(PyObject *self, PyObject *args){
	CTSTR scenename = OBSGetSceneName();
	return PyUnicode_FromWideChar(scenename, wcslen(scenename));
}

static PyObject *
py_OBSGetAPIVersion(PyObject *self, PyObject *args){
	return PyInt_FromLong(OBSGetAPIVersion());
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
	PyObject *obj = PyInt_FromLong(maxfps);	
	return obj;
}


static PyObject *
py_OBSGetCaptureFPS(PyObject *self, PyObject *args){
	long capfps = OBSGetCaptureFPS();
	PyObject *obj = PyInt_FromLong(capfps);
	return obj;
}


static PyObject *
py_OBSGetTotalFrames(PyObject *self, PyObject *args){
	long totalFrames = OBSGetTotalFrames();
	PyObject *obj = PyInt_FromLong(totalFrames);
	return obj;
}



static PyObject *
py_OBSGetFramesDropped(PyObject *self, PyObject *args){
	long droppedFrames = OBSGetFramesDropped();
	PyObject *obj = PyInt_FromLong(droppedFrames);
	return obj;
}


static PyObject *
py_OBSGetLanguage(PyObject *self, PyObject *args){
	CTSTR language = OBSGetLanguage();
	return PyUnicode_FromWideChar(language, wcslen(language));
}


static PyObject *
py_OBSGetMainWindow(PyObject *self, PyObject *args){
	return  PyCObject_FromVoidPtr(OBSGetMainWindow(), NULL);	
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
	PyObject *obj = PyInt_FromLong(streamTime);
	return obj;
}


static PyObject *
py_OBSGetBytesPerSec(PyObject *self, PyObject *args){
	PyObject *obj = PyInt_FromLong(OBSGetBytesPerSec());
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
py_GetImageSource(PyObject *self, PyObject *args){
	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
		return Py_BuildValue("");
	}
	//Get the image source and set it 
	if (pyPlug->pImageSource == NULL){
		Log(TEXT("Python pImageSource NULL"));
		return Py_BuildValue("");
	}
	PyObject *imgsrc = pyPlug->pImageSource->getImageSource();
	if (imgsrc == NULL){
		Log(TEXT("Python getImageSource NULL"));
		return Py_BuildValue("");
	}

	return imgsrc;
}





static PyMethodDef pyOBS_methods[] = {
		{ "OBSGetAPIVersion", py_OBSGetAPIVersion, METH_VARARGS, "Gets OBS API version" },
		{ "OBSGetSceneName", py_OBSGetSceneName, METH_VARARGS, "Gets OBS scene name" },
		{ "OBSGetBaseSize", py_OBSGetBaseSize, METH_VARARGS, "Get the base scene size " },
		{ "OBSGetRenderFrameSize", py_OBSGetRenderFrameSize, METH_VARARGS, "Get the render frame size " },
		{ "OBSGetOutputSize", py_OBSGetOutputSize, METH_VARARGS, "Get the stream output size " },
		{ "OBSGetMaxFPS", py_OBSGetMaxFPS, METH_VARARGS, "Get the max FPS " },
		{ "OBSGetCaptureFPS", py_OBSGetCaptureFPS, METH_VARARGS, "Get the capture FPS " },
		{ "OBSGetTotalFrames", py_OBSGetTotalFrames, METH_VARARGS, "Get total number of frames " },
		{ "OBSGetFramesDropped", py_OBSGetFramesDropped, METH_VARARGS, "Get number of dropped frames " },
		{ "OBSGetLanguage", py_OBSGetLanguage, METH_VARARGS, "Get language of OBS" },
		{ "OBSGetMainWindow", py_OBSGetMainWindow, METH_VARARGS, "Get HWNDID for OBS main window" },
		{ "OBSGetAppDataPath", py_OBSGetAppDataPath, METH_VARARGS, "Get App data path" },
		{ "OBSGetPluginDataPath", py_OBSGetPluginDataPath, METH_VARARGS, "Get Plugin data path" },
		{ "OBSGetTotalStreamTime", py_OBSGetTotalStreamTime, METH_VARARGS, "Get stream time" },
		{ "OBSGetBytesPerSec", py_OBSGetBytesPerSec, METH_VARARGS, "Get bytes per second" },
		{ "OBSGetAppPath", py_OBSGetAppPath, METH_VARARGS, "Get application path" },
		{ "OBSGetVersion", py_OBSGetVersion, METH_VARARGS, "Gets OBS version" },
		{ "OBSGetAudioTime", py_OBSGetAudioTime, METH_VARARGS, "Gets Audio Time" },
		{ "OBSStartStopStream", py_OBSStartStopStream, METH_VARARGS, "Starts or Stops Stream" },
		{ "OBSStartStopPreview", py_OBSStartStopPreview, METH_VARARGS, "Starts or Stops Preview" },
		{ "OBSStartStopRecording", py_OBSStartStopRecording, METH_VARARGS, "Starts or Stops Recording" },
		{ "OBSGetImageSource", py_GetImageSource, METH_VARARGS, "Gets the imagesource python object" },
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


}



