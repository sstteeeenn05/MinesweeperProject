#include <algorithm>
#include <cstring>
#include "command.hpp"

#define toLower(str) std::for_each(str.begin(), str.end(), [](auto& c) { c = tolower(c); });

int main(int argc, char* argv[]) {
	std::string mode = (argc > 1) ? argv[1] : "";
	while (mode != "quit") {
		toLower(mode);
		if (mode == "commandfile") {
			if (argc < 3) {
				puts("Lost argument");
				goto input;
			}
			openCommandFile(argv[2], argv[3]);
		} else if (mode == "commandinput") {
			openCommandInput();
		} else if (mode == "gui") {
			std::cout << "FLTK exit with code : " << openGUI() << std::endl;
		} else {
			puts("\n[Minesweeper Help] - Type \"quit\" to exit the program\n");
			puts("File Stream Mode: CommandFile <Input File> <Output File>");
			puts("\tRun Minesweeper command by reading .txt file");
			puts("\tThe result will output to the specified file\n");
			puts("Command Input Mode: CommandInput");
			puts("\tRun Minesweeper command by stdin input");
			puts("\tThe result will output to the stdout\n");
			puts("Graphics User Interface Mode: GUI");
			puts("\tRun Minesweeper command by FLTK window");
			puts("\tThe result will output to the stdout and the window\n");
		}
		input:
		printf("MineSweeper.exe > ");
		std::cin >> mode;
	}
	return 0;
}