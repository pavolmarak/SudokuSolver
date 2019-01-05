#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "sudokuboard.h"

#define SUDOKU_CELL_SIZE 50

#define PRIMARY_COLOR qRgb(230,230,230)
#define SECONDARY_COLOR qRgb(255, 255, 255)
#define SELECTION_COLOR qRgb(0, 163, 204)

#define NORMAL_FONT QFont("Tahoma",12)
#define CANDIDATE_FONT QFont("Tahoma",8)
#define NORMAL_HIGHLIGHT_FONT QFont("Tahoma",12,QFont::Bold)
#define CANDIDATE_HIGHLIGHT_FONT QFont("Tahoma",8)

#define CANDIDATE_TEXT_COLOR qRgb(130,130,130)
#define CELL_TEXT_COLOR qRgb(0,0,0)

#define NEIGHBOR_BACKGROUND_COLOR qRgb(0, 153, 204)
#define NEIGHBOR_FOREGROUND_COLOR qRgb(255,255,255)
#define CELL_BACKGROUND_COLOR qRgb(0,0,0)
#define CELL_FOREGROUND_COLOR qRgb(255,255,255)

namespace Ui {
class Sudoku;
}

class Sudoku : public QMainWindow
{
    Q_OBJECT

public:
    explicit Sudoku(QWidget *parent = nullptr);
    ~Sudoku();

private slots:
    // SLOTS
    void onItemClicked(QTableWidgetItem*);
    void onItemEntered(QTableWidgetItem*);
    void on_regenarateBoardButton_clicked();
    void on_unhighlightButton_clicked();
    void on_solveButton_clicked();
    // CUSTOM SLOTS
    void redrawBoardSlot();
    void highlightCellSlot(int,int,QColor,QColor);
    void debugPrintSlot(QString message, QColor background, QColor foreground);
    void on_testButton_clicked();

    void on_advanced_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::Sudoku *ui;
    SudokuBoard sudoku_board;
    void createBoardUI();
    void resetBoardColorUI();
    void generateBoardUI();
    void redrawBoardUI();
    void highlightNeighbors(int,int, QColor, QColor,QColor,QColor);
    void highlightCell(int,int,QColor,QColor);
    void test(int);

signals:
    void debugPrint(QString message, QColor background = Qt::white, QColor foreground = Qt::black);
};

#endif // SUDOKUSOLVER_H
