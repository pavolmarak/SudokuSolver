#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QMainWindow>
#include "sudokuboard.h"
#include <QTableWidgetItem>


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
    void on_pushButton_clicked();
    void onItemClicked(QTableWidgetItem*);

private:
    Ui::SudokuSolver *ui;
    void createBoardUI();
    void resetBoardColorUI();
    SudokuBoard sudoku_board;

};

#endif // SUDOKUSOLVER_H
