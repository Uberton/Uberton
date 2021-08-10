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

#else
#error "viewcreator_globals.h can only be included once"
#endif