#pragma once

#include <string>
#include <vector>

#include "FL/Fl.H"
#include "FL/Fl_Button.h"
#include "FL/Fl_Input.h"
#include "FL/Fl_Int_Input.h"
#include "FL/Fl_Radio_Round_Button.h"
#include "FL/Fl_File_Chooser.h"

class Board;
class BoardWindow;
class ConfigWindow;
class RankWindow;

struct Widget {
	const char* text;
	const void* callback;
	void* args = nullptr;
	Fl_Widget* component = nullptr;
};

struct RadioArgs {
	std::vector<Fl_Radio_Round_Button*> buttons;
	int selection = 0;
	Fl_File_Chooser* chooser = nullptr;
	Fl_Input* iptPath = nullptr;
	Fl_Int_Input* iptNumber = nullptr, * iptColumn = nullptr, * iptRow = nullptr;
	Fl_Button* btnChooser = nullptr;
	std::string boardPath;
	int row = 10, col = 10, number = 25;
};

struct GenericArgs {
	Fl_Button* button = nullptr;
};

struct GameArgs:GenericArgs {
	BoardWindow* window = nullptr;
	Board* board = nullptr;
	RadioArgs* radioArgs = nullptr;
};

struct RankArgs :GenericArgs {
	RankWindow* window = nullptr;
};
