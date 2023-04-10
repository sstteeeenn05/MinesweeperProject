#include "DataType.h"
#include "Board.h"
#include "Handler.h"

//Example A - using global variables and functions
Board a;
void readFile() {
	a = Board("C:/users/user/downloads/board.txt");
}
void printGameAnswer() {
	a.print(PRINT_ANSWER);
}
void example_a() {
	Handler::execute("Load BoardFile board.txt", readFile);
	Handler::execute("Print GameAnswer", printGameAnswer);
}

//Example B - using namespace or class public static member
namespace TestNamespaceB {
	Board b;
	void readFile() {
		b = Board("C:/users/user/downloads/board.txt");
	}
	void printGameAnswer() {
		b.print(PRINT_ANSWER);
	}
};

class TestClassB {
public:
	static Board b;
	static void readFile() {
		b = Board("C:/users/user/downloads/board.txt");
	}
	static void printGameAnswer() {
		b.print(PRINT_ANSWER);
	}
};
Board TestClassB::b = Board();

void example_b() {
	Handler::execute("Load BoardFile board.txt", TestNamespaceB::readFile);
	Handler::execute("Print GameAnswer", TestNamespaceB::printGameAnswer);

	Handler::execute("Load BoardFile board.txt", TestClassB::readFile);
	Handler::execute("Print GameAnswer", TestClassB::printGameAnswer);
}

//Example C - using class public dynamic member
class TestClassC {
	Board c;
public:
	void readFile() {
		c = Board("C:/users/user/downloads/board.txt");
	}
	void printGameAnswer() {
		c.print(PRINT_ANSWER);
	}
};

void example_c() {
	TestClassC t;
	Handler::execute("Load BoardFile board.txt", callFunction(t, TestClassC::readFile));
	Handler::execute("Print GameAnswer", callFunction(t, TestClassC::printGameAnswer));
}

//Example D - using lambda functions
void example_d() {
	Board d;
	Handler::execute("Load BoardFile board.txt", [&] {d = Board("C:/users/user/downloads/board.txt"); });
	Handler::execute("Print GameAnswer", [&] { d.print(PRINT_ANSWER); });
}