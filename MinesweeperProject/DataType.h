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

enum {
	RADIO_READ_BOARD,
	RADIO_INPUT_COUNT,
	RADIO_INPUT_RATE
};

enum {
	BOARD_STATUS_CONTINUE,
	BOARD_STATUS_LOSE,
	BOARD_STATUS_WIN
};

enum {
	MINE_MINE = 'X',
	MINE_FLAG = 'f',
	MINE_SUS = '?',
	MINE_MASK = '#',
	MINE_NULL = '0'
};

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
	Fl_Button* btnChooser = nullptr, * btnRandom = nullptr;
	std::string boardPath;
	int row = 10, col = 10, number = 10;
};

struct MineArgs {
	Fl_Button* button = nullptr;
	Board* board = nullptr;
	BoardWindow* parent = nullptr;
	bool isProcessing = false;
	int x, y;
	int status = 0;
};

struct GenericArgs {
	Fl_Button* button = nullptr;
};

struct GameArgs :GenericArgs {
	BoardWindow* window = nullptr;
	Board* board = nullptr;
	RadioArgs* radioArgs = nullptr;
};

struct RankArgs :GenericArgs {
	RankWindow* window = nullptr;
};

struct BoardArgs {
	std::vector<std::vector<char>> board;
	int gameStatus = BOARD_STATUS_CONTINUE;
};