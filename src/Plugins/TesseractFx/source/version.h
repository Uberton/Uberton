
// Plugin name and version of TesseractFx
//
// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the Überton project. Copyright (C) 2021 Überton
//
// Überton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Überton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with Überton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#pragma once

#include <pluginterfaces/base/fplatform.h>
#include <uberton.h>

#define MAJOR_VERSION_STR "1"
#define MAJOR_VERSION_INT 1

#define MINOR_VERSION_STR "0"
#define MINOR_VERSION_INT 0

#define PATCH_NUMBER_STR "0"
#define PATCH_NUMBER_INT 0

#define BUILD_NUMBER_STR "1"
#define BUILD_NUMBER_INT 1

#define FULL_VERSION_STR MAJOR_VERSION_STR "." MINOR_VERSION_STR "." PATCH_NUMBER_STR "." BUILD_NUMBER_STR

#define VERSION_STR MAJOR_VERSION_STR "." MINOR_VERSION_STR "." PATCH_NUMBER_STR

#define stringPluginName "TesseractFx"

#define stringOriginalFilename "TesseractFx.vst3"
#if SMTG_PLATFORM_64
#define stringFileDescription stringPluginName " (64Bit)"
#else
#define stringFileDescription stringPluginName
#endif
