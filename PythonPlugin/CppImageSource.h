#pragma once

#include <Python.h>
#include "OBSApi.h"



/*This Header contains A python module*/



enum ColourFormat{
ALPHA,
GRAYSCALE,
RGB,
RGBA,
BGR,
BGRA,
};

class CppImageSource : public ImageSource{


private:
	
	Vect2 imageSize;
	Texture *texture;


	Texture* MakeTexture();
	GSColorFormat texFormat = GS_BGRA;
	GSImageFormat imgFormat = GS_IMAGEFORMAT_BGRA;

	

public:
	CppImageSource(XElement *data);
	~CppImageSource();
	PyObject *pyImgSrc = NULL;
	PyObject *pyRender = NULL;
	Texture *GetTexture() { return texture; }
	void ClearTexture(void* pxdata);
	void ColourArea(void *pxdata);

	void getImageFromPython();

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

	void setupDoubleBuffers(void *A, void *b);
	void setupSingleBuffer(void* A, char *);
	bool setupFormats(char *format);


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
	//void Tick(float fSeconds);
	void Render(const Vect2 &pos, const Vect2 &size);
	/*
	void GlobalSourceLeaveScene();
	void GlobalSourceEnterScene();

	void ChangeScene();
	void UpdateSettings();*/
	Vect2 GetSize() const;
	
};



