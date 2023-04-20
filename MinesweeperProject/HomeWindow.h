#pragma once
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fstream>
#include <functional>
#include <map>
#include <vector>
#include <utility>

#include <Windows.h>
#pragma comment(lib, "winmm.lib")

#include "Board.h"
#include "BoardWindow.h"
#include "FL/Fl.h"
#include "FL/fl_ask.H"
#include "FL/Fl_Button.h"
#include "FL/Fl_Choice.H"
#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Light_Button.H"
#include "FL/Fl_Menu_Item.H"
#include "FL/Fl_Pack.H"
#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_Radio_Round_Button.H"
#include "FL/Fl_Scroll.H"
#include "FL/Fl_Slider.H"
#include "FL/Fl_Spinner.H"
#include "FL/Fl_Timer.H"
#include "FL/Fl_Window.h"

namespace HomeWindow{
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
	const int RECORD_WINDOW_WIDTH = 300;
	const int RECORD_WINDOW_HEIGHT = 340;

	extern Fl_Window* mainWindow;
	extern Fl_Button* devButton;
	extern Fl_Button* logo;
	extern GameArgs* gameArgs;
	extern ModeArgs* modeArgs;
	extern std::vector<std::pair<const char*, Widget>> modeListItem;
	extern std::map<const char*, Widget> buttonList;

	extern Fl_Window* devWindow;
	extern Fl_Button* printButton;
	extern Fl_Choice* printList;
	extern std::vector<std::pair<const char*, Widget>> printListItem;
	extern Fl_Slider* volumeSlider;

	extern Fl_Window* recordWindow;
	extern Fl_Scroll* recordView;
	extern Fl_Pack* recordList;
	extern std::vector<RecordArgs*> recordListItem;

	extern int loadCounter;
	extern bool easterEgg;
	extern Fl_Timer* easterEggTimer;

	void open();
	void initVariables();

	void initMainWindow();
	void initLogo();
	void createModeList();
	void createDevButton();
	void openDevWindow();
	void closeDevWindow();
	void createBoardChooser();
	void createInput();
	void createButton();
	void initDefaultChoice();

	void initDevWindow();
	void createPrintChoice();
	void createVolumeSlider();

	void initRecordWindow();
	void clearRecordList();
	void readRecordFile();
	void insertRecord();

	void selectMode(Fl_Widget*, void*);
	void loadGame(Fl_Widget*, void*);
	void startGame(Fl_Widget*, void*);
	void openRecord(Fl_Widget*, void*);
	void closeRecord(Fl_Widget*, void*);
	void close(Fl_Widget*, void*);

	void selectPrint(Fl_Widget*, void*);
	void changeVolume(Fl_Widget*, void*);

	void easterEggCallback(Fl_Widget*, void*);
}