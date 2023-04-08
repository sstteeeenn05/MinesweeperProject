#pragma once

#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "DataType.h"

class RankWindow{
public:
	bool isOpen = false;
	Fl_Window* mainWindow = nullptr;
	RankWindow();
};
