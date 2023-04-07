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
#define MAX_ROW 30

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
	GameArgs* gameArgs = nullptr;
	RankArgs* rankArgs = nullptr;
	RadioArgs* radioArgs = nullptr;
	std::vector<Widget> radioList;
	std::vector<Widget> buttonList;
	HomeWindow();

	void open();

	void initVariables();
	void initRadios();
	void initRadioReadBoard();
	void initRadioInput();
	void initButtons();
	void initDefaultRadio();

	static void radioCallback(Fl_Widget*, void*);
	static void startGame(Fl_Widget*, void*);
	static void openRank(Fl_Widget*, void*);
	static void close(Fl_Widget*, void*);
};