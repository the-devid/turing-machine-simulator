#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <algorithm>
#include <QTimer>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    turing_tape = new SequenceCells(this, this->width() / 2 - SequenceCells::CELL_SIZE / 2
                                    - SequenceCells::CELLS_AMOUNT / 2 * (SequenceCells::CELL_SIZE + SequenceCells::MARGIN)
                                    - SequenceCells::MARGIN / 2 - SequenceCells::BANNER_SIZE, 100);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete turing_tape;
}
void MainWindow::on_statesButton_clicked()
{
    QString statesLine = ui->statesEdit->text();
    bool is_ok = 1;
    int states_count = statesLine.toInt(&is_ok);
    if (is_ok) {
        this->states = states_count;
        this->statesFixed = true;
        ui->statesEdit->setReadOnly(true);
        ui->statesButton->setDisabled(true);
        ui->statesButton->repaint();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Нужно ввести число состояний");
        msgBox.setWindowTitle("Ошибка");
        msgBox.exec();
    }
}

void MainWindow::on_symbolsButton_clicked()
{
    QString symbolsLine = ui->symbolsEdit->text();
    symbolsLine = symbolsLine.simplified();
    symbolsLine.replace(" ", "");
    std::sort(symbolsLine.begin(), symbolsLine.end());
    auto it = std::unique(symbolsLine.begin(), symbolsLine.end());
    symbolsLine.remove(it - symbolsLine.begin(), symbolsLine.end() - it);
    if (!symbolsLine.count('_')) {
        symbolsLine.push_back('_');
        this->symbols = symbolsLine;
        this->symbolsFixed = true;
        ui->symbolsEdit->setReadOnly(true);
        ui->symbolsButton->setDisabled(true);
        ui->symbolsButton->repaint();
    } else if (symbolsLine.count(',')) {
        QMessageBox msgBox;
        msgBox.setText("Символ , зарезервирован как разделитель триплета");
        msgBox.setWindowTitle("Ошибка");
        msgBox.exec();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Символ _ зарезервирован как пустой");
        msgBox.setWindowTitle("Ошибка");
        msgBox.exec();
    }
}

void MainWindow::on_tableButton_clicked()
{
    if (!this->statesFixed || !this->symbolsFixed) {
        QMessageBox msgBox;
        msgBox.setText("Сначала введите количество состояний и все символы (кроме пустого _)");
        msgBox.setWindowTitle("Ошибка");
        msgBox.exec();
        return;
    }
    ui->tableWidget->setRowCount(this->states + 1);
    ui->tableWidget->setColumnCount(this->symbols.size() + 1);
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem());
    ui->tableWidget->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->item(0, 0)->setFlags(ui->tableWidget->item(0, 0)->flags() ^ (Qt::ItemIsEditable));
    ui->tableWidget->item(0, 0)->setBackgroundColor("#AAAAAA");
    for (int state = 0; state < this->states; ++state) {
        ui->tableWidget->setItem(state+1, 0, new QTableWidgetItem());
        ui->tableWidget->item(state+1, 0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(state+1, 0)->setFlags(ui->tableWidget->item(state+1, 0)->flags() ^ (Qt::ItemIsEditable));
        ui->tableWidget->item(state+1, 0)->setText(QString::number((state)));
    }
    for (int ind = 0; ind < this->symbols.size(); ++ind) {
        ui->tableWidget->setItem(0, ind+1, new QTableWidgetItem());
        ui->tableWidget->item(0, ind+1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(0, ind+1)->setFlags(ui->tableWidget->item(0, ind+1)->flags() ^ (Qt::ItemIsEditable));
        ui->tableWidget->item(0, ind+1)->setText(QString(1, symbols[ind]));
    }
    for (int row = 1; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 1; col < ui->tableWidget->columnCount(); ++col) {
            ui->tableWidget->setItem(row, col, new QTableWidgetItem());
            ui->tableWidget->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MainWindow::on_inputButton_clicked()
{
    if (!this->statesFixed || !this->symbolsFixed) {
        QMessageBox msgBox;
        msgBox.setText("Сначала введите количество состояний и все символы (кроме пустого _)");
        msgBox.setWindowTitle("Ошибка");
        msgBox.exec();
        return;
    }
    QString inputLine = ui->inputEdit->text();
    inputLine = inputLine.simplified();
    inputLine.replace(" ", "");
    while (inputLine.size() < SequenceCells::CELLS_AMOUNT / 2 + 1)
        inputLine.push_back('_');
    for (auto c : inputLine) {
        if (!this->symbols.count(c)) {
            QMessageBox msgBox;
            msgBox.setText("Неизвестные символы");
            msgBox.setWindowTitle("Ошибка");
            msgBox.exec();
            return;
        }
    }
    this->tape = QString(SequenceCells::CELLS_AMOUNT / 2, '_') + inputLine;
    this->tapeStart = SequenceCells::CELLS_AMOUNT / 2;
    for (int ind = 0; ind < SequenceCells::CELLS_AMOUNT; ++ind) {
        this->turing_tape->change_text(ind, QString(1, tape[ind]));
    }
    this->inputFixed = true;
    ui->inputEdit->setReadOnly(true);
    ui->inputButton->setDisabled(true);
    ui->inputButton->repaint();
}

void MainWindow::on_startButton_clicked()
{

    if (!this->statesFixed || !this->symbolsFixed || !this->inputFixed) {
        QMessageBox msgBox;
        msgBox.setText("Сначала задайте и зафиксируйте все входные параметры");
        msgBox.setWindowTitle("Ошибка");
        msgBox.exec();
        return;
    }
    for (int row = 1; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 1; col < ui->tableWidget->columnCount(); ++col) {
            QString actions = ui->tableWidget->item(row, col)->text();
            actions = actions.simplified();
            actions.replace(" ", "");
            if (actions.count(',') != 2) {
                QMessageBox msgBox;
                msgBox.setText("В клетке (" + QString::number(row) + ", " + QString::number(col)
                               + ") должен быть триплет разделенный двумя запятыми");
                msgBox.setWindowTitle("Ошибка");
                msgBox.exec();
                return;
            }
            auto triplet = actions.split(",");
            if (triplet[0].size() != 0 && (triplet[0].size() != 1 || this->symbols.count(triplet[0][0]) != 1)) {
                QMessageBox msgBox;
                msgBox.setText("В клетке (" + QString::number(row) + ", " + QString::number(col)
                               + ") некорректный символ для записи");
                msgBox.setWindowTitle("Ошибка");
                msgBox.exec();
                return;
            }
            if (triplet[1].size() != 1 || QString("LNR").count(triplet[1][0]) != 1) {
                QMessageBox msgBox;
                msgBox.setText("В клетке (" + QString::number(row) + ", " + QString::number(col)
                               + ") некорректное направление");
                msgBox.setWindowTitle("Ошибка");
                msgBox.exec();
                return;
            }
            bool is_ok = 1;
            int nextState = triplet[2].toInt(&is_ok);
            if (triplet[2].size() != 0 && (!is_ok || nextState < -1 || nextState >= states)) {
                QMessageBox msgBox;
                msgBox.setText("В клетке (" + QString::number(row) + ", " + QString::number(col)
                               + ") некорректное следующее состояние");
                msgBox.setWindowTitle("Ошибка");
                msgBox.exec();
                return;
            }
        }
    }

    for (int row = 1; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 1; col < ui->tableWidget->columnCount(); ++col) {
            ui->tableWidget->item(row, col)->setFlags(
                        ui->tableWidget->item(row, col)->flags() ^ (Qt::ItemIsEditable));

        }
    }
    ui->startButton->setDisabled(true);
    ui->startButton->repaint();
    ui->againButton->setDisabled(true);
    ui->againButton->repaint();
    ui->tableButton->setDisabled(true);
    ui->tableButton->repaint();
    this->stopped = false;
    this->curState = 0;
    this->curStep += 0; // or =
    this->shouldStopped = false;
    simulate_one_move();
}
void MainWindow::simulate_one_move() {
    this->curStep += 1;
    QChar curSym = this->tape[this->tapeStart];
    int col = this->symbols.indexOf(curSym) + 1;
    int row = this->curState + 1;
    QString actions = ui->tableWidget->item(row, col)->text();
    actions = actions.simplified();
    actions.replace(" ", "");
    auto triplet = actions.split(",");
    auto nextSym = triplet[0].size() == 0 ? this->tape[tapeStart] : triplet[0][0];
    auto direction = triplet[1][0];
    auto nextState = triplet[2].size() == 0 ? this->curState : triplet[2].toInt();
    this->turing_tape->changeCentral(QString(1, nextSym));
    this->tape[this->tapeStart] = nextSym;
    this->curState = nextState;
    int millisTimer = 0;
    int moveMillisTimer = SequenceCells::TIME_MOVE * 2 * 1000;
    if (direction == 'N') {
        millisTimer = SequenceCells::TIME_MOVE * 2 * 1000;
    } else if (direction == 'L') {
        if (this->tapeStart == SequenceCells::CELLS_AMOUNT / 2) {
            this->tape = "_" + this->tape;
            this->tapeStart += 1;
        }
        QTimer *moveTimer = new QTimer(this);
        moveTimer->setSingleShot(true);
        connect(moveTimer, &QTimer::timeout, this, [&] ()
        {this->turing_tape->addToLeft(QString(1, this->tape[int(tapeStart - SequenceCells::CELLS_AMOUNT / 2)]));});
        moveTimer->start(moveMillisTimer + 250);
        tapeStart -= 1;
        millisTimer = SequenceCells::TIME_MOVE * 3 * 1000;
    } else { // R
        if (this->tapeStart + SequenceCells::CELLS_AMOUNT / 2 + 1 >= this->tape.size()) {
            this->tape.push_back('_');
        }

        QTimer *moveTimer = new QTimer(this);
        moveTimer->setSingleShot(true);
        connect(moveTimer, &QTimer::timeout, this, [&] ()
        {this->turing_tape->addToRight(QString(1, this->tape[int(tapeStart + SequenceCells::CELLS_AMOUNT / 2)]));});
        moveTimer->start(moveMillisTimer + 250);
        tapeStart += 1;
        millisTimer = SequenceCells::TIME_MOVE * 3 * 1000;
    }
    if (nextState != -1 && !shouldStopped && this->curStep <= this->MAXSTEPS) {
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, &MainWindow::simulate_one_move);
        timer->start(millisTimer + 500);
    } else if (nextState == -1 || shouldStopped) {
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, &MainWindow::unlock_all);
        timer->start(millisTimer + 500);
    } else if (this->curStep > this->MAXSTEPS) {
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, &MainWindow::unlock_all);

        connect(timer, &QTimer::timeout, this, &MainWindow::on_againButton_clicked);
        timer->start(millisTimer + 500);

        QMessageBox msgBox;
        msgBox.setText("Программа выполняется слишком долго (уже " + QString::number(this->MAXSTEPS) + " итераций)");
        msgBox.setWindowTitle("Ошибка");
        msgBox.exec();
    }
}

void MainWindow::unlock_all() {
    for (int row = 1; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 1; col < ui->tableWidget->columnCount(); ++col) {
            ui->tableWidget->item(row, col)->setFlags(
                        ui->tableWidget->item(row, col)->flags() ^ (Qt::ItemIsEditable));

        }
    }
    ui->startButton->setDisabled(false);
    ui->startButton->repaint();
    ui->againButton->setDisabled(false);
    ui->againButton->repaint();
    ui->tableButton->setDisabled(false);
    ui->tableButton->repaint();
    this->stopped = true;
}

void MainWindow::on_stopButton_clicked()
{
    if (!this->stopped) {
        shouldStopped = true;
    }
}

void MainWindow::on_againButton_clicked()
{

    for (int ind = 0; ind < SequenceCells::CELLS_AMOUNT; ++ind) {
        this->turing_tape->change_text(ind, "");
    }
    this->statesFixed = 0;
    ui->statesEdit->setReadOnly(false);
    ui->statesEdit->setText("");
    ui->statesEdit->repaint();
    ui->statesButton->setDisabled(false);
    ui->statesButton->repaint();
    this->symbolsFixed = 0;
    ui->symbolsEdit->setReadOnly(false);
    ui->symbolsEdit->setText("");
    ui->symbolsEdit->repaint();
    ui->symbolsButton->setDisabled(false);
    ui->symbolsButton->repaint();
    this->inputFixed = 0;
    ui->inputEdit->setReadOnly(false);
    ui->inputEdit->setText("");
    ui->inputEdit->repaint();
    ui->inputButton->setDisabled(false);
    ui->inputButton->repaint();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->repaint();
    //this->intableFixed = 0;
    this->shouldStopped = 0;
    this->stopped = 1;
    this->states = 0;
    this->symbols = "";
    this->tape = "";
    this->tapeStart = 0;
    this->curState = 0;
    this->curStep = 0;
}

void MainWindow::on_helpButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Справка по симулятору.\nДля начала необходимо ввести количество состояний (нумерация с 0)\
 и строку содержащую все используемые в ходе вычислений символы, после чего необходимо данные параметры зафиксировать нажатием на кнопку.\
 \nПосле этого необходимо начать ввод таблицы переходов автомата - строки соответствуют состояниям автомата, а столбцы\
 - текущему символу ленты. В каждой клетке должен находиться триплет - тройка соотвтествующая тому, какой символ записать в клетку,\
 куда сдвинуться (L - влево, R - вправо, N - остаться на месте) и в какое состояние перейти, причем если указан номер -1, то машина\
 Тьюринга завершает работу (финальное состояние). \n\
Пишущая головка машины наодится в середине отображаемой ленты. \n\
Символ _ зарезервирован как пустой символ, находящийся во всех клетках ленты по умолчанию. Запятая зарезервирована как разделитель в триплете.\
В триплете могут быть пропущены символ и/или состояние - следующим будет текущее. Кнопка Стоп поставит симуляцию на паузу, ее можно будет продолжить.\
Кнопка Заново сбросит все поля и переменные и вернет программу к состоянию при запуске. Ограничение на количество операций - 100 штук.");
    msgBox.setWindowTitle("Ошибка");
    msgBox.exec();
}
