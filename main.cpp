#include "sudoku.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(PRO_PWD+QString("/sudoku_icon.png")));
    a.setApplicationDisplayName("by Pavol Marak © 2019");
    Sudoku w;
    w.setMouseTracking(true);
    w.show();
    return a.exec();
}
