#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "sequencecells.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    SequenceCells *turing_tape = nullptr;
    bool statesFixed = 0;
    bool symbolsFixed = 0;
    bool inputFixed = 0;
    bool intableFixed = 0;
    bool shouldStopped = 0;
    bool stopped = 1;
    int states = 0;
    QString symbols = "";
    QString tape = "";
    int tapeStart = 0;
    int curState = 0;
    int curStep = 0;
    static constexpr int MAXSTEPS = 100;

private slots:

    void on_statesButton_clicked();

    void on_symbolsButton_clicked();

    void on_tableButton_clicked();

    void on_inputButton_clicked();

    void on_startButton_clicked();

    void on_helpButton_clicked();

    void simulate_one_move();

    void unlock_all();

    void on_stopButton_clicked();

    void on_againButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
