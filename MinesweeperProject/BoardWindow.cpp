#include "BoardWindow.h"

BoardWindow::BoardWindow(Board* b) :board(b), boardArgs(b->getBoardArgs()) {

	mainWindow = new Fl_Window(MARGIN * 2 + BTN_MINE_SIZE * boardArgs.column, MARGIN * 2 + BTN_MINE_SIZE * boardArgs.row);
	mainWindow->begin();

	initMine();

	mainWindow->end();

	initWindowTItle(mainWindow);
}

void BoardWindow::initWindowTItle(Fl_Window* w) {
	std::stringstream* title = new std::stringstream();
	(*title) << "Game (" << boardArgs.column << "x" << boardArgs.row << ") 0%" << std::endl;
	w->label(title->str().c_str());
	delete title;
}

void BoardWindow::initMine(){
	bool isNewWindow = !mineList.size();
	for (int i = 0; i < boardArgs.row; i++) {
		std::vector<MineArgs*> mines;
		for (int j = 0; j < boardArgs.column; j++) {
			MineArgs* mineArgs = (isNewWindow) ? new MineArgs() : mineList[i][j];
			if (isNewWindow) mineArgs->button = new Fl_Button(MARGIN + BTN_MINE_SIZE * j, MARGIN + BTN_MINE_SIZE * i, BTN_MINE_SIZE, BTN_MINE_SIZE);
			if (isNewWindow) mineArgs->button->callback([](Fl_Widget* w, void* args) {
				auto mineArgs = (MineArgs*)args;
				if (mineArgs->parent->boardArgs.status == BOARD_STATUS_CONTINUE) {
					if (mineArgs->isProcessing) return;
					mineArgs->isProcessing = true;
					if (Fl::event_button() == FL_LEFT_MOUSE && mineArgs->parent->boardArgs.board[mineArgs->y][mineArgs->x] != MINE_FLAG)
						mineArgs->board->leftClick(mineArgs->x, mineArgs->y);
					if (Fl::event_button() == FL_RIGHT_MOUSE)
						mineArgs->board->rightClick(mineArgs->x, mineArgs->y);
					mineArgs->parent->update(mineArgs->parent);
					mineArgs->isProcessing = false;
				}
			}, (void*)mineArgs);
			mineArgs->board = board;
			mineArgs->x = j;
			mineArgs->y = i;
			mineArgs->parent = this;
			if (isNewWindow) mines.push_back(mineArgs);
		}
		if (isNewWindow) mineList.push_back(mines);
	}
}

void BoardWindow::update(BoardWindow* bw) {
	std::stringstream* title = new std::stringstream();
	(*title) << "Game (" << boardArgs.column << "x" << boardArgs.row << ") " << std::fixed << std::setprecision(1) <<
		(double)bw->boardArgs.openBlankCount / (bw->boardArgs.openBlankCount + bw->boardArgs.remainBlankCount) * 100 << "%" << std::endl;
	bw->mainWindow->label(title->str().c_str());
	delete title;

	std::vector<std::future<void> > allocateTaskList;
	for (auto& mines : bw->mineList) for (auto& mine : mines) allocateTaskList.push_back(std::async(std::launch::async, [=]() {
		if (mine->button->label()) delete mine->button->label();
		mine->button->image(nullptr);
		mine->button->label(nullptr);
	}));
	for (auto& task : allocateTaskList) task.get();

	for (int i = 0; i < bw->boardArgs.row; i++) {
		for (int j = 0; j < bw->boardArgs.column; j++) {
			auto currentButton = bw->mineList[i][j]->button;
			const auto currentValue = (int)bw->boardArgs.board[i][j];
			switch (currentValue) {
			case MINE_FLAG:
			case MINE_SUS:
				currentButton->image(PATH_ICON.at(currentValue));
				break;
			case MINE_MASK:
			case MINE_MINE:
				currentButton->value(0);
				currentButton->color(FL_GRAY);
				currentButton->selection_color(FL_GRAY);
				currentButton->activate();
				break;
			default: {
				const char* label = new char[2] {bw->boardArgs.board[i][j], '\0'};
				currentButton->label(label);
			}case MINE_NULL:
				currentButton->set();
				currentButton->deactivate();
				break;
			}
			Fl::awake();
		}
	}

	switch (bw->boardArgs.status) {
		case BOARD_STATUS_WIN:
			bw->win(bw);
			break;
		case BOARD_STATUS_LOSE:
			bw->lose(bw);
			break;
		case BOARD_STATUS_CONTINUE:
			return;
	}

	initResultWindow();
	bw->resultWindow->show();
}

void BoardWindow::win(BoardWindow* bw) {
	for (int i = 0; i < bw->boardArgs.row; i++) {
		for (int j = 0; j < bw->boardArgs.column; j++) {
			clock_t start = clock();
			auto currentButton = bw->mineList[i][j]->button;
			const auto currentValue = (int)bw->boardArgs.answer[i][j];
			if (currentButton->label()) delete currentButton->label();
			currentButton->image(nullptr);
			currentButton->label(nullptr);
			currentButton->deactivate();
			currentButton->set();
			currentButton->selection_color(FL_GREEN);
			currentButton->redraw();
			while (clock() - start < CLOCKS_PER_SEC / 100) Fl::flush();
		}
	}
}

void BoardWindow::lose(BoardWindow* bw) {
	for (int i = 0; i < bw->boardArgs.row; i++) {
		for (int j = 0; j < bw->boardArgs.column; j++) {
			clock_t start = clock();
			auto currentButton = bw->mineList[i][j]->button;
			const auto currentValue = (int)bw->boardArgs.answer[i][j];
			if (currentValue == MINE_MINE) {
				currentButton->image(bw->PATH_ICON.at(MINE_MINE));
				currentButton->color(FL_RED);
			} else currentButton->deactivate();
			currentButton->selection_color(FL_RED);
			currentButton->redraw();
			while (clock() - start < CLOCKS_PER_SEC / 200) Fl::flush();
		}
	}
}

void BoardWindow::initResultWindow() {
	resultWindow = new Fl_Window(RESULT_WINDOW_WIDTH, RESULT_WINDOW_HEIGHT);
	resultWindow->set_modal();
	resultWindow->begin();

	initResultVariables();
	initResultButtonArgs();

	mainWindow->end();
}

void BoardWindow::initResultVariables() {
	resultButtonList = {
		{"Play Again",&BoardWindow::playAgain,(void*)this},
		{"New Game",&BoardWindow::newGame,(void*)this},
		{"Submit Score",&BoardWindow::submitScore,(void*)this},
		{"Exit",&BoardWindow::closeGame,(void*)this}
	};
}

void BoardWindow::initResultButtonArgs() {
	int buttonIndex = 0;
	for (auto& button : resultButtonList) {
		Fl_Button* component = new Fl_Button(MARGIN + (MARGIN + RESULT_BUTTON_WIDTH) * (buttonIndex++), MARGIN, RESULT_BUTTON_WIDTH, RESULT_BUTTON_HEIGHT, button.text);
		button.component = component;
		button.component->callback((Fl_Callback*)button.callback, button.args);
	}
}

void BoardWindow::playAgain(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;
	bw->board->maskBoard();
	bw->initWindowTItle(bw->mainWindow);
	bw->update(bw);
	bw->mainWindow->redraw();
	bw->closeResultWindow(bw->resultWindow, args);
}

void BoardWindow::newGame(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;

}

void BoardWindow::submitScore(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;

}

void BoardWindow::closeResultWindow(Fl_Widget* w, void* args) {
	auto resultWindow = (Fl_Window*)w;
	auto bw = (BoardWindow*)args;
	resultWindow->~Fl_Window();
	for (auto& button : bw->resultButtonList) button.component->~Fl_Widget();
}

void BoardWindow::closeGame(Fl_Widget* w, void* args){
	auto bw = (BoardWindow*)args;
	bw->mainWindow->~Fl_Window();
	bw->closeResultWindow(bw->resultWindow, args);
	delete bw;
}
