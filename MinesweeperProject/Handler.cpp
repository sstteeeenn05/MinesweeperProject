#include "Handler.h"

std::stringstream* Handler::pipe = new std::stringstream();

void Handler::output() {
	std::string buffer = pipe->str();
	if (!buffer.length()) {
		std::cout << "Success" << std::endl;
		return;
	}
	std::cout << buffer << std::endl;
}

void Handler::resetPipe() {
	pipe->str("");
	pipe->clear();
}

bool Handler::execute(const std::string commandName, std::function<void()> function) {
	std::cout << "<" << commandName << "> : ";
	try {
		function();
	}
	catch (std::exception e) {
		std::cout << "Failed" << std::endl;
		return false;
	}
	output();
	resetPipe();
	return true;
}