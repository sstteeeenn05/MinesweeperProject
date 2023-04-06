#pragma once

#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "DataType.h"

class RankWindow{
public:
	ConfigArgs* args = nullptr;
	bool isOpen = false;
	Fl_Window* window = nullptr;
	RankWindow();
};
