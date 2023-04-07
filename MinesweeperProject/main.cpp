#include "Board.h"
#include "BoardWindow.h"
#include "HomeWindow.h"
#include "RankWindow.h"

int main(int argc, char* argv[]) {
	HomeWindow hw;
	hw.open();
	return Fl::run();
}