
#include <Python.h>

#include "structmember.h"

#include "CppImageSource.h"
#include "PythonPlugin.h"
#include "PyImageSource.h"
#include "PyVect2.h"




CppImageSource::CppImageSource(XElement *data)
{
	Log(TEXT("Python Source Constructor"));

	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
	}

	//Set a basic image size
	imageSize = Vect2(1920, 1080);

	//MakeTexture();

	
	

	pyRender = NULL;
	pyImgSrc = NULL;
	
	
}

CppImageSource::~CppImageSource(){	

	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
	}

	/*
	DWORD dwWaitResult = WaitForSingleObject(
		pyPlug->ghMutex,    // handle to mutex
		INFINITE);  // no time-out interval
	*/

	
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();



	pyImageSource* tmp = (pyImageSource*)pyImgSrc;
	tmp->cppImageSource = NULL;

	PyObject *stop = tmp->stop;

	if (stop != NULL && PyCallable_Check(stop)){
		PyObject *result = PyObject_CallObject(stop, Py_BuildValue("()"));
	}






	Py_XDECREF(pyImgSrc);
	
	
	
	pyPlug->pImageSource = NULL;
	pyRender = NULL;
	pyImgSrc = NULL;
	Log(TEXT("Python Source Destructor"));
	
	
	
	//ReleaseMutex(pyPlug->ghMutex);


	

	if (texture) {
		delete texture;
		texture = nullptr;
	}

	PyGILState_Release(gstate);
	

}





void CppImageSource::Render(const Vect2 &pos, const Vect2 &size){
	


	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
		return;
	}


	if (!texture) {
		return;
		//texture = MakeTexture();
	}




	

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();


	if (pyRender == NULL){
		
		//Try to get the object
		if (pyImgSrc == NULL){
			Log(TEXT("No pyImageSource"));
			PyGILState_Release(gstate);

			return;
		}
		pyRender = ((pyImageSource*)pyImgSrc)->render;
		if (pyRender != NULL && PyCallable_Check(pyRender)){
			
		}else{
			
			pyRender = NULL;
			Log(TEXT("rendercallback not callable"));
			PyGILState_Release(gstate);
		
			return;
		}
	}
	

	Log(TEXT("Render"));

	//PyObject *obj = py_CreateVect2(size);
	PyObject * OBSModule = PyImport_ImportModule("OBS");
	if (OBSModule == NULL){
		Log(TEXT("OBSModule null"));
	}
	PyObject * vect2 = PyObject_GetAttr(OBSModule, PyString_FromString("Vect2"));
	if (vect2 == NULL){
		Log(TEXT("vect2 null"));
	}
	
	PyObject *argList = Py_BuildValue("(dd)", size.x, size.y);
	PyObject *pySize = PyObject_CallObject(vect2, argList);
	if (pySize == NULL){
		Log(TEXT("obj null"));
		
		PyGILState_Release(gstate);
	
		return;
	}


	Py_DECREF(argList);
	argList = Py_BuildValue("(dd)", pos.x, pos.y);
	//PyObject *pyPos = PyObject_CallObject((PyObject *)&PyVect2_Object, argList);
	PyObject *pyPos = PyObject_CallObject(vect2, argList);
	if (pySize == NULL){
		Log(TEXT("obj null"));
		PyGILState_Release(gstate);
		
		return;
	}
	Py_DECREF(argList);


	argList = Py_BuildValue("(OOO)", pyPos,pySize,pyImgSrc);
	PyObject *result = PyObject_CallObject(pyRender, argList);


	
	//Py_DECREF(argList);

	//PyObject *render = PyObject_GetAttr(pyImgSrc, PyString_FromString("render"));

	//PyObject *arglist = Py_BuildValue("");
	//PyObject *result = PyObject_CallObject(render, arglist);*/
	
	Py_XDECREF(OBSModule);
	Py_XDECREF(pySize);
	Py_XDECREF(pyPos);
	Py_XDECREF(vect2);
	Py_XDECREF(argList);
	Py_XDECREF(result);
	
	


	
	
	//ColourArea();
	
	if (texture){
		//getImageFromPython();
		if (getFrontBuffer() != NULL){
			texture->SetImage(getFrontBuffer(), imgFormat, texture->Width() * 4);
			//Draw the texture
			//Top left and bottum right coords
			DrawSprite(texture, 0xFFFFFFFF, pos.x, pos.y, pos.x + size.x, size.y + pos.y);
		}
		
	}

	PyGILState_Release(gstate);
	

	return;
}


void *CppImageSource::getBackBuffer(){
	if (pixelFront == pixelA && isDoubleBuffer == true){
		return pixelB;
	}else{
		return pixelA;
	}
	
}

void *CppImageSource::getFrontBuffer(){
	if (pixelFront == pixelA){
		return pixelA;
	}
	else{
		return pixelB;
	}

}

void CppImageSource::flipPixelBuffers(){
	if (pixelFront == pixelA && isDoubleBuffer == true){
		pixelFront = pixelB;
	}
	else{
		pixelFront = pixelA;
	}

}



void CppImageSource::getImageFromPython(){
	
	PythonPlugin *pyPlug = PythonPlugin::instance;
	if (pyPlug == NULL){
		Log(TEXT("Python instance Does not exist"));
		return;
	}

	/*
	DWORD dwWaitResult = WaitForSingleObject(
		pyPlug->ghMutex,    // handle to mutex
		1);  // time-out  in ms
	
	if (dwWaitResult == WAIT_TIMEOUT){
		return;
	}

	*/
	
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	//Try to get the object
	if (pyImgSrc == NULL){
		Log(TEXT("No pyImageSource"));
		PyGILState_Release(gstate);
		//ReleaseMutex(pyPlug->ghMutex);

		return;
	}
	PyObject *pyImageData = PyObject_GetAttr(pyImgSrc, PyString_FromString("data"));



	//int size = PyString_GET_SIZE(pyImageData);
	//char* data = NULL;

	// point to the address from ctypes ''feels hacky''
	//void* data = (void*)PyInt_AsUnsignedLongMask(pyImageData);
	//memcpy(pixelData, data, texture->Width() * 4 * texture->Height());
	//PyString_AsStringAndSize(pyImageData,&data,&size);

	
	
	Py_XDECREF(pyImageData);

	PyGILState_Release(gstate);

	
	

	


}





 Vect2 CppImageSource::GetSize() const{
	return  imageSize;
}

 int CppImageSource::getHeight(){
	 return imageSize.y;
 }
 int CppImageSource::getWidth(){
	 return imageSize.x;
 }


 void CppImageSource::setHeight(int y){

	 imageSize.y = y;

	//Need to update the texture.
	 texture = MakeTexture();
 }

 void CppImageSource::ColourArea(void* pxdata){
	 for (int i = 0; i < texture->Width() * 4 * texture->Height(); i += 4){
		 BYTE* data = ((UCHAR*)pxdata);
		 data[i + 0] = 0xff; //Red
		 data[i + 1] = 0xff; //Green
		 data[i + 2] = 0xff; //Blue
		 data[i + 3] = 0xff; //Alpha
	 }
 }


 void CppImageSource::setWidth(int x){
	//need to update the texture.
		
	 imageSize.x = x;

	 //Need to update the texture.
	 texture = MakeTexture();
 }


 Texture* CppImageSource::MakeTexture(){

	 texture = CreateTexture(imageSize.x, imageSize.y, texFormat, nullptr, FALSE, FALSE);
	 pixelFront = pixelA;
	 ClearTexture(pixelA);
	 ClearTexture(pixelB);
	 return texture;
 }
 

 void CppImageSource::ClearTexture(void *pxdata)
 {
	 BYTE *lpData;
	 UINT pitch;
	 if (texture) {
		 texture->Map(lpData, pitch);
		 memset(lpData, 0, pitch * texture->Height());
		 memset(pxdata, 0, (texture->Width() * 4) * texture->Height());
		 texture->Unmap();
	 }
 }

 void CppImageSource::setupDoubleBuffers(void *A, void *B){
	 this->pixelA = A;
	 this->pixelB = B;
	 this->pixelFront = this->pixelA;
	 this->isDoubleBuffer = true;
	 this->texture = MakeTexture();

 }



 void CppImageSource::setupSingleBuffer(void *A, char *){
	 this->pixelA = A;
	 this->pixelFront = this->pixelA;
	 this->isDoubleBuffer = false;

	 this->texture = MakeTexture();

 }




 bool CppImageSource::setupFormats(char* format){
	 if (strcmp(format, "ALPHA") == 0){
		 texFormat = GS_ALPHA;
		 imgFormat = GS_IMAGEFORMAT_A8;
	 }
	
	 else if (strcmp(format, "GRAYSCALE") == 0){
		 texFormat = GS_GRAYSCALE;
		 imgFormat = GS_IMAGEFORMAT_L8;
	 }
	 else if (strcmp(format, "RGB") == 0){

		 texFormat = GS_RGB;
		 imgFormat = GS_IMAGEFORMAT_RGB;
	 }
	 else if (strcmp(format, "RGBA") == 0){

		 texFormat = GS_RGBA;
		 imgFormat = GS_IMAGEFORMAT_RGBA;
	 }
	 else if (strcmp(format, "BGR") == 0){
		 texFormat = GS_BGR;
		 imgFormat = GS_IMAGEFORMAT_BGR;
	 }
	 else if (strcmp(format, "BGRA") == 0){
		 texFormat = GS_BGRA;
		 imgFormat = GS_IMAGEFORMAT_BGRA;
	 }
	 else{
		 return false;	 
	 }

	 return true;
 }

 
