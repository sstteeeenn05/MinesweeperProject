#include "BoardWindow.h"

#include <sstream>
#include <future>

BoardWindow::BoardWindow(Board* b) :board(b), row(b->getRow()), col(b->getColumn()), boardArgs(b->getCurrentBoard()), answerBoard(b->getAnswer()) {
	std::stringstream* title=new std::stringstream();
	(*title) << "Game (" << col << "x" << row << ")" << std::endl;
	window = new Fl_Window(MARGIN * 2 + BTN_MINE_SIZE * col, MARGIN * 2 + BTN_MINE_SIZE * row, title->str().c_str());
	delete title;

	window->begin();

	initMine();

	window->end();
}

void BoardWindow::initMine(){
	for (int i = 0; i < row; i++) {
		std::vector<MineArgs*> mines;
		for (int j = 0; j < col; j++) {
			MineArgs* mineArgs = new MineArgs();
			mineArgs->button = new Fl_Button(MARGIN + BTN_MINE_SIZE * j, MARGIN + BTN_MINE_SIZE * i, BTN_MINE_SIZE, BTN_MINE_SIZE);
			mineArgs->button->callback([](Fl_Widget* w, void* args) {
				auto mineArgs = (MineArgs*)args;
				if (mineArgs->isProcessing) return;
				mineArgs->isProcessing = true;
				if (Fl::event_button() == FL_LEFT_MOUSE && mineArgs->parent->boardArgs.board[mineArgs->y][mineArgs->x] != MINE_FLAG)
					mineArgs->board->leftClick(mineArgs->x, mineArgs->y);
				if (Fl::event_button() == FL_RIGHT_MOUSE)
					mineArgs->board->rightClick(mineArgs->x, mineArgs->y);
				mineArgs->parent->update(mineArgs->parent);
				mineArgs->isProcessing = false;
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
	std::vector<std::future<void> > allocateTaskList, redrawTaskList;

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
			case MINE_MINE:
				currentButton->color(FL_RED);
			case MINE_FLAG:
			case MINE_SUS:
				currentButton->image(PATH_ICON[currentValue]);
				break;
			case MINE_MASK:
				break;
			default: {
				const char* label = new char[2] {bw->boardArgs.board[i][j], '\0'};
				currentButton->label(label);
			}case MINE_NULL:
				currentButton->set();
				currentButton->deactivate();
				break;
			}
		}
	}

	for (auto& mines : bw->mineList) for (auto& mine : mines) redrawTaskList.push_back(std::async(std::launch::async, [=]() { mine->button->redraw(); }));
	for (auto& task : redrawTaskList) task.get();

	switch (bw->boardArgs.gameStatus) {
		case BOARD_STATUS_WIN:
		case BOARD_STATUS_LOSE:
			bw->lose(bw);
			break;
	}
}

void BoardWindow::lose(BoardWindow* bw) {
	bw->window->redraw();
}
