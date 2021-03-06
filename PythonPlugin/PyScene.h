
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
#include "PythonPlugin.h"
#include "PyXElement.h"
#include "PySceneItem.h"
#include "utils.h"



/*Object Data*/
typedef struct{
	PyObject_HEAD
	Scene *scene;
}PyScene;



static bool sceneExists(PyScene *self){
	if (self->scene == NULL){
		PyErr_SetString(PyExc_TypeError, "Scene is NULL");
		return false;
	}
	else{
		return true;
	}
}



static void
pyScene_dealloc(PyScene* self)
{
	self->scene = NULL;
	Py_TYPE(self)->tp_free((PyObject*)self);

}


static PyObject * pyScene_new(PyTypeObject *type, PyObject *args, PyObject *kwds){

	PyScene *self;
	self = (PyScene *)type->tp_alloc(type, 0);
	if (self == NULL) {
		return NULL;
	}
	self->scene = NULL;

	return (PyObject *)self;
}


static int
pyScene_init(PyScene *self, PyObject *args, PyObject *kwds)
{
	/*Do nothing for now*/
	return 0;
}

static PyObject* pyScene_InsertImageSource(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	UINT pos;
	PyObject *pyElement;

	if (!PyArg_ParseTuple(args, "IO", &pos, &pyElement)){
		return NULL;
	}

	XElement *cElement = ((PyXElement*)pyElement)->element;
	SceneItem *ret = self->scene->InsertImageSource(pos, cElement);


	PyObject *pyRet = PyObject_CallObject((PyObject*)&pySceneItemType, NULL);
	((PySceneItem*)pyRet)->sceneItem = ret;


	return pyRet;
}
static PyObject* pyScene_AddImageSource(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}


	PyObject *pyElement;

	if (!PyArg_ParseTuple(args, "O", &pyElement)){
		return NULL;
	}

	XElement *cElement = ((PyXElement*)pyElement)->element;
	SceneItem *ret = self->scene->AddImageSource(cElement);


	PyObject *pyRet = PyObject_CallObject((PyObject*)&pySceneItemType, NULL);
	((PySceneItem*)pyRet)->sceneItem = ret;


	return pyRet;
}
static PyObject* pyScene_RemoveImageSource(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	if (PyTuple_Size(args) != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	
	PyUnicodeObject *name;

	if (!PyArg_ParseTuple(args, "U", &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);


	if ( wname == NULL){
		return NULL;
	}

	self->scene->RemoveImageSource(wname);
	delete[] wname;
	return Py_BuildValue("");
}
static PyObject* pyScene_Tick(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	if (PyTuple_Size(args) != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}


	float seconds;

	if (!PyArg_ParseTuple(args, "f", &seconds)){
		return NULL;
	}	
	self->scene->Tick(seconds);
	return Py_BuildValue("");
}
static PyObject* pyScene_Render(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	self->scene->Render();
	return Py_BuildValue("");
}
static PyObject* pyScene_Preprocess(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	self->scene->Preprocess();
	return Py_BuildValue("");
}
static PyObject* pyScene_UpdateSettings(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	self->scene->UpdateSettings();
	return Py_BuildValue("");
}
static PyObject* pyScene_BeginScene(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	self->scene->BeginScene();
	return Py_BuildValue("");
}
static PyObject* pyScene_EndScene(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	self->scene->EndScene();
	return Py_BuildValue("");
}
static PyObject* pyScene_HasMissingSources(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	if (self->scene->HasMissingSources()){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject* pyScene_NumSceneItems(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}
	return PyLong_FromLong(self->scene->NumSceneItems());
}
static PyObject* pyScene_GetSceneItemByName(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}


	if (PyTuple_Size(args) != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	
	PyUnicodeObject *name;

	if (!PyArg_ParseTuple(args, "U", &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);	

	if (wname == NULL){
		return NULL;
	}

	SceneItem *ret = self->scene->GetSceneItem(wname);
	delete[] wname;

	if (!ret){
		Py_RETURN_NONE;
	}
	PyObject *pyRet = PyObject_CallObject((PyObject*)&pySceneItemType, NULL);
	((PySceneItem*)pyRet)->sceneItem = ret;


	return pyRet;	
}
static PyObject* pyScene_GetSceneItemByID(PyScene *self, PyObject *args){
	if (!sceneExists(self)){
		return NULL;
	}

	if (PyTuple_Size(args) != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	UINT id;

	if (!PyArg_ParseTuple(args, "I", &id)){
		return NULL;
	}
	
	//check ID bounds
	if (!(id >= 0 && id < self->scene->NumSceneItems())){
		Py_RETURN_NONE;
	}
	SceneItem *ret = self->scene->GetSceneItem(id);
	
	if (!ret){
		Py_RETURN_NONE;
	}

	PyObject *pyRet = PyObject_CallObject((PyObject*)&pySceneItemType, NULL);
	((PySceneItem*)pyRet)->sceneItem = ret;


	return pyRet;
}

/*Method Table*/
static PyMethodDef pyScene_methods[] = {
		{ "InsertImageSource", (PyCFunction)pyScene_InsertImageSource, METH_VARARGS, "InsertImageSource" },
		{ "AddImageSource", (PyCFunction)pyScene_AddImageSource, METH_VARARGS, "AddImageSource" },
		{ "RemoveImageSource", (PyCFunction)pyScene_RemoveImageSource, METH_VARARGS, "RemoveImageSource" },
		{ "Tick", (PyCFunction)pyScene_Tick, METH_VARARGS, "Tick" },
		{ "Render", (PyCFunction)pyScene_Render, METH_VARARGS, "Render" },
		{ "Preprocess", (PyCFunction)pyScene_Preprocess, METH_VARARGS, "Preprocess" },
		{ "UpdateSettings", (PyCFunction)pyScene_UpdateSettings, METH_VARARGS, "UpdateSettings" },
		{ "BeginScene", (PyCFunction)pyScene_BeginScene, METH_VARARGS, "BeginScene" },
		{ "EndScene", (PyCFunction)pyScene_EndScene, METH_VARARGS, "EndScene" },
		{ "HasMissingSources", (PyCFunction)pyScene_HasMissingSources, METH_VARARGS, "HasMissingSources" },
		{ "NumSceneItems", (PyCFunction)pyScene_NumSceneItems, METH_VARARGS, "NumSceneItems" },
		{ "GetSceneItemByName", (PyCFunction)pyScene_GetSceneItemByName, METH_VARARGS, "GetSceneItemByName" },
		{ "GetSceneItemByID", (PyCFunction)pyScene_GetSceneItemByID, METH_VARARGS, "GetSceneItemByID" },
		{ NULL }  /* Sentinel */
};

/*Member table*/
static PyMemberDef pyScene_members[] = {
		{ NULL }  /* Sentinel */
};



/*Python Type Object */
static PyTypeObject pySceneType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"OBS.Scene",         /*tp_name*/
	sizeof(PyScene),     /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)pyScene_dealloc, /*tp_dealloc*/
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
	pyScene_methods,             /* tp_methods */
	pyScene_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)pyScene_init,      /* tp_init */
	0,                         /* tp_alloc */
	pyScene_new,                 /* tp_new */
};