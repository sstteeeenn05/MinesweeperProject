#pragma once

#include <functional>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>

#include "DataType.h"

#define callFunction(component,functionName) std::bind(&functionName,component)
#define hout Handler::pipe

class Handler{
	static int method;
	static bool outputEnable;
	static void output(bool, const std::string&);
	static bool isPipeEmpty();
	static void resetPipe();
public:
	static std::stringstream pipe;
	static std::ofstream outputFile;
	static bool init(int, std::string = std::string());
	static void enableOutput();
	static void disableOutput();
	static bool execute(const std::string, std::function<void()>);
};