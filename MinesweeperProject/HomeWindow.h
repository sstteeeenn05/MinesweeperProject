#pragma once
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fstream>
#include <functional>
#include <vector>

#include "Board.h"
#include "BoardWindow.h"
#include "RankWindow.h"

#include "FL/Fl.h"
#include "FL/fl_ask.H"
#include "FL/Fl_Button.h"
#include "FL/Fl_Choice.H"
#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Light_Button.H"
#include "FL/Fl_Menu_Item.H"
#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_Radio_Round_Button.H"
#include "FL/Fl_Spinner.H"
#include "FL/Fl_Window.h"

class HomeWindow{
	const int WINDOW_WIDTH = 400;
	const int WINDOW_HEIGHT = 350;
	const int LOGO_Y = 10;
	const int LOGO_WIDTH = 380;
	const int LOGO_HEIGHT = 150;
	const int COMPONENT_Y[3] = { 170,210,250 };
	const int MENU_MODE_WIDTH = 200;
	const int COMPONENT_HEIGHT = 30;
	const int PATH_WIDTH = 300;
	const int CHOOSER_WIDTH = 30;
	const int LABEL_WIDTH = 50;
	const int VALUE_WIDTH = 65;
	const int BTN_RANDOM_WIDTH = 75;
	const int INPUT_WIDTH = 50;
	const int BUTTON_Y = 300;
	const int BUTTON_WIDTH = 120;
	const int BUTTON_HEIGHT = 40;
	const int MARGIN = 10;
public:
	Fl_Window* mainWindow = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper");
	Fl_Window* devWindow = new Fl_Window(300,100,"Dev Toolkit");
	Fl_Button* logo;
	GameArgs* gameArgs = new GameArgs();
	RankArgs* rankArgs = new RankArgs();
	ModeArgs* modeArgs = new ModeArgs();
	std::vector<Widget> modeList;
	std::vector<Widget> buttonList;
	HomeWindow();
	~HomeWindow();

	void open();

	void initLogo();
	void initVariables();
	void createModeList();
	void createBoardChooser();
	void createInput();
	void createButton();
	void initDefaultChoice();

	void createDevButton();

	static void selectMode(Fl_Widget*, void*);
	static void startGame(Fl_Widget*, void*);
	static void openRank(Fl_Widget*, void*);
	static void close(Fl_Widget*, void*);
};