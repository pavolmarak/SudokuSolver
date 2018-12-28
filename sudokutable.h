#ifndef SUDOKUTABLE_H
#define SUDOKUTABLE_H

#include <QObject>
#include <QTableWidget>

class SudokuTable : public QTableWidget
{
public:
    SudokuTable();
protected:
     void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // SUDOKUTABLE_H
