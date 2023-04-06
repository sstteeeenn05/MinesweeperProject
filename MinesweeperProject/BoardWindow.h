#pragma once

#include "DataType.h"

#include "FL/Fl.h"
#include "FL/Fl_Window.h"

class BoardWindow{
public:
	GameArgs* args = nullptr;
	bool isOpen = false;
	Fl_Window* window;
	BoardWindow(int, int);
};
