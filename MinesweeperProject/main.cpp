#include <algorithm>
#include <cstring>
#include <cstdio>
#include <string>
#include "command.hpp"

#define toLower(str) std::for_each((str).begin(), (str).end(), [](auto& c) { c = tolower(c); })

int main(int argc, char* argv[]) {
	std::string command = (argc > 1) ? argv[1] : "help";
	if (command == "commandfile") {
		if (argc < 3) {
			puts("Lost argument, should be: CommandFile <Input File> <Output File>");
		}
		else {
			std::cout << "commandFile exit with code : " << openCommandFile(argv[2], argv[3]) << std::endl;
		}
		goto input;
	}
	while (command != "exit") {
		toLower(command);
		if (command == "commandfile") {
			std::string inputPath, outputPath;
			std::cin >> inputPath >> outputPath;
			std::cout << "commandFile exit with code : " << openCommandFile(inputPath, outputPath) << std::endl;
			
		} else if (command == "commandinput") {
			openCommandInput();
		} else if (command == "gui") {
			std::cout << "FLTK exit with code : " << openGUI() << std::endl;
		} else if (command == "help") {
			puts("\n[Minesweeper Help] - Type \"exit\" to exit the program\n");
			puts("File Stream Mode: CommandFile <Input File> <Output File>");
			puts("\tRun Minesweeper command by reading .txt file");
			puts("\tThe result will output to the specified file\n");
			puts("Command Input Mode: CommandInput");
			puts("\tRun Minesweeper command by stdin input");
			puts("\tThe result will output to the stdout\n");
			puts("Graphics User Interface Mode: GUI");
			puts("\tRun Minesweeper command by FLTK window");
			puts("\tThe result will output to the stdout(with dev mode on) and the window");
		} else std::cout << "'" << command << "' - Unknown Command. Type 'help' to view all command" << std::endl;
		input:
		Handler::init(METHOD_PENDING);
		printf("\nMineSweeper.exe > ");
		std::cin >> command;
	}
	return 0;
}