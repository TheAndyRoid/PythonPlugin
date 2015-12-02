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
#include "PyVect2.h"
#include "PyVect4.h"
#include "utils.h"



/*Object Data*/
typedef struct{
	PyObject_HEAD
		SceneItem *sceneItem;
}PySceneItem;



static bool sceneItemExists(PySceneItem *self){
	if (self->sceneItem == NULL){
		PyErr_SetString(PyExc_TypeError, "SceneItems is NULL");
		return false;
	}
	else{ 
		return true; 
	}
}



static void
pySceneItem_dealloc(PySceneItem* self)
{
	self->sceneItem = NULL;
	Py_TYPE(self)->tp_free((PyObject*)self);

}


static PyObject * pySceneItem_new(PyTypeObject *type, PyObject *args, PyObject *kwds){

	PySceneItem *self;
	self = (PySceneItem *)type->tp_alloc(type, 0);
	if (self == NULL) {
		return NULL;
	}
	self->sceneItem = NULL;

	return (PyObject *)self;
}


static int
pySceneItem_init(PySceneItem *self, PyObject *args, PyObject *kwds)
{
	/*Do nothing for now*/
	return 0;
}


static PyObject* pySceneItem_GetName(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	return  CTSTRtoPyUnicode(self->sceneItem->GetName());		
}
static PyObject* pySceneItem_GetSource(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	CppImageSource *cImgSrc = dynamic_cast<CppImageSource*>(self->sceneItem->GetSource());
	if (cImgSrc != NULL){
		//This image source is controlled by python 
		Py_INCREF(cImgSrc->pyImgSrc);
		return cImgSrc->pyImgSrc;
	}
	else{
		//This is a Imagesource of another we can't interact with it in python. 
		Py_RETURN_NONE;
	}	
}
static PyObject* pySceneItem_GetElement(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	PyObject *pyElement = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyElement)->element = self->sceneItem->GetElement();
	return pyElement;
}
static PyObject* pySceneItem_GetPos(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	Vect2	pos = self->sceneItem->GetPos();

	PyObject *argList = Py_BuildValue("(dd)", pos.x, pos.y);
	PyObject *pyPos = PyObject_CallObject((PyObject*)&PyVect2_Object, argList);
	Py_DECREF(argList);

	return  pyPos;
}
static PyObject* pySceneItem_GetSize(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	Vect2	pos = self->sceneItem->GetSize();

	PyObject *argList = Py_BuildValue("(dd)", pos.x, pos.y);
	PyObject *pyPos = PyObject_CallObject((PyObject*)&PyVect2_Object, argList);
	Py_DECREF(argList);

	return  pyPos;
}
static PyObject* pySceneItem_GetScale(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	Vect2	pos = self->sceneItem->GetScale();

	PyObject *argList = Py_BuildValue("(dd)", pos.x, pos.y);
	PyObject *pyPos = PyObject_CallObject((PyObject*)&PyVect2_Object, argList);
	Py_DECREF(argList);

	return  pyPos;
}
static PyObject* pySceneItem_IsCropped(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	if (self->sceneItem->IsCropped()){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject* pySceneItem_Select(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	if (PyTuple_Size(args) != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	PyObject *pyBool;

	if (!PyArg_ParseTuple(args, "O", &pyBool)){
		return NULL;
	}
	

	if (PyObject_IsTrue(pyBool)){
		self->sceneItem->Select(true);
	}
	else{
		self->sceneItem->Select(false);
	}
	return Py_BuildValue("");
}
static PyObject* pySceneItem_IsSelected(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	if (self->sceneItem->IsSelected()){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject* pySceneItem_GetID(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	UINT id = self->sceneItem->GetID();
	return PyLong_FromLong(id);
}
static PyObject* pySceneItem_SetName(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
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

	self->sceneItem->SetName(wname);
	delete[] wname;
	return Py_BuildValue("");
}
static PyObject* pySceneItem_SetRender(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	if (PyTuple_Size(args) != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	PyObject *pyBool;

	if (!PyArg_ParseTuple(args, "O", &pyBool)){
		return NULL;
	}


	if (PyObject_IsTrue(pyBool)){
		self->sceneItem->SetRender(true);
	}
	else{
		self->sceneItem->SetRender(false);
	}
	return Py_BuildValue("");
}
static PyObject* pySceneItem_GetCrop(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	Vect4	crop = self->sceneItem->GetCrop();

	PyObject *argList = Py_BuildValue("(dddd)", crop.x, crop.y, crop.z,crop.w);
	PyObject *pyCrop = PyObject_CallObject((PyObject*)&PyVect4_Object, argList);
	Py_DECREF(argList);

	return  pyCrop;
}
static PyObject* pySceneItem_Update(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}
	
	self->sceneItem->Update();
	return Py_BuildValue("");
}

static PyObject* pySceneItem_UpdateSettings(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	self->sceneItem->GetSource()->UpdateSettings();
	return Py_BuildValue("");
}

static PyObject* pySceneItem_MoveUp(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	self->sceneItem->MoveUp();
	return Py_BuildValue("");
}
static PyObject* pySceneItem_MoveDown(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	self->sceneItem->MoveDown();
	return Py_BuildValue("");
}
static PyObject* pySceneItem_MoveToTop(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	self->sceneItem->MoveToTop();
	return Py_BuildValue("");
}
static PyObject* pySceneItem_MoveToBottom(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	self->sceneItem->MoveToBottom();
	return Py_BuildValue("");
}

static PyObject* pySceneItem_SetPos(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	if (PyTuple_Size(args) != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	float x, y;
	if (!PyArg_ParseTuple(args, "ff", &x,&y)){
		return NULL;
	}
	Vect2 pos(x, y);
	self->sceneItem->SetPos(pos);

	
	return Py_BuildValue("");
}


static PyObject* pySceneItem_SetSize(PySceneItem *self, PyObject *args){
	if (!sceneItemExists(self)){
		return NULL;
	}

	if (PyTuple_Size(args) != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}

	float x, y;
	if (!PyArg_ParseTuple(args, "ff", &x, &y)){
		return NULL;
	}
	Vect2 pos(x, y);
	self->sceneItem->SetSize(pos);
	
	return Py_BuildValue("");
}




/*Method Table*/
static PyMethodDef pySceneItem_methods[] = {
		{ "GetName", (PyCFunction)pySceneItem_GetName, METH_VARARGS, "GetName" },
		{ "GetSource", (PyCFunction)pySceneItem_GetSource, METH_VARARGS, "GetSource" },
		{ "GetElement", (PyCFunction)pySceneItem_GetElement, METH_VARARGS, "GetElement" },
		{ "GetPos", (PyCFunction)pySceneItem_GetPos, METH_VARARGS, "GetPos" },
		{ "GetSize", (PyCFunction)pySceneItem_GetSize, METH_VARARGS, "GetSize" },
		{ "SetPos", (PyCFunction)pySceneItem_SetPos, METH_VARARGS, "SetPos" },
		{ "SetSize", (PyCFunction)pySceneItem_SetSize, METH_VARARGS, "SetSize" },
		{ "GetScale", (PyCFunction)pySceneItem_GetScale, METH_VARARGS, "GetScale" },
		{ "IsCropped", (PyCFunction)pySceneItem_IsCropped, METH_VARARGS, "IsCropped" },
		{ "Select", (PyCFunction)pySceneItem_Select, METH_VARARGS, "Select" },
		{ "IsSelected", (PyCFunction)pySceneItem_IsSelected, METH_VARARGS, "IsSelected" },
		{ "GetID", (PyCFunction)pySceneItem_GetID, METH_VARARGS, "GetID" },
		{ "SetName", (PyCFunction)pySceneItem_SetName, METH_VARARGS, "SetName" },
		{ "SetRender", (PyCFunction)pySceneItem_SetRender, METH_VARARGS, "SetRender" },
		{ "GetCrop", (PyCFunction)pySceneItem_GetCrop, METH_VARARGS, "GetCrop" },
		{ "Update", (PyCFunction)pySceneItem_Update, METH_VARARGS, "Update" },
		{ "UpdateSettings", (PyCFunction)pySceneItem_UpdateSettings, METH_VARARGS, "UpdateSettings" },
		{ "MoveUp", (PyCFunction)pySceneItem_MoveUp, METH_VARARGS, "MoveUp" },
		{ "MoveDown", (PyCFunction)pySceneItem_MoveDown, METH_VARARGS, "MoveDown" },
		{ "MoveToTop", (PyCFunction)pySceneItem_MoveToTop, METH_VARARGS, "MoveToTop" },
		{ "MoveToBottom", (PyCFunction)pySceneItem_MoveToBottom, METH_VARARGS, "MoveToBottom" },
		{ NULL }  /* Sentinel */
};

/*Member table*/
static PyMemberDef pySceneItem_members[] = {
		{ NULL }  /* Sentinel */
};



/*Python Type Object */
static PyTypeObject pySceneItemType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"OBS.SceneItem",         /*tp_name*/
	sizeof(PySceneItem),     /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)pySceneItem_dealloc, /*tp_dealloc*/
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
	pySceneItem_methods,             /* tp_methods */
	pySceneItem_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)pySceneItem_init,      /* tp_init */
	0,                         /* tp_alloc */
	pySceneItem_new,                 /* tp_new */
};