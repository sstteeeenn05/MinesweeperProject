#include "HomeWindow.h"

HomeWindow::HomeWindow() {
	initVariables();

	mainWindow->begin();
	initLogo();
	createModeList();
	createBoardChooser();
	createInput();
	createButton();
	initDefaultChoice();
	mainWindow->end();
	mainWindow->callback(HomeWindow::close, this);

	devWindow->begin();
	
	devWindow->end();
}

HomeWindow::~HomeWindow() {
	delete devWindow;
	delete logo;
}

void HomeWindow::open() {
	mainWindow->show();
}

void HomeWindow::initLogo() {
	logo = new Fl_Button(MARGIN, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT);
	logo->image((new Fl_PNG_Image("img/logo.png"))->copy(LOGO_WIDTH, LOGO_HEIGHT));
	logo->callback([](Fl_Widget* w, void* args) {
		system("start https://www.youtube.com/watch?v=dQw4w9WgXcQ");
	});
}

void HomeWindow::initVariables() {
	gameArgs->modeArgs = modeArgs;

	modeList = {
		{ "Read Board File", &HomeWindow::radioCallback },
		{ "Input Mine Count", &HomeWindow::radioCallback },
		{ "Input Respawn Rate", &HomeWindow::radioCallback }
	};

	buttonList = {
		{"New Game", &HomeWindow::startGame, gameArgs},
		{"Leaderboard", &HomeWindow::openRank, rankArgs},
		{"Exit", &HomeWindow::close, this}
	};
}

void HomeWindow::createModeList() {
	modeArgs->mode = new Fl_Choice(LABEL_WIDTH, COMPONENT_Y[0], MENU_MODE_WIDTH, COMPONENT_HEIGHT, "Mode:");
	for (auto &item : modeList) modeArgs->mode->add(item.text, "", item.callback, modeArgs);

	auto devButton = new Fl_Light_Button(LABEL_WIDTH + MENU_MODE_WIDTH + MARGIN, COMPONENT_Y[0], 130, COMPONENT_HEIGHT, " Developer Mode");
	devButton->callback([](Fl_Widget* w, void* args) {
		auto devButton = (Fl_Light_Button*)w;
		auto devWindow = (Fl_Window*)args;
		if (devButton->value()) devWindow->show();
		else devWindow->hide();
	},devWindow);
}

void HomeWindow::createBoardChooser() {

	auto chooser = new Fl_File_Chooser("board.txt", CHOOSER_ARGS);
	auto iptPath = new Fl_Input(LABEL_WIDTH, COMPONENT_Y[1], PATH_WIDTH, COMPONENT_HEIGHT, "Path:");
	auto btnChooser = new Fl_Button(WINDOW_WIDTH - CHOOSER_WIDTH - MARGIN, COMPONENT_Y[1], CHOOSER_WIDTH, COMPONENT_HEIGHT, "...");

	modeArgs->iptPath = iptPath;
	modeArgs->chooser = chooser;
	modeArgs->btnChooser = btnChooser;

	chooser->callback([](Fl_File_Chooser* c, void* args) {
		std::string path = c->value();
		if (path.substr(path.find_last_of(".") + 1) == "txt" && !c->shown()) {
			auto modeArgs = (ModeArgs*)args;
			modeArgs->boardPath = path;
			modeArgs->iptPath->value(path.c_str());
		}
	}, modeArgs);

	btnChooser->callback([](Fl_Widget* w, void* args) {
		auto modeArgs = (ModeArgs*)args;
		modeArgs->chooser->show();
	}, modeArgs);
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
		auto modeArgs = (ModeArgs*)args;
		modeArgs->number = min(input->value(), input->maximum());
		input->value(modeArgs->number);
	}, modeArgs);

	auto iptSizeCallback = [](Fl_Widget* w, void* args) {
		auto input = (Fl_Spinner*)w;
		auto modeArgs = (ModeArgs*)args;

		modeArgs->col = modeArgs->iptColumn->value();
		modeArgs->row = modeArgs->iptRow->value();
		modeArgs->iptNumber->range(1, (modeArgs->selection == MODE_INPUT_RATE) ? MAX_PERCENT : modeArgs->col * modeArgs->row);
		modeArgs->iptNumber->do_callback();
	};
	iptColumn->callback(iptSizeCallback, modeArgs);
	iptRow->callback(iptSizeCallback, modeArgs);

	btnRandom->callback([](Fl_Widget* w, void* args) {
		auto modeArgs = (ModeArgs*)args;
		if (!modeArgs->iptNumber || !modeArgs->iptColumn || !modeArgs->iptRow) return;

		if (modeArgs->selection==MODE_INPUT_COUNT)
			modeArgs->number = rand() % (modeArgs->col * modeArgs->row) + 1;
		if (modeArgs->selection == MODE_INPUT_RATE)
			modeArgs->number = rand() % MAX_PERCENT + 1;

		modeArgs->iptNumber->value(modeArgs->number);
	}, modeArgs);
}

void HomeWindow::initDefaultChoice(){
	modeArgs->mode->value(0);
	radioCallback(NULL, modeArgs);
}

void HomeWindow::radioCallback(Fl_Widget* w, void* args) {
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

void HomeWindow::createButton() {
	int buttonIndex = 0;
	for (auto &button : buttonList) {
		Fl_Button* component = new Fl_Button(MARGIN + (MARGIN + BUTTON_WIDTH) * (buttonIndex++), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, button.text);
		button.component = component;
		button.component->callback(button.callback, button.args);
	}
}

void HomeWindow::startGame(Fl_Widget* w, void* args) {
	auto gameArgs = (GameArgs*)args;
	auto modeArgs = gameArgs->modeArgs;
	modeArgs->iptNumber->do_callback();

	std::stringstream title("");
	if (modeArgs->selection == MODE_READ_BOARD) title << "Load BoardFile " << modeArgs->boardPath;
	if (modeArgs->selection == MODE_INPUT_RATE) title << "Load RandomRate " << modeArgs->col << " " << modeArgs->row << " " << modeArgs->number / MAX_PERCENT;
	if (modeArgs->selection == MODE_INPUT_COUNT) title << "Load RandomCount " << modeArgs->col << " " << modeArgs->row << " " << modeArgs->number;

	if (!Handler::execute(title.str().c_str(), [&] {
		switch (modeArgs->selection) {
			case MODE_READ_BOARD: {
				if (!modeArgs->boardPath.length()) throw std::exception("Please select a board file or try another mode");
				gameArgs->board = new Board(modeArgs->iptPath->value());
				break;
			} case MODE_INPUT_COUNT: {
				gameArgs->board = new Board(modeArgs->row, modeArgs->col, (int)modeArgs->number);
				break;
			} case MODE_INPUT_RATE: {
				gameArgs->board = new Board(modeArgs->row, modeArgs->col, modeArgs->number / MAX_PERCENT);
				break;
			} default: throw std::exception("Logic error");
		}
	})) return;

	gameArgs->mainWindow = new BoardWindow(gameArgs->board);
	auto boardWindow = (BoardWindow*)gameArgs->mainWindow;
	boardWindow->mainWindow->show();

	while (BoardWindow::getWindowCount()) Fl::wait();
	fl_alert("Game Stopped");

	gameArgs = new GameArgs();
	modeArgs = new ModeArgs();
	gameArgs->modeArgs = modeArgs;
}

void HomeWindow::openRank(Fl_Widget* w, void* args) {
	delete w;
	exit(EXIT_SUCCESS);
}

void HomeWindow::close(Fl_Widget* w, void* args) {
	auto window = ((HomeWindow*)args)->mainWindow;
	window->hide();
}