#include "sudokusolver.h"
#include "ui_sudokusolver.h"
#include <QHeaderView>
#include <QLabel>
#include <QDebug>
#include <QRandomGenerator>

bool checkVal(QTableWidget* t, int row, int col, int val){
    //check row
    for(int i=0;i<9;i++){
        if(qobject_cast<QLabel*>(t->cellWidget(row,i))->text().toInt() == val){

            return false;
        }

    }
    //check col
    for(int i=0;i<9;i++){
        if(qobject_cast<QLabel*>(t->cellWidget(i,col))->text().toInt() == val){

            return false;
        }

    }

    //check 3x3 box
    int box_r = floor(row/3.0)*3;
    int box_c = floor(col/3.0)*3;
    for(int i=box_r; i<box_r+3;i++){
        for(int j=box_c;j<box_c+3;j++){
            if(qobject_cast<QLabel*>(t->cellWidget(i,j))->text().toInt() == val){

                return false;
            }
        }
    }
    return true;

}

SudokuSolver::SudokuSolver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SudokuSolver)
{
    ui->setupUi(this);
    ui->tableWidget->setRowCount(9);
    ui->tableWidget->setColumnCount(9);
    for (int i=0;i< ui->tableWidget->rowCount();i++) {
        ui->tableWidget->setRowHeight(i,50);
    }

    for (int i=0;i< ui->tableWidget->columnCount();i++) {
        ui->tableWidget->setColumnWidth(i,50);
    }
    ui->tableWidget->setMinimumSize(452,452);
    ui->tableWidget->setMaximumSize(452,452);

    for (int i=0;i< ui->tableWidget->rowCount();i++) {
        for (int j=0;j< ui->tableWidget->columnCount();j++) {

            ui->tableWidget->setCellWidget(i,j,new QLabel(QString::number(0)));
            qobject_cast<QLabel*>(ui->tableWidget->cellWidget(i,j))->setAlignment(Qt::AlignCenter);

        }
    }




}

SudokuSolver::~SudokuSolver()
{
    delete ui;
}

void SudokuSolver::genNumbers()
{
    for (int i=0;i< ui->tableWidget->rowCount();i++) {
        for (int j=0;j< ui->tableWidget->columnCount();j++) {

            int r = QRandomGenerator::securelySeeded().generate()%9+1;
            int cnt = 0;
            while(!checkVal(ui->tableWidget,i,j,r)){
                r = ++r%10;
                if(r==0){
                    r++;
                }
                cnt++;
                if(cnt>9){
                    break;
                }

            }
            qobject_cast<QLabel*>(ui->tableWidget->cellWidget(i,j))->setText(QString::number(r));
            qApp->processEvents();
        }
    }
}

void SudokuSolver::on_pushButton_clicked()
{
    genNumbers();
}
