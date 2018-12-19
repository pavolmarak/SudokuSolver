#include "sudokusolver.h"
#include "ui_sudokusolver.h"

SudokuSolver::SudokuSolver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SudokuSolver)
{
    ui->setupUi(this);
}

SudokuSolver::~SudokuSolver()
{
    delete ui;
}
