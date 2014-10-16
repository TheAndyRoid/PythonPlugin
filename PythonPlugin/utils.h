
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
#include "OBSApi.h"



//Allocates memory that must be freed
static wchar_t * pyObjectToWSTR(PyObject *str){
	wchar_t *wstr = NULL;
		
	if (PyUnicode_Check(str)){
		Py_ssize_t len = PyUnicode_GET_DATA_SIZE(str);
		wstr = new  wchar_t[len];
		PyUnicode_AsWideChar((PyUnicodeObject*)str, wstr, len);
		return wstr;
	}
	else if (PyString_Check(str)){
		char * cstr = PyString_AsString(str);
		long len = PyString_Size(str) + 1;
		wstr = new  wchar_t[len];
		mbstowcs(&*wstr, cstr, len);
		return wstr;
	}
	
	return NULL;
}


static PyObject * CTSTRtoPyUnicode(const wchar_t *ctstr){
	if (ctstr == NULL){
		return Py_BuildValue("");
	}
	return PyUnicode_FromWideChar(ctstr, wcslen(ctstr));
}

static bool isNULL(void * obj){
	if (obj == NULL){
		PyErr_SetString(PyExc_TypeError, "Wrong number of arguments");
		return true;
	}
	else{
		return false;
	}


}

static char* getPythonTraceback()
{
	// Python equivilant:
	// import traceback, sys
	// return "".join(traceback.format_exception(sys.exc_type,
	//    sys.exc_value, sys.exc_traceback))

	PyObject *type, *value, *traceback;
	PyObject *tracebackModule;
	char *chrRetval;

	PyErr_Fetch(&type, &value, &traceback);

	tracebackModule = PyImport_ImportModule("traceback");
	if (tracebackModule != NULL)
	{
		PyObject *tbList, *emptyString, *strRetval;

		tbList = PyObject_CallMethod(
			tracebackModule,
			"format_exception",
			"OOO",
			type,
			value == NULL ? Py_None : value,
			traceback == NULL ? Py_None : traceback);

		emptyString = PyString_FromString("");
		strRetval = PyObject_CallMethod(emptyString, "join",
			"O", tbList);

		chrRetval = strdup(PyString_AsString(strRetval));

		Py_DECREF(tbList);
		Py_DECREF(emptyString);
		Py_DECREF(strRetval);
		Py_DECREF(tracebackModule);
	}
	else
	{
		chrRetval = strdup("Unable to import traceback module.");
	}

	Py_DECREF(type);
	Py_XDECREF(value);
	Py_XDECREF(traceback);

	return chrRetval;
}


static bool pyHasError(){
	bool ret = false;

	if (PyErr_Occurred()){

		char * cstr = getPythonTraceback();
		int len = strlen(cstr) + 1;
		wchar_t *wstr = new  wchar_t[len];
		mbstowcs(&*wstr, cstr, len);

		Log(TEXT("PYTHON ERROR:"));
		String str(wstr);
		for (int i = 0; i < str.NumTokens('\n'); i++){
			LogRaw(str.GetToken(i, '\n'));
		}
		delete[] wstr;
		/*
		PyErr_Print();
		String path = OBSGetAppPath();
		path.AppendString(String ("\\plugins\\Python\\pyErr.txt"));
		OSMessageBox(TEXT("PYTHON ERROR:\n CHECK DEFAULT LOG FILE: %ls"), path);
		*/
		ret = true;
	}

	return ret;
}



static String addToPythonPath(String cstr){
	//make sure that sys is imported
	PyRun_SimpleString("import sys");
	PyObject *path = PySys_GetObject("path");

	//convert to a constant seperator
	cstr = cstr.FindReplace(TEXT("\\"), TEXT("/"));
	String dir;
	for (int i = 0 ; i < (cstr.NumTokens('/') -1); i++){
		if (i != 0){
			dir.AppendString(TEXT("/"));
		}
		dir.AppendString(cstr.GetToken(i, '/'));
	}
	PyList_Append(path, CTSTRtoPyUnicode(dir));
	Log(TEXT("%ws"), dir);

	String moduleName = cstr.GetToken(cstr.NumTokens('/')-1, '/');
	moduleName = moduleName.GetToken(moduleName.NumTokens('.') - 2, '.');
	return moduleName;
}


static bool isPyObjectBaseClass(PyObject *obj, String *type ){
	

	PyObject *tuple = obj->ob_type->tp_bases;

	Py_ssize_t len = PyTuple_Size(tuple);
	for (int i = 0; i < len; i++){
		PyObject *base = PyTuple_GetItem(tuple, i);
		PyObject *name = PyObject_GetAttrString(base, "__name__");
		pyHasError();
		wchar_t *cname = NULL;
		if (name != NULL){
			 cname = pyObjectToWSTR(name);
		}
		if (cname != NULL ){
			if (wcscmp(cname, *type)== 0){
				delete[] cname;
				return true;
			}
			else{
				delete[] cname;
			}
					
		}
	}
	return false;


}
