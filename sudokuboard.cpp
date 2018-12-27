#include "sudokuboard.h"
#include <algorithm>

SudokuBoard::SudokuBoard(QObject *parent) : QObject(parent)
{

}

// constructor that resizes Sudoku board to size x size
SudokuBoard::SudokuBoard(int size)
{
    board.resize(size);
    for(auto& elem : board){
        elem.resize(size);
    }
}

// function to generate Sudoku board (fill it with valid numbers)
void SudokuBoard::generate()
{
    generateCells();
}

// recursive function to generate Sudoku board cell by cell
bool SudokuBoard::generateCells(int row, int col)
{
    QSet<val> all_options{1,2,3,4,5,6,7,8,9};
    QSet<val> valid_options = all_options-getNeighbors(row,col);
    QVector<val> v_options = valid_options.toList().toVector();
    std::random_shuffle(v_options.begin(),v_options.end());
    for(int option : v_options) {
        // assign the current cell a valid option
        board[row][col] = option;

        // base case (no remaining cells)
        if (row == SUDOKU_BOARD_SIDE-1 && col == SUDOKU_BOARD_SIDE-1) {
            return true;
        }

        // increase column index
        int ncol = (col+1)%SUDOKU_BOARD_SIDE;
        int nrow = row;
        // if we are about to jump to the new line, than increase row index
        if(ncol==0){
            nrow++;
        }
        // recursively generate an option for another cell in row
        if (generateCells(nrow,ncol)){
            return true; // when we are done generating all cells from (row,col) until the end
        }
    }
    // we get here only when we cannot assign the cell any value, so we need to fix it by backtracking
    board[row][col] = 0;
    return false;
}

// function to reset the contents of Sudoku board to 0
void SudokuBoard::clear()
{
    for(auto& elem : board){
        elem.fill(0);
    }
}

// function to print Sudoku board in the text form
void SudokuBoard::print()
{
    for(const auto& e: board){
        qDebug() << e;
    }
}

// function to check whether Sudoku board is valid
// valid means all cells are assigned a number and numbers 1-9 occupy each row, column and box just once
bool SudokuBoard::isOk()
{
    for(int i=0; i<board.count();i++){
        for(int j=0;j<board[0].count();j++){
            QSet<val> neighbors = getNeighbors(i,j);
            if(neighbors != QSet<val>{1,2,3,4,5,6,7,8,9} ){
                return false;
            }
        }
    }
    return true;
}

// function to get neighbors of specific cell at coordinates (row,col)
QSet<val> SudokuBoard::getNeighbors(int row, int col){
    QSet<val> s;
    // get all column neighbors
    for(int i=0;i<board.count();i++){
        s.insert(board.at(i).at(col));
    }
    // get all row neighbors
    for(int i=0;i<board.at(row).count();i++){
        s.insert(board.at(row).at(i));
    }
    int box_r = floor(row/3.0)*3; // row index of the box top left position
    int box_c = floor(col/3.0)*3; // column index of the box top left position
    for(int i=box_r; i<box_r+3;i++){
        for(int j=box_c;j<box_c+3;j++){
            s.insert(board.at(i).at(j));
        }
    }
    return s;
}

const QVector<QVector<val> > &SudokuBoard::getBoard() const
{
    return board;
}

QVector<QPoint> SudokuBoard::getNeighbors(QPoint cell)
{
    QVector<QPoint> s;
    // get all column neighbors
    for(int i=0;i<board.count();i++){
        s.push_back(QPoint(i,cell.x()));
    }
    // get all row neighbors
    for(int i=0;i<board.at(cell.y()).count();i++){
        s.push_back(QPoint(cell.y(),i));
    }
    int box_r = floor(cell.y()/3.0)*3; // row index of the box top left position
    int box_c = floor(cell.x()/3.0)*3; // column index of the box top left position
    for(int i=box_r; i<box_r+3;i++){
        for(int j=box_c;j<box_c+3;j++){
            s.push_back(QPoint(i,j));
        }
    }
    return s;
}
