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
#include "DataType.h"

#include "FL/Fl.h"
#include "FL/Fl_PNG_Image.h"
#include "FL/Fl_Window.h"

class BoardWindow{
	static std::unordered_set<void*> objAddrList;

	const int MARGIN = 10;
	const int BTN_MINE_SIZE = 30;
	const int RESULT_BUTTON_WIDTH = 120;
	const int RESULT_BUTTON_HEIGHT = 40;
	const int RESULT_BUTTON_COUNT = 4;
	const int RESULT_WINDOW_WIDTH = MARGIN + (RESULT_BUTTON_WIDTH + MARGIN) * RESULT_BUTTON_COUNT;
	const int RESULT_WINDOW_HEIGHT = RESULT_BUTTON_HEIGHT + MARGIN * 2;
public:
	static int getWindowCount();

	const std::map<int, Fl_Image*> PATH_ICON = {
		{MINE_MINE,(new Fl_PNG_Image("img/mine.png"))->copy(30, 30)},
		{MINE_FLAG,(new Fl_PNG_Image("img/flag.png"))->copy(30, 30)},
		{MINE_SUS,(new Fl_PNG_Image("img/sus.png"))->copy(30, 30)}
	};

	Board* board = nullptr;
	const BoardArgs& boardArgs;
	std::vector<std::vector<MineArgs*> > mineList;
	Fl_Window* mainWindow, * resultWindow;
	std::vector<Widget> resultButtonList;
	std::mutex* m_btnCallback = new std::mutex();

	BoardWindow(Board*);
	~BoardWindow();

	void initMine();
	void initWindowTItle();
	void update();
	void win();
	void lose();

	void initResultWindow();
	void initResultVariables();
	void initResultButtonArgs();

	static void playAgain(Fl_Widget*, void*);
	static void newGame(Fl_Widget*, void*);
	static void submitScore(Fl_Widget*, void*);
	static void closeResultWindow(Fl_Widget*, void*);
	static void closeGame(Fl_Widget*, void*);
};