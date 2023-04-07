#pragma once
#include <vector>
#include <functional>

#include "Board.h"
#include "BoardWindow.h"
#include "RankWindow.h"

#include "FL/Fl.h"
#include "FL/Fl_Button.h"
#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Radio_Round_Button.h"
#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_Window.h"

#define MAX_COL 50
#define MAX_ROW 50

enum {
	RADIO_READ_BOARD,
	RADIO_INPUT_COUNT,
	RADIO_INPUT_RATE,
	RADIO_COUNT
};

class HomeWindow{
	const int WINDOW_WIDTH = 400;
	const int WINDOW_HEIGHT = 350;
	const int LOGO_Y = 10;
	const int LOGO_WIDTH = 380;
	const int LOGO_HEIGHT = 150;
	const int RADIO_Y[3] = { 170,210,250 };
	const int RADIO_WIDTH = 150;
	const int RADIO_HEIGHT = 30;
	const int PATH_WIDTH = 150;
	const int CHOOSER_WIDTH = 30;
	const int LABEL_WIDTH = 30;
	const int VALUE_WIDTH = 105;
	const int BTN_RANDOM_WIDTH = 75;
	const int INPUT_WIDTH = 75;
	const int BUTTON_Y = 300;
	const int BUTTON_WIDTH = 120;
	const int BUTTON_HEIGHT = 40;
	const int MARGIN = 10;
public:
	Fl_Button* logo = nullptr;
	Fl_Window* window = nullptr;
	GameArgs* gameArgs = new GameArgs();
	RankArgs* rankArgs = new RankArgs();
	RadioArgs* radioArgs = new RadioArgs();
	std::vector<Widget> radioList = { 
		{ "Read Board File", &HomeWindow::radioCallback, (void*)radioArgs },
		{ "Input Mine Count", &HomeWindow::radioCallback, (void*)radioArgs },
		{ "Input Respawn Rate", &HomeWindow::radioCallback, (void*)radioArgs } };
	std::vector<Widget> buttonList = {
		{"New Game",&HomeWindow::startGame,(void*)gameArgs},
		{"Leaderboard",&HomeWindow::openRank,(void*)rankArgs},
		{"Exit",&HomeWindow::close}
	};
	HomeWindow();

	void initRadios();
	void initRadioReadBoard();
	void initRadioRandom();
	void initButtons();

	static void radioCallback(Fl_Widget*, void*);
	static void startGame(Fl_Widget*, void*);
	static void openRank(Fl_Widget*, void*);
	static void close(Fl_Widget*, void*);
};