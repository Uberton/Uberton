// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "locations.h"
#ifdef _WIN32
//#include <Windows.h>
#include <ShlObj.h>
#else
#include <cstdlib>
#endif

namespace Uberton {
namespace Installer {
std::string getVSTLocation() {
#ifdef _WIN32
	char path[MAX_PATH];
	SHGetSpecialFolderPathA(0, path, CSIDL_PROGRAM_FILES, FALSE);
	std::string pathS{ path };
	return pathS + "/Common Files/VST3/";
#elif __APPLE__
	char* homeDir = getenv("HOME");
	if (homeDir == nullptr) return "";
	return std::string(homeDir) + "/Library/Audio/Plug-Ins/VST3/";
#elif __gnu_linux__ || __linux__
	char* homeDir = getenv("HOME");
	if (homeDir == nullptr) return "";
	return std::string(homeDir) + "/.vst3/";
#endif
}

std::string getFactoryPresetLocation() {
#ifdef _WIN32
	char path[MAX_PATH];
	SHGetSpecialFolderPathA(0, path, CSIDL_COMMON_APPDATA, FALSE);
	std::string pathS{ path };
	return pathS + "/VST3 Presets/Uberton/";
#elif __APPLE__
	char* homeDir = getenv("HOME");
	if (homeDir == nullptr) return "";
	return "Library/Audio/Presets/Uberton/";
#elif __gnu_linux__ || __linux__
	return "/usr/local/share/vst3/presets/Uberton/";
#endif
}

std::string getUserguideLocation() {
#ifdef _WIN32
	char path[MAX_PATH];
	SHGetSpecialFolderPathA(0, path, CSIDL_PROGRAM_FILES, FALSE);
	std::string pathS{ path };
	return pathS + "/Uberton/";
#elif __APPLE__
	return "Applications/Uberton/";
#endif
}
}
}
