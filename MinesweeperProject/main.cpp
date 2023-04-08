#include "Board.h"
#include "HomeWindow.h"

int main(int argc, char* argv[]) {
	HomeWindow hw;
	hw.open();
	return Fl::run();
}