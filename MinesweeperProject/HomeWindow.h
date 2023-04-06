#pragma once
#include <vector>
#include <functional>

#include "Board.h"
#include "BoardWindow.h"
#include "ConfigWindow.h"
#include "RankWindow.h"

#include "FL/Fl.h"
#include "FL/Fl_Button.h"
#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_Window.h"

#define WINDOW_WIDTH 310
#define WINDOW_HEIGHT 180
#define WINDOW_PADDING 10
#define LOGO_X 10
#define LOGO_Y 10
#define LOGO_WIDTH 290
#define LOGO_HEIGHT 110
#define BUTTON_X 10
#define BUTTON_Y 130
#define BUTTON_MARGIN 10
#define BUTTON_WIDTH 90
#define BUTTON_HEIGHT 40

class HomeWindow{
public:
	Fl_Button* logo = nullptr;
	Fl_Window* window = nullptr;
	GameArgs* gameArgs = new GameArgs();
	ConfigArgs* configArgs = new ConfigArgs();
	std::vector<Widget> buttonList = {
		{"New Game",&HomeWindow::startGame,(void*)gameArgs},
		{"Config",&HomeWindow::openConfig,(void*)configArgs},
		{"Exit",&HomeWindow::close}
	};
	HomeWindow();
	static void startGame(Fl_Widget*, void*);
	static void openConfig(Fl_Widget*, void*);
	static void close(Fl_Widget*, void*);
};