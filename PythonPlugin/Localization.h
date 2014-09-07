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
	KEY("Settings"), L"Video Source Settings",
	KEY("PathOrUrl"), L"Enter the path of the video file you would like to load:",
	KEY("Video"), L"Video Settings",
	KEY("VideoWidth"), L"Video Width:",
	KEY("VideoHeight"), L"Video Height:",
	KEY("StretchImage"), L"Stretch image",
	KEY("Audio"), L"Audio Settings",
	KEY("Volume"), L"Video Volume (0-100)",
	KEY("AudioOutputToStream"), L"Output audio to stream",
	KEY("AudioOutputToDevice"), L"Output audio to device",
	KEY("AudioOutputType"), L"Audio Module",
	KEY("AudioOutputDevice"), L"Audio Device",
	KEY("MediaFileOrUrl"), L"Enter a file path or a URL pointing to a media object",
	KEY("Playlist"), L"Playlist Settings",
	KEY("PlaylistLoop"), L"Playlist looping",
	KEY("PlaylistEditor"), L"Playlist Editor",
	KEY("Deinterlacing"), L"Deinterlacing:",
	KEY("ApplyVideoFilter"), L"Apply Video Filters",
	KEY("VideoFilter"), L"Video Filters",
	KEY("VideoGamma"), L"Gamma",
	KEY("VideoContrast"), L"Contrast",
	KEY("VideoBrightness"), L"Brightness",
	KEY("ResetVideoFilter"), L"Reset Filters"
};