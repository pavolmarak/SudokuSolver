#include "sudokusolver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(PRO_PWD+QString("/sudoku_icon.png")));
    SudokuSolver w;
    w.setMouseTracking(true);
    w.show();
    return a.exec();
}
