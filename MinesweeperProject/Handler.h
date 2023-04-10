#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <sstream>

#include "DataType.h"

#define callFunction(component,functionName) std::bind(&functionName,component)
#define hout (*Handler::pipe)

class Handler{
	static void output(bool);
	static void resetPipe();
public:
	static int method;
	static std::stringstream* pipe,* ofstream;
	static bool execute(const std::string, std::function<void()>);
};