#pragma once
#pragma warning(disable:4996)

#include <string>
#include <vector>

#include "FL/Fl.H"
#include "FL/Fl_Button.h"
#include "FL/Fl_Spinner.h"
#include "FL/Fl_Radio_Round_Button.h"
#include "FL/Fl_File_Chooser.h"

#define CHOOSER_ARGS "Text Files(*.txt)", Fl_File_Chooser::SINGLE, "Select Board File"

#define DEF_COL 10
#define DEF_ROW 10
#define DEF_CNT 10
#define MAX_COL 50
#define MAX_ROW 30
#define MAX_PERCENT 100

class Board;
class BoardWindow;
class ConfigWindow;
class RankWindow;

enum {
	METHOD_PENDING,
	METHOD_CMD_FILE,
	METHOD_CMD_INPUT,
	METHOD_GUI
};

enum {
	MODE_READ_BOARD,
	MODE_INPUT_COUNT,
	MODE_INPUT_RATE
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

enum {
	PRINT_BOARD,
	PRINT_ANSWER,
	PRINT_BOMB_COUNT,
	PRINT_FLAG_COUNT,
	PRINT_OPEN_BLANK,
	PRINT_REMAIN_BLANK,
	PRINT_STATE
};

enum {
	STATE_STANDBY,
	STATE_PLAYING,
	STATE_GAME_OVER
};

struct Widget {
	const char* text;
	Fl_Callback* callback;
	void* args;
	Fl_Widget* component;
};

struct ModeArgs {
	std::vector<Fl_Radio_Round_Button*> buttons;
	Fl_Choice* mode;
	int selection = 0;
	Fl_File_Chooser* chooser;
	Fl_Input* iptPath;
	Fl_Spinner* iptNumber, * iptColumn, * iptRow;
	Fl_Button* btnChooser, * btnRandom;
	std::string boardPath;
	int row = DEF_ROW, col = DEF_COL;
	double number = DEF_CNT;
};

struct MineArgs {
	Fl_Button* button;
	Board* board;
	BoardWindow* parent;
	bool isProcessing = false;
	int x = 0, y = 0;
};

struct GameArgs {
	BoardWindow* mainWindow;
	Board* board;
	ModeArgs* modeArgs;
};

struct RankArgs {
	RankWindow* window;
};

struct BoardArgs {
	std::vector<std::vector<char>> board;
	std::vector<std::vector<char>> answer;
	int row = 0, column = 0;
	double randomRate = 0;
	int bombCount = 0;
    int flagCount = 0;
    int openBlankCount = 0;
    int remainBlankCount = 0;
	int status = BOARD_STATUS_CONTINUE;
	int mode = MODE_READ_BOARD;
	int state = STATE_STANDBY;
	std::string path = "board.txt";
};