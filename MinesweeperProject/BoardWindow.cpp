#include "BoardWindow.h"

BoardWindow::BoardWindow(Board* b) :board(b), boardArgs(b->getBoardArgs()) {

	mainWindow = new Fl_Window(MARGIN * 2 + BTN_MINE_SIZE * boardArgs.column, MARGIN * 2 + BTN_MINE_SIZE * boardArgs.row);
	mainWindow->begin();

	initMine();

	mainWindow->end();

	initWindowTItle();
}

void BoardWindow::initWindowTItle() {
	std::stringstream* title = new std::stringstream();
	title->str("");
	title->clear();
	(*title) << "Game (" << boardArgs.column << "x" << boardArgs.row << ") 0.0%" << std::endl;
	mainWindow->label(title->str().c_str());
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
					mineArgs->parent->update();
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

void BoardWindow::update() {
	std::stringstream* title = new std::stringstream();
	(*title) << "Game (" << boardArgs.column << "x" << boardArgs.row << ") " << std::fixed << std::setprecision(1) <<
		(double)boardArgs.openBlankCount / (boardArgs.openBlankCount + boardArgs.remainBlankCount) * 100 << "%" << std::endl;
	mainWindow->label(title->str().c_str());
	delete title;

	std::vector<std::future<void> > allocateTaskList;
	for (auto& mines : mineList) for (auto& mine : mines) allocateTaskList.push_back(std::async(std::launch::async, [=] {
		if (mine->button->label()) delete mine->button->label();
		mine->button->image(nullptr);
		mine->button->label(nullptr);
	}));
	for (auto& task : allocateTaskList) task.get();

	for (int i = 0; i < boardArgs.row; i++) {
		for (int j = 0; j < boardArgs.column; j++) {
			auto currentButton = mineList[i][j]->button;
			const auto currentValue = (int)boardArgs.board[i][j];
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
				const char* label = new char[2] {boardArgs.board[i][j], '\0'};
				currentButton->label(label);
			}case MINE_NULL:
				currentButton->set();
				currentButton->deactivate();
				break;
			}
			Fl::awake();
		}
	}

	switch (boardArgs.status) {
		case BOARD_STATUS_WIN:
			win();
			break;
		case BOARD_STATUS_LOSE:
			lose();
			break;
		case BOARD_STATUS_CONTINUE:
			return;
	}

	initResultWindow();
	resultWindow->show();
}

void BoardWindow::win() {
	for (int i = 0; i < boardArgs.row; i++) {
		for (int j = 0; j < boardArgs.column; j++) {
			clock_t start = clock();
			auto currentButton = mineList[i][j]->button;
			const auto currentValue = (int)boardArgs.answer[i][j];
			if (currentButton->label()) delete currentButton->label();
			currentButton->image(nullptr);
			currentButton->label(nullptr);
			currentButton->deactivate();
			currentButton->set();
			currentButton->selection_color(FL_GREEN);
			currentButton->redraw();
			while (clock() - start < CLOCKS_PER_SEC / 200) Fl::flush();
		}
	}
}

void BoardWindow::lose() {
	for (int i = 0; i < boardArgs.row; i++) {
		for (int j = 0; j < boardArgs.column; j++) {
			clock_t start = clock();
			auto currentButton = mineList[i][j]->button;
			const auto currentValue = (int)boardArgs.answer[i][j];
			if (currentValue == MINE_MINE)
				currentButton->image(PATH_ICON.at(MINE_MINE));
			else currentButton->deactivate();
			currentButton->color(FL_RED);
			currentButton->selection_color(FL_RED);
			currentButton->redraw();
			while (clock() - start < CLOCKS_PER_SEC / 200) Fl::flush();
		}
	}
}

void BoardWindow::initResultWindow() {
	resultWindow = new Fl_Window(RESULT_WINDOW_WIDTH, RESULT_WINDOW_HEIGHT, (boardArgs.status == BOARD_STATUS_WIN) ? "You Win!" : "You Lose");
	resultWindow->set_modal();
	resultWindow->begin();

	initResultVariables();
	initResultButtonArgs();

	mainWindow->end();
}

void BoardWindow::initResultVariables() {
	resultButtonList = {
		{"Play Again",&BoardWindow::playAgain,(void*)this},
		{"Regenerate",&BoardWindow::newGame,(void*)this},
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
	bw->initWindowTItle();
	bw->update();
	bw->mainWindow->redraw();
	bw->closeResultWindow(bw->resultWindow, args);
}

void BoardWindow::newGame(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;
	auto& boardArgs = bw->boardArgs;
	//Board* b = nullptr;
	std::promise<Board*> p;
	std::future<Board*> b = p.get_future();
	try {
		switch (boardArgs.mode) {
			case MODE_READ_BOARD: {
				Fl_File_Chooser* chooser = new Fl_File_Chooser(CHOOSER_ARGS);
				chooser->callback([](Fl_File_Chooser* c, void* args) {
					std::string path = c->value();
					if (!c->shown()) {
						std::ifstream file(path);
						((std::promise<Board*>*)args)->set_value(new Board(file));
					}
				}, &p);
				chooser->show();
				while(chooser->shown()) Fl::wait();
				break;
			} case MODE_INPUT_RATE:
				p.set_value(new Board(boardArgs.row, boardArgs.column, boardArgs.randomRate));
				break;
			case MODE_INPUT_COUNT:
				p.set_value(new Board(boardArgs.row, boardArgs.column, boardArgs.bombCount));
				break;
			default: throw std::exception("Logic error");
		}
		auto boardWindow = new BoardWindow(b.get());
		boardWindow->mainWindow->show();
		bw->closeGame(NULL, args);
	} catch (std::exception e) {
		fl_alert(e.what());
		return;
	}
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
