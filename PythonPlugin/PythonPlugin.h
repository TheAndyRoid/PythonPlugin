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
#include "Localization.h"
#include "utils.h"
#include "CppImageSource.h"


#include <map>

	

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

	struct StringComp
	{
		bool operator()(const String& lhs, const String& rhs)
		const {
			return scmp(lhs,rhs)>0;
		}
	};

	typedef std::map<String, PyObject*, StringComp> shutdown_map;
	


class PythonPlugin 
{
public:
	static PythonPlugin *instance;

	void initPython();


	void addShutdownFunction(String filename, PyObject *function);
	void clearShutdownFunctions();

public:
	PythonPlugin();
	~PythonPlugin();
	bool ConfigGUIActive;
	 

	shutdown_map shutdownFunc;
	
	 
	CppImageSource *tmpImgSrc;

	
private:
	bool isDynamicLocale;

private:
	
};

EXTERN_DLL_EXPORT bool LoadPlugin();
EXTERN_DLL_EXPORT void UnloadPlugin();
EXTERN_DLL_EXPORT void OnStartStream();
EXTERN_DLL_EXPORT void OnStopStream();
EXTERN_DLL_EXPORT CTSTR GetPluginName();
EXTERN_DLL_EXPORT CTSTR GetPluginDescription();


