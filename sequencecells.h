#ifndef SEQUENCECELLS_H
#define SEQUENCECELLS_H

#include <QObject>

#include <QLabel>
#include <QThread>
#include <functional>
#include <QDebug>
class SequenceCells : public QObject {
    Q_OBJECT
private:
    const QString stylesheet = "QLabel {background-color: #DBDBFD; font-family: \"PT Sans\"; qproperty-alignment: AlignCenter;}",
                    pinkColor = "#FDDBDB", blueColor = "#DBDBFD";
    QVector<QLabel*> cells;
    QLabel *left_banner = 0, *right_banner = 0;
    int64_t x, y; // position of upper left
    QWidget *parent = 0;

    QString _textToSet = "";

private slots:
    void move_cells_left();
    void delete_first();
    void move_cells_right();
    void delete_last();
    void change_background(int pos, QString color);

public:
    static constexpr size_t BANNER_SIZE = 50, CELL_SIZE = 40, MARGIN = 10;
    static constexpr size_t CELLS_AMOUNT = 11, ITER_MOVE = 25;
    static constexpr double_t TIME_MOVE = 0.5;
    explicit SequenceCells(QObject *parent = nullptr);
    SequenceCells() {}
    SequenceCells(QWidget *parent, int64_t x = 0, int64_t y = 0);
    ~SequenceCells();
    QString getText(int pos);
public slots:
    void change_text(int pos, QString newText);
    void addToRight(QString newText);
    void addToLeft(QString newText);
    void changeCentral(QString newText);
};

#endif // SEQUENCECELLS_H
