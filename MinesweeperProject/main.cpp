#include "Board.h"
#include "BoardWindow.h"
#include "HomeWindow.h"
#include "ScoreWindow.h"

int main() {
	HomeWindow hw;
	hw.window->show();
	Fl::run();
}