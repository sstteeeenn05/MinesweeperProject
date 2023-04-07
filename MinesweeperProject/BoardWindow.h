#pragma once
#include <map>

#include "Board.h"
#include "DataType.h"

#include "FL/Fl.h"
#include "FL/Fl_PNG_Image.h"
#include "FL/Fl_Window.h"

class BoardWindow{
	const int MARGIN = 10;
	const int BTN_MINE_SIZE = 30;
public:
	std::map<int, Fl_Image*> PATH_ICON = {
		{MINE_MINE,(new Fl_PNG_Image("mine.png"))->copy(30, 30)},
		{MINE_FLAG,(new Fl_PNG_Image("flag.png"))->copy(30, 30)},
		{MINE_SUS,(new Fl_PNG_Image("sus.png"))->copy(30, 30)}
	};

	Board* board = nullptr;
	const BoardArgs& boardArgs;
	const std::vector<std::vector<char> >& answerBoard;
	const int row, col;
	std::vector<std::vector<MineArgs*> > mineList;
	Fl_Window* window;
	BoardWindow(Board*);
	void initMine();
	void update(BoardWindow*);
	void lose(BoardWindow*);
};
