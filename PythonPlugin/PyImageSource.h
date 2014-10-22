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
#include "pyerrors.h"
#include "OBSApi.h"
#include "CppImageSource.h"
#include "PythonPlugin.h"
#include "PyVect4.h"



/*Object Data*/
typedef struct{
	PyObject_HEAD
	/* Specific data for this python object */
	//Pointer to the cpp imagesource
	CppImageSource* cppImageSource;
	PyObject *bufferA;  //Byte array
	PyObject *bufferB;  //Byte array
	PyObject *formatString; // string
}pyImageSource;




/*Python Object Methods*/

static void
pyImageSource_dealloc(pyImageSource* self)
{
	

	Py_XDECREF(self->bufferA);
	Py_XDECREF(self->bufferB);
	Py_XDECREF(self->formatString);
	self->cppImageSource = NULL;
	self->ob_type->tp_free((PyObject*)self);
	
}


static PyObject * pyImageSource_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
	
	pyImageSource *self;
	
	
	//Get the plugin instance
	PythonPlugin *pyPlug = PythonPlugin::instance;
	




	self = (pyImageSource*)type->tp_alloc(type, 0);

	if (self != NULL) {
		
		
		//Get the image source and set it 
		if (pyPlug->tmpImgSrc == NULL){
			Py_DECREF(self);
			return NULL;
		}
		//Get the CppImage source from pyPlug
		self->cppImageSource = pyPlug->tmpImgSrc;
		

		
	}
	

	return (PyObject*)self;
}

static int
pyImageSource_init(pyImageSource *self, PyObject *args, PyObject *kwds)
{
	/*Do nothing for now*/
	return 0;
}




static PyObject * pyImageSource_FlipBuffers(pyImageSource *self){

	
	if (self->cppImageSource == NULL){
		PyErr_SetString(PyExc_UserWarning, "Image source does not exist yet or has been destroyed");
		return Py_BuildValue("");
	}

	self->cppImageSource->flipPixelBuffers();

	return Py_BuildValue("");

}



static PyObject * pyImageSource_GetAddrBackBuffer(pyImageSource *self){
	
	if (self->cppImageSource == NULL){
		return PyLong_FromLong((long)PyByteArray_AsString(self->bufferA));
	}else{
		return PyLong_FromLong((long)self->cppImageSource->getBackBuffer());
	}
}


static PyObject * pyImageSource_GetBackBuffer(pyImageSource *self){
	void * backAddr = self->cppImageSource->getBackBuffer();
	void *a = NULL;
	void *b = NULL;
	if (self->cppImageSource->isDoubleBuffer){
		a = PyByteArray_AsString(self->bufferA);
		b = PyByteArray_AsString(self->bufferA);
		if (backAddr = a){
			return self->bufferA;
		}
		else{
			return self->bufferB;
		}
	}
	else {
		return self->bufferA;	
	}
}


static PyObject * pyImageSource_DrawSprite(pyImageSource *self, PyObject *args){
	CppImageSource * imgSrc = self->cppImageSource;
	if (imgSrc == NULL){
		PyErr_SetString(PyExc_TypeError, "Missing CppInstance");
		return NULL;
	}	
	Texture * tex = imgSrc->GetTexture();
	if (!tex && imgSrc->getFrontBuffer() != NULL){
		PyErr_SetString(PyExc_TypeError, "Missing texture");
		return NULL;
	}
	else{
		tex->SetImage(imgSrc->getFrontBuffer(), imgSrc->getImageFormat(), tex->Width() * imgSrc->getBytesPerPixel());
	}

	long argLength = PyTuple_Size(args);
	float x, y, x2, y2, u, w, u2, w2;
	DWORD colour;
	Matrix original,custom;
	Euler a;
	switch (argLength){
	case 3:
		if (!PyArg_ParseTuple(args, "kff", &colour,&x, &y)){
			return NULL;
		}
		DrawSprite(tex, colour, x, y, x + self->cppImageSource->renderSize.x, y + self->cppImageSource->renderSize.y);
		break;
	case 5:		
		if (!PyArg_ParseTuple(args, "kffff", &colour, &x, &y,&x2,&y2)){
			return NULL;
		}
		DrawSprite(tex, colour, x, y,x2,y2);
		break;
	case 9:
		
		if (!PyArg_ParseTuple(args, "kffffffff", &colour, &x, &y, &x2, &y2,&u,&w,&u2,&w2)){
			return NULL;
		}	
		DrawSpriteEx(tex, colour, x, y, x2, y2,u,w,u2,w2);		
		break;
	default:
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
		break;
	}
	return Py_BuildValue("");
}



static PyObject * pyImageSource_SetCropping(pyImageSource *self, PyObject *args){
	CppImageSource * imgSrc = self->cppImageSource;
	if (imgSrc == NULL){
		PyErr_SetString(PyExc_TypeError, "Missing CppInstance");
		return NULL;
	}

	long argLength = PyTuple_Size(args);
	if (argLength != 4){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	float top, left, right, bottom;

	int value;

	if (!PyArg_ParseTuple(args, "ffff", &left, &top, &right, &bottom )){
		return NULL;
	}

	GS->SetCropping(left, top, bottom, right);


	return Py_BuildValue("");
}



static PyObject * pyImageSource_GetCropping(pyImageSource *self, PyObject *args){
	CppImageSource * imgSrc = self->cppImageSource;
	if (imgSrc == NULL){
		PyErr_SetString(PyExc_TypeError, "Missing CppInstance");
		return NULL;
	}

	Scene *scene = OBSGetScene();
	SceneItem *sceneItem = scene->GetSceneItem(imgSrc->getSourceName());
	if (!sceneItem){
		PyErr_SetString(PyExc_Warning, "Can't get cropping of global sources");
		return NULL;
	}
	Vect4 crop = sceneItem->GetCrop();

	PyObject * OBSModule = PyImport_ImportModule("OBS");
	if (OBSModule == NULL){
		Log(TEXT("OBSModule null"));
	}

	PyObject * vect4 = PyObject_GetAttr(OBSModule, PyString_FromString("Vect4"));
	if (vect4 == NULL){
		Log(TEXT("vect4 null"));
		return Py_BuildValue("");
	}

	PyObject *argList = Py_BuildValue("(dddd)", crop.x, crop.y,crop.z,crop.w);
	PyObject *pySize = PyObject_CallObject(vect4, argList);
	if (pySize == NULL){
		return Py_BuildValue("");
	}
	return pySize;
}



static PyObject * pyImageSource_SetBuffers(pyImageSource *self, PyObject *args){
	PyObject *bufA,*bufB,*format;

	CppImageSource* cimg = self->cppImageSource;
	long width, height;
	long argLength = PyTuple_Size(args);
	bool doubleBuffer;
	if (argLength == 5){
		doubleBuffer = TRUE;
	}else if(argLength == 4){
		doubleBuffer = FALSE;
	}
	else{				
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	if (doubleBuffer){
		if (!PyArg_ParseTuple(args, "OOOll:set_callback", &bufA, &bufB, &format, &width, &height)){
			return NULL;
		}
		if (!PyByteArray_Check(bufA)){
			PyErr_SetString(PyExc_TypeError, "Argument 1 must be a Python bytearray compatable object");
			return NULL;
		}
		if (!PyByteArray_Check(bufB)){
			PyErr_SetString(PyExc_TypeError, "Argument 2 must be a Python bytearray compatable object");
			return NULL;
		}
		if (!PyString_Check(format)){
			PyErr_SetString(PyExc_TypeError, "Argument 3 must be a Python string object");
			return NULL;
		}

		///check string format here
		if (!cimg->setupFormats(PyString_AsString(format))){
			PyErr_SetString(PyExc_TypeError, "Unknow buffer format");
			return NULL;
		}

		int bitdepth = cimg->getBytesPerPixel();

		//check that the buffers are large enough 
		long bufsize = PyByteArray_Size(bufA);

		if (bufsize < bitdepth*width*height){
			PyErr_SetString(PyExc_TypeError, "Attempted to use undersized buffer for bufferA");
			return NULL;
		}
		bufsize = PyByteArray_Size(bufB);
		if (bufsize < bitdepth*width*height){
			PyErr_SetString(PyExc_TypeError, "Attempted to use undersized buffer for bufferB");
			return NULL;
		}

		
	
		
		Py_XDECREF(self->bufferA);
		Py_INCREF(bufA);
		self->bufferA = bufA;	


		Py_XDECREF(self->bufferB);
		Py_INCREF(bufB);
		self->bufferB = bufB;
		
		Py_XDECREF(self->formatString);
		Py_INCREF(format);
		self->formatString = format;


		cimg->setupDoubleBuffers(PyByteArray_AsString(bufA), PyByteArray_AsString(bufB),width,height);

	}
	else{
		if (!PyArg_ParseTuple(args, "OOll:set_callback", &bufA, &format, &width, &height)){
			return NULL;
		}
		if (!PyByteArray_Check(bufA)){
			PyErr_SetString(PyExc_TypeError, "Argument 1 must be a Python bytearray compatable object");
			return NULL;
		}

		if (!PyString_Check(format)){
			PyErr_SetString(PyExc_TypeError, "Argument 2 must be a Python string object"); 
			return NULL;
		}	

	    //check string format here
		if (!cimg->setupFormats(PyString_AsString(format))){
			PyErr_SetString(PyExc_TypeError, "Unknow buffer format");
			return NULL;
		}
		
		int bitdepth = cimg->getBytesPerPixel();
		//check that the buffers are large enough 
		long bufsize = PyByteArray_Size(bufA);

		if (bufsize < bitdepth*width*height){
			PyErr_SetString(PyExc_TypeError, "Attempted to use undersized buffer");
			return NULL;
		}
		

		Py_XDECREF(self->formatString);
		Py_INCREF(format);
		self->formatString = format;

		Py_XDECREF(self->bufferA);
		Py_XDECREF(self->bufferB);
		Py_INCREF(bufA);
		self->bufferA = bufA;

		cimg->setupSingleBuffer(PyByteArray_AsString(bufA), width, height);


	}
	
	return Py_BuildValue("");
}

static PyObject * pyImageSource_CopyToBackBuffer(pyImageSource *self, PyObject *args){

	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	unsigned long addr;
	if (!PyArg_ParseTuple(args, "I", &addr)){
		return NULL;
	}
	if (self->cppImageSource){
		void * backBuf = self->cppImageSource->getBackBuffer();
		int width = self->cppImageSource->getWidth();
		int height = self->cppImageSource->getHeight();
		int depth = self->cppImageSource->getBytesPerPixel();
		memcpy(backBuf, (void*)addr, width*height*depth);
	}
	return Py_BuildValue("");

}




static PyObject * pyImageSource_Render(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_Tick(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_UpdateSettings(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_Destructor(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_Load(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_Export(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_ToBackground(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_ToForeground(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_BeginScene(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_EndScene(pyImageSource *self){
	return Py_BuildValue("");
}

static PyObject * pyImageSource_GlobalSourceEnterScene(pyImageSource *self){
	return Py_BuildValue("");
}

static PyObject * pyImageSource_GlobalSourceLeaveScene(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_ChangeScene(pyImageSource *self){
	return Py_BuildValue("");
}


static PyObject *
pyImageSource_CreateHotKey(pyImageSource *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	PyObject *callback;
	ULONG key;

	if (!PyArg_ParseTuple(args, "kO", &key, &callback)){
		PyErr_SetString(PyExc_TypeError, "Wrong type of arguments");
		return NULL;
	}

	if (!PyCallable_Check(callback)){
		PyErr_SetString(PyExc_TypeError, "Callback argument was not callable");
		return NULL;
	}

	UINT cRet;
	PyObject *pyRet;
	cRet = OBSCreateHotkey(key, (OBSHOTKEYPROC)Hotkey, (UPARAM)callback);
	pyRet = PyInt_FromLong(cRet);
	Py_INCREF(callback);

	
	self->cppImageSource->hotkeyToCallable[cRet] = callback;

	return Py_BuildValue("O", pyRet);
}

static PyObject *
pyImageSource_DeleteHotKey(pyImageSource *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}


	ULONG key;

	if (!PyArg_ParseTuple(args, "k", &key)){
		PyErr_SetString(PyExc_TypeError, "Wrong type of arguments");
		return NULL;
	}

	OBSDeleteHotkey(key);

	if (self->cppImageSource->hotkeyToCallable.find(key) != self->cppImageSource->hotkeyToCallable.end()){
		PyObject *callback = self->cppImageSource->hotkeyToCallable[key];
		Py_DECREF(callback);
		self->cppImageSource->hotkeyToCallable.erase(key);
	}
	return Py_BuildValue("");
}



/*Method Table*/
static PyMethodDef pyImageSource_methods[] = {
		{ "SetBuffers", (PyCFunction)pyImageSource_SetBuffers, METH_VARARGS, "Set which buffers to use for pixeldata and their format" },
		{ "FlipBuffers", (PyCFunction)pyImageSource_FlipBuffers, METH_VARARGS, "Flips buffers in double buffer mode" },
		{ "DrawSprite", (PyCFunction)pyImageSource_DrawSprite, METH_VARARGS, "Draws front buffer to screen" },
		{ "Render", (PyCFunction)pyImageSource_Render, METH_VARARGS, "Function to be overidden" },
		{ "UpdateSettings", (PyCFunction)pyImageSource_UpdateSettings, METH_VARARGS, "Function to be overidden" },
		{ "BeginScene", (PyCFunction)pyImageSource_BeginScene, METH_VARARGS, "Function to be overidden" },
		{ "EndScene", (PyCFunction)pyImageSource_EndScene, METH_VARARGS, "Function to be overidden" },
		{ "GlobalSourceEnterScene", (PyCFunction)pyImageSource_GlobalSourceEnterScene, METH_VARARGS, "Function to be overidden" },
		{ "GlobalSourceLeaveScene", (PyCFunction)pyImageSource_GlobalSourceLeaveScene, METH_VARARGS, "Function to be overidden" },
		{ "ChangeScene", (PyCFunction)pyImageSource_ChangeScene, METH_VARARGS, "Function to be overidden" },
		{ "Tick", (PyCFunction)pyImageSource_Tick, METH_VARARGS, "Function to be overidden" },
		{ "SetCropping", (PyCFunction)pyImageSource_SetCropping, METH_VARARGS, "Set the draw cropping of the current frame" },
		{ "GetCropping", (PyCFunction)pyImageSource_GetCropping, METH_VARARGS, "Gets cropping of current frame" },
		{ "Destructor", (PyCFunction)pyImageSource_Destructor, METH_VARARGS, "Function to be overidden" },
		{ "load", (PyCFunction)pyImageSource_Load, METH_VARARGS, "Function to be overidden" },
		{ "export", (PyCFunction)pyImageSource_Export, METH_VARARGS, "Function to be overidden" },
		{ "toBackground", (PyCFunction)pyImageSource_ToBackground, METH_VARARGS, "Function to be overidden" },
		{ "toForeground", (PyCFunction)pyImageSource_ToForeground, METH_VARARGS, "Function to be overidden" },
		{ "GetAddrBackBuffer", (PyCFunction)pyImageSource_GetAddrBackBuffer, METH_VARARGS, "Gets memory address for buffer for use with ctypes" },
		{ "GetBackBuffer", (PyCFunction)pyImageSource_GetBackBuffer, METH_VARARGS, "Gets byte buffer" },
		{ "CopyToBackBuffer", (PyCFunction)pyImageSource_CopyToBackBuffer, METH_VARARGS, "Copies data from addr to backbuffer" },
		{ "CreateHotKey", (PyCFunction)pyImageSource_CreateHotKey, METH_VARARGS, "Creates a hotkey" },
		{ "DeleteHotKey", (PyCFunction)pyImageSource_DeleteHotKey, METH_VARARGS, "Deletes a hotkey" },
	{ NULL }  /* Sentinel */
};

/*Member table*/
static PyMemberDef pyImageSource_members[] = {
	{ NULL }  /* Sentinel */
};



/*Python Type Object */
static PyTypeObject pyImageSourceType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"OBS.ImageSource",         /*tp_name*/
	sizeof(pyImageSource),     /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)pyImageSource_dealloc, /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"OBS Image Source",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	pyImageSource_methods,             /* tp_methods */
	pyImageSource_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)pyImageSource_init,      /* tp_init */
	0,                         /* tp_alloc */
	pyImageSource_new,                 /* tp_new */
};