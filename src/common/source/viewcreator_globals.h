
// Instantiate custom ui view factories. 
// This file is to be included in one (!) .cpp file, i.e. factory.cpp of a plugin project. 
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

#ifndef UBERTON_VIEWCREATOR_GLOBALS
#define UBERTON_VIEWCREATOR_GLOBALS

#include "viewcreator.h"

static const Uberton::FadingFrameAnimationButtonFactory __gFadingFrameAnimationButtonFactory;
static const Uberton::UbertonContextMenuFactory __gUbertonContextMenuFactory;
static const Uberton::HistoryButtonFactory __gHistoryButtonFactory;
static const Uberton::DiagonalSliderFactory __gDiagonalSliderFactory;
static const Uberton::StringMapLabelFactory __gStringMapLabelFactory;
static const Uberton::TextEditUnitsFactory __gTextEditUnitsFactory;
static const Uberton::LogVUMeterFactory __gLogVUMeterFactory;
static const Uberton::LinkButtonFactory __gLinkButtonFactory;

#else
#error "viewcreator_globals.h can only be included once"
#endif