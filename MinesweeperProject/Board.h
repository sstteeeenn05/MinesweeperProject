#pragma once

#include <vector>
#include <fstream>

constexpr int WIN = 2;
constexpr int LOSE = 1;
constexpr int CONTINUE = 0;

struct BoardArgs {
    std::vector<std::vector<char>> board;
    int gameStatus = CONTINUE;
};

class Board {
private:
    BoardArgs boardArgs;
    std::vector<std::vector<char>> answerBoard;
    int row = 0;
    int column = 0;
    int bombCount = 0;
    int flagCount = 0;
    int openBlankCount = 0;
    int remainBlankCount = 0;

    void initializeBoards();

    void calculateAnswerByInput(std::ifstream &inputFile);

    void randomTheBoard(double randomRate);

    void plusOneAroundTheMine(int x, int y);

    void countBlank();

    bool isCoordinateValid(int x, int y) const;

    void showMine();

    void revealGrid(int x, int y);

public:

    Board();

    explicit Board(std::ifstream &inputFile);

    Board(int inputRow, int inputColumn, double randomRate);

    Board(int inputRow, int inputColumn, int mineCount);

    void leftClick(int x, int y);

    void rightClick(int x, int y);

    int getRow() const;

    int getColumn() const;

    int getBombCount() const;

    int getFlagCount() const;

    int getRemainBlank() const;

    int getOpenBlank() const;

    const std::vector<std::vector<char>> &getAnswer() const;

    const BoardArgs &getCurrentBoard() const;

};