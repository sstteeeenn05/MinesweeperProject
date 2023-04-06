#include "HomeWindow.h"

HomeWindow::HomeWindow() {
	window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper");
	window->begin();
	logo = new Fl_Button(LOGO_X, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT);
	logo->image((new Fl_PNG_Image("logo.png"))->copy(LOGO_WIDTH, LOGO_HEIGHT));
	for (auto buttonText = buttonTextList.begin(); buttonText != buttonTextList.end(); buttonText++) {
		static int buttonCount = 0;
		buttonList.push_back(new Fl_Button(BUTTON_X + (BUTTON_MARGIN + BUTTON_WIDTH) * (buttonCount++), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, *buttonText));
	}
	window->end();
}