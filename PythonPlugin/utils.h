
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
	else{
		//Not a string
		return NULL;
	}
}





static PyObject * CTSTRtoPyUnicode(const wchar_t *ctstr){
	if (ctstr == NULL){
		return Py_BuildValue("");
	}
	return PyUnicode_FromWideChar(ctstr, wcslen(ctstr));
}

static bool isNULL(void * obj){
	if (obj == NULL){
		PyErr_SetString(PyExc_RuntimeWarning, "XElement element in NULL(probably deleted)");
		return true;
	}
	else{
		return false;
	}


}


static bool pyHasError(){
	bool ret = false;

	if (PyErr_Occurred()){

		String path = OBSGetPluginDataPath();
		path.FindReplace(TEXT("\\"), TEXT("/"));
		path = path + String("/Python/stdErr.txt");

		//Don't want to fill obs logs with lots of errors
		Log(TEXT("PYTHON ERROR: Check log %ls"),path);
		PyErr_Print();
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
	//Log(TEXT("%ws"), dir);

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


static void PythonRunString(String toRun){

	char *cRun = new char[toRun.DataLength()];
	wcstombs(cRun, TSTR(toRun), toRun.DataLength());
	
	PyRun_SimpleString(cRun);
	delete[]cRun;
}



static bool isPythonInPath(){
	char* pypath = getenv("PYTHONPATH");
	if (pypath == NULL){
		Log(TEXT("Python Plugin - Did Not find `PYTHONPATH` environment variable. Will search in `PATH`"));
	}
	char* path = getenv("PATH");
	if (path){
		char *python = strstr(path, "Python27");
		if (python){
			return true;
		}
		else{
			AppWarning(TEXT("Python Plugin - Could not find 'Python27' in PATH environment variable."));
		}
	}
	else{
		Log(TEXT("Python Plugin - Could not get PATH environment variable."));
	}
	return false;
}