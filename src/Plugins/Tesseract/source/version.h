
// Plugin name and version of Tesseract
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

#define TESSERACT_MAJOR_VERSION_STR "1"
#define TESSERACT_MAJOR_VERSION_INT 1

#define TESSERACT_MINOR_VERSION_STR "0"
#define TESSERACT_MINOR_VERSION_INT 0

#define TESSERACT_PATCH_NUMBER_STR "1"
#define TESSERACT_PATCH_NUMBER_INT 1

#define TESSERACT_BUILD_NUMBER_STR "1"
#define TESSERACT_BUILD_NUMBER_INT 1

#define TESSERACT_FULL_VERSION_STR TESSERACT_MAJOR_VERSION_STR "." TESSERACT_MINOR_VERSION_STR "." TESSERACT_PATCH_NUMBER_STR "." TESSERACT_BUILD_NUMBER_STR

#define TESSERACT_VERSION_STR TESSERACT_MAJOR_VERSION_STR "." TESSERACT_MINOR_VERSION_STR "." TESSERACT_PATCH_NUMBER_STR

#define TESSERACT_stringPluginName "Tesseract"

#define TESSERACT_stringOriginalFilename "Tesseract.vst3"
#if SMTG_PLATFORM_64
#define TESSERACT_stringFileDescription TESSERACT_stringPluginName " (64Bit)"
#else
#define TESSERACT_stringFileDescription TESSERACT_stringPluginName
#endif
