#include "Board.h"
#include "Handler.h"
#include "HomeWindow.h"

int openCommandFile(std::string inputPath, std::string outputPath) {
	Handler::method = METHOD_CMD_FILE;
	//Place the code here

	//
	return 0;
}

int openCommandInput() {
	Handler::method = METHOD_CMD_INPUT;
	//Place the code here

	//
	return 0;
}

int openGUI() {
	Handler::method = METHOD_GUI;
	HomeWindow hw;
	hw.open();
	return Fl::run();
}