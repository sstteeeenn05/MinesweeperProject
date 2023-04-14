#include "BoardWindow.h"

std::unordered_set<BoardWindow*> BoardWindow::objAddrList;
std::mutex BoardWindow::btnCallbackLock;

int BoardWindow::getWindowCount() {
	return objAddrList.size();
}

bool BoardWindow::isWindowAvailable(BoardWindow* bw) {
	return objAddrList.find(bw) != objAddrList.end();
}

void BoardWindow::closeAll() {
	for (auto bw : objAddrList) delete bw;
}

BoardWindow::~BoardWindow() {
	delete board;
	delete mainWindow;
	delete resultWindow;
	board = nullptr;
	objAddrList.erase(this);
}

void BoardWindow::reload(int x, int y) {
	mainWindow->resize(
		x == -1 ? mainWindow->x() : x,
		y == -1 ? mainWindow->y() : y,
		MARGIN * 2 + BTN_MINE_SIZE * boardArgs.column,
		MARGIN * 2 + BTN_MINE_SIZE * boardArgs.row
	);
	removeMine();

	mainWindow->begin();
	initMine();
	mainWindow->end();
	mainWindow->callback([](Fl_Widget* w, void* args) { delete (BoardWindow*)args; }, (void*)this);

	initWindowTItle();
	initResultWindow();
	objAddrList.insert(this);
}

void BoardWindow::initWindowTItle() {
	std::ostringstream title("");
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
				BoardWindow::btnCallbackLock.lock();
				std::string title = (std::stringstream(" ") << mineArgs->x << " " << mineArgs->y).str();
				if (Fl::event_button() == FL_LEFT_MOUSE)
					Handler::execute("Leftclick" + title, [&] { mineArgs->board->leftClick(mineArgs->x, mineArgs->y); });
				if (Fl::event_button() == FL_RIGHT_MOUSE)
					Handler::execute("Rightclick" + title, [&] { mineArgs->board->rightClick(mineArgs->x, mineArgs->y); });
				mineArgs->parent->update();
				BoardWindow::btnCallbackLock.unlock();
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

void BoardWindow::removeMine() {
	for (auto& mines : mineList) for (auto& mine : mines) {
		delete mine->button;
		delete mine;
	}
	mineList.clear();
}

void BoardWindow::update() {
	std::ostringstream title("");
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

	for (int i = 0; i < boardArgs.row; i++) for (int j = 0; j < boardArgs.column; j++) {
		auto currentButton = mineList[i][j]->button;
		const auto currentValue = (int)boardArgs.board[i][j];
		switch (currentValue) {
			case MINE_FLAG:
			case MINE_SUS:
				currentButton->image(IMG_ICON.at(currentValue));
				break;
			case MINE_MASK:
			case MINE_MINE:
				currentButton->value(0);
				currentButton->color(FL_GRAY);
				currentButton->selection_color(FL_GRAY);
				currentButton->activate();
				break;
			default: 
				currentButton->label(new char[2] {boardArgs.board[i][j], '\0'});
			case MINE_NULL:
				currentButton->set();
				currentButton->deactivate();
				break;
		}
		Fl::awake();
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
				currentButton->image(IMG_ICON.at(MINE_MINE));
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

	resultWindow->end();
	resultWindow->callback([](Fl_Widget* w, void* args) { delete (BoardWindow*)args; }, (void*)this);
}

void BoardWindow::initResultVariables() {
	resultButtonList = {
		{"Play Again", &BoardWindow::playAgain, (void*)this},
		{"Regenerate", &BoardWindow::newGame, (void*)this},
		{"Submit Score", &BoardWindow::submitScore, (void*)this},
		{"Exit",[](Fl_Widget* w,void* args) { delete (BoardWindow*)args; },(void*)this}
	};
}

void BoardWindow::initResultButtonArgs() {
	int buttonIndex = 0;
	for (auto& button : resultButtonList) {
		auto component = new Fl_Button(MARGIN + (MARGIN + RESULT_BUTTON_WIDTH) * (buttonIndex++), MARGIN, RESULT_BUTTON_WIDTH, RESULT_BUTTON_HEIGHT, button.text);
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
	bw->resultWindow->hide();
	bw->board->startGame();
}

void BoardWindow::newGame(Fl_Widget* w, void* args) {
	auto oldBoardWindow = (BoardWindow*)args;
	auto newBoardWindow = new BoardWindow();
	auto &oldBoardArgs = oldBoardWindow->boardArgs;
	auto& newBoardArgs = newBoardWindow->boardArgs;
	auto newBoard = newBoardWindow->board;

	std::ostringstream title("");
	const char* path;
	if (oldBoardArgs.mode == MODE_READ_BOARD) {
		auto chooser = new Fl_File_Chooser(oldBoardArgs.path.c_str(), CHOOSER_ARGS);
		chooser->show();
		while (chooser->visible()) Fl::wait();
		if (!chooser->count()) {
			fl_alert("Please select a file");
			return;
		}
		path = chooser->value();
		title << "Load BoardFile " << chooser->value();
	}
	if (oldBoardArgs.mode == MODE_INPUT_RATE) title << "Load RandomRate " << oldBoardArgs.column << " " << oldBoardArgs.row << " " << oldBoardArgs.randomRate;
	if (oldBoardArgs.mode == MODE_INPUT_COUNT) title << "Load RandomCount " << oldBoardArgs.column << " " << oldBoardArgs.row << " " << oldBoardArgs.bombCount;

	if (!Handler::execute(title.str(), [&] {
		switch (oldBoardArgs.mode) {
			case MODE_READ_BOARD:
				newBoard->load(path);
				break;
			case MODE_INPUT_RATE:
				newBoard->load(oldBoardArgs.row, oldBoardArgs.column, oldBoardArgs.randomRate);
				break;
			case MODE_INPUT_COUNT:
				newBoard->load(oldBoardArgs.row, oldBoardArgs.column, oldBoardArgs.bombCount);
				break;
			default: throw std::exception("Logic error");
		}
	})) return delete newBoardWindow;

	newBoard->startGame();
	newBoardWindow->reload(oldBoardWindow->mainWindow->x(), oldBoardWindow->mainWindow->y());
	newBoardWindow->mainWindow->show();
	delete oldBoardWindow;
}

void BoardWindow::submitScore(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;

}

void BoardWindow::openResultWindow() {
	resultWindow->label((boardArgs.status == BOARD_STATUS_WIN) ? "You Win!" : "You Lose");
	resultWindow->hotspot(0, 0);
	resultWindow->show();
}
