#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QMainWindow>
#include "sudokuboard.h"
#include <QTableWidgetItem>
#include <QMouseEvent>

namespace Ui {
class SudokuSolver;
}

class SudokuSolver : public QMainWindow
{
    Q_OBJECT

public:
    explicit SudokuSolver(QWidget *parent = nullptr);
    ~SudokuSolver();

private slots:
    void onItemClicked(QTableWidgetItem*);
    void onItemEntered(QTableWidgetItem*);
    void on_regenarateBoardButton_clicked();

    void on_unhighlightButton_clicked();

private:
    Ui::SudokuSolver *ui;
    void createBoardUI();
    void resetBoardColorUI();
    void generateBoardUI();
    SudokuBoard sudoku_board;

};

#endif // SUDOKUSOLVER_H
