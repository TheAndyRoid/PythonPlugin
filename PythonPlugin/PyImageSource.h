#pragma once


#include <Python.h>
#include "structmember.h"
#include "pyerrors.h"
#include "OBSApi.h"
#include "CppImageSource.h"
#include "PythonPlugin.h"



/*Object Data*/
typedef struct{
	PyObject_HEAD
	/* Specific data for this python object */
	//Pointer to the cpp imagesource
	CppImageSource* cppImageSource;
	//Python callable objects
	PyObject *render; //Render Callback
	PyObject *tick; // Tick Callback
	PyObject *stop; //Render Callback
	PyObject *updateSettings; // Settings Callback
	PyObject *bufferA;  //Byte array
	PyObject *bufferB;  //Byte array
	PyObject *formatString; // string
}pyImageSource;




/*Python Object Methods*/

static void
pyImageSource_dealloc(pyImageSource* self)
{
	
	Py_XDECREF(self->tick);
	Py_XDECREF(self->stop);
	Py_XDECREF(self->render);
	Py_XDECREF(self->updateSettings);
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
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
		return NULL;
	}

	if (pyPlug->pImageSource == NULL){
		return NULL;
	}

	if (pyPlug->pImageSource->getImageSource() != NULL){
		//Py_INCREF(pyPlug->pImageSource->getImageSource());
		return pyPlug->pImageSource->pyImgSrc;
	}

	self = (pyImageSource*)type->tp_alloc(type, 0);

	if (self != NULL) {
		
		
		//Get the image source and set it 
		if (pyPlug->pImageSource == NULL){
			Py_DECREF(self);
			return NULL;
		}
		self->cppImageSource = pyPlug->pImageSource;

		
		
		

		//Create non-callable objects as default
		self->render = PyFloat_FromDouble(0.0);
		if (self->render == NULL){
			Py_DECREF(self);
			return NULL;
		}
		self->stop = PyFloat_FromDouble(0.0);
		if (self->stop == NULL){
			Py_DECREF(self);
			return NULL;
		}
		self->tick = PyFloat_FromDouble(0.0);
		if (self->tick == NULL){
			Py_DECREF(self);
			return NULL;
		}
		self->updateSettings = PyFloat_FromDouble(0.0);
		if (self->updateSettings == NULL){
			Py_DECREF(self);
			return NULL;
		}
	}
	
	//set the object here
	pyPlug->pImageSource->pyImgSrc = (PyObject*)self;
	//Py_INCREF(self);


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
		return PyInt_FromLong((long)PyByteArray_AsString(self->bufferA));
	}else{
		return PyInt_FromLong((long)self->cppImageSource->getBackBuffer());
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

static PyObject * pyImageSource_SetBuffers(pyImageSource *self, PyObject *args){
	PyObject *bufA,*bufB,*format;

	CppImageSource* cimg = self->cppImageSource;
	
	long argLength = PyTuple_Size(args);
	bool doubleBuffer;
	if (argLength == 3){
		doubleBuffer = TRUE;
	}else if(argLength = 2){
		doubleBuffer = FALSE;
	}
	else{				
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	if (doubleBuffer){
		PyArg_ParseTuple(args, "OOO:set_callback", &bufA, &bufB,&format);
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
	
		
		//Py_DECREF(self->bufferA);
		Py_INCREF(bufA);
		self->bufferA = bufA;
		


		//Py_DECREF(self->bufferB);
		Py_INCREF(bufB);
		self->bufferB = bufB;
		


		cimg->setupDoubleBuffers(PyByteArray_AsString(bufA), PyByteArray_AsString(bufB));


		Py_INCREF(format);
		self->formatString = format;



		
	}
	else{
		PyArg_ParseTuple(args, "OO:set_callback", &bufA, &format);
		if (!PyByteArray_Check(bufA)){
			PyErr_SetString(PyExc_TypeError, "Argument 1 must be a Python bytearray compatable object");
			return NULL;
		}
		if (!PyString_Check(format)){
			PyErr_SetString(PyExc_TypeError, "Argument 2 must be a Python string object"); 
			return NULL;
		}	

		///check string format here
		if (!cimg->setupFormats(PyString_AsString(format))){
			PyErr_SetString(PyExc_TypeError, "Unknow buffer format");
			return NULL;
		}
		


	}
	
	return Py_BuildValue("");
}






static PyObject * pyImageSource_Render(pyImageSource *self){
	return Py_BuildValue("");
}
static PyObject * pyImageSource_Tick(pyImageSource *self){
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




/*Method Table*/
static PyMethodDef pyImageSource_methods[] = {
		{ "SetBuffers", (PyCFunction)pyImageSource_SetBuffers, METH_VARARGS, "Set which buffers to use for pixeldata and their format" },
		{ "flipBuffers", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Flips buffers in double buffer mode" },
		{ "render", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Function to be overidden" },
		{ "tick", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Function to be overidden" },
		{ "destructor", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Function to be overidden" },
		{ "load", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Function to be overidden" },
		{ "export", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Function to be overidden" },
		{ "toBackground", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Function to be overidden" },
		{ "toForeground", (PyCFunction)pyImageSource_FlipBuffers, METH_NOARGS, "Function to be overidden" },
		{ "getAddrBackBuffer", (PyCFunction)pyImageSource_GetAddrBackBuffer, METH_NOARGS, "Gets memory address for buffer for use with ctypes" },
		{ "getBackBuffer", (PyCFunction)pyImageSource_GetBackBuffer, METH_NOARGS, "Gets byte buffer" },
	{ NULL }  /* Sentinel */
};

/*Member table*/
static PyMemberDef pyImageSource_members[] = {
		{ "renderCallBack", T_OBJECT_EX, offsetof(pyImageSource, render), 0, "renderCallBack" },
		{ "stopCallBack", T_OBJECT_EX, offsetof(pyImageSource, stop), 0, "stopCallBack" },
		{ "tickCallBack", T_OBJECT_EX, offsetof(pyImageSource, tick), 0, "tickCallBack" },
		//{ "data", T_OBJECT_EX, offsetof(pyImageSource, data), 0, "data" },
		{ "updateSettingsCallBack", T_OBJECT_EX, offsetof(pyImageSource, updateSettings), 0, "updateSettingsCallBack" },
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