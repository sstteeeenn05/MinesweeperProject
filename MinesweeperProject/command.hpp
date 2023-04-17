#pragma once

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "Board.h"
#include "Handler.h"
#include "HomeWindow.h"


const std::map<std::string, int> PRINT_TABLE =
{
	{"GameState", PRINT_STATE},
	{"GameBoard", PRINT_BOARD},
	{"GameAnswer", PRINT_ANSWER},
	{"BombCount", PRINT_BOMB_COUNT},
	{"FlagCount", PRINT_FLAG_COUNT},
	{"OpenBlankCount", PRINT_OPEN_BLANK},
	{"RemainBlankCount", PRINT_REMAIN_BLANK}
};

std::string toStringWithPrecisionTwo(double input) {
	std::ostringstream out("");
	out.precision(2);
	out << std::fixed <<input;
	return std::move(out).str();
}

int openCommandFile(std::string inputPath, std::string outputPath) {
	std::ifstream file;
	file.open(inputPath, std::ios::in);
	bool isWinLosePrinted = false;
	if (file.is_open() && Handler::init(METHOD_CMD_FILE, outputPath)) //if the file can be opened successfully
	{
		std::string command;
		Board board;
		while (file >> command)
		{
			if (command == "Print")
			{
				std::string output;
				file >> output;
				if (PRINT_TABLE.find(output) == PRINT_TABLE.end()) //if the output is invalid
				{
					if (file.peek() != '\n') //if it is a sentence
					{
						std::string trash;
						std::getline(file, trash);
						output += trash;
					}
					Handler::execute("Print " + output, [&] { throw std::exception(); }); //call lambda to cout "failed"
				}
				else //it is a valid output
				{
					Handler::execute("Print " + output, [&] { board.print(PRINT_TABLE.at(output)); }); //call lambda to cout 
				}
			}
			else if (command == "Load")
			{
				std::string mode;
				file >> mode;
				if (mode == "BoardFile") //input file
				{
					std::string filePath;
					file >> filePath;
					Handler::execute("Load BoardFile " + filePath, [&] {board.load(filePath.c_str()); });
				}
				else if (mode == "RandomCount")
				{
					int m, n, bombAmount;
					file >> m >> n >> bombAmount;
					Handler::execute("Load RandomCount " + std::to_string(m) + " " + std::to_string(n) + " " + std::to_string(bombAmount),
						[&] {board.load(m, n, bombAmount); });
				}
				else if (mode == "RandomRate")
				{
					int m, n;
					double bombRate;
					file >> m >> n >> bombRate;
					Handler::execute("Load RandomCount " + std::to_string(m) + " " + toStringWithPrecisionTwo(n) + " " + std::to_string(bombRate),
						[&] {board.load(m, n, bombRate); });
				}
				else
				{
					command += " " + mode;
					if (file.peek() != '\n') //if it is a sentence
					{
						std::string trash;
						std::getline(file, trash);
						command += trash;
					}
					Handler::execute(command, [&] { throw std::exception(); });
				}
			}
			else if (command == "LeftClick")
			{
				int row, col;
				file >> row >> col;
				Handler::execute("LeftClick " + std::to_string(row) + " " + std::to_string(col), [&] {board.leftClick(col, row); });
				const auto& boardArgs = board.getBoardArgs();
				if (boardArgs.status == BOARD_STATUS_WIN && !isWinLosePrinted) //if game win
				{
					Handler::execute("", [&] {hout<< "You win the game"; });
					isWinLosePrinted = true;
				}
				else if (boardArgs.status == BOARD_STATUS_LOSE && !isWinLosePrinted) //if game lose
				{
					Handler::execute("", [&] {hout << "You lose the game"; });
					isWinLosePrinted = true;
				}
			}
			else if (command == "RightClick")
			{
				int row, col;
				file >> row >> col;
				Handler::execute("RightClick " + std::to_string(row) + " " + std::to_string(col), [&] {board.rightClick(col, row); });
			}
			else if (command == "StartGame")
			{
				Handler::execute("StartGame", [&] { board.startGame(); });
			}
			else if (command == "Replay") //judge the command
			{
				const auto &boardArgs = board.getBoardArgs();
				if (boardArgs.state == STATE_GAME_OVER) //if game is over
				{
					isWinLosePrinted = false;
					Handler::execute("Replay", [&] { board = Board(); });
				}
				else {
					Handler::execute("Replay", [&] { throw std::exception(); });
				}
			}
			else if (command == "Quit") //judge the command
			{
				const auto &boardArgs = board.getBoardArgs();
				if (boardArgs.state == STATE_GAME_OVER) //if game is over
				{
					Handler::execute("Quit", [&] {});
					break;
				}
				else {
					Handler::execute("Quit", [&] { throw std::exception(); });
				}
			}
			else {
				if (file.peek() != '\n') //if it is a sentence
				{
					std::string trash;
					std::getline(file, trash);
					command += trash;
				}
				Handler::execute(command, [&] { throw std::exception(); });
			}
		}
	}
	else {
		std::cout << "Failed to open the input or output file" << std::endl;
		if (file.is_open()) file.close();
		if (Handler::outputFile.is_open()) Handler::outputFile.close();
		return -1;
	}
	if(file.is_open()) file.close();
	if (Handler::outputFile.is_open()) Handler::outputFile.close();
	return 0;
}

int openCommandInput() {
	Handler::init(METHOD_CMD_INPUT);
	Board board;
	std::string input;
	bool isWinLosePrinted = false;
	while (std::cin >> input) //input command
	{
		if (input == "Print") //judge the command
		{
			std::string output;
			std::cin >> output;
			if (PRINT_TABLE.find(output) == PRINT_TABLE.end()) //if the output is invalid
			{
				if (std::cin.peek() != '\n') //if it is a sentence
				{
					std::string trash;
					std::getline(std::cin, trash);
					output = output + trash;
				}
				Handler::execute("Print " + output, [&] { throw std::exception(); }); //call lambda to cout "failed"
			}
			else //it is a valid output
			{
				Handler::execute("Print " + output, [&] { board.print(PRINT_TABLE.at(output)); }); //call lambda to cout 
			}
		}
		else if (input == "Load") //judge the command
		{
			std::string boardset;
			std::cin >> boardset;
			if (boardset == "BoardFile") //input file
			{
				std::string filePath;
				std::cin >> filePath;
				Handler::execute("Load BoardFile " + filePath, [&] {board.load(filePath.c_str()); });
			}
			else if (boardset == "RandomCount") //if set the bomb number
			{
				int m, n, count;
				std::cin >> m >> n >> count;
				Handler::execute("RandomCount " + std::to_string(m) + " " + std::to_string(n) + " " + std::to_string(count), [&] {board.load(m, n, count); });
			}
			else if (boardset == "RandomRate") //if set the bomb rate
			{
				int m, n;
				double rate;
				std::cin >> m >> n >> rate;
				Handler::execute("RandomRate " + std::to_string(m) + " " + std::to_string(n) + " " + toStringWithPrecisionTwo(rate), [&] {board.load(m, n, rate); });
			}
			else
			{
				input = input + " " + boardset;
				if (std::cin.peek() != '\n') //if it is a sentence
				{
					std::string trash;
					std::getline(std::cin, trash);
					input += trash;
				}
				Handler::execute(input, [&] { throw std::exception(); });
			}
		}
		else if (input == "StartGame") //judge the command
		{
			Handler::execute("StartGame", [&] {board.startGame(); });
		}
		else if (input == "LeftClick")
		{
			int row, col;
			std::cin >> row >> col;
			Handler::execute("LeftClick " + std::to_string(row) + " " + std::to_string(col), [&] {board.leftClick(col, row); });
			const auto& boardArgs = board.getBoardArgs();
			if (boardArgs.status ==BOARD_STATUS_WIN && !isWinLosePrinted) //if game win
			{
				std::cout << "You win the game" << std::endl;
				isWinLosePrinted = true;
			}
			else if (boardArgs.status == BOARD_STATUS_LOSE && !isWinLosePrinted) //if game lose
			{
				std::cout << "You lose the game" << std::endl;
				isWinLosePrinted = true;
			}
		}
		else if (input == "RightClick") //judge the command
		{
			int row, col;
			std::cin >> row >> col;
			Handler::execute("RightClick " + std::to_string(row) + " " + std::to_string(col), [&] {board.rightClick(col, row); });
		}
		else if (input == "Replay") //judge the command
		{
			const auto& boardArgs = board.getBoardArgs();
			if (boardArgs.state == STATE_GAME_OVER) //if game is over
			{
				Handler::execute("Replay", [&] { board = Board(); });
				isWinLosePrinted = false;
			}
			else
			{
				Handler::execute("Replay", [&] { throw std::exception(); });
			}
		}
		else if (input == "Quit") //judge the command
		{
			const auto& boardArgs = board.getBoardArgs();
			if (boardArgs.state == STATE_GAME_OVER) //if game is over
			{
				Handler::execute("Quit", [&] {});
				break;
			}
			else
			{
				Handler::execute("Quit", [&] { throw std::exception(); });
			}
		}
		else
		{
			if (std::cin.peek() != '\n') //if it is a sentence
			{
				std::string trash;
				std::getline(std::cin, trash);
				input += trash;
			}
			Handler::execute(input, [&] { throw std::exception(); });
		}
	}
	return 0;
}

int openGUI() {
	Handler::init(METHOD_GUI);
	HomeWindow::open();
	return Fl::run();
}