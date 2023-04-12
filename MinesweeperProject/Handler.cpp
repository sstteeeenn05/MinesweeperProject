#include "Handler.h"

bool Handler::outputEnable;
int Handler::method;

std::stringstream Handler::pipe;
std::ofstream Handler::outputFile;

void Handler::output(bool isSuccess, const std::string& commandName) {
	std::ostream& outputStream = (method == METHOD_CMD_FILE ? outputFile : std::cout);
	outputStream << "<" << commandName << "> : ";
	if (!isSuccess) {
		outputStream << "Failed" << std::endl;
		return;
	}
	if (pipe.eof()) {
		outputStream << "Success" << std::endl;
		return;
	}
	std::string buffer;
	while(getline(pipe,buffer)) outputStream << buffer << std::endl;
}

void Handler::resetPipe() {
	pipe.str(std::string());
	pipe.clear();
	pipe.get();
}

void Handler::init(int m, std::string path) {
	method = m;
	if(path.length()) outputFile.open(path);
	method == METHOD_GUI ? disableOutput() : enableOutput();
	resetPipe();
}

void Handler::enableOutput() {
	outputEnable = true;
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

void Handler::disableOutput() {
	outputEnable = false;
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

bool Handler::execute(const std::string commandName, std::function<void()> function) {
	bool status = true;
	try {
		function();
	} catch (std::exception e) {
		if (method == METHOD_GUI) fl_alert(e.what());
		status = false;
	}
	if(outputEnable) output(status, commandName);
	resetPipe();
	return status;
}