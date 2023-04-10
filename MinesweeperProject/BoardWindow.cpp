#include "BoardWindow.h"

std::unordered_set<void*> BoardWindow::objAddrList = std::unordered_set<void*>();

int BoardWindow::getWindowCount() {
	return objAddrList.size();
}

BoardWindow::BoardWindow(Board* b) :board(b), boardArgs(b->getBoardArgs()) {
	mainWindow->begin();

	initMine();

	mainWindow->end();

	initWindowTItle();
	initResultWindow();
	objAddrList.insert(this);
}

BoardWindow::~BoardWindow() {
	delete board;
	delete mainWindow;
	delete resultWindow;
	board = nullptr;
	objAddrList.erase(this);
}

void BoardWindow::initWindowTItle() {
	std::stringstream title("");
	title << "Game (" << boardArgs.column << "x" << boardArgs.row << ") 0.0%" << std::endl;
	mainWindow->label(title.str().c_str());
}

void BoardWindow::initMine(){
	for (int i = 0; i < boardArgs.row; i++) {
		std::vector<MineArgs*> mines;
		for (int j = 0; j < boardArgs.column; j++) {
			MineArgs* mineArgs = new MineArgs();
			mineArgs->button = new Fl_Button(MARGIN + BTN_MINE_SIZE * j, MARGIN + BTN_MINE_SIZE * i, BTN_MINE_SIZE, BTN_MINE_SIZE);
			mineArgs->button->callback([](Fl_Widget* w, void* args) {
				auto mineArgs = (MineArgs*)args;
				if (mineArgs->parent->boardArgs.status == BOARD_STATUS_CONTINUE) {
					mineArgs->parent->m_btnCallback->lock();
					if (Fl::event_button() == FL_LEFT_MOUSE && mineArgs->parent->boardArgs.board[mineArgs->y][mineArgs->x] != MINE_FLAG)
						mineArgs->board->leftClick(mineArgs->x, mineArgs->y);
					if (Fl::event_button() == FL_RIGHT_MOUSE)
						mineArgs->board->rightClick(mineArgs->x, mineArgs->y);
					mineArgs->parent->update();
					mineArgs->parent->m_btnCallback->unlock();
				}
			}, (void*)mineArgs);
			mineArgs->board = board;
			mineArgs->x = j;
			mineArgs->y = i;
			mineArgs->parent = this;
			mines.push_back(mineArgs);
		}
		mineList.push_back(mines);
	}
}

void BoardWindow::update() {
	std::stringstream title("");
	title << "Game (" << boardArgs.column << "x" << boardArgs.row << ") " << std::fixed << std::setprecision(1) <<
		(double)boardArgs.openBlankCount / (boardArgs.openBlankCount + boardArgs.remainBlankCount) * 100 << "%" << std::endl;
	mainWindow->label(title.str().c_str());

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

	openResultWindow();
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
			if (currentValue != MINE_MINE) currentButton->set();
			currentButton->deactivate();
			currentButton->color(FL_GREEN);
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
		button.component->callback(button.callback, button.args);
	}
}

void BoardWindow::playAgain(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;
	bw->board->maskBoard();
	bw->initWindowTItle();
	bw->update();
	bw->mainWindow->redraw();
	bw->closeResultWindow();
}

void BoardWindow::newGame(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;
	auto& boardArgs = bw->boardArgs;
	Board* b = nullptr;
	const char* path;

	std::stringstream title("");
	if (boardArgs.mode == MODE_READ_BOARD) {
		Fl_File_Chooser* chooser = new Fl_File_Chooser(boardArgs.path.c_str(), CHOOSER_ARGS);
		chooser->show();
		while (chooser->visible()) Fl::wait();
		if (!chooser->count()) {
			fl_alert("Please select a file");
			return;
		}
		path = chooser->value();
		title << "Load BoardFile " << chooser->value();
	}
	if (boardArgs.mode == MODE_INPUT_RATE) title << "Load RandomRate " << boardArgs.column << " " << boardArgs.row << " " << boardArgs.randomRate;
	if (boardArgs.mode == MODE_INPUT_COUNT) title << "Load RandomCount " << boardArgs.column << " " << boardArgs.row << " " << boardArgs.bombCount;

	Handler::execute(title.str().c_str(), [&] {
		switch (boardArgs.mode) {
			case MODE_READ_BOARD:
				b = new Board(path);
				break;
			case MODE_INPUT_RATE:
				b = new Board(boardArgs.row, boardArgs.column, boardArgs.randomRate);
				break;
			case MODE_INPUT_COUNT:
				b = new Board(boardArgs.row, boardArgs.column, boardArgs.bombCount);
				break;
			default: throw std::exception("Logic error");
		}
		if (b == nullptr) throw std::exception("Please select a file");
		auto boardWindow = new BoardWindow(b);
		boardWindow->mainWindow->resize(bw->mainWindow->x(), bw->mainWindow->y(), boardWindow->mainWindow->w(), boardWindow->mainWindow->h());
		boardWindow->mainWindow->show();
		bw->closeGame(NULL, args);
	});
}

void BoardWindow::submitScore(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;

}

void BoardWindow::openResultWindow() {
	resultWindow->label((boardArgs.status == BOARD_STATUS_WIN) ? "You Win!" : "You Lose");
	resultWindow->hotspot(0, 0);
	resultWindow->show();
}

void BoardWindow::closeResultWindow() {
	resultWindow->hide();
}

void BoardWindow::closeGame(Fl_Widget* w, void* args){
	auto bw = (BoardWindow*)args;
	bw->mainWindow->~Fl_Window();
	bw->closeResultWindow();
	delete bw;
}
