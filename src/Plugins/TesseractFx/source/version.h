
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

#define TESSERACTFX_MAJOR_VERSION_STR "1"
#define TESSERACTFX_MAJOR_VERSION_INT 1

#define TESSERACTFX_MINOR_VERSION_STR "0"
#define TESSERACTFX_MINOR_VERSION_INT 0

#define TESSERACTFX_PATCH_NUMBER_STR "1"
#define TESSERACTFX_PATCH_NUMBER_INT 1

#define TESSERACTFX_BUILD_NUMBER_STR "1"
#define TESSERACTFX_BUILD_NUMBER_INT 1

#define TESSERACTFX_FULL_VERSION_STR TESSERACTFX_MAJOR_VERSION_STR "." TESSERACTFX_MINOR_VERSION_STR "." TESSERACTFX_PATCH_NUMBER_STR "." TESSERACTFX_BUILD_NUMBER_STR

#define TESSERACTFX_VERSION_STR TESSERACTFX_MAJOR_VERSION_STR "." TESSERACTFX_MINOR_VERSION_STR "." TESSERACTFX_PATCH_NUMBER_STR

#define TESSERACTFX_stringPluginName "TesseractFx"

#define TESSERACTFX_stringOriginalFilename "TesseractFx.vst3"
#if SMTG_PLATFORM_64
#define TESSERACTFX_stringFileDescription TESSERACTFX_stringPluginName " (64Bit)"
#else
#define TESSERACTFX_stringFileDescription TESSERACTFX_stringPluginName
#endif
