#pragma once
#include <ctime>
#include <functional>
#include <future>
#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>
#include <typeinfo>
#include <unordered_set>

#include "Board.h"
#include "HomeWindow.h"

#include "FL/Fl.h"
#include "FL/Fl_PNG_Image.h"
#include "FL/Fl_Window.h"

class BoardWindow{
	static std::unordered_set<BoardWindow*> objAddrList;

	const int MARGIN = 10;
	const int BTN_MINE_SIZE = 30;
	const int RESULT_BUTTON_WIDTH = 120;
	const int RESULT_BUTTON_HEIGHT = 40;
	const int RESULT_BUTTON_COUNT = 4;
	const int RESULT_WINDOW_WIDTH = MARGIN + (RESULT_BUTTON_WIDTH + MARGIN) * RESULT_BUTTON_COUNT;
	const int RESULT_WINDOW_HEIGHT = RESULT_BUTTON_HEIGHT + MARGIN * 2;
public:
	static int playMode;
	static std::mutex btnCallbackLock;
	static bool isWindowAvailable(BoardWindow*);
	static int getWindowCount();
	static void easterAll(Fl_Color);
	static void closeAll();

	const std::map<int, Fl_Image*> IMG_ICON = {
		{MINE_MINE,(new Fl_PNG_Image("img/mine.png"))->copy(30, 30)},
		{MINE_FLAG,(new Fl_PNG_Image("img/flag.png"))->copy(30, 30)},
		{MINE_SUS,(new Fl_PNG_Image("img/sus.png"))->copy(30, 30)}
	};

	Board* board = new Board();
	const BoardArgs& boardArgs = board->getBoardArgs();
	std::vector<std::vector<MineArgs*>> mineList;
	Fl_Window* mainWindow = new Fl_Window(MARGIN * 2 + BTN_MINE_SIZE * boardArgs.column, MARGIN * 2 + BTN_MINE_SIZE * boardArgs.row);
	Fl_Window* resultWindow = new Fl_Window(RESULT_WINDOW_WIDTH, RESULT_WINDOW_HEIGHT);
	std::map<const char*, Widget> resultButtonList;

	BoardWindow() = default;
	BoardWindow(Board*);
	~BoardWindow();

	void reload(int, int);

	void initMine();
	void removeMine();
	void initWindowTitle();
	void update();
	void win();
	void lose();

	void initResultWindow();
	void initResultVariables();
	void createResultButton();
	void openResultWindow();

	static void playAgain(Fl_Widget*, void*);
	static void newGame(Fl_Widget*, void*);
	static void submitScore(Fl_Widget*, void*);
};