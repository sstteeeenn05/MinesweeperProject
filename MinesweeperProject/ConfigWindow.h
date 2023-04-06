#pragma once

#include "CustomDataType.h"

#include "FL/Fl_Window.H"

class ConfigWindow{
public:
	ConfigArgs* args = nullptr;
	bool isOpen = false;
	Fl_Window* window;
	ConfigWindow();

};
