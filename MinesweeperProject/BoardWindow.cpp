#include "BoardWindow.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <future>

BoardWindow::BoardWindow(Board* b) :board(b), row(b->getRow()), col(b->getColumn()), boardArgs(b->getCurrentBoard()), answerBoard(b->getAnswer()), totalBlank(b->getRemainBlank()) {
	window = new Fl_Window(MARGIN * 2 + BTN_MINE_SIZE * col, MARGIN * 2 + BTN_MINE_SIZE * row);

	window->begin();

	initMine();

	window->end();

	std::stringstream* title = new std::stringstream();
	(*title) << "Game (" << col << "x" << row << ") 0%" << std::endl;
	window->label(title->str().c_str());
	delete title;
}

void BoardWindow::initMine(){
	for (int i = 0; i < row; i++) {
		std::vector<MineArgs*> mines;
		for (int j = 0; j < col; j++) {
			MineArgs* mineArgs = new MineArgs();
			mineArgs->button = new Fl_Button(MARGIN + BTN_MINE_SIZE * j, MARGIN + BTN_MINE_SIZE * i, BTN_MINE_SIZE, BTN_MINE_SIZE);
			mineArgs->button->callback([](Fl_Widget* w, void* args) {
				auto mineArgs = (MineArgs*)args;
				if (mineArgs->parent->boardArgs.gameStatus == BOARD_STATUS_CONTINUE) {
					if (mineArgs->isProcessing) return;
					mineArgs->isProcessing = true;
					if (Fl::event_button() == FL_LEFT_MOUSE && mineArgs->parent->boardArgs.board[mineArgs->y][mineArgs->x] != MINE_FLAG)
						mineArgs->board->leftClick(mineArgs->x, mineArgs->y);
					if (Fl::event_button() == FL_RIGHT_MOUSE)
						mineArgs->board->rightClick(mineArgs->x, mineArgs->y);
					mineArgs->parent->update(mineArgs->parent);
					mineArgs->isProcessing = false;
				}
			}, (void*)mineArgs);
			mineArgs->board = board;
			mineArgs->x = j;
			mineArgs->y = i;
			mineArgs->parent = this;
			mines.push_back(mineArgs);
		}
		mineList.push_back(mines);
	}
}

void BoardWindow::update(BoardWindow* bw) {
	std::stringstream* title = new std::stringstream();
	(*title) << "Game (" << col << "x" << row << ") " << std::fixed << std::setprecision(1) << (double)bw->board->getOpenBlank() / totalBlank * 100 << "%" << std::endl;
	bw->window->label(title->str().c_str());
	delete title;

	std::vector<std::future<void> > allocateTaskList;

	for (auto& mines : bw->mineList) for (auto& mine : mines) allocateTaskList.push_back(std::async(std::launch::async, [=]() {
		if (mine->button->label()) delete mine->button->label();
		mine->button->image(nullptr);
		mine->button->label(nullptr);
		}));
	for (auto& task : allocateTaskList) task.get();

	for (int i = 0; i < bw->row; i++) {
		for (int j = 0; j < bw->col; j++) {
			auto currentButton = bw->mineList[i][j]->button;
			const auto currentValue = (int)bw->boardArgs.board[i][j];
			switch (currentValue) {
			case MINE_FLAG:
			case MINE_SUS:
				currentButton->image(PATH_ICON[currentValue]);
				break;
			case MINE_MASK:
			case MINE_MINE:
				break;
			default: {
				const char* label = new char[2] {bw->boardArgs.board[i][j], '\0'};
				currentButton->label(label);
			}case MINE_NULL:
				currentButton->set();
				currentButton->deactivate();
				break;
			}
			Fl::awake();
		}
	}

	switch (bw->boardArgs.gameStatus) {
		case BOARD_STATUS_WIN:
			bw->win(bw);
			break;
		case BOARD_STATUS_LOSE:
			bw->lose(bw);
			break;
	}
}

void BoardWindow::win(BoardWindow* bw) {
	for (int i = 0; i < bw->row; i++) {
		for (int j = 0; j < bw->col; j++) {
			clock_t start = clock();
			auto currentButton = bw->mineList[i][j]->button;
			const auto currentValue = (int)bw->answerBoard[i][j];
			if (currentButton->label()) delete currentButton->label();
			currentButton->image(nullptr);
			currentButton->label(nullptr);
			currentButton->deactivate();
			currentButton->selection_color(FL_GREEN);
			currentButton->redraw();
			while (clock() - start < CLOCKS_PER_SEC / 100) Fl::flush();
		}
	}
}

void BoardWindow::lose(BoardWindow* bw) {
	for (int i = 0; i < bw->row; i++) {
		for (int j = 0; j < bw->col; j++) {
			clock_t start = clock();
			auto currentButton = bw->mineList[i][j]->button;
			const auto currentValue = (int)bw->answerBoard[i][j];
			if (currentValue == MINE_MINE) {
				currentButton->image(bw->PATH_ICON[MINE_MINE]);
				currentButton->color(FL_RED);
			} else currentButton->deactivate();
			currentButton->selection_color(FL_RED);
			currentButton->redraw();
			while (clock() - start < CLOCKS_PER_SEC / 100) Fl::flush();
		}
	}
}
