#ifndef SUDOKUBOARD_H
#define SUDOKUBOARD_H

#include <QObject>
#include <QVector>
#include <QSet>
#include <QDebug>

#define SUDOKU_BOARD_SIDE 9
#define SUDOKU_CELL_SIZE 50

#define PRIMARY_COLOR qRgb(255, 195, 77)
#define SECONDARY_COLOR qRgb(255, 255, 255)

typedef unsigned char val;

class SudokuBoard : public QObject
{
    Q_OBJECT
public:
    explicit SudokuBoard(QObject *parent = nullptr);
    SudokuBoard(int size);
    void generate();
    void clear();
    void print();
    bool isOk();
    const QVector<QVector<val>>& getBoard() const;
    QVector<QPoint> getNeighbors(QPoint);

signals:
public slots:
private:
    QVector<QVector<val>> board;
    QSet<val> getNeighbors(int row, int col);
    bool generateCells(int row=0, int col=0);
};

#endif // SUDOKUBOARD_H
