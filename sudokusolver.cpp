#include "sudokusolver.h"
#include "ui_sudokusolver.h"
#include <QHeaderView>
#include <QLabel>
#include <QDebug>
#include <QRandomGenerator>
#include <QSet>

SudokuSolver::SudokuSolver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SudokuSolver),
    sudoku_board(SUDOKU_BOARD_SIDE)
{
    ui->setupUi(this);
    this->createBoardUI();

    connect(ui->tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onItemClicked(QTableWidgetItem*)));

}

SudokuSolver::~SudokuSolver()
{
    delete ui;
}

void SudokuSolver::onItemClicked(QTableWidgetItem* item)
{
    resetBoardColorUI();
    //qDebug() << item->text();
    QVector<QPoint> vp = sudoku_board.getNeighbors(QPoint(item->row(),item->column()));
    for(const QPoint& p : vp){
        //ui->tableWidget->item(p.y(),p.x())->setBackground(QBrush(QColor(128, 77, 0)));
        ui->tableWidget->item(p.y(),p.x())->setSelected(true);
    }

}

void SudokuSolver::createBoardUI()
{
    // set up dimensions
    ui->tableWidget->setRowCount(SUDOKU_BOARD_SIDE);
    ui->tableWidget->setColumnCount(SUDOKU_BOARD_SIDE);
    for (int i=0;i< ui->tableWidget->rowCount();i++) {
        ui->tableWidget->setRowHeight(i,SUDOKU_CELL_SIZE);
    }

    for (int i=0;i< ui->tableWidget->columnCount();i++) {
        ui->tableWidget->setColumnWidth(i,SUDOKU_CELL_SIZE);
    }
    ui->tableWidget->setMinimumSize(SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2,SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2);
    ui->tableWidget->setMaximumSize(SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2,SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2);

    // generate initial values
    for (int i=0;i< ui->tableWidget->rowCount();i++) {
        for (int j=0;j< ui->tableWidget->columnCount();j++) {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(0)));
            ui->tableWidget->item(i,j)->setFont(QFont("Tahoma",12));
            ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->item(i,j)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        }
    }

    resetBoardColorUI();
}

void SudokuSolver::resetBoardColorUI()
{
    QVector<QVector<QRgb>> colorbox;
    colorbox.push_back({PRIMARY_COLOR,SECONDARY_COLOR,PRIMARY_COLOR});
    colorbox.push_back({SECONDARY_COLOR,PRIMARY_COLOR,SECONDARY_COLOR});
    colorbox.push_back({PRIMARY_COLOR,SECONDARY_COLOR,PRIMARY_COLOR});

    for(int i=0; i<ui->tableWidget->rowCount();i++){
        for(int j =0; j< ui->tableWidget->columnCount();j++){
            int box_r = floor(i/3.0); // row index of the box top left position
            int box_c = floor(j/3.0); // column index of the box top left position
            ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(colorbox[box_r][box_c])));
            ui->tableWidget->item(i,j)->setSelected(false);
        }
    }
}

void SudokuSolver::on_pushButton_clicked()
{
    resetBoardColorUI();
    sudoku_board.clear();
    sudoku_board.generate();
    //board.print();
    //qDebug() << sudoku_board.isOk();
    auto brd = sudoku_board.getBoard();
    // generate initial values
    for (int i=0;i< ui->tableWidget->rowCount();i++) {
        for (int j=0;j< ui->tableWidget->columnCount();j++) {
            ui->tableWidget->item(i,j)->setText(QString::number(brd[i][j]));
        }
    }
}

