#include "Board.h"
#include "Handler.h"
#include "HomeWindow.h"

int openCommandFile(std::string inputPath, std::string outputPath) {
	Handler::init(METHOD_CMD_FILE, outputPath);
	//Place the code here

	//
	return 0;
}

int openCommandInput() {
	Handler::init(METHOD_CMD_INPUT);
	//Place the code here

	//
	return 0;
}

int openGUI() {
	Handler::init(METHOD_GUI);
	HomeWindow hw;
	hw.open();
	return Fl::run();
}