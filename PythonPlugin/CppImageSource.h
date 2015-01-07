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
#include "utils.h"



/*This Header contains A python module*/



enum ColourFormat{
ALPHA,
GRAYSCALE,
RGB,
RGBA,
BGR,
BGRA,
};


typedef std::map <unsigned long, PyObject* > hotkey_map;
class CppImageSource : public ImageSource{


private:
	
	Vect2 imageSize;
	Texture *texture;
	XElement *data;
	Texture* MakeTexture();
	GSColorFormat texFormat = GS_BGRA;
	GSImageFormat imgFormat = GS_IMAGEFORMAT_BGRA;
	int imgDepth = 4;
	
	

	

public:
	
	CppImageSource(XElement *data);
	~CppImageSource();
	Vect2 renderSize;
	hotkey_map hotkeyToCallable;
	PyObject *pyImgSrc = NULL;
	Texture *GetTexture() { return texture; }
	void ClearTexture(void* pxdata);
	void ColourArea(void *pxdata);
	
	void CallPythonFunction(char* funcName);

	void* getBackBuffer();
	void* getFrontBuffer();
	void flipPixelBuffers();

	bool canRender(); //Check that pointers are not null.
	bool isDoubleBuffer;

	void* pixelA;
	void* pixelB;
	void* pixelFront; // This is the buffer currently active on screen.
	Vect2 pos;
	Vect2 size;

	int getBytesPerPixel(){ return imgDepth; };
	GSImageFormat getImageFormat(){ return imgFormat; };
	void setupDoubleBuffers(void *A, void *b, long width, long height);
	void setupSingleBuffer(void* A, long width, long height);
	bool setupFormats(char *format);
	

	String getSourceName();
	String getSceneName();


public:
	/*Setters*/
	void setWidth(int x);
	void setHeight(int y);
	void setImageFormat(GSImageFormat format);

	/*Locks*/
	void lockTexture();
	void unlockTexture();

	/*Getters*/
	int getWidth();
	int getHeight();
	PyObject *getImageSource(){ return pyImgSrc; }



	/*Config*/
	void loadConfig();
	void saveConfig();


public:
	/*Image Source Overloads*/
	void Tick(float fSeconds);
	void Render(const Vect2 &pos, const Vect2 &size);
	void UpdateSettings();

	void BeginScene();
	void EndScene();

	
	void GlobalSourceLeaveScene();
	void GlobalSourceEnterScene();
	

	void ChangeScene();


	Vect2 GetSize() const;

		
};



static void STDCALL Hotkey(DWORD key, UPARAM *userData, bool isDown){

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();


	//userData is a pointer to a python callable function
	PyObject *pyFunc = (PyObject *)userData;
	if (pyFunc == NULL || !PyCallable_Check(pyFunc)|| pyFunc->ob_refcnt == 0){
		Log(TEXT("Hotkey callback is not callable"));
		PyGILState_Release(gstate);
		return;
	}
	PyObject *pyIsDown;
	if (isDown){
		pyIsDown = Py_True;
	}
	else{
		pyIsDown = Py_False;
	}

	PyObject *pyKey = PyLong_FromUnsignedLong(key);

	PyObject *argList = Py_BuildValue("(OO)", pyKey, pyIsDown);
	PyObject *result = PyObject_CallObject(pyFunc, argList);

	

	Py_XDECREF(result);
	Py_XDECREF(argList);
	Py_XDECREF(pyKey);
	pyHasError();
	PyGILState_Release(gstate);
	return;
}