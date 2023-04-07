#include "Board.h"
#include <algorithm>
#include <random>
#include <exception>

void Board::initializeBoards() {
    boardArgs.board = std::vector<std::vector<char>>
            (row, std::vector<char>(column, MINE_MASK));

    answerBoard = std::vector<std::vector<char>>
            (row, std::vector<char>(column, MINE_NULL));
}

void Board::calculateAnswerByInput(std::ifstream &inputFile) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (inputFile.get() == MINE_MINE) {
                answerBoard[i][j] = MINE_MINE;
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
            if (generator() % 100 + 1 <= mineRange) {
                answerBoard[i][j] = MINE_MINE;
                bombCount++;
                plusOneAroundTheMine(j, i);
            }
        }
    }
}

void Board::plusOneAroundTheMine(int x, int y) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (isCoordinateValid(j, i) && answerBoard[i][j] != MINE_MINE) {
                answerBoard[i][j]++;
            }
        }
    }
}

void Board::countBlank() {
    int tempBlankCount = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if ((boardArgs.board[i][j] == MINE_MASK ||
                 boardArgs.board[i][j] == MINE_FLAG ||
                 boardArgs.board[i][j] == MINE_SUS) &&
                answerBoard[i][j] != MINE_MINE)
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
            if (answerBoard[i][j] == MINE_MINE)
                boardArgs.board[i][j]=MINE_MINE;
        }
    }
}

void Board::revealGrid(int x, int y) {
    struct Pos {
        int x;
        int y;
    };

    std::vector<Pos> queue;
    queue.reserve(100);
    queue.push_back(Pos{x, y});

    while (!queue.empty()) {
        if (answerBoard[queue[0].y][queue[0].x] == MINE_NULL &&
            (boardArgs.board[queue[0].y][queue[0].x] == MINE_MASK
             || boardArgs.board[queue[0].y][queue[0].x] == MINE_SUS)) {

            for (int i = queue[0].y - 1; i <= queue[0].y + 1; i++) {
                for (int j = queue[0].x - 1; j <= queue[0].x + 1; j++) {
                    if (isCoordinateValid(j, i)) {
                        if((boardArgs.board[i][j] == MINE_MASK || boardArgs.board[i][j] == MINE_SUS))
                            queue.push_back(Pos{j, i});
                    }
                }
            }

             }

        if (answerBoard[queue[0].y][queue[0].x] != MINE_MINE && boardArgs.board[queue[0].y][queue[0].x] != MINE_FLAG)
            boardArgs.board[queue[0].y][queue[0].x] = answerBoard[queue[0].y][queue[0].x];

        queue.erase(queue.begin());
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

    std::vector<char> tempBoard(row * column, MINE_NULL);
    std::fill_n(tempBoard.begin(), mineCount, MINE_MINE);
    std::shuffle(tempBoard.begin(), tempBoard.end(), generator);

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (tempBoard[i * row + j] == MINE_MINE) {
                answerBoard[i][j] = MINE_MINE;
                plusOneAroundTheMine(j, i);
            }
        }
    }

    countBlank();
}

void Board::leftClick(int x, int y) {
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");

    if (boardArgs.board[y][x] == MINE_FLAG) throw std::exception("Left click on flag.");

    if(boardArgs.board[y][x]!=MINE_MASK && boardArgs.board[y][x]!=MINE_SUS)
        throw std::exception("Left click on clicked grid.");

    if (answerBoard[y][x] == MINE_MINE) {
        showMine();
        boardArgs.gameStatus = BOARD_STATUS_LOSE;
        return;
    }

    revealGrid(x,y);

    countBlank();
    if (remainBlankCount == 0)boardArgs.gameStatus = BOARD_STATUS_WIN;
}

void Board::rightClick(int x, int y) {
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");
    if(boardArgs.board[y][x]!=MINE_MASK && boardArgs.board[y][x]!=MINE_SUS && boardArgs.board[y][x] != MINE_FLAG)
        throw std::exception("Right click on clicked grid.");
    
    constexpr char symbol[3] = {MINE_MASK, MINE_FLAG, MINE_SUS};

    for (int i = 0; i < 3; i++) {
        if (boardArgs.board[y][x] == symbol[i]) {
            boardArgs.board[y][x] = symbol[((i == 2) ? 0 : (i + 1))];
            if (symbol[i] == MINE_FLAG) flagCount--;
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