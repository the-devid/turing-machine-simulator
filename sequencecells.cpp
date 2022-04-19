/**
 * Implements the "Sequence of cells" class consisting of QLabels
 * that are used in visualizing of Turing's Machine
 */

#include "sequencecells.h"

#include <QLabel>
#include <functional>
#include <QDebug>
#include <QTimer>

SequenceCells::SequenceCells(QObject *parent) : QObject(parent) {}

SequenceCells::SequenceCells(QWidget *parent, int64_t x, int64_t y) {
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->cells.resize(CELLS_AMOUNT);
    this->left_banner = new QLabel(parent);
    this->left_banner->setGeometry(x, y, BANNER_SIZE, BANNER_SIZE);
    this->left_banner->setText(QString(""));
    this->left_banner->setStyleSheet(this->stylesheet);
    this->right_banner = new QLabel(parent);
    this->right_banner->setGeometry(x + BANNER_SIZE + (CELL_SIZE + MARGIN) * CELLS_AMOUNT,
                                    y, BANNER_SIZE, BANNER_SIZE);
    this->right_banner->setText(QString(""));
    this->right_banner->setStyleSheet(this->stylesheet);
    for (size_t ind = 0; ind < cells.size(); ++ind) {
        this->cells[ind] = new QLabel(parent);
        this->cells[ind]->setGeometry(x + BANNER_SIZE + MARGIN / 2 + ind * (CELL_SIZE + MARGIN),
                                      y + MARGIN / 2, CELL_SIZE, CELL_SIZE);
        this->cells[ind]->setText(QString(" "));
        this->cells[ind]->setStyleSheet(this->stylesheet);
        this->cells[ind]->lower();
    }
}

SequenceCells::~SequenceCells() {
    delete this->left_banner;
    this->left_banner = nullptr;
    delete this->right_banner;
    this->right_banner = nullptr;
    for (size_t ind = 0; ind < cells.size(); ++ind) {
        delete this->cells[ind];
        this->cells[ind] = nullptr;
    }
}

void SequenceCells::move_cells_left() {
    for (auto &cell : this->cells) {
        cell->setGeometry(cell->geometry().x() - (CELL_SIZE + MARGIN) / ITER_MOVE,
                          cell->geometry().y(), cell->geometry().width(), cell->geometry().height());
        cell->show();
    }
};

void SequenceCells::delete_first() {
    delete this->cells.front();
    this->cells.erase(this->cells.begin());
}

void SequenceCells::addToRight(QString newText) { // [...] + ADD
    this->cells.push_back(new QLabel(this->parent));
    this->cells.back()->lower();
    this->cells.back()->setGeometry(x + BANNER_SIZE + (CELL_SIZE + MARGIN) * CELLS_AMOUNT + MARGIN / 2,
                                    y + MARGIN / 2, CELL_SIZE, CELL_SIZE);
    this->cells.back()->setStyleSheet(this->stylesheet);
    this->cells.back()->setText(newText);
    this->cells.back()->show();
    for (size_t iter = 0; iter < ITER_MOVE; ++iter) {
        QTimer::singleShot((iter + 1) * TIME_MOVE / ITER_MOVE * 1000, this, &SequenceCells::move_cells_left);
    }
    QTimer::singleShot((ITER_MOVE + 1) * TIME_MOVE / ITER_MOVE * 1000, this, &SequenceCells::delete_first);

}

void SequenceCells::move_cells_right() {
    for (auto &cell : this->cells) {
        cell->setGeometry(cell->geometry().x() + (CELL_SIZE + MARGIN) / ITER_MOVE,
                          cell->geometry().y(), cell->geometry().width(), cell->geometry().height());
        cell->show();
    }
};

void SequenceCells::delete_last() {
    delete this->cells.back();
    this->cells.erase(this->cells.end()-1);
}

void SequenceCells::addToLeft(QString newText) { // [...] + ADD
    this->cells.insert(cells.begin(), new QLabel(this->parent));
    this->cells.front()->lower();
    this->cells.front()->setGeometry(x + MARGIN /2,
                                    y + MARGIN / 2, CELL_SIZE, CELL_SIZE);
    this->cells.front()->setStyleSheet(this->stylesheet);
    this->cells.front()->setText(newText);
    this->cells.front()->show();
    for (size_t iter = 0; iter < ITER_MOVE; ++iter) {
        QTimer::singleShot((iter + 1) * TIME_MOVE / ITER_MOVE * 1000, this, &SequenceCells::move_cells_right);
    }
    QTimer::singleShot((ITER_MOVE + 1) * TIME_MOVE / ITER_MOVE * 1000, this, &SequenceCells::delete_last);
}

void SequenceCells::change_background(int pos, QString color) {
    this->cells.at(pos)->setStyleSheet("QLabel {background-color: " + color + ";}");
    this->cells.at(pos)->repaint();
}

void SequenceCells::change_text(int pos, QString newText) {
    this->cells.at(pos)->setText(newText);
    this->cells.at(pos)->repaint();
}

void SequenceCells::changeCentral(QString newText) {
    QTimer *timerpink = new QTimer(this);
    timerpink->setSingleShot(true);
    QTimer *timertext = new QTimer(this);
    timertext->setSingleShot(true);
    QTimer *timerblue = new QTimer(this);
    timerblue->setSingleShot(true);
    this->_textToSet = newText;
    connect(timerpink, &QTimer::timeout, this, [&](){this->change_background(CELLS_AMOUNT / 2, pinkColor);});
    connect(timertext, &QTimer::timeout, this, [&](){this->change_text(CELLS_AMOUNT / 2, _textToSet);});
    connect(timerblue, &QTimer::timeout, this, [&](){this->change_background(CELLS_AMOUNT / 2, blueColor);});

    timerpink->start((0) * TIME_MOVE / 2 * 1000);
    timertext->start((1) * TIME_MOVE / 2 * 1000);
    timerblue->start((2) * TIME_MOVE / 2 * 1000);
}

QString SequenceCells::getText(int pos) {
    return this->cells[pos]->text();
}
