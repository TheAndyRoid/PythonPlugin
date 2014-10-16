
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





/*Wrapping this class for python is soul destroying*/




typedef struct {
	PyObject_HEAD
	XElement *element;
} PyXElement;


static void
PyXElement_dealloc(PyXElement* self)
{
	self->element = NULL;
	self->ob_type->tp_free((PyObject*)self);
}


static PyObject *
PyXElement_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyXElement *self;
	self = (PyXElement *)type->tp_alloc(type, 0);
	if (self == NULL) {
		return NULL;
	}
	self->element = NULL;

	return (PyObject *)self;
}


static int
PyXElement_init(PyXElement *self, PyObject *args, PyObject *kwds)
{
	//nothing to do here

	return 0;
}



//Forward Declarations
static PyObject *pyXElement_CopyElement(PyXElement *self, PyObject *args);
static PyObject *pyXElement_GetParent(PyXElement *self, PyObject *args);
static PyObject *pyXElement_GetElement(PyXElement *self, PyObject *args);
static PyObject *pyXElement_CreateElement(PyXElement *self, PyObject *args);
static PyObject *pyXElement_GetElementByItem(PyXElement *self, PyObject *args);
static PyObject *pyXElement_GetElementByID(PyXElement *self, PyObject *args);
static PyObject *pyXElement_RemoveElement(PyXElement *self, PyObject *args);
static PyObject *pyXElement_InsertElement(PyXElement *self, PyObject *args);
static PyObject *pyXElement_ReverseOrder(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 0){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}

	 self->element->ReverseOrder();
	 	 
	 return Py_BuildValue("");
}
static PyObject *pyXElement_HasItem(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *str;
	if (!PyArg_ParseTuple(args, "O", &str)){
		return NULL;
	}
	wchar_t *wstr = pyObjectToWSTR(str);

	if (wstr == NULL){
		return NULL;
	}

	if (self->element->HasItem(wstr)){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}

}
static PyObject *pyXElement_GetString(PyXElement *self, PyObject *args){


	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}	
	PyObject *str;
	if (!PyArg_ParseTuple(args, "O", &str)){
		return NULL;
	}
	wchar_t *wstr = pyObjectToWSTR(str);
		
	if (wstr == NULL){
		return NULL;
	}

	CTSTR ctstr = self->element->GetString(wstr);
	delete[] wstr;

	return CTSTRtoPyUnicode(ctstr);
}
static PyObject *pyXElement_GetInt(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *str;
	if (!PyArg_ParseTuple(args, "O", &str)){
		return NULL;
	}
	wchar_t *wstr = pyObjectToWSTR(str);

	if (wstr == NULL){
		return NULL;
	}

	int result = self->element->GetInt(wstr);
	delete[] wstr;

	return PyInt_FromLong(result);
}
static PyObject *pyXElement_GetFloat(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *str;
	if (!PyArg_ParseTuple(args, "O", &str)){
		return NULL;
	}
	wchar_t *wstr = pyObjectToWSTR(str);

	if (wstr == NULL){
		return NULL;
	}

	float result = self->element->GetFloat(wstr);
	delete[] wstr;

	return PyFloat_FromDouble(result);
}
static PyObject *pyXElement_GetColor(PyXElement *self, PyObject *args){
	return pyXElement_GetInt(self, args);
}
static PyObject *pyXElement_GetHex(PyXElement *self, PyObject *args){
	return pyXElement_GetInt(self, args);
}
static PyObject *pyXElement_SetString(PyXElement *self, PyObject *args){

	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name, *value;

	if (!PyArg_ParseTuple(args, "OO", &name, &value)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);
	wchar_t *wvalue = pyObjectToWSTR(value);

	if (wvalue == NULL || wname == NULL){
		return NULL;
	}

	self->element->SetString(wname, wvalue);
	delete[] wname;
	delete[] wvalue;

	return Py_BuildValue("");
}
static PyObject *pyXElement_SetInt(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;
	int value;

	if (!PyArg_ParseTuple(args, "Oi", &name, &value)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}

	self->element->SetInt(wname, value);
	delete[] wname;

	return Py_BuildValue("");
}
static PyObject *pyXElement_SetFloat(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;
	float value;

	if (!PyArg_ParseTuple(args, "Of", &name, &value)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}

	self->element->SetFloat(wname, value);
	delete[] wname;

	return Py_BuildValue("");
}
static PyObject *pyXElement_SetHex(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;
	DWORD value;

	if (!PyArg_ParseTuple(args, "Ok", &name, &value)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}

	self->element->SetHex(wname, value);
	delete[] wname;

	return Py_BuildValue("");
}
static PyObject *pyXElement_SetColor(PyXElement *self, PyObject *args){
	return pyXElement_SetHex(self, args);
}
static PyObject *pyXElement_RemoveItem(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;

	if (!PyArg_ParseTuple(args, "O", &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}

	self->element->RemoveItem(wname);
	delete[] wname;

	return Py_BuildValue("");
}
static PyObject *pyXElement_GetName(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 0){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}


	CTSTR ctstr = self->element->GetName();

	return CTSTRtoPyUnicode(ctstr);
}
static PyObject *pyXElement_SetName(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;

	if (!PyArg_ParseTuple(args, "O", &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}

	self->element->SetName(wname);
	delete[] wname;

	return Py_BuildValue("");
}
static PyObject *pyXElement_IsData(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 0){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}


	bool ret = self->element->IsData();
	if (ret == TRUE){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *pyXElement_IsElement(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 0){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}


	bool ret = self->element->IsData();
	if (ret == TRUE){
		Py_RETURN_TRUE;
	}
	else{
		Py_RETURN_FALSE;
	}
}
static PyObject *pyXElement_MoveUp(PyXElement *self, PyObject *args){
	self->element->MoveUp();
	return Py_BuildValue("");
}
static PyObject *pyXElement_MoveDown(PyXElement *self, PyObject *args){
	self->element->MoveDown();
	return Py_BuildValue("");
}
static PyObject *pyXElement_MoveToTop(PyXElement *self, PyObject *args){
	self->element->MoveToTop();
	return Py_BuildValue("");
}
static PyObject *pyXElement_MoveToBottom(PyXElement *self, PyObject *args){
	self->element->MoveToBottom();
	return Py_BuildValue("");
}
static PyObject *pyXElement_GetStringList(PyXElement *self, PyObject *args){

	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *str;
	if (!PyArg_ParseTuple(args, "O", &str)){
		return NULL;
	}
	wchar_t *wstr = pyObjectToWSTR(str);

	if (wstr == NULL){
		return NULL;
	}

	StringList clist;
	self->element->GetStringList(wstr,clist);
	delete[] wstr;

	PyObject *pylist = PyList_New(clist.Num());
	for (int i = 0; i < clist.Num(); i++){
		PyObject *item = CTSTRtoPyUnicode(clist[i]);
		PyList_Append(pylist, item);	
	}	
	return pylist;
}
static PyObject *pyXElement_GetIntList(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *str;
	if (!PyArg_ParseTuple(args, "O", &str)){
		return NULL;
	}
	wchar_t *wstr = pyObjectToWSTR(str);

	if (wstr == NULL){
		return NULL;
	}

	List<int> clist;
	self->element->GetIntList(wstr, clist);
	delete[] wstr;

	PyObject *pylist = PyList_New(clist.Num());
	for (int i = 0; i < clist.Num(); i++){
		PyObject *item = PyInt_FromLong(clist.GetElement(i));
		PyList_Append(pylist, item);
	}
	return pylist;
}
static PyObject *pyXElement_GetFloatList(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *str;
	if (!PyArg_ParseTuple(args, "O", &str)){
		return NULL;
	}
	wchar_t *wstr = pyObjectToWSTR(str);

	if (wstr == NULL){
		return NULL;
	}

	List<float> clist;
	//self->element->GetFloatList(wstr, clist);
	delete[] wstr;

	PyObject *pylist = PyList_New(clist.Num());
	for (int i = 0; i < clist.Num(); i++){
		PyObject *item = PyFloat_FromDouble(clist.GetElement(i));
		PyList_Append(pylist, item);
	}
	return pylist;
}
static PyObject *pyXElement_GetColourList(PyXElement *self, PyObject *args){
	return pyXElement_GetIntList(self, args);
}
static PyObject *pyXElement_GetHexList(PyXElement *self, PyObject *args){
	return pyXElement_GetIntList(self, args);
}
static PyObject *pyXElement_SetStringList(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name, *pylist;

	if (!PyArg_ParseTuple(args, "OO", &name, &pylist)){
		return NULL;
	}
	

	if (!PyList_Check(pylist)){
		PyErr_SetString(PyExc_TypeError, "Argument 2 must be a list");
		return NULL;
	}

	wchar_t *wname = pyObjectToWSTR(name);
	if (wname == NULL){
		return NULL;
	}

	StringList clist;
	for (int i = 0; i < PyList_Size(pylist); i++){
		PyObject *item = PyList_GetItem(pylist,i);
		if (item == NULL){
			delete[] wname;
			return NULL;
		}
		wchar_t *val = pyObjectToWSTR(item);
		if (val == NULL){
			PyErr_SetString(PyExc_TypeError, "Found non-String in list ignoring item");
			break;
		}
		clist.Add(val);	
		delete[] val;
	}


	self->element->SetStringList(wname, clist);
	delete[] wname;
	return Py_BuildValue("");
}
static PyObject *pyXElement_SetIntList(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name, *pylist;

	if (!PyArg_ParseTuple(args, "OO", &name, &pylist)){
		return NULL;
	}


	if (!PyList_Check(pylist)){
		PyErr_SetString(PyExc_TypeError, "Argument 2 must be a list");
		return NULL;
	}

	wchar_t *wname = pyObjectToWSTR(name);
	if (wname == NULL){
		return NULL;
	}

	List<int> clist;
	for (int i = 0; i < PyList_Size(pylist); i++){
		PyObject *item = PyList_GetItem(pylist, i);
		if (item == NULL){
			delete[] wname;
			return NULL;
		}
		if (!PyInt_Check(item)){
			PyErr_SetString(PyExc_TypeError, "Found non-int in list ignoring");
			break;
		}
		long val = PyInt_AsLong(item);		
		clist.Add(val);
		
	}


	self->element->SetIntList(wname, clist);
	delete[] wname;
	return Py_BuildValue("");
}
static PyObject *pyXElement_SetFloatList(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name, *pylist;

	if (!PyArg_ParseTuple(args, "OO", &name, &pylist)){
		return NULL;
	}


	if (!PyList_Check(pylist)){
		PyErr_SetString(PyExc_TypeError, "Argument 2 must be a list");
		return NULL;
	}

	wchar_t *wname = pyObjectToWSTR(name);
	if (wname == NULL){
		return NULL;
	}

	List<float> clist;
	for (int i = 0; i < PyList_Size(pylist); i++){
		PyObject *item = PyList_GetItem(pylist, i);
		if (item == NULL){
			delete[] wname;
			return NULL;
		}
		if (!PyFloat_Check(item)){
			PyErr_SetString(PyExc_TypeError, "Found non-float in list ignoring");
			break;
		}
		float val = PyFloat_AsDouble(item);
		clist.Add(val);
	}


	self->element->SetFloatList(wname, clist);
	delete[] wname;
	return Py_BuildValue("");
}
static PyObject *pyXElement_SetHexList(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 2){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name, *pylist;

	if (!PyArg_ParseTuple(args, "OO", &name, &pylist)){
		return NULL;
	}


	if (!PyList_Check(pylist)){
		PyErr_SetString(PyExc_TypeError, "Argument 2 must be a list");
		return NULL;
	}

	wchar_t *wname = pyObjectToWSTR(name);
	if (wname == NULL){
		return NULL;
	}

	List<DWORD> clist;
	for (int i = 0; i < PyList_Size(pylist); i++){
		PyObject *item = PyList_GetItem(pylist, i);
		if (item == NULL){
			delete[] wname;
			return NULL;
		}
		if (!PyLong_Check(item)){
			PyErr_SetString(PyExc_TypeError, "Found non-Long in list ignoring");
			break;
		}
		DWORD val = PyLong_AsUnsignedLong(item);
		clist.Add(val);

	}


	self->element->SetHexList(wname, clist);
	delete[] wname;
	return Py_BuildValue("");
}
static PyObject *pyXElement_SetColourList(PyXElement *self, PyObject *args){
	return pyXElement_SetHexList(self, args);
}

static PyObject *pyXElement_AddString(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddInt(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddFloat(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddHex(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddColor(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddStringList(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddIntList(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddFloatList(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddHexList(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_AddColorList(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}




static PyObject *pyXElement_GetBaseItem(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_GetBaseItemByID(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_GetDataItem(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_GetDataItemByID(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_NumElements(PyXElement *self, PyObject *args){
	return PyInt_FromLong(self->element->NumElements());
}
static PyObject *pyXElement_NumBaseItems(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_NumDataItems(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}



static PyObject *pyXElement_Import(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}
static PyObject *pyXElement_Export(PyXElement *self, PyObject *args){
	return Py_BuildValue("");
}






static PyMemberDef PyXElement_members[] = {
		{ NULL }  /* Sentinel */
};

static PyMethodDef PyXElement_methods[] = {
		{ "CopyElement", (PyCFunction)pyXElement_CopyElement, METH_VARARGS, "CopyElement" },
		{ "ReverseOrder", (PyCFunction)pyXElement_ReverseOrder, METH_VARARGS, "ReverseOrder" },
		{ "HasItem", (PyCFunction)pyXElement_HasItem, METH_VARARGS, "HasItem" },
		{ "GetString", (PyCFunction)pyXElement_GetString, METH_VARARGS, "GetString" },
		{ "GetInt", (PyCFunction)pyXElement_GetInt, METH_VARARGS, "GetInt" },
		{ "GetFloat", (PyCFunction)pyXElement_GetFloat, METH_VARARGS, "GetFloat" },
		{ "GetColor", (PyCFunction)pyXElement_GetColor, METH_VARARGS, "GetColor" },
		{ "GetHex", (PyCFunction)pyXElement_GetHex, METH_VARARGS, "GetHex" },
		{ "GetStringList", (PyCFunction)pyXElement_GetStringList, METH_VARARGS, "GetStringList" },
		{ "GetIntList", (PyCFunction)pyXElement_GetIntList, METH_VARARGS, "GetIntList" },
		{ "GetFloatList", (PyCFunction)pyXElement_GetFloatList, METH_VARARGS, "GetFloatList" },
		{ "GetColourList", (PyCFunction)pyXElement_GetColourList, METH_VARARGS, "GetColourList" },
		{ "GetHexList", (PyCFunction)pyXElement_GetHexList, METH_VARARGS, "GetHexList" },
		{ "SetString", (PyCFunction)pyXElement_SetString, METH_VARARGS, "SetString" },
		{ "SetInt", (PyCFunction)pyXElement_SetInt, METH_VARARGS, "SetInt" },
		{ "SetFloat", (PyCFunction)pyXElement_SetFloat, METH_VARARGS, "SetFloat" },
		{ "SetHex", (PyCFunction)pyXElement_SetHex, METH_VARARGS, "SetHex" },
		{ "SetColor", (PyCFunction)pyXElement_SetColor, METH_VARARGS, "SetColor" },
		{ "SetStringList", (PyCFunction)pyXElement_SetStringList, METH_VARARGS, "SetStringList" },
		{ "SetIntList", (PyCFunction)pyXElement_SetIntList, METH_VARARGS, "SetIntList" },
		{ "SetFloatList", (PyCFunction)pyXElement_SetFloatList, METH_VARARGS, "SetFloatList" },
		{ "SetColourList", (PyCFunction)pyXElement_SetColourList, METH_VARARGS, "SetColourList" },
		{ "SetHexList", (PyCFunction)pyXElement_SetHexList, METH_VARARGS, "SetHexList" },
		{ "AddString", (PyCFunction)pyXElement_AddString, METH_VARARGS, "AddString" },
		{ "AddInt", (PyCFunction)pyXElement_AddInt, METH_VARARGS, "AddInt" },
		{ "AddFloat", (PyCFunction)pyXElement_AddFloat, METH_VARARGS, "AddFloat" },
		{ "AddHex", (PyCFunction)pyXElement_AddHex, METH_VARARGS, "AddHex" },
		{ "AddColor", (PyCFunction)pyXElement_AddColor, METH_VARARGS, "AddColor" },
		{ "AddStringList", (PyCFunction)pyXElement_AddStringList, METH_VARARGS, "AddStringList" },
		{ "AddIntList", (PyCFunction)pyXElement_AddIntList, METH_VARARGS, "AddIntList" },
		{ "AddFloatList", (PyCFunction)pyXElement_AddFloatList, METH_VARARGS, "AddFloatList" },
		{ "AddHexList", (PyCFunction)pyXElement_AddHexList, METH_VARARGS, "AddHexList" },
		{ "AddColorList", (PyCFunction)pyXElement_AddColorList, METH_VARARGS, "AddColorList" },
		{ "RemoveItem", (PyCFunction)pyXElement_RemoveItem, METH_VARARGS, "RemoveItem" },
		{ "GetElement", (PyCFunction)pyXElement_GetElement, METH_VARARGS, "GetElement" },
		{ "GetElementByID", (PyCFunction)pyXElement_GetElementByID, METH_VARARGS, "GetElementByID" },
		{ "GetElementByItem", (PyCFunction)pyXElement_GetElementByItem, METH_VARARGS, "GetElementByItem" },
		{ "CreateElement", (PyCFunction)pyXElement_CreateElement, METH_VARARGS, "CreateElement" },
		{ "InsertElement", (PyCFunction)pyXElement_InsertElement, METH_VARARGS, "InsertElement" },
		{ "CopyElement", (PyCFunction)pyXElement_CopyElement, METH_VARARGS, "CopyElement" },
		{ "RemoveElement", (PyCFunction)pyXElement_RemoveElement, METH_VARARGS, "RemoveElement" },
		{ "GetParent", (PyCFunction)pyXElement_GetParent, METH_VARARGS, "GetParent" },
		{ "GetBaseItem", (PyCFunction)pyXElement_GetBaseItem, METH_VARARGS, "GetBaseItem" },
		{ "GetBaseItemByID", (PyCFunction)pyXElement_GetBaseItemByID, METH_VARARGS, "GetBaseItemByID" },
		{ "GetDataItem", (PyCFunction)pyXElement_GetDataItem, METH_VARARGS, "GetDataItem" },
		{ "GetDataItemByID", (PyCFunction)pyXElement_GetDataItemByID, METH_VARARGS, "GetDataItemByID" },
		{ "NumElements", (PyCFunction)pyXElement_NumElements, METH_VARARGS, "NumElements" },
		{ "NumBaseItems", (PyCFunction)pyXElement_NumBaseItems, METH_VARARGS, "NumBaseItems" },
		{ "NumDataItems", (PyCFunction)pyXElement_NumDataItems, METH_VARARGS, "NumDataItems" },
		{ "MoveUp", (PyCFunction)pyXElement_MoveUp, METH_VARARGS, "MoveUp" },
		{ "MoveDown", (PyCFunction)pyXElement_MoveDown, METH_VARARGS, "MoveDown" },
		{ "MoveToTop", (PyCFunction)pyXElement_MoveToTop, METH_VARARGS, "MoveToTop" },
		{ "MoveToBottom", (PyCFunction)pyXElement_MoveToBottom, METH_VARARGS, "MoveToBottom" },
		{ "Import", (PyCFunction)pyXElement_Import, METH_VARARGS, "Import" },
		{ "Export", (PyCFunction)pyXElement_Export, METH_VARARGS, "Export" },
		{ "GetName", (PyCFunction)pyXElement_GetName, METH_VARARGS, "GetName" },
		{ "SetName", (PyCFunction)pyXElement_SetName, METH_VARARGS, "SetName" },
		{ "IsData", (PyCFunction)pyXElement_IsData, METH_VARARGS, "IsData" },
		{ "IsElement", (PyCFunction)pyXElement_IsElement, METH_VARARGS, "IsElement" },
		{ NULL }  /* Sentinel */
};

static PyTypeObject PyXElement_Object = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"OBS.XElement",             /*tp_name*/
	sizeof(PyXElement),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)PyXElement_dealloc, /*tp_dealloc*/
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
	"XElement",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	PyXElement_methods,             /* tp_methods */
	PyXElement_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)PyXElement_init,      /* tp_init */
	0,                         /* tp_alloc */
	PyXElement_new,                 /* tp_new */
};


static PyObject *pyXElement_GetParent(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 0){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}

	XElement* res = self->element->GetParent();


	PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyConfig)->element = res;

	return pyConfig;
}
static PyObject *pyXElement_GetElement(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;

	if (!PyArg_ParseTuple(args, "O", &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}
	XElement* res = self->element->GetElement(wname);
	delete[] wname;


	PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyConfig)->element = res;

	return pyConfig;
}
static PyObject *pyXElement_CopyElement(PyXElement *self, PyObject *args){

	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;

	if (!PyArg_ParseTuple(args, "O", &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}
	XElement* res = self->element->CopyElement(self->element,wname);
	delete[] wname;


	PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyConfig)->element = res;

	return pyConfig;
}
static PyObject *pyXElement_CreateElement(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;

	if (!PyArg_ParseTuple(args, "O", &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}
	XElement* res = self->element->CreateElement(wname);
	delete[] wname;


	PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyConfig)->element = res;

	return pyConfig;
}
static PyObject *pyXElement_GetElementByItem(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name,*itemName,*itemValue;

	if (!PyArg_ParseTuple(args, "OOO", &name,&itemName,&itemValue)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);
	wchar_t *witemName = pyObjectToWSTR(itemName);
	wchar_t *witemValue = pyObjectToWSTR(itemValue);

	if (wname == NULL){
		return NULL;
	}
	XElement* res = self->element->GetElementByItem(wname,witemName,witemValue);
	delete[] wname;
	delete[] witemName;
	delete[] witemValue;


	PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyConfig)->element = res;

	return pyConfig;
}
static PyObject *pyXElement_GetElementByID(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	DWORD elementID;

	if (!PyArg_ParseTuple(args, "k", &elementID)){
		return NULL;
	}
	XElement* res = self->element->GetElementByID(elementID);

	PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyConfig)->element = res;

	return pyConfig;
}
static PyObject *pyXElement_RemoveElement(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *obj;

	if (!PyArg_ParseTuple(args, "O", &obj)){
		return NULL;
	}
	wchar_t *wname = NULL;
	if (PyString_Check(obj) || PyUnicode_Check(obj)){
		//String type object
		wname = pyObjectToWSTR(obj);
		if (wname == NULL){
			return NULL;
		}
		self->element->RemoveElement(wname);
		delete[] wname;
	}
	else if (strcmp(obj->ob_type->tp_name, "OBS.XElement") == 0){
		//element
		
		PyXElement *element = (PyXElement *)obj;
		self->element->RemoveElement(element->element);
	}
	else{
		PyErr_SetString(PyExc_TypeError, "Wrong type of argument");
		return NULL;
	}
	return Py_BuildValue("");
}
static PyObject *pyXElement_InsertElement(PyXElement *self, PyObject *args){
	long argLength = PyTuple_Size(args);
	if (argLength != 1){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return NULL;
	}
	if (isNULL(self->element)){
		return NULL;
	}
	PyObject *name;
	UINT pos;

	if (!PyArg_ParseTuple(args, "IO",&pos, &name)){
		return NULL;
	}
	wchar_t *wname = pyObjectToWSTR(name);

	if (wname == NULL){
		return NULL;
	}
	XElement* res = self->element->InsertElement(pos,wname);
	delete[] wname;


	PyObject *pyConfig = PyObject_CallObject((PyObject*)&PyXElement_Object, NULL);
	((PyXElement*)pyConfig)->element = res;

	return pyConfig;
}