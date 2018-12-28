#ifndef SUDOKUBOARD_H
#define SUDOKUBOARD_H

#include <QObject>
#include <QVector>
#include <QSet>
#include <QDebug>

typedef unsigned char val;

#define SUDOKU_BOARD_SIDE 9
#define SUDOKU_CELL_SIZE 50

#define PRIMARY_COLOR qRgb(230,230,230)
#define SECONDARY_COLOR qRgb(255, 255, 255)
#define SELECTION_COLOR qRgb(0, 163, 204)

#define DEFAULT_CANDIDATES QSet<val>{1,2,3,4,5,6,7,8,9}

typedef struct {
    QPoint coord;
    val value;
    bool revealed;
    QSet<val> candidates;
} CELL_INFO;

class SudokuBoard : public QObject
{
    Q_OBJECT
public:
    explicit SudokuBoard(QObject *parent = nullptr);
    SudokuBoard(int size);
    void generate();
    void clear();
    void print(bool detailed =false);
    bool isOk();
    void solve();
    const QVector<QVector<CELL_INFO>>& getBoard() const;
    QVector<CELL_INFO> getNeighbors(int,int);
    QSet<val> getUniqueNeighborValues(int,int);

signals:
public slots:
private:
    QVector<QVector<CELL_INFO>> board;
    bool generateCells(int row=0, int col=0);
    void showClues();
};

#endif // SUDOKUBOARD_H
