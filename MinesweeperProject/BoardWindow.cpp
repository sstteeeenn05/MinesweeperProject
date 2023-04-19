#include "BoardWindow.h"

std::unordered_set<BoardWindow*> BoardWindow::objAddrList;
const std::map<int, Fl_Image*> BoardWindow::IMG_ICON = {
		{MINE_MINE,(new Fl_PNG_Image("img/mine.png"))->copy(30, 30)},
		{MINE_FLAG,(new Fl_PNG_Image("img/flag.png"))->copy(30, 30)},
		{MINE_SUS,(new Fl_PNG_Image("img/sus.png"))->copy(30, 30)}
};
const std::map<int, Fl_Image*> BoardWindow::IMG_REACT = {
		{MINE_MINE,(new Fl_PNG_Image("img/joy.png"))->copy(30, 30)},
		{MINE_FLAG,(new Fl_PNG_Image("img/ayo.png"))->copy(30, 30)},
		{MINE_SUS,(new Fl_PNG_Image("img/flushed.png"))->copy(30, 30)},
		{MINE_NULL,(new Fl_PNG_Image("img/innocent.png"))->copy(30, 30)}
};
const std::map<int, Fl_Image*> BoardWindow::IMG_RESULT = {
		{BOARD_STATUS_LOSE,(new Fl_PNG_Image("img/joy.png"))->copy(180, 180)},
		{BOARD_STATUS_WIN,(new Fl_PNG_Image("img/innocent.png"))->copy(180, 180)}
};
int BoardWindow::playMode = PLAY_MODE_NORMAL;
std::mutex BoardWindow::btnCallbackLock;

int BoardWindow::getWindowCount() {
	return objAddrList.size();
}

bool BoardWindow::isWindowAvailable(BoardWindow* bw) {
	return objAddrList.find(bw) != objAddrList.end();
}

void BoardWindow::easterAll(Fl_Color c) {
	for (auto bw : objAddrList) {
		bw->mainWindow->color(c);
		for (auto& mines : bw->mineList) for (auto mine : mines) mine->button->color(c);
	}
}

void BoardWindow::closeAll() {
	for (auto bw : objAddrList) delete bw;
	PlaySoundA("audio\\funkytown.wav", NULL, SND_LOOP | SND_ASYNC);
}

BoardWindow::~BoardWindow() {
	removeMine();
	delete board;
	delete mainWindow;
	delete resultWindow;
	board = nullptr;
	objAddrList.erase(this);
	PlaySound(NULL, NULL, SND_ASYNC);
}

void BoardWindow::reload(int x, int y) {
	mainWindow->resize(
		x == -1 ? mainWindow->x() : x,
		y == -1 ? mainWindow->y() : y,
		MARGIN * 2 + BTN_MINE_SIZE * boardArgs.column,
		MARGIN * 3 + TOOLBAR_HEIGHT + BTN_MINE_SIZE * boardArgs.row
	);
	removeMine();

	mainWindow->begin();
	initToolbar();
	initMine();
	mainWindow->end();
	if (playMode == PLAY_MODE_DEV) mainWindow->callback((Fl_Callback*)nullptr);
	else mainWindow->callback([](Fl_Widget* w, void* args) {
		delete (BoardWindow*)args;
		PlaySoundA("audio\\funkytown.wav", NULL, SND_LOOP | SND_ASYNC);
	}, (void*)this);

	initResultWindow();

	mainWindow->show();
	mainWindow->icon((Fl_RGB_Image*)IMG_ICON.at(MINE_SUS));
	updateWindowTitle();
	updateToolbar();
	update();

	objAddrList.insert(this);
	PlaySoundA("audio\\amongus.wav", NULL, SND_LOOP | SND_ASYNC);
	resetTimer();
}

void BoardWindow::initToolbar() {
	timer = new Fl_Timer(FL_VALUE_TIMER, MARGIN, MARGIN, (mainWindow->w() - MARGIN * 4 - TOOLBAR_HEIGHT) / 2, TOOLBAR_HEIGHT, "");
	timer->direction(1);
	timer->labelfont(FL_SCREEN_BOLD);
	timer->callback([](Fl_Widget* w, void*) {fl_alert(std::to_string(((Fl_Timer*)w)->value()).c_str()); });

	reaction = new Fl_Box(FL_UP_BOX, MARGIN * 2 + timer->w(), MARGIN, TOOLBAR_HEIGHT, TOOLBAR_HEIGHT, nullptr);

	remainBombCountView = new Fl_Box(FL_DOWN_BOX, MARGIN * 3 + timer->w() + reaction->w(), MARGIN, timer->w(), TOOLBAR_HEIGHT, "");
	remainBombCountView->labelfont(FL_SCREEN_BOLD);
}

void BoardWindow::initMine(){
	for (int i = 0; i < boardArgs.row; i++) {
		std::vector<MineArgs*> mines;
		for (int j = 0; j < boardArgs.column; j++) {
			MineArgs* mineArgs = new MineArgs();
			mineArgs->button = new Fl_Button(MARGIN + BTN_MINE_SIZE * j, MARGIN * 2 + TOOLBAR_HEIGHT + BTN_MINE_SIZE * i, BTN_MINE_SIZE, BTN_MINE_SIZE);
			mineArgs->button->callback([](Fl_Widget* w, void* args) {
				auto mineArgs = (MineArgs*)args;
				BoardWindow::btnCallbackLock.lock();
				std::string title = (std::stringstream(" ") << mineArgs->y << " " << mineArgs->x).str();
				if (Fl::event_button() == FL_LEFT_MOUSE)
					Handler::execute("Leftclick" + title, [&] { mineArgs->board->leftClick(mineArgs->x, mineArgs->y); });
				if (Fl::event_button() == FL_RIGHT_MOUSE)
					Handler::execute("Rightclick" + title, [&] { mineArgs->board->rightClick(mineArgs->x, mineArgs->y); });
				mineArgs->parent->updateToolbar(mineArgs->parent->boardArgs.board[mineArgs->y][mineArgs->x]);
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

void BoardWindow::resetTimer() {
	timer->value(1LL << 16);
	timer->suspended(false);
	startTime = clock();
}

void BoardWindow::updateWindowTitle() {
	std::ostringstream title("");
	title << "Game (" << boardArgs.column << "x" << boardArgs.row << ") 0.0%" << std::endl;
	mainWindow->label(title.str().c_str());
}

void BoardWindow::updateToolbar(char value) {
	if (IMG_REACT.find(value) != IMG_REACT.end()) reaction->image(IMG_REACT.at(value));
	else reaction->image(IMG_REACT.at(MINE_NULL));
	reaction->redraw();

	std::string remainCount = std::to_string(boardArgs.bombCount - boardArgs.flagCount);
	remainBombCountView->label(remainCount.c_str());
	remainBombCountView->redraw();

	Fl::flush();
}

void BoardWindow::update() {
	endTime = clock();

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
		currentButton->redraw();
	}

	Fl::flush();

	switch (boardArgs.status) {
		case BOARD_STATUS_WIN:
			timer->suspended(true);
			win();
			break;
		case BOARD_STATUS_LOSE:
			timer->suspended(true);
			lose();
			break;
		case BOARD_STATUS_CONTINUE:
			return;
	}

	if(playMode!=PLAY_MODE_REPLAY) openResultWindow();
}

void BoardWindow::win() {
	PlaySoundA("audio\\win.wav", NULL, SND_ASYNC);
	Handler::execute("", [] {hout << "You win the game"; });
	for (int i = 0; i < boardArgs.row; i++) {
		for (int j = 0; j < boardArgs.column; j++) {
			clock_t start = clock();
			auto currentButton = mineList[i][j]->button;
			const auto currentValue = (int)boardArgs.board[i][j];
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
	PlaySoundA("audio\\boom.wav", NULL, SND_ASYNC);
	Handler::execute("", [] {hout << "You lose the game"; });
	for (int i = 0; i < boardArgs.row; i++) {
		for (int j = 0; j < boardArgs.column; j++) {
			clock_t start = clock();
			auto currentButton = mineList[i][j]->button;
			const auto currentValue = (int)boardArgs.board[i][j];
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
	createResultButton();

	resultWindow->end();
}

void BoardWindow::initResultVariables() {
	if (playMode == PLAY_MODE_NORMAL) resultButtonList = {
		{"Play Again", {0, &BoardWindow::playAgain, (void*)this}},
		{"Regenerate", {1, &BoardWindow::newGame, (void*)this}},
		{"Submit Score", {2, &BoardWindow::submitScore, (void*)this}},
		{"Exit",{3, [](Fl_Widget* w,void* args) {
			delete (BoardWindow*)args;
			PlaySoundA("audio\\funkytown.wav", NULL, SND_LOOP | SND_ASYNC);
		}, (void*)this}}
	};
	if(playMode == PLAY_MODE_DEV) resultButtonList = {
		{"Replay", {0, [](Fl_Widget* w, void* args) {
			delete (BoardWindow*)args;
			PlaySoundA("audio\\funkytown.wav", NULL, SND_LOOP | SND_ASYNC);
		},(void*)this}},
		{"Quit",{1,[](Fl_Widget* w,void* args) {
			Handler::execute("Quit",[] {});
			HomeWindow::close(w,args);
		}}}
	};
	resultLabel->labelfont(FL_SCREEN_BOLD);
	resultLabel->labelsize(RESULT_LABEL_SIZE);
	if (playMode == PLAY_MODE_NORMAL) {
		resultWindow->add(resultImage);
		resultWindow->add(resultLabel);
		resultWindow->add(resultTime);
	} else {
		resultWindow->remove(resultImage);
		resultWindow->remove(resultLabel);
		resultWindow->remove(resultTime);
	}
	resultWindow->resize(resultWindow->x(), resultWindow->y(), MARGIN + (RESULT_BUTTON_WIDTH + MARGIN) * resultButtonList.size(), playMode == PLAY_MODE_NORMAL ? RESULT_WINDOW_HEIGHT : (RESULT_BUTTON_HEIGHT + MARGIN * 2));
}

void BoardWindow::createResultButton() {
	for (auto& button : resultButtonList) {
		auto component = new Fl_Button(MARGIN + (MARGIN + RESULT_BUTTON_WIDTH) * button.second.index, playMode == PLAY_MODE_NORMAL ? (MARGIN * 2 + RESULT_IMG_SIZE) : MARGIN, RESULT_BUTTON_WIDTH, RESULT_BUTTON_HEIGHT, button.first);
		button.second.component = component;
		button.second.component->callback(button.second.callback, button.second.args);
	}
}

void BoardWindow::playAgain(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;
	bw->board->maskBoard();
	bw->updateWindowTitle();
	bw->updateToolbar();
	bw->update();
	bw->mainWindow->redraw();
	bw->resultWindow->hide();
	bw->board->startGame();
	PlaySoundA("audio\\amongus.wav", NULL, SND_LOOP | SND_ASYNC);
	bw->resetTimer();
}

void BoardWindow::newGame(Fl_Widget* w, void* args) {
	auto oldBoardWindow = (BoardWindow*)args;
	auto newBoardWindow = new BoardWindow();
	auto &oldBoardArgs = oldBoardWindow->boardArgs;
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

	int x = oldBoardWindow->mainWindow->x();
	int y = oldBoardWindow->mainWindow->y();
	delete oldBoardWindow;
	newBoard->startGame();
	newBoardWindow->reload(x, y);
}

void BoardWindow::submitScore(Fl_Widget* w, void* args) {
	auto bw = (BoardWindow*)args;
	const char* userName = fl_input("Please enter your name",nullptr);
	if (!userName) return fl_alert("Submit failed");
	bw->board->submitScore(std::string(userName), ((double)bw->endTime - bw->startTime) / CLOCKS_PER_SEC);
	bw->resultButtonList["Submit Score"].component->deactivate();
}

void BoardWindow::openResultWindow() {
	std::string time = "Duration:\n" + std::to_string(((double)endTime - startTime) / CLOCKS_PER_SEC);
	resultWindow->label(boardArgs.status == BOARD_STATUS_WIN ? "You Win!" : "You Lose!");
	resultImage->image(IMG_RESULT.at(boardArgs.status));
	resultLabel->label(boardArgs.status == BOARD_STATUS_WIN ? "You Win!" : "You Lose!");
	resultTime->label(time.c_str());
	if (playMode == PLAY_MODE_NORMAL) {
		auto submitButton = resultButtonList["Submit Score"].component;
		if (!submitButton->active()) submitButton->label("Can't submit\nafter replay");
		if (boardArgs.status == BOARD_STATUS_LOSE) {
			submitButton->deactivate();
		}
		if (boardArgs.mode == MODE_READ_BOARD) {
			submitButton->deactivate();
			submitButton->label("Can't submit in\nRead-board mode");
		}
	}
	else {
		resultWindow->resize(resultWindow->x(), resultWindow->y(), resultWindow->w(), RESULT_BUTTON_HEIGHT + MARGIN * 2);
	}
	resultWindow->hotspot(0, 0);
	resultWindow->show();
}