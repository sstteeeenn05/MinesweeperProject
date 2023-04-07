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

	gameArgs->radioArgs = radioArgs;

	initRadios();
	initRadioReadBoard();
	initRadioRandom();
	initButtons();

	((Fl_Radio_Round_Button*)radioArgs->buttons[RADIO_READ_BOARD])->do_callback();
	((Fl_Radio_Round_Button*)radioArgs->buttons[RADIO_READ_BOARD])->take_focus();
	((Fl_Radio_Round_Button*)radioArgs->buttons[RADIO_READ_BOARD])->set();

	window->callback(HomeWindow::close);
	window->end();
}

void HomeWindow::initRadios() {
	for (auto radio = radioList.begin(); radio != radioList.end(); radio++) {
		static int radioCount = 0;
		Fl_Radio_Round_Button* button = new Fl_Radio_Round_Button(MARGIN, RADIO_Y[radioCount++], RADIO_WIDTH, RADIO_HEIGHT, radio->text);
		button->callback((Fl_Callback*)radio->callback, radioArgs);
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
			RadioArgs* radioArgs = (RadioArgs*)args;
			radioArgs->boardPath = path;
			radioArgs->iptPath->value(path.c_str());
		}
	}, (void*)radioArgs);

	btnChooser->callback([](Fl_Widget* w, void* args) {
		RadioArgs* radioArgs = (RadioArgs*)args;
		radioArgs->chooser->show();
	}, (void*)radioArgs);
}

void HomeWindow::initRadioRandom() {
	Fl_Int_Input* iptNumber = new Fl_Int_Input(RADIO_WIDTH + LABEL_WIDTH + MARGIN * 2, RADIO_Y[RADIO_INPUT_COUNT], VALUE_WIDTH, RADIO_HEIGHT, "Val:");
	Fl_Button* btnRandom = new Fl_Button(RADIO_WIDTH + LABEL_WIDTH + VALUE_WIDTH + MARGIN * 3, RADIO_Y[RADIO_INPUT_COUNT], BTN_RANDOM_WIDTH, RADIO_HEIGHT, "Random");
	Fl_Int_Input* iptColumn = new Fl_Int_Input(RADIO_WIDTH + LABEL_WIDTH + MARGIN * 2, RADIO_Y[RADIO_INPUT_RATE], INPUT_WIDTH, RADIO_HEIGHT, "Col:");
	Fl_Int_Input* iptRow = new Fl_Int_Input(RADIO_WIDTH + LABEL_WIDTH * 2 + INPUT_WIDTH + MARGIN * 3, RADIO_Y[RADIO_INPUT_RATE], INPUT_WIDTH, RADIO_HEIGHT, "Row:");

	iptNumber->value("25");
	iptColumn->value("10");
	iptRow->value("10");
	iptNumber->maximum_size(3);
	iptColumn->maximum_size(3);
	iptRow->maximum_size(3);

	radioArgs->iptNumber = iptNumber;
	radioArgs->iptColumn = iptColumn;
	radioArgs->iptRow = iptRow;

	iptNumber->callback([](Fl_Widget* w, void* args) {
		Fl_Int_Input* input = (Fl_Int_Input*)w;
		RadioArgs* radioArgs = (RadioArgs*)args;
		if (!radioArgs->iptNumber || !radioArgs->iptColumn || !radioArgs->iptRow) return;

		if (radioArgs->selection == RADIO_INPUT_COUNT)
			radioArgs->number = max(min(std::stoi(input->value()), radioArgs->col * radioArgs->row), 1);
		if (radioArgs->selection == RADIO_INPUT_RATE)
			radioArgs->number = max(min(std::stoi(input->value()), 100), 1);

		input->value(std::to_string(radioArgs->number).c_str());
	}, (void*)radioArgs);

	auto iptSizeCallback = [](Fl_Widget* w, void* args) {
		Fl_Int_Input* input = (Fl_Int_Input*)w;
		RadioArgs* radioArgs = (RadioArgs*)args;

		radioArgs->col = max(min(std::stoi(radioArgs->iptColumn->value()), MAX_COL), 1);
		radioArgs->row = max(min(std::stoi(radioArgs->iptRow->value()), MAX_ROW), 1);

		radioArgs->iptColumn->value(std::to_string(radioArgs->col).c_str());
		radioArgs->iptRow->value(std::to_string(radioArgs->row).c_str());
		radioArgs->iptNumber->do_callback();
	};
	iptColumn->callback(iptSizeCallback, (void*)radioArgs);
	iptRow->callback(iptSizeCallback, (void*)radioArgs);

	btnRandom->callback([](Fl_Widget* w, void* args) {
		RadioArgs* radioArgs = (RadioArgs*)args;
		if (!radioArgs->iptNumber || !radioArgs->iptColumn || !radioArgs->iptRow) return;

		if (radioArgs->selection==RADIO_INPUT_COUNT)
			radioArgs->number = rand() % (radioArgs->col * radioArgs->row) + 1;
		if (radioArgs->selection == RADIO_INPUT_RATE)
			radioArgs->number = rand() % 100 + 1;

		radioArgs->iptNumber->value(std::to_string(radioArgs->number).c_str());
	}, (void*)radioArgs);
}

void HomeWindow::radioCallback(Fl_Widget* w, void* args) {
	Fl_Radio_Round_Button* radio = (Fl_Radio_Round_Button*)w;
	RadioArgs* radioArgs = (RadioArgs*)args;
	auto itr = std::find(radioArgs->buttons.begin(), radioArgs->buttons.end(), radio);
	if (itr == radioArgs->buttons.end()) return;
	int index = std::distance(radioArgs->buttons.begin(), itr);
	radioArgs->selection = index;
	switch (index) {
		case RADIO_READ_BOARD:
			if (radioArgs->iptNumber) radioArgs->iptNumber->deactivate();
			if (radioArgs->iptColumn) radioArgs->iptColumn->deactivate();
			if (radioArgs->iptRow) radioArgs->iptRow->deactivate();
			if (radioArgs->iptPath) radioArgs->iptPath->activate();
			if (radioArgs->btnChooser) radioArgs->btnChooser->activate();
			break;
		case RADIO_INPUT_COUNT:
		case RADIO_INPUT_RATE:
			if (radioArgs->iptNumber) {
				if (index == RADIO_INPUT_COUNT) radioArgs->iptNumber->label("Count:");
				if (index == RADIO_INPUT_RATE) radioArgs->iptNumber->label("Rate:");
				radioArgs->iptNumber->activate();
			}
			if (radioArgs->iptColumn) radioArgs->iptColumn->activate();
			if (radioArgs->iptRow) radioArgs->iptRow->activate();
			if (radioArgs->iptPath) radioArgs->iptPath->deactivate();
			if (radioArgs->btnChooser) radioArgs->btnChooser->deactivate();
			break;
	}
}

void HomeWindow::initButtons() {
	for (auto button = buttonList.begin(); button != buttonList.end(); button++) {
		static int buttonIndex = 0;
		Fl_Button* component = new Fl_Button(MARGIN + (MARGIN + BUTTON_WIDTH) * (buttonIndex++), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, button->text);
		button->component = component;
		GenericArgs* genericArgs = (GenericArgs*)(button->args);
		if (genericArgs) genericArgs->button = component;
		button->component->callback((Fl_Callback*)button->callback, button->args);
	}
}

void HomeWindow::startGame(Fl_Widget* w, void* args) {
	GameArgs* gameArgs = (GameArgs*)args;
	RadioArgs* radioArgs = gameArgs->radioArgs;
	if (gameArgs->board) delete gameArgs->board;
	try {
		switch (radioArgs->selection) {
			case RADIO_READ_BOARD: {
				std::ifstream file(radioArgs->iptPath->value());
				gameArgs->board = new Board(file);
			} case RADIO_INPUT_COUNT: {
				gameArgs->board = new Board(std::stoi(radioArgs->iptRow->value()), std::stoi(radioArgs->iptColumn->value()), std::stoi(radioArgs->iptNumber->value()));
			} case RADIO_INPUT_RATE: {

			}
		}
	} catch (std::exception e) { return; }

	if (gameArgs->window && gameArgs->window->isOpen) return;
	gameArgs->window = new BoardWindow(gameArgs->board->getColumn(), gameArgs->board->getRow());
	BoardWindow* boardWindow = (BoardWindow*)gameArgs->window;
	boardWindow->args = gameArgs;
	boardWindow->window->show();
}

void HomeWindow::openRank(Fl_Widget* w, void* args) {
	w->~Fl_Widget();
	exit(0);
}

void HomeWindow::close(Fl_Widget* w, void* args) {
	w->~Fl_Widget();
	exit(0);
}