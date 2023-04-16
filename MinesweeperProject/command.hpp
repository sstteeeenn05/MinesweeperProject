#include <fstream>
#include "Board.h"
#include "Handler.h"
#include "HomeWindow.h"

#define TEST

int openCommandFile(std::string inputPath, std::string outputPath) {
#ifndef TEST
	Handler::init(METHOD_CMD_FILE, outputPath);
	//Place the code here
	std::string command;
	std::ifstream file;
	file.open(inputPath, std::ios::in);
	if (file.is_open())
	{
		Board board;
		while (file >> command)
		{
			if (command == "Print")
			{
				std::string output;
				file >> output;
				if (output == "GameState")
				{
					Handler::execute("Print GameState", [&] { board.print(PRINT_ANSWER); });
				}
				else if (output == "GameBoard")
				{
					Handler::execute("Print GameBoard", [&] { board.print(PRINT_ANSWER); });
				}
				else //output = GameAnswer
				{
					Handler::execute("Print GameAnswer", [&] { board.print(PRINT_ANSWER); });
				}
			}
			else if (command == "Load")
			{
				int state=board.getBoardArgs().
				if()
				std::string mode;
				file >> mode;
				if (mode == "BoardFile")
				{
					std::string filePath;
					file >> filePath;
					Handler::execute("Load BoardFile "+ filePath, [&] {board = Board(filePath.c_str()); });
				}
				else if (mode == "RandomCount")
				{
					int m, n, bombAmount;
					file >> m >> n >> bombAmount;
					board = Board(m, n, bombAmount);
				}
				else if (mode == "RandomRate")
				{
					int m, n;
					float bombRate;
					file >> m >> n >> bombRate;
					board = Board(m, n, bombRate);
				}
			}
			else if (command == "LeftClick")
			{
				int x, y;
				file >> x >> y;
				Handler::execute("LeftClick " + std::to_string(x) + " " + std::to_string(y), [&] {board.leftClick(x, y); });
			}
			else if (command == "RightClick")
			{
				int x, y;
				file >> x >> y;
				Handler::execute("RightClick " + std::to_string(x) + " " + std::to_string(y), [&] {board.rightClick(x, y); });
			}
			else if (command == "StartGame")
			{
				board.startGame();
			}
		}
		file.close();
	}
#endif
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
	HomeWindow::open();
	return Fl::run();
}