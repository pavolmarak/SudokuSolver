#include "sudokutable.h"
#include <QDebug>
#include <QMouseEvent>

SudokuTable::SudokuTable()
{

}

void SudokuTable::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << event->pos();
}
