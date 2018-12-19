#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QMainWindow>

namespace Ui {
class SudokuSolver;
}

class SudokuSolver : public QMainWindow
{
    Q_OBJECT

public:
    explicit SudokuSolver(QWidget *parent = nullptr);
    ~SudokuSolver();
    void genNumbers();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SudokuSolver *ui;

};

#endif // SUDOKUSOLVER_H
