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
    this->generateBoardUI();
    connect(ui->sudoku_ui, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onItemClicked(QTableWidgetItem*)));
    connect(ui->sudoku_ui, SIGNAL(itemEntered(QTableWidgetItem*)), this, SLOT(onItemEntered(QTableWidgetItem*)));
}

SudokuSolver::~SudokuSolver()
{
    delete ui;
}

void SudokuSolver::onItemClicked(QTableWidgetItem* item)
{
    // ...
}

void SudokuSolver::onItemEntered(QTableWidgetItem* item)
{
    resetBoardColorUI();
    //qDebug() << item->text();
    QVector<CELL_INFO> vp = sudoku_board.getNeighbors(item->row(),item->column());
    QSet<val> neigh_set;
    for(const CELL_INFO& p : vp){
        //ui->tableWidget->item(p.y(),p.x())->setBackground(QBrush(QColor(128, 77, 0)));
        ui->sudoku_ui->item(p.coord.x(),p.coord.y())->setBackground(QBrush(QColor(SELECTION_COLOR)));
        ui->sudoku_ui->item(p.coord.x(),p.coord.y())->setForeground(QBrush(Qt::white));
        if(p.revealed){
            neigh_set.insert(p.value);
        }
    }
    item->setSelected(false);
    item->setBackground(QBrush(Qt::black));
    item->setForeground(QBrush(Qt::white));
    QString cand_str;
    QSet<val> all_cands_set{1,2,3,4,5,6,7,8,9};
    all_cands_set.subtract(neigh_set);
    QList<val> ql = all_cands_set.toList();
    std::sort(ql.begin(),ql.end());
    for (const val& v : ql){
        cand_str += QString::number(v)+" ";
    }
    ui->currItem->setText("("+QString::number(item->row()+1)+","+QString::number(item->column()+1)+")");
    ui->candidates->setText(cand_str);
}



void SudokuSolver::createBoardUI()
{
    // mouse tracking
    ui->sudoku_ui->setMouseTracking(true);

    // set up dimensions
    ui->sudoku_ui->setRowCount(SUDOKU_BOARD_SIDE);
    ui->sudoku_ui->setColumnCount(SUDOKU_BOARD_SIDE);
    for (int i=0;i< ui->sudoku_ui->rowCount();i++) {
        ui->sudoku_ui->setRowHeight(i,SUDOKU_CELL_SIZE);
    }

    for (int i=0;i< ui->sudoku_ui->columnCount();i++) {
        ui->sudoku_ui->setColumnWidth(i,SUDOKU_CELL_SIZE);
    }
    ui->sudoku_ui->setMinimumSize(SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2,SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2);
    ui->sudoku_ui->setMaximumSize(SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2,SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+2);

    // generate initial values
    for (int i=0;i< ui->sudoku_ui->rowCount();i++) {
        for (int j=0;j< ui->sudoku_ui->columnCount();j++) {
            ui->sudoku_ui->setItem(i,j,new QTableWidgetItem(QString::number(0)));
            ui->sudoku_ui->item(i,j)->setFont(QFont("Tahoma",12));
            ui->sudoku_ui->item(i,j)->setTextAlignment(Qt::AlignCenter);
            ui->sudoku_ui->item(i,j)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
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

    for(int i=0; i<ui->sudoku_ui->rowCount();i++){
        for(int j =0; j< ui->sudoku_ui->columnCount();j++){
            int box_r = floor(i/3.0); // row index of the box top left position
            int box_c = floor(j/3.0); // column index of the box top left position
            ui->sudoku_ui->item(i,j)->setBackground(QBrush(QColor(colorbox[box_r][box_c])));
            ui->sudoku_ui->item(i,j)->setForeground(QBrush(Qt::black));
            ui->sudoku_ui->item(i,j)->setSelected(false);
        }
    }
}

void SudokuSolver::generateBoardUI()
{
    resetBoardColorUI();
    sudoku_board.generate();
    //board.print();
    //qDebug() << sudoku_board.isOk();
    auto brd = sudoku_board.getBoard();
    // fill board with initial values
    for (int i=0;i< ui->sudoku_ui->rowCount();i++) {
        for (int j=0;j< ui->sudoku_ui->columnCount();j++) {
            if(brd[i][j].revealed){
                ui->sudoku_ui->item(i,j)->setText(QString::number(brd[i][j].value));
                //ui->sudoku_ui->item(i,j)->setFont(QFont("Tahoma",12,QFont::Bold));
            }
            else{
                ui->sudoku_ui->item(i,j)->setText("");
            }
        }
    }
}

void SudokuSolver::on_regenarateBoardButton_clicked()
{
    generateBoardUI();
}



void SudokuSolver::on_unhighlightButton_clicked()
{
    resetBoardColorUI();
}
