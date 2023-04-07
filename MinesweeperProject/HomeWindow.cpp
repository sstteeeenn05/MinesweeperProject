#include <algorithm>
#include <cstdio>
#include <exception>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include "HomeWindow.h"
#include "DataType.h"

#include "FL/fl_ask.H"

HomeWindow::HomeWindow() {

	window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper");
	window->begin();

	logo = new Fl_Button(MARGIN, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT);
	logo->image((new Fl_PNG_Image("logo.png"))->copy(LOGO_WIDTH, LOGO_HEIGHT));
	logo->callback([](Fl_Widget* w, void* args) {
		system("start https://www.youtube.com/watch?v=dQw4w9WgXcQ");
	});

	initVariables();
	initRadios();
	initRadioReadBoard();
	initRadioInput();
	initButtons();
	initDefaultRadio();

	window->callback(HomeWindow::close);
	window->end();
}

void HomeWindow::open() {
	window->show();
}

void HomeWindow::initVariables() {
	gameArgs = new GameArgs();
	rankArgs = new RankArgs();
	radioArgs = new RadioArgs();
	gameArgs->radioArgs = radioArgs;

	radioList = {
		{ "Read Board File", &HomeWindow::radioCallback, (void*)radioArgs },
		{ "Input Mine Count", &HomeWindow::radioCallback, (void*)radioArgs },
		{ "Input Respawn Rate", &HomeWindow::radioCallback, (void*)radioArgs }
	};

	buttonList = {
		{"New Game",&HomeWindow::startGame,(void*)gameArgs},
		{"Leaderboard",&HomeWindow::openRank,(void*)rankArgs},
		{"Exit",&HomeWindow::close}
	};
}

void HomeWindow::initRadios() {
	for (auto &radio : radioList) {
		static int radioCount = 0;
		Fl_Radio_Round_Button* button = new Fl_Radio_Round_Button(MARGIN, RADIO_Y[radioCount++], RADIO_WIDTH, RADIO_HEIGHT, radio.text);
		button->callback((Fl_Callback*)radio.callback, radioArgs);
		radioArgs->buttons.push_back(button);
	}
}

void HomeWindow::initRadioReadBoard() {

	Fl_File_Chooser* chooser = new Fl_File_Chooser("board.txt", "Text Files(*.txt)", Fl_File_Chooser::SINGLE, "Select Board File");
	Fl_Input* iptPath = new Fl_Input(RADIO_WIDTH + LABEL_WIDTH + MARGIN * 2, RADIO_Y[RADIO_READ_BOARD], PATH_WIDTH, RADIO_HEIGHT, "Path:");
	Fl_Button* btnChooser = new Fl_Button(RADIO_WIDTH + LABEL_WIDTH + PATH_WIDTH + MARGIN * 3, RADIO_Y[RADIO_READ_BOARD], CHOOSER_WIDTH, RADIO_HEIGHT, "...");

	radioArgs->iptPath = iptPath;
	radioArgs->chooser = chooser;
	radioArgs->btnChooser = btnChooser;

	chooser->callback([](Fl_File_Chooser* c, void* args) {
		std::string path = c->value();
		if (path.substr(path.find_last_of(".") + 1) == "txt") {
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
	Fl_Int_Input* iptNumber = new Fl_Int_Input(RADIO_WIDTH + LABEL_WIDTH + MARGIN * 2, RADIO_Y[RADIO_INPUT_COUNT], VALUE_WIDTH, RADIO_HEIGHT, "Val:");
	Fl_Button* btnRandom = new Fl_Button(RADIO_WIDTH + LABEL_WIDTH + VALUE_WIDTH + MARGIN * 3, RADIO_Y[RADIO_INPUT_COUNT], BTN_RANDOM_WIDTH, RADIO_HEIGHT, "Random");
	Fl_Int_Input* iptColumn = new Fl_Int_Input(RADIO_WIDTH + LABEL_WIDTH + MARGIN * 2, RADIO_Y[RADIO_INPUT_RATE], INPUT_WIDTH, RADIO_HEIGHT, "Col:");
	Fl_Int_Input* iptRow = new Fl_Int_Input(RADIO_WIDTH + LABEL_WIDTH * 2 + INPUT_WIDTH + MARGIN * 3, RADIO_Y[RADIO_INPUT_RATE], INPUT_WIDTH, RADIO_HEIGHT, "Row:");

	iptNumber->value("10");
	iptColumn->value("10");
	iptRow->value("10");
	iptNumber->maximum_size(3);
	iptColumn->maximum_size(3);
	iptRow->maximum_size(3);

	radioArgs->iptNumber = iptNumber;
	radioArgs->iptColumn = iptColumn;
	radioArgs->iptRow = iptRow;
	radioArgs->btnRandom = btnRandom;

	iptNumber->callback([](Fl_Widget* w, void* args) {
		auto input = (Fl_Int_Input*)w;
		auto radioArgs = (RadioArgs*)args;
		if (!radioArgs->iptNumber || !radioArgs->iptColumn || !radioArgs->iptRow) return;

		if (radioArgs->selection == RADIO_INPUT_COUNT)
			radioArgs->number = max(min(std::stoi(input->value()), radioArgs->col * radioArgs->row), 1);
		if (radioArgs->selection == RADIO_INPUT_RATE)
			radioArgs->number = max(min(std::stoi(input->value()), 100), 1);

		input->value(std::to_string(radioArgs->number).c_str());
	}, (void*)radioArgs);

	auto iptSizeCallback = [](Fl_Widget* w, void* args) {
		auto input = (Fl_Int_Input*)w;
		auto radioArgs = (RadioArgs*)args;

		radioArgs->col = max(min(std::stoi(radioArgs->iptColumn->value()), MAX_COL), 1);
		radioArgs->row = max(min(std::stoi(radioArgs->iptRow->value()), MAX_ROW), 1);

		radioArgs->iptColumn->value(std::to_string(radioArgs->col).c_str());
		radioArgs->iptRow->value(std::to_string(radioArgs->row).c_str());
		radioArgs->iptNumber->do_callback();
	};
	iptColumn->callback(iptSizeCallback, (void*)radioArgs);
	iptRow->callback(iptSizeCallback, (void*)radioArgs);

	btnRandom->callback([](Fl_Widget* w, void* args) {
		auto radioArgs = (RadioArgs*)args;
		if (!radioArgs->iptNumber || !radioArgs->iptColumn || !radioArgs->iptRow) return;

		srand(time(NULL));
		if (radioArgs->selection==RADIO_INPUT_COUNT)
			radioArgs->number = rand() % (radioArgs->col * radioArgs->row) + 1;
		if (radioArgs->selection == RADIO_INPUT_RATE)
			radioArgs->number = rand() % 100 + 1;

		radioArgs->iptNumber->value(std::to_string(radioArgs->number).c_str());
	}, (void*)radioArgs);
}

void HomeWindow::initDefaultRadio(){
	auto radioReadBoard = (Fl_Radio_Round_Button*)radioArgs->buttons[RADIO_READ_BOARD];
	radioReadBoard->do_callback();
	radioReadBoard->take_focus();
	radioReadBoard->set();
}

void HomeWindow::radioCallback(Fl_Widget* w, void* args) {
	auto radio = (Fl_Radio_Round_Button*)w;
	auto radioArgs = (RadioArgs*)args;
	auto itr = std::find(radioArgs->buttons.begin(), radioArgs->buttons.end(), radio);
	if (itr == radioArgs->buttons.end()) return;
	int index = std::distance(radioArgs->buttons.begin(), itr);
	radioArgs->selection = index;
	switch (index) {
		case RADIO_READ_BOARD:
			radioArgs->iptPath->activate();
			radioArgs->btnChooser->activate();
			radioArgs->iptNumber->deactivate();
			radioArgs->iptColumn->deactivate();
			radioArgs->iptRow->deactivate();
			radioArgs->btnRandom->deactivate();
			break;
		case RADIO_INPUT_COUNT:
		case RADIO_INPUT_RATE:
			radioArgs->iptPath->deactivate();
			radioArgs->btnChooser->deactivate();
			if (index == RADIO_INPUT_COUNT) radioArgs->iptNumber->label("Count:");
			if (index == RADIO_INPUT_RATE) radioArgs->iptNumber->label("Rate(%):");
			radioArgs->iptNumber->activate();
			radioArgs->iptColumn->activate();
			radioArgs->iptRow->activate();
			radioArgs->btnRandom->activate();
			break;
	}
	radioArgs->iptNumber->do_callback();
}

void HomeWindow::initButtons() {
	for (auto &button : buttonList) {
		static int buttonIndex = 0;
		Fl_Button* component = new Fl_Button(MARGIN + (MARGIN + BUTTON_WIDTH) * (buttonIndex++), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, button.text);
		button.component = component;
		auto genericArgs = (GenericArgs*)button.args;
		if (genericArgs) genericArgs->button = component;
		button.component->callback((Fl_Callback*)button.callback, button.args);
	}
}

void HomeWindow::startGame(Fl_Widget* w, void* args) {
	auto gameArgs = (GameArgs*)args;
	auto radioArgs = gameArgs->radioArgs;
	radioArgs->iptNumber->do_callback();

	try {
		switch (radioArgs->selection) {
			case RADIO_READ_BOARD: {
				if(!strlen(radioArgs->iptPath->value())) throw std::exception("Please select a board file or try another mode");
				std::ifstream file(radioArgs->iptPath->value());
				if (!file.is_open()) throw std::exception("The board file does not exist");
				gameArgs->board = new Board(file);
				break;
			} case RADIO_INPUT_COUNT: {
				gameArgs->board = new Board(std::stoi(radioArgs->iptRow->value()), std::stoi(radioArgs->iptColumn->value()), std::stoi(radioArgs->iptNumber->value()));
				break;
			} case RADIO_INPUT_RATE: {
				gameArgs->board = new Board(std::stoi(radioArgs->iptRow->value()), std::stoi(radioArgs->iptColumn->value()), (double)std::stoi(radioArgs->iptNumber->value()) / 100);
				break;
			} default: throw std::exception("Logic error");
		}
	} catch (std::exception e) {
		fl_alert(e.what());
		return;
	}

	gameArgs->window = new BoardWindow(gameArgs->board);
	auto boardWindow = (BoardWindow*)gameArgs->window;
	boardWindow->window->show();

	gameArgs = new GameArgs();
	radioArgs = new RadioArgs();
	gameArgs->radioArgs = radioArgs;
}

void HomeWindow::openRank(Fl_Widget* w, void* args) {
	w->~Fl_Widget();
	exit(0);
}

void HomeWindow::close(Fl_Widget* w, void* args) {
	w->~Fl_Widget();
	exit(0);
}