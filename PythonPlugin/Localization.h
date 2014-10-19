#pragma once

#include "OBSApi.h"

#define STR_PREFIX L"Plugins.PythonSource."
#define KEY(k) (STR_PREFIX L ## k)
#define STR(text) locale->LookupString(KEY(text))

#ifndef VSP_VERSION
#define VSP_VERSION L"! INTERNAL TESTING !"
#endif


static CTSTR localizationStrings[] = {
	KEY("PluginName"), L"Python Source",
	KEY("PluginDescription"), L"Runs Python Scripts\n\n"
	L"Plugin Version: " VSP_VERSION,
	KEY("ClassName"), L"Python",
};