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
#include "FL/Fl_Output.h"
#include "FL/Fl_PNG_Image.h"
#include "FL/Fl_Timer.h"
#include "FL/Fl_Window.h"

class BoardWindow{
	static std::unordered_set<BoardWindow*> objAddrList;

	const int MARGIN = 10;
	const int TOOLBAR_HEIGHT = 40;
	const int BTN_MINE_SIZE = 30;

	const int RESULT_BUTTON_WIDTH = 120;
	const int RESULT_BUTTON_HEIGHT = 40;
	const int RESULT_BUTTON_COUNT = 4;
	const int RESULT_IMG_SIZE = 200;
	const int RESULT_LABEL_SIZE = 50;
	const int RESULT_WINDOW_WIDTH = MARGIN + (RESULT_BUTTON_WIDTH + MARGIN) * RESULT_BUTTON_COUNT;
	const int RESULT_WINDOW_HEIGHT = RESULT_BUTTON_HEIGHT + MARGIN * 3 + RESULT_IMG_SIZE;
public:
	static const std::map<int, Fl_Image*> IMG_ICON;
	static const std::map<int, Fl_Image*> IMG_REACT;
	static const std::map<int, Fl_Image*> IMG_RESULT;
	static int playMode;
	static std::mutex btnCallbackLock;
	static bool isWindowAvailable(BoardWindow*);
	static int getWindowCount();
	static void easterAll(Fl_Color);
	static void closeAll();

	Board* board = new Board();
	const BoardArgs& boardArgs = board->getBoardArgs();

	Fl_Window* mainWindow = new Fl_Window(MARGIN * 2 + BTN_MINE_SIZE * boardArgs.column, MARGIN * 2 + BTN_MINE_SIZE * boardArgs.row);
	std::vector<std::vector<MineArgs*>> mineList;
	Fl_Timer* timer;
	clock_t startTime, endTime;
	Fl_Box* reaction;
	Fl_Box* remainBombCountView;

	Fl_Window* resultWindow = new Fl_Window(RESULT_WINDOW_WIDTH, RESULT_WINDOW_HEIGHT);
	Fl_Box* resultImage = new Fl_Box(MARGIN * 2, MARGIN, RESULT_IMG_SIZE, RESULT_IMG_SIZE);
	Fl_Box* resultLabel = new Fl_Box(MARGIN * 3 + RESULT_IMG_SIZE, 50 + MARGIN, 270, RESULT_LABEL_SIZE);
	Fl_Box* resultTime = new Fl_Box(MARGIN * 3 + RESULT_IMG_SIZE, 50 + MARGIN * 2 + RESULT_LABEL_SIZE, 270, RESULT_LABEL_SIZE);
	std::map<const char*, Widget> resultButtonList;

	BoardWindow() = default;
	BoardWindow(Board*);
	~BoardWindow();

	void reload(int, int);

	void initToolbar();
	void initMine();
	void removeMine();
	void resetTimer();
	void updateWindowTitle();
	void updateToolbar(char = MINE_NULL);
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