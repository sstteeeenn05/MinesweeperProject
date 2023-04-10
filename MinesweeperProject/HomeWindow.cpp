#include "HomeWindow.h"

HomeWindow::HomeWindow() {
	mainWindow->begin();

	logo = new Fl_Button(MARGIN, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT);
	logo->image((new Fl_PNG_Image("img/logo.png"))->copy(LOGO_WIDTH, LOGO_HEIGHT));
	logo->callback([](Fl_Widget* w, void* args) {
		system("start https://www.youtube.com/watch?v=dQw4w9WgXcQ");
	});

	initVariables();
	initRadioArgs();
	initRadioReadBoard();
	initRadioInput();
	initButtonArgs();
	initDefaultRadio();

	mainWindow->callback(HomeWindow::close);
	mainWindow->end();
}

void HomeWindow::open() {
	mainWindow->show();
}

void HomeWindow::initVariables() {
	gameArgs->radioArgs = radioArgs;

	radioList = {
		{ "Read Board File", &HomeWindow::radioCallback, (void*)radioArgs },
		{ "Input Mine Count", &HomeWindow::radioCallback, (void*)radioArgs },
		{ "Input Respawn Rate", &HomeWindow::radioCallback, (void*)radioArgs }
	};

	resultButtonList = {
		{"New Game",&HomeWindow::startGame,(void*)gameArgs},
		{"Leaderboard",&HomeWindow::openRank,(void*)rankArgs},
		{"Exit",&HomeWindow::close,(void*)mainWindow}
	};
}

void HomeWindow::initRadioArgs() {
	new Fl_Light_Button(LABEL_WIDTH + MENU_MODE_WIDTH + MARGIN, COMPONENT_Y[0], 130, COMPONENT_HEIGHT, " Developer Mode");
	radioArgs->mode = new Fl_Choice(LABEL_WIDTH, COMPONENT_Y[0], MENU_MODE_WIDTH, COMPONENT_HEIGHT, "Mode:");
	int radioCount = 0;
	for (auto &radio : radioList) radioArgs->mode->add(radio.text, "", radio.callback, radioArgs);
}

void HomeWindow::initRadioReadBoard() {

	Fl_File_Chooser* chooser = new Fl_File_Chooser("board.txt", CHOOSER_ARGS);
	Fl_Input* iptPath = new Fl_Input(LABEL_WIDTH, COMPONENT_Y[1], PATH_WIDTH, COMPONENT_HEIGHT, "Path:");
	Fl_Button* btnChooser = new Fl_Button(WINDOW_WIDTH - CHOOSER_WIDTH - MARGIN, COMPONENT_Y[1], CHOOSER_WIDTH, COMPONENT_HEIGHT, "...");

	radioArgs->iptPath = iptPath;
	radioArgs->chooser = chooser;
	radioArgs->btnChooser = btnChooser;

	chooser->callback([](Fl_File_Chooser* c, void* args) {
		std::string path = c->value();
		if (path.substr(path.find_last_of(".") + 1) == "txt" && !c->shown()) {
			auto radioArgs = (RadioArgs*)args;
			radioArgs->boardPath = path;
			radioArgs->iptPath->value(path.c_str());
		}
	}, (void*)radioArgs);

	btnChooser->callback([](Fl_Widget* w, void* args) {
		auto radioArgs = (RadioArgs*)args;
		radioArgs->chooser->show();
	}, (void*)radioArgs);
}

void HomeWindow::initRadioInput() {
	Fl_Spinner* iptColumn = new Fl_Spinner(LABEL_WIDTH, COMPONENT_Y[2], INPUT_WIDTH, COMPONENT_HEIGHT, "Col:");
	Fl_Spinner* iptRow = new Fl_Spinner(LABEL_WIDTH + (LABEL_WIDTH + INPUT_WIDTH) * 1, COMPONENT_Y[2], INPUT_WIDTH, COMPONENT_HEIGHT, "Row:");
	Fl_Spinner* iptNumber = new Fl_Spinner(VALUE_WIDTH + (LABEL_WIDTH + INPUT_WIDTH) * 2, COMPONENT_Y[2], INPUT_WIDTH, COMPONENT_HEIGHT, " Count : ");
	Fl_Button* btnRandom = new Fl_Button(WINDOW_WIDTH - BTN_RANDOM_WIDTH - MARGIN, COMPONENT_Y[2], BTN_RANDOM_WIDTH, COMPONENT_HEIGHT, "Random");

	iptNumber->type(FL_INT_INPUT);
	iptColumn->type(FL_INT_INPUT);
	iptRow->type(FL_INT_INPUT);
	iptNumber->value(DEF_CNT);
	iptColumn->value(DEF_COL);
	iptRow->value(DEF_ROW);
	iptNumber->range(1, DEF_COL * DEF_ROW);
	iptColumn->range(1, MAX_COL);
	iptRow->range(1, MAX_ROW);

	radioArgs->iptNumber = iptNumber;
	radioArgs->iptColumn = iptColumn;
	radioArgs->iptRow = iptRow;
	radioArgs->btnRandom = btnRandom;

	iptNumber->callback([](Fl_Widget* w, void* args) {
		auto input = (Fl_Spinner*)w;
		auto radioArgs = (RadioArgs*)args;
		radioArgs->number = min(input->value(), input->maximum());
		input->value(radioArgs->number);
	}, (void*)radioArgs);

	auto iptSizeCallback = [](Fl_Widget* w, void* args) {
		auto input = (Fl_Spinner*)w;
		auto radioArgs = (RadioArgs*)args;

		radioArgs->col = radioArgs->iptColumn->value();
		radioArgs->row = radioArgs->iptRow->value();
		radioArgs->iptNumber->range(1, (radioArgs->selection == MODE_INPUT_RATE) ? MAX_PERCENT : radioArgs->col * radioArgs->row);
		radioArgs->iptNumber->do_callback();
	};
	iptColumn->callback(iptSizeCallback, (void*)radioArgs);
	iptRow->callback(iptSizeCallback, (void*)radioArgs);

	btnRandom->callback([](Fl_Widget* w, void* args) {
		auto radioArgs = (RadioArgs*)args;
		if (!radioArgs->iptNumber || !radioArgs->iptColumn || !radioArgs->iptRow) return;

		if (radioArgs->selection==MODE_INPUT_COUNT)
			radioArgs->number = rand() % (radioArgs->col * radioArgs->row) + 1;
		if (radioArgs->selection == MODE_INPUT_RATE)
			radioArgs->number = rand() % MAX_PERCENT + 1;

		radioArgs->iptNumber->value(radioArgs->number);
	}, (void*)radioArgs);
}

void HomeWindow::initDefaultRadio(){
	radioArgs->mode->value(0);
	radioCallback(NULL, radioArgs);
}

void HomeWindow::radioCallback(Fl_Widget* w, void* args) {
	srand(time(NULL));
	auto radioArgs = (RadioArgs*)args;
	radioArgs->selection = radioArgs->mode->value();
	switch (radioArgs->selection) {
		case MODE_READ_BOARD:
			radioArgs->iptPath->activate();
			radioArgs->btnChooser->activate();
			radioArgs->iptNumber->deactivate();
			radioArgs->iptColumn->deactivate();
			radioArgs->iptRow->deactivate();
			radioArgs->btnRandom->deactivate();
			break;
		case MODE_INPUT_COUNT:
		case MODE_INPUT_RATE:
			radioArgs->iptPath->deactivate();
			radioArgs->btnChooser->deactivate();
			if (radioArgs->selection == MODE_INPUT_COUNT) radioArgs->iptNumber->label(" Count : ");
			if (radioArgs->selection == MODE_INPUT_RATE) radioArgs->iptNumber->label("Chance:");
			radioArgs->iptNumber->activate();
			radioArgs->iptColumn->activate();
			radioArgs->iptRow->activate();
			radioArgs->btnRandom->activate();
			break;
	}
	radioArgs->iptNumber->range(1, (radioArgs->selection == MODE_INPUT_RATE) ? MAX_PERCENT : radioArgs->col * radioArgs->row);
	radioArgs->iptNumber->do_callback();
}

void HomeWindow::initButtonArgs() {
	int buttonIndex = 0;
	for (auto &button : resultButtonList) {
		Fl_Button* component = new Fl_Button(MARGIN + (MARGIN + BUTTON_WIDTH) * (buttonIndex++), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, button.text);
		button.component = component;
		button.component->callback(button.callback, button.args);
	}
}

void HomeWindow::startGame(Fl_Widget* w, void* args) {
	auto gameArgs = (GameArgs*)args;
	auto radioArgs = gameArgs->radioArgs;
	radioArgs->iptNumber->do_callback();

	try {
		switch (radioArgs->selection) {
			case MODE_READ_BOARD: {
				if(!strlen(radioArgs->iptPath->value())) throw std::exception("Please select a board file or try another mode");
				gameArgs->board = new Board(radioArgs->iptPath->value());
				break;
			} case MODE_INPUT_COUNT: {
				gameArgs->board = new Board(radioArgs->iptRow->value(), radioArgs->iptColumn->value(), (int)radioArgs->iptNumber->value());
				break;
			} case MODE_INPUT_RATE: {
				gameArgs->board = new Board(radioArgs->iptRow->value(), radioArgs->iptColumn->value(), radioArgs->iptNumber->value() / MAX_PERCENT);
				break;
			} default: throw std::exception("Logic error");
		}
	} catch (std::exception e) {
		fl_alert(e.what());
		return;
	}

	gameArgs->mainWindow = new BoardWindow(gameArgs->board);
	auto boardWindow = (BoardWindow*)gameArgs->mainWindow;
	boardWindow->mainWindow->show();

	while (BoardWindow::getWindowCount()) Fl::wait();
	fl_alert("Game Stopped");

	gameArgs = new GameArgs();
	radioArgs = new RadioArgs();
	gameArgs->radioArgs = radioArgs;
}

void HomeWindow::openRank(Fl_Widget* w, void* args) {
	w->~Fl_Widget();
	exit(EXIT_SUCCESS);
}

void HomeWindow::close(Fl_Widget* w, void* args) {
	if(args) ((Fl_Widget*)args)->~Fl_Widget();
	else w->~Fl_Widget();
}