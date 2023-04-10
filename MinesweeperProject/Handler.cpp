#include "Handler.h"

std::stringstream* Handler::pipe = new std::stringstream();

void Handler::output(bool isSuccess) {
	if (!isSuccess) {
		std::cout << "Failed" << std::endl;
		return;
	}
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
	bool status = true;
	std::cout << "<" << commandName << "> : ";
	try {
		function();
	}
	catch (std::exception e) {
		std::cout << "Failed" << std::endl;
		status = false;
	}
	output(status);
	resetPipe();
	return status;
}