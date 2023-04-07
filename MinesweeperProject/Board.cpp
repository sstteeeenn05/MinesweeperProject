#include "Board.h"
#include <algorithm>
#include <random>
#include <exception>

void Board::initializeBoards() {
    boardArgs.board = std::vector<std::vector<char>>
            (row, std::vector<char>(column, '#'));

    answerBoard = std::vector<std::vector<char>>
            (row, std::vector<char>(column, '0'));
}

void Board::calculateAnswerByInput(std::ifstream &inputFile) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (inputFile.get() == 'X') {
                answerBoard[i][j] = 'X';
                bombCount++;
                plusOneAroundTheMine(j, i);
            }
        }
        // check column count is valid
        if (inputFile.get() != '\n' && !inputFile.eof()) throw std::exception("File format error.");

        // check row count is valid
        if (inputFile.eof() && i != row - 1) throw std::exception("File format error.");
    }
}

void Board::randomTheBoard(double randomRate) {
    std::random_device dev;
    std::mt19937 generator(dev());
    const unsigned int mineRange = static_cast<int>(randomRate * 100);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (generator() % 100 + 1 < mineRange) {
                answerBoard[i][j] = 'X';
                bombCount++;
                plusOneAroundTheMine(j, i);
            }
        }
    }
}

void Board::plusOneAroundTheMine(int x, int y) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (isCoordinateValid(j, i) && answerBoard[i][j] != 'X') {
                answerBoard[i][j]++;
            }
        }
    }
}

void Board::countBlank() {
    int tempBlankCount = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if ((boardArgs.board[i][j] == '#' ||
                 boardArgs.board[i][j] == 'f' ||
                 boardArgs.board[i][j] == '?') &&
                answerBoard[i][j] != 'X')
                tempBlankCount++;
        }
    }

    remainBlankCount = tempBlankCount;
    openBlankCount = row * column - bombCount - remainBlankCount;
}

bool Board::isCoordinateValid(int x, int y) const {
    return (x >= 0 && x < column && y >= 0 && y < row);
}

void Board::showMine() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (answerBoard[i][j] == 'X')
                boardArgs.board[i][j]='X';
        }
    }
}

void Board::revealGrid(int x, int y) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (isCoordinateValid(j, i)) {
                if (answerBoard[i][j] == '0' &&
                    (boardArgs.board[i][j] == '#'
                     || boardArgs.board[i][j] == '?')) {
                    boardArgs.board[i][j] = answerBoard[i][j];
                    revealGrid(j, i);
                }
                else if (answerBoard[i][j] != 'X' && boardArgs.board[i][j] != 'f')
                    boardArgs.board[i][j] = answerBoard[i][j];
            }
        }
    }
}

Board::Board() = default;

Board::Board(std::ifstream &inputFile) {
    inputFile >> row >> column;
    inputFile.get();
    if (row <= 0 && column <= 0) {
        inputFile.close();
        throw std::exception("Row or column out of range.");
    }

    initializeBoards();
    calculateAnswerByInput(inputFile);
    inputFile.close();
    countBlank();
}

Board::Board(int inputRow, int inputColumn, double randomRate) : row(inputRow), column(inputColumn) {
    if (row <= 0 && column <= 0) throw std::exception("Row or column out of range.");
    if (randomRate < 0) throw std::exception("Random rate out of range.");

    initializeBoards();
    randomTheBoard(randomRate);
    countBlank();

}

Board::Board(int inputRow, int inputColumn, int mineCount) : row(inputRow), column(inputColumn) {
    if (row <= 0 && column <= 0) throw std::exception("Row or column out of range.");
    if (mineCount > row * column) throw std::exception("Mine count out of range.");
    bombCount = mineCount;

    initializeBoards();
    std::random_device dev;
    std::mt19937 generator(dev());

    std::vector<char> tempBoard(row * column, '0');
    std::fill_n(tempBoard.begin(), mineCount, 'X');
    std::shuffle(tempBoard.begin(), tempBoard.end(), generator);

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (tempBoard[i * row + j] == 'X') {
                answerBoard[i][j] = 'X';
                plusOneAroundTheMine(j, i);
            }
        }
    }

    countBlank();
}

void Board::leftClick(int x, int y) {
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");

    if (boardArgs.board[y][x] != 'f') boardArgs.board[y][x] = answerBoard[y][x];
    else return;

    openBlankCount++;
    remainBlankCount--;

    if (answerBoard[y][x] == 'X') {
        showMine();
        boardArgs.gameStatus = LOSE;
        return;
    }

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (isCoordinateValid(j, i)) {
                if (answerBoard[i][j] == '0' &&
                    (boardArgs.board[i][j] == '#'
                     || boardArgs.board[i][j] == '?')) {
                    revealGrid(j, i);
                }
            }
        }
    }

    countBlank();
    if (remainBlankCount == 0)boardArgs.gameStatus = WIN;
}

void Board::rightClick(int x, int y) {
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");
    constexpr char symbol[3] = {'#', 'f', '?'};

    for (int i = 0; i < 3; i++) {
        if (boardArgs.board[y][x] == symbol[i]) {
            boardArgs.board[y][x] = symbol[((i == 2) ? 0 : (i + 1))];
            if (symbol[i] == 'f') flagCount--;
            else flagCount++;
            return;
        }
    }

}

int Board::getRow() const {
    return row;
}

int Board::getColumn() const {
    return column;
}

int Board::getBombCount() const {
    return bombCount;
}

int Board::getFlagCount() const {
    return flagCount;
}

int Board::getRemainBlank() const {
    return remainBlankCount;
}

int Board::getOpenBlank() const {
    return openBlankCount;
}

const std::vector<std::vector<char>> &Board::getAnswer() const {
    return answerBoard;
}

const BoardArgs &Board::getCurrentBoard() const {
    return boardArgs;
}