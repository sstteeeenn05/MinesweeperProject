#include "HomeWindow.h"

using namespace HomeWindow;

Fl_Window* HomeWindow::mainWindow = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper");
Fl_Button* HomeWindow::devButton;
Fl_Button* HomeWindow::logo;
GameArgs* HomeWindow::gameArgs = new GameArgs();
ModeArgs* HomeWindow::modeArgs = new ModeArgs();
std::vector<std::pair<const char*, Widget>> HomeWindow::modeListItem;
std::map<const char*, Widget> HomeWindow::buttonList;

Fl_Window* HomeWindow::devWindow = new Fl_Window(270, 100, "Dev Toolkit");
Fl_Button* HomeWindow::printButton;
Fl_Choice* HomeWindow::printList;
std::vector<std::pair<const char*, Widget>> HomeWindow::printListItem;
Fl_Slider* HomeWindow::volumeSlider;

Fl_Window* HomeWindow::rankWindow = new Fl_Window(WINDOW_WIDTH, WINDOW_WIDTH, "LeaderBoard");
std::vector<RankArgs*> HomeWindow::leaderboardList;

int HomeWindow::loadCounter = 0;
bool HomeWindow::easterEgg = false;
Fl_Timer* HomeWindow::easterEggTimer = new Fl_Timer(FL_HIDDEN_TIMER,0,0,0,0,"");

void HomeWindow::open() {
	PlaySound(L"audio\\funkytown.wav", NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
	initVariables();

	mainWindow->begin();
	initMainWindow();
	mainWindow->end();
	mainWindow->callback(HomeWindow::close);

	devWindow->begin();
	initDevWindow();
	devWindow->end();
	devWindow->callback([](Fl_Widget*, void*) {
		HomeWindow::devButton->value(0);
		HomeWindow::devButton->do_callback();
	});

	mainWindow->show();
}

void HomeWindow::initVariables() {
	gameArgs->window = new BoardWindow();
	gameArgs->modeArgs = modeArgs;

	modeListItem = {
		{"Read Board File", {MODE_READ_BOARD, &HomeWindow::selectMode}},
		{"Input Mine Count", {MODE_INPUT_COUNT, &HomeWindow::selectMode}},
		{"Input Respawn Rate", {MODE_INPUT_RATE, &HomeWindow::selectMode}}
	};

	buttonList = {
		{"New Game", {0, &HomeWindow::loadGame}},
		{"Leaderboard", {1, &HomeWindow::openRank}},
		{"Exit", {2, &HomeWindow::close}}
	};

	printListItem = {
		{"GameState", {PRINT_STATE}},
		{"GameBoard", {PRINT_BOARD}},
		{"GameAnswer", {PRINT_ANSWER}},
		{"BombCount", {PRINT_BOMB_COUNT}},
		{"FlagCount", {PRINT_FLAG_COUNT}},
		{"OpenBlankCount", {PRINT_OPEN_BLANK}},
		{"RemainBlankCount", {PRINT_REMAIN_BLANK}}
	};

	easterEggTimer->value(0);
	mainWindow->color(FL_GRAY);
}

void HomeWindow::initMainWindow() {
	initLogo();
	createModeList();
	createDevButton();
	createBoardChooser();
	createInput();
	createButton();
	initDefaultChoice();
}

void HomeWindow::initLogo() {
	logo = new Fl_Button(MARGIN, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT);
	logo->image((new Fl_PNG_Image("img/logo.png"))->copy(LOGO_WIDTH, LOGO_HEIGHT));
	logo->callback([](Fl_Widget* w, void* args) {
		system("start https://www.youtube.com/watch?v=dQw4w9WgXcQ");
	});
}

void HomeWindow::createModeList() {
	modeArgs->mode = new Fl_Choice(LABEL_WIDTH, COMPONENT_Y[0], MENU_MODE_WIDTH, COMPONENT_HEIGHT, "Mode:");
	for (auto &item : modeListItem) modeArgs->mode->add(item.first, "", item.second.callback, modeArgs);
	mainWindow->add(modeArgs->mode);
}

void HomeWindow::createDevButton() {
	devButton = new Fl_Light_Button(LABEL_WIDTH + MENU_MODE_WIDTH + MARGIN, COMPONENT_Y[0], 130, COMPONENT_HEIGHT, " Developer Mode");
	devButton->color2(FL_GREEN);
	devButton->callback([](Fl_Widget* w, void* args) {
		auto devButton = (Fl_Light_Button*)w;
		devButton->value() ? openDevWindow() : closeDevWindow();
	});
	mainWindow->add(devButton);
}

void HomeWindow::openDevWindow() {
	auto firstButton = buttonList["New Game"].component;
	firstButton->label("Load");

	auto secondButton = buttonList["Leaderboard"].component;
	secondButton->label("Start Game");
	secondButton->callback((Fl_Callback*)&HomeWindow::startGame);

	BoardWindow::playMode = PLAY_MODE_DEV;
	Handler::enableOutput();
	mainWindow->show();
	devWindow->show();
}

void HomeWindow::closeDevWindow() {
	auto firstButton = buttonList["New Game"].component;
	firstButton->label("New Game");

	auto secondButton = buttonList["Leaderboard"].component;
	secondButton->label("Leaderboard");
	secondButton->callback((Fl_Callback*)&HomeWindow::openRank);

	BoardWindow::playMode = PLAY_MODE_NORMAL;
	Handler::disableOutput();
	devWindow->hide();
}

void HomeWindow::createBoardChooser() {
	auto chooser = new Fl_File_Chooser("board.txt", CHOOSER_ARGS);
	auto iptPath = new Fl_Input(LABEL_WIDTH, COMPONENT_Y[1], PATH_WIDTH, COMPONENT_HEIGHT, "Path:");
	auto btnChooser = new Fl_Button(WINDOW_WIDTH - CHOOSER_WIDTH - MARGIN, COMPONENT_Y[1], CHOOSER_WIDTH, COMPONENT_HEIGHT, "@fileopen");

	modeArgs->iptPath = iptPath;
	modeArgs->chooser = chooser;
	modeArgs->btnChooser = btnChooser;

	chooser->callback([](Fl_File_Chooser* c, void* args) {
		std::string path = c->value();
		if (path.substr(path.find_last_of(".") + 1) == "txt" && !c->shown()) {
			modeArgs->boardPath = path;
			modeArgs->iptPath->value(path.c_str());
		}
	});

	btnChooser->callback([](Fl_Widget* w, void* args) {
		modeArgs->chooser->show();
	});

	mainWindow->add(iptPath);
	mainWindow->add(btnChooser);
}

void HomeWindow::createInput() {
	auto iptColumn = new Fl_Spinner(LABEL_WIDTH, COMPONENT_Y[2], INPUT_WIDTH, COMPONENT_HEIGHT, "Col:");
	auto iptRow = new Fl_Spinner(LABEL_WIDTH + (LABEL_WIDTH + INPUT_WIDTH) * 1, COMPONENT_Y[2], INPUT_WIDTH, COMPONENT_HEIGHT, "Row:");
	auto iptNumber = new Fl_Spinner(VALUE_WIDTH + (LABEL_WIDTH + INPUT_WIDTH) * 2, COMPONENT_Y[2], INPUT_WIDTH, COMPONENT_HEIGHT, " Count : ");
	auto btnRandom = new Fl_Button(WINDOW_WIDTH - BTN_RANDOM_WIDTH - MARGIN, COMPONENT_Y[2], BTN_RANDOM_WIDTH, COMPONENT_HEIGHT, "Random");

	iptNumber->type(FL_INT_INPUT);
	iptColumn->type(FL_INT_INPUT);
	iptRow->type(FL_INT_INPUT);
	iptNumber->value(DEF_CNT);
	iptColumn->value(DEF_COL);
	iptRow->value(DEF_ROW);
	iptNumber->range(1, DEF_COL * DEF_ROW);
	iptColumn->range(1, MAX_COL);
	iptRow->range(1, MAX_ROW);

	modeArgs->iptNumber = iptNumber;
	modeArgs->iptColumn = iptColumn;
	modeArgs->iptRow = iptRow;
	modeArgs->btnRandom = btnRandom;

	iptNumber->callback([](Fl_Widget* w, void* args) {
		auto input = (Fl_Spinner*)w;
		modeArgs->number = min(input->value(), input->maximum());
		input->value(modeArgs->number);
	}, modeArgs);

	auto iptSizeCallback = [](Fl_Widget* w, void* args) {
		auto input = (Fl_Spinner*)w;

		modeArgs->col = modeArgs->iptColumn->value();
		modeArgs->row = modeArgs->iptRow->value();
		modeArgs->iptNumber->range(1, (modeArgs->selection == MODE_INPUT_RATE) ? MAX_PERCENT : modeArgs->col * modeArgs->row);
		modeArgs->iptNumber->do_callback();
	};
	iptColumn->callback(iptSizeCallback, modeArgs);
	iptRow->callback(iptSizeCallback, modeArgs);

	btnRandom->callback([](Fl_Widget* w, void* args) {
		if (!modeArgs->iptNumber || !modeArgs->iptColumn || !modeArgs->iptRow) return;

		if (modeArgs->selection==MODE_INPUT_COUNT)
			modeArgs->number = rand() % (modeArgs->col * modeArgs->row) + 1;
		if (modeArgs->selection == MODE_INPUT_RATE)
			modeArgs->number = rand() % MAX_PERCENT + 1;

		modeArgs->iptNumber->value(modeArgs->number);
	}, modeArgs);

	mainWindow->add(iptColumn);
	mainWindow->add(iptRow);
	mainWindow->add(iptNumber);
	mainWindow->add(btnRandom);
}

void HomeWindow::createButton() {
	for (auto& button : buttonList) {
		Fl_Button* component = new Fl_Button(MARGIN + (MARGIN + BUTTON_WIDTH) * button.second.index, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, button.first);
		button.second.component = component;
		button.second.component->callback(button.second.callback, button.second.args);
	}
}

void HomeWindow::initDefaultChoice(){
	modeArgs->mode->value(1);
	selectMode(NULL, modeArgs);
}

void HomeWindow::initDevWindow() {
	createPrintChoice();
	createVolumeSlider();
}

void HomeWindow::createPrintChoice() {
	printButton = new Fl_Button(MARGIN, MARGIN, 50, COMPONENT_HEIGHT, "Print");
	printButton->callback(HomeWindow::selectPrint);
	printList = new Fl_Choice(MARGIN * 2 + 50, MARGIN, 200, COMPONENT_HEIGHT);
	for (auto& item : printListItem) printList->add(item.first);
	printList->value(0);
}

void HomeWindow::createVolumeSlider() {
	volumeSlider = new Fl_Slider(FL_HOR_NICE_SLIDER, MARGIN, MARGIN * 2 + COMPONENT_HEIGHT, 260, 20, "Volumn:");
	volumeSlider->color2(FL_GREEN);
	volumeSlider->range(0, 255);
	DWORD volume;
	waveOutGetVolume(NULL, &volume);
	volumeSlider->value(volume >> 8);
	volumeSlider->callback(HomeWindow::changeVolume);
	volumeSlider->do_callback();
}

void HomeWindow::selectMode(Fl_Widget* w, void* args) {
	srand(time(NULL));
	auto modeArgs = (ModeArgs*)args;
	modeArgs->selection = modeArgs->mode->value();
	switch (modeArgs->selection) {
		case MODE_READ_BOARD:
			modeArgs->iptPath->activate();
			modeArgs->btnChooser->activate();
			modeArgs->iptNumber->deactivate();
			modeArgs->iptColumn->deactivate();
			modeArgs->iptRow->deactivate();
			modeArgs->btnRandom->deactivate();
			break;
		case MODE_INPUT_COUNT:
		case MODE_INPUT_RATE:
			modeArgs->iptPath->deactivate();
			modeArgs->btnChooser->deactivate();
			if (modeArgs->selection == MODE_INPUT_COUNT) modeArgs->iptNumber->label(" Count : ");
			if (modeArgs->selection == MODE_INPUT_RATE) modeArgs->iptNumber->label("Chance:");
			modeArgs->iptNumber->activate();
			modeArgs->iptColumn->activate();
			modeArgs->iptRow->activate();
			modeArgs->btnRandom->activate();
			break;
	}
	modeArgs->iptNumber->range(1, (modeArgs->selection == MODE_INPUT_RATE) ? MAX_PERCENT : modeArgs->col * modeArgs->row);
	modeArgs->iptNumber->do_callback();
}

void HomeWindow::loadGame(Fl_Widget* w, void* args) {
	modeArgs->iptNumber->do_callback();

	std::ostringstream title("");
	if (modeArgs->selection == MODE_READ_BOARD) title << "Load BoardFile " << modeArgs->boardPath;
	if (modeArgs->selection == MODE_INPUT_RATE) title << "Load RandomRate " << modeArgs->col << " " << modeArgs->row << " " << modeArgs->number / MAX_PERCENT;
	if (modeArgs->selection == MODE_INPUT_COUNT) title << "Load RandomCount " << modeArgs->col << " " << modeArgs->row << " " << modeArgs->number;

	auto bw = gameArgs->window;
	auto board = bw->board;

	Handler::execute(title.str(), [&] {
		switch (modeArgs->selection) {
			case MODE_READ_BOARD: {
				if (!modeArgs->boardPath.length()) throw std::exception("Please select a board file or try another mode");
				board->load(modeArgs->iptPath->value());
				break;
			} case MODE_INPUT_COUNT: {
				board->load(modeArgs->row, modeArgs->col, (int)modeArgs->number);
				break;
			} case MODE_INPUT_RATE: {
				board->load(modeArgs->row, modeArgs->col, modeArgs->number / MAX_PERCENT);
				break;
			} default: throw std::exception("Logic error");
		}
	});

	if (BoardWindow::playMode == PLAY_MODE_DEV) {
		loadCounter++;
		if (easterEgg = loadCounter >= 5) {
			//system("start https://www.youtube.com/watch?v=qMQ-y9dHE2k");
			easterEggTimer->callback(HomeWindow::easterEggCallback);
			easterEggTimer->value(0.02);
		}
	} else {
		loadCounter = 0;
		startGame(w, args);
	}
}

void HomeWindow::startGame(Fl_Widget* w, void* args) {
	auto bw = gameArgs->window;
	auto board = bw->board;

	if (!Handler::execute("StartGame", [&] {board->startGame(); })) return;

	auto devWindowCallback = std::move(devWindow->callback());
	devWindow->callback((Fl_Callback*)nullptr);
	devButton->deactivate();
	bw->mainWindow->show();
	bw->reload(-1, -1);

	if (BoardWindow::playMode == PLAY_MODE_NORMAL) gameArgs->window = new BoardWindow();
	while (BoardWindow::isWindowAvailable(bw)) Fl::wait();
	if (BoardWindow::playMode == PLAY_MODE_DEV) gameArgs->window = new BoardWindow();
	while (BoardWindow::getWindowCount()) Fl::wait();
	devWindow->callback(devWindowCallback);
	devButton->activate();
}

void HomeWindow::openRank(Fl_Widget* w, void* args) {
	delete w;
	exit(EXIT_SUCCESS);
}

void HomeWindow::close(Fl_Widget* w, void* args) {
	BoardWindow::closeAll();
	devButton->value(0);
	devButton->do_callback();
	devWindow->hide();
	mainWindow->hide();
	PlaySound(NULL, NULL, SND_ASYNC);
}

void HomeWindow::selectPrint(Fl_Widget* w, void* args){
	auto board = gameArgs->window->board;
	int selection = printList->value();

	Handler::execute(std::string("Print ") + std::string(printListItem[selection].first), [&] {
		board->print(selection);
	});
}

void HomeWindow::changeVolume(Fl_Widget* w, void* args) {
	auto volumeSlider = (Fl_Slider*)w;
	double volume = volumeSlider->value();
	waveOutSetVolume(NULL, ((DWORD)volume) << 8);
	std::string& label = *(new std::string("Volume:"));
	label += std::to_string((int)(volume / 255 * 1000) / 10) + "%";
	volumeSlider->label(label.c_str());
}

void HomeWindow::easterEggCallback(Fl_Widget* w, void* args){
	uchar r, g, b;
	uchar color[3] = { rand() % 128 ,rand() % 128 ,rand() % 128 };
	color[rand() % 3] += 128;
	Fl_Color c = fl_rgb_color(color[0], color[1], color[2]);
	mainWindow->color(c);
	BoardWindow::easterAll(c);
	Fl::redraw();
	easterEggTimer->value(0.02);
}