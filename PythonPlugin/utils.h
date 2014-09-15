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


