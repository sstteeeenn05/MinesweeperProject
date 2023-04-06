#pragma once
#include <map>
#include <vector>
#include "FL/Fl.h"
#include "FL/Fl_Button.h"
#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_Window.h"
#define WINDOW_WIDTH 410
#define WINDOW_HEIGHT 210
#define WINDOW_PADDING 10
#define BUTTON_X 10
#define BUTTON_Y 160
#define LOGO_X 10
#define LOGO_Y 10
#define LOGO_WIDTH 390
#define LOGO_HEIGHT 140
#define BUTTON_MARGIN 10
#define BUTTON_WIDTH 90
#define BUTTON_HEIGHT 40

struct Button {
	const char* text;
	Fl_Button* component;
};

class HomeWindow{
public:
	Fl_Button* logo;
	Fl_Window* window;
	std::vector<const char*> buttonTextList = {"New Game","Load","Config","Exit"};
	std::vector<Fl_Button*> buttonList;
	HomeWindow();

};