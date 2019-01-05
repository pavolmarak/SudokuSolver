#include "sudoku.h"
#include "ui_sudokusolver.h"
#include <QDebug>
#include <QDateTime>

Sudoku::Sudoku(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Sudoku),
    sudoku_board(SUDOKU_BOARD_SIDE)
{
    ui->setupUi(this);
    ui->debugTextEdit->setFont(QFont("Consolas",10));

    // SIGNAL/SLOT CONNECTIONS
    connect(ui->sudoku_ui, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onItemClicked(QTableWidgetItem*)));
    connect(ui->sudoku_ui, SIGNAL(itemEntered(QTableWidgetItem*)), this, SLOT(onItemEntered(QTableWidgetItem*)));
    connect(&sudoku_board, SIGNAL(redrawBoardSignal()),this,SLOT(redrawBoardSlot()));
    connect(&sudoku_board, SIGNAL(highlightCellSignal(int,int,QColor,QColor)),this,SLOT(highlightCellSlot(int,int,QColor,QColor)));
    connect(&sudoku_board,SIGNAL(debugPrint(QString,QColor,QColor)),this,SLOT(debugPrintSlot(QString,QColor,QColor)));
    connect(this,SIGNAL(debugPrint(QString,QColor,QColor)),this,SLOT(debugPrintSlot(QString,QColor,QColor)));
    createBoardUI();
    generateBoardUI();

}

Sudoku::~Sudoku()
{
    delete ui;
}

// function to create visible Sudoku board, fill it with initial values and set initial colors
void Sudoku::createBoardUI()
{
    // enable mouse tracking
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
    int extra_border = 0;
    ui->sudoku_ui->setMinimumSize(SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+extra_border,SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+extra_border);
    ui->sudoku_ui->setMaximumSize(SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+extra_border,SUDOKU_BOARD_SIDE*SUDOKU_CELL_SIZE+extra_border);

    // generate initial cell values
    for (int i=0;i< ui->sudoku_ui->rowCount();i++) {
        for (int j=0;j< ui->sudoku_ui->columnCount();j++) {
            ui->sudoku_ui->setItem(i,j,new QTableWidgetItem(QString::number(0)));
            ui->sudoku_ui->item(i,j)->setFont(QFont("Tahoma",12));
            ui->sudoku_ui->item(i,j)->setTextAlignment(Qt::AlignCenter);
            ui->sudoku_ui->item(i,j)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        }
    }
    //ui->sudoku_ui->setStyleSheet("gridline-color: rgb(145,145,145);");
    // reset board color to defaults
    resetBoardColorUI();
}

// function to generate new Sudoku board and reset colors
void Sudoku::generateBoardUI()
{
    sudoku_board.generate();
    resetBoardColorUI();
    redrawBoardUI();
    qApp->processEvents();
}

// funtion to reset Sudoku board colors to defaults and disable selection status
void Sudoku::resetBoardColorUI()
{
    QVector<QVector<QRgb>> colorbox;
    colorbox.push_back({PRIMARY_COLOR,SECONDARY_COLOR,PRIMARY_COLOR});
    colorbox.push_back({SECONDARY_COLOR,PRIMARY_COLOR,SECONDARY_COLOR});
    colorbox.push_back({PRIMARY_COLOR,SECONDARY_COLOR,PRIMARY_COLOR});

    auto board = sudoku_board.getBoard();
    for(int i=0; i<ui->sudoku_ui->rowCount();i++){
        for(int j =0; j< ui->sudoku_ui->columnCount();j++){
            int box_r = floor(i/SUDOKU_BOX_SIZE); // row index of the box top left position
            int box_c = floor(j/SUDOKU_BOX_SIZE); // column index of the box top left position

            if(board[ui->sudoku_ui->item(i,j)->row()][ui->sudoku_ui->item(i,j)->column()].revealed){
                ui->sudoku_ui->item(i,j)->setForeground(QBrush(CELL_TEXT_COLOR));
                ui->sudoku_ui->item(i,j)->setFont(NORMAL_FONT);
                ui->sudoku_ui->item(i,j)->setBackground(QBrush(QColor(colorbox[box_r][box_c])));
            }
            else{
                ui->sudoku_ui->item(i,j)->setForeground(QBrush(CANDIDATE_TEXT_COLOR));
                ui->sudoku_ui->item(i,j)->setFont(CANDIDATE_FONT);
                ui->sudoku_ui->item(i,j)->setBackground(QBrush(QColor(colorbox[box_r][box_c])));
            }
            ui->sudoku_ui->item(i,j)->setSelected(false);
        }
    }
}

// function to redraw Sudoku board values and candidate lists
void Sudoku::redrawBoardUI()
{
    auto board = sudoku_board.getBoard();
    for (int i=0;i< ui->sudoku_ui->rowCount();i++) {
        for (int j=0;j< ui->sudoku_ui->columnCount();j++) {
            if(board[i][j].revealed){
                ui->sudoku_ui->item(i,j)->setText(QString::number(board[i][j].value));
                ui->sudoku_ui->item(i,j)->setFont(NORMAL_FONT);
                ui->sudoku_ui->item(i,j)->setTextAlignment(Qt::AlignCenter);
            }
            else{
                QString str;
                QList<val> ql = board[i][j].candidates.toList();
                std::sort(ql.begin(),ql.end());
                for(const val& v : ql){
                    str += QString::number(v) + " ";
                }
                ui->sudoku_ui->item(i,j)->setText(str);
                ui->sudoku_ui->item(i,j)->setFont(CANDIDATE_FONT);
                ui->sudoku_ui->item(i,j)->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
                //ui->sudoku_ui->item(i,j)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable);
            }
        }
    }
}



// function to highlight cell neighbors
void Sudoku::highlightNeighbors(int row, int col, QColor neighborBcolor, QColor neighborFcolor, QColor selItemBcolor, QColor selItemFcolor)
{
    QVector<CELL_INFO> all_neighbors = sudoku_board.getNeighbors(row,col);
    for(const CELL_INFO& n : all_neighbors){
        // highlighting of neighbors --- START
        ui->sudoku_ui->item(n.coord.x(),n.coord.y())->setBackground(QBrush(neighborBcolor));
        ui->sudoku_ui->item(n.coord.x(),n.coord.y())->setForeground(QBrush(neighborFcolor));
        if(n.revealed){
            ui->sudoku_ui->item(n.coord.x(),n.coord.y())->setFont(NORMAL_HIGHLIGHT_FONT);
        }
        else{
            ui->sudoku_ui->item(n.coord.x(),n.coord.y())->setFont(CANDIDATE_HIGHLIGHT_FONT);
        }

        // highlighting of neighbors --- END
    }

    // highlighting of selected cell --- START
    ui->sudoku_ui->item(row,col)->setSelected(false);
    ui->sudoku_ui->item(row,col)->setBackground(QBrush(selItemBcolor));
    ui->sudoku_ui->item(row,col)->setForeground(QBrush(selItemFcolor));
    // highlightingof selected cell --- END
}

// function to highlight cell
void Sudoku::highlightCell(int row, int col, QColor bcolor, QColor fcolor)
{
    ui->sudoku_ui->item(row,col)->setBackground(QBrush(bcolor));
    ui->sudoku_ui->item(row,col)->setForeground(QBrush(fcolor));
}

void Sudoku::test(int num_tests)
{
    QString msg = "\n\n\n************** TESTING STARTED **************\nTime: " + QDateTime::currentDateTime().toString("dd.MM.yyyy,hh:mm:ss") + "\n";
    emit debugPrint(msg);
    SudokuBoard sudoku;
    for(int i=0;i< num_tests; i++){
        msg = "........ TEST " + QString::number(i+1) + "/" +  QString::number(num_tests) + " ........";
        qDebug() << msg;
        emit debugPrint(msg);
        qApp->processEvents();
        sudoku.generate();
        sudoku.solve();
    }
    msg = "\n************** TESTING FINISHED **************\nTime: " + QDateTime::currentDateTime().toString("dd.MM.yyyy,hh:mm:ss") + "\n\n\n";
    emit debugPrint(msg);
}

// SLOTS

// function to respond on cell click
void Sudoku::onItemClicked(QTableWidgetItem* item)
{
    item;
    // ...
}

// function to respond on cell enter
void Sudoku::onItemEntered(QTableWidgetItem* item)
{
    resetBoardColorUI();
    highlightNeighbors(item->row(),
                       item->column(),
                       NEIGHBOR_BACKGROUND_COLOR,
                       NEIGHBOR_FOREGROUND_COLOR,
                       CELL_BACKGROUND_COLOR,
                       CELL_FOREGROUND_COLOR);

    // show coordinates of item
    ui->currentItemCoord->setText("("+QString::number(item->row()+1)+","+QString::number(item->column()+1)+")");

    // show candidates of item
    QString cand_str = "n/a";
    QList<val> ql = sudoku_board.getCandidates(item->row(),item->column()).toList();
    if(!ql.isEmpty()){
        cand_str.clear();
    }
    std::sort(ql.begin(),ql.end());
    for (const val& v : ql){
        cand_str += QString::number(v)+" ";
    }
    ui->candidates->setText(cand_str);
}

// function to respond when 'Regenerate board' button is clicked
void Sudoku::on_regenarateBoardButton_clicked()
{
    generateBoardUI();
    ui->currentItemCoord->setText("(?,?)");
    ui->candidates->setText("");
    qApp->processEvents();
}

// function to respond when 'Unhighlight' button is clicked
void Sudoku::on_unhighlightButton_clicked()
{
    resetBoardColorUI();
    qApp->processEvents();
}

// function to respond when 'Solve' button is clicked
void Sudoku::on_solveButton_clicked()
{
    sudoku_board.solve();
    redrawBoardUI();
    qApp->processEvents();
}

// CUSTOM SLOTS

// function to respond when board redraw signal is emitted
void Sudoku::redrawBoardSlot()
{
    resetBoardColorUI();
    redrawBoardUI();
    qApp->processEvents();
}

// function to respond when cell highlight signal is emitted
void Sudoku::highlightCellSlot(int row, int col, QColor bc, QColor fc)
{
    highlightCell(row,col,bc,fc);
    qApp->processEvents();
}

void Sudoku::debugPrintSlot(QString message, QColor background, QColor foreground)
{
    QColor bsave = ui->debugTextEdit->textBackgroundColor();
    QColor fsave = ui->debugTextEdit->textColor();
    ui->debugTextEdit->setTextBackgroundColor(background);
    ui->debugTextEdit->setTextColor(foreground);
    ui->debugTextEdit->append(message);
    ui->debugTextEdit->setTextBackgroundColor(bsave);
    ui->debugTextEdit->setTextColor(fsave);
    qApp->processEvents();
}

void Sudoku::on_testButton_clicked()
{
    test(ui->numtests->value());
}

void Sudoku::on_advanced_toggled(bool checked)
{
    if(checked){
        ui->debugGroupBox->show();
    }
    else{
        ui->debugGroupBox->hide();
    }
}

void Sudoku::on_pushButton_clicked()
{
    ui->debugTextEdit->clear();
}
