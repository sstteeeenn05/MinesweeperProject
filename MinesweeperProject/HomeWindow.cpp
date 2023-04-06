#include "HomeWindow.h"

#include "CustomDataType.h"

#include "FL/fl_ask.H"

#include <cstdio>
#include <exception>

HomeWindow::HomeWindow() {
	configArgs->window = new ConfigWindow();

	window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper");
	window->begin();

	logo = new Fl_Button(LOGO_X, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT);
	logo->image((new Fl_PNG_Image("logo.png"))->copy(LOGO_WIDTH, LOGO_HEIGHT));

	for (auto button = buttonList.begin(); button != buttonList.end(); button++) {
		static int buttonIndex = 0;
		Fl_Button* component = new Fl_Button(BUTTON_X + (BUTTON_MARGIN + BUTTON_WIDTH) * (buttonIndex++), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, button->text);
		button->component = component;
		GenericArgs* genericArgs = (GenericArgs*)(button->args);
		if (genericArgs) genericArgs->button = component;
		button->component->callback((Fl_Callback*)button->callback,button->args);
	}

	window->callback(HomeWindow::close);
	window->end();
}

void HomeWindow::startGame(Fl_Widget* w, void* args) {
	GameArgs* gameArgs = (GameArgs*)args;
	if (!gameArgs->board) {
		fl_alert("The board is not ready yet. Try Config?");
		return;
	}
	if (gameArgs->window && gameArgs->window->isOpen) return;
	gameArgs->window = new BoardWindow(gameArgs->board->getColumn(), gameArgs->board->getRow());
	BoardWindow* boardWindow = (BoardWindow*)gameArgs->window;
	boardWindow->args = gameArgs;
	boardWindow->window->show();
}

void HomeWindow::openConfig(Fl_Widget* w, void* args) {
	ConfigArgs* configArgs = (ConfigArgs*)args;
	if (configArgs->window && configArgs->window->isOpen) return;
	configArgs->window = new ConfigWindow();
	ConfigWindow* configWindow = (ConfigWindow*)configArgs->window;
}

void HomeWindow::close(Fl_Widget* w, void* args) {
	w->~Fl_Widget();
	exit(0);
}