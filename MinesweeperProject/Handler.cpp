#include "Handler.h"

std::stringstream* Handler::pipe = new std::stringstream();
std::stringstream* Handler::ofstream = new std::stringstream();
int Handler::method = 0;

void Handler::output(bool isSuccess) {
	if (!isSuccess) {
		(method == METHOD_CMD_FILE ? (*ofstream) : std::cout) << "Failed" << std::endl;
		return;
	}
	std::string buffer = pipe->str();
	if (!buffer.length()) {
		(method == METHOD_CMD_FILE ? (*ofstream) : std::cout) << "Success" << std::endl;
		return;
	}
	(method == METHOD_CMD_FILE ? (*ofstream) : std::cout) << buffer << std::endl;
}

void Handler::resetPipe() {
	pipe->str("");
	pipe->clear();
}

bool Handler::execute(const std::string commandName, std::function<void()> function) {
	bool status = true;
	(method == METHOD_CMD_FILE ? (*ofstream) : std::cout) << "<" << commandName << "> : ";
	try {
		function();
	}
	catch (std::exception e) {
		if (method == METHOD_GUI) fl_alert(e.what());
		status = false;
	}
	output(status);
	resetPipe();
	return status;
}