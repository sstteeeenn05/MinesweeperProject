#pragma once

#include "FL/Fl.H"
#include "FL/Fl_Button.h"

class Board;
class BoardWindow;
class ConfigWindow;

struct Widget {
	const char* text;
	const void* callback;
	void* args = nullptr;
	Fl_Widget* component = nullptr;
};

struct GenericArgs {
	Fl_Button* button = nullptr;
};

struct GameArgs:GenericArgs {
	BoardWindow* window = nullptr;
	Board* board = nullptr;
};

struct ConfigArgs:GenericArgs {
	ConfigWindow* window = nullptr;
	GameArgs* gameArgs;
};