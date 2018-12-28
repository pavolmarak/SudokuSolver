#include "sudokuboard.h"
#include <algorithm>
#include <random>
#include <chrono>


SudokuBoard::SudokuBoard(QObject *parent) : QObject(parent)
{

}

// constructor that creates empty Sudoku board of size 's' x 's'
SudokuBoard::SudokuBoard(int s)
{
    // set size of the board and assign coordinates to each cell
    board.resize(s);
    int rowi = 0;
    int coli = 0;
    for(auto& row : board){
        row.resize(s);
        for(CELL_INFO& cell : row){
            cell.coord = QPoint(rowi,coli);
            coli++;
        }
        coli = 0;
        rowi++;
    }
    // clear the board
    clear();
}

// function to generate Sudoku board (fill it with valid numbers)
void SudokuBoard::generate()
{
    if(board.isEmpty()){
        qDebug() << "Board has no dimensions.";
        return;
    }
    clear();
    generateCells();
    showClues();
    //print(true);
}

// recursive function to generate Sudoku board cell by cell
bool SudokuBoard::generateCells(int row, int col)
{   
    QSet<val> all_options{1,2,3,4,5,6,7,8,9};
    QSet<val> valid_options = all_options-getUniqueNeighborValues(row,col);
    QVector<val> v_options = valid_options.toList().toVector();
    std::random_shuffle(v_options.begin(),v_options.end());
    for(int option : v_options) {
        // assign the current cell a valid option
        board[row][col].value = option;

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
    // or in other words trying different values for previous cells
    board[row][col].value = 0;
    return false;
}

// function to reveal clues on Sudoku board, requires already generated Sudoku board
// what really happens here is that some cells are set to revealed
void SudokuBoard::showClues()
{
    QVector<int> v(SUDOKU_BOARD_SIDE*SUDOKU_BOARD_SIDE);
    std::iota(v.begin(),v.end(),0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    int c;
    for (int i = 0; i<30;i++) {
        c = v[i];
        board[c/SUDOKU_BOARD_SIDE][c%SUDOKU_BOARD_SIDE].revealed = true;
    }
}

// function to reset the contents of Sudoku board to 0, set default candidates and set cell revelation status to false
void SudokuBoard::clear()
{
    for(auto& row : board){
        for(CELL_INFO& cell : row){
            cell.value = 0;
            cell.candidates = DEFAULT_CANDIDATES;
            cell.revealed = false;
        }
    }
}

// function to print Sudoku board in the text form
void SudokuBoard::print(bool detailed)
{
    QString str_row;;
    for(const auto& row: board){
        str_row.clear();
        for(const CELL_INFO& cell: row){
           str_row += QString::number(cell.value);
           if(detailed){
               str_row += " (" + QString::number(cell.coord.x()) + "," + QString::number(cell.coord.y()) + ")";
               str_row += " [" + QString::number(cell.revealed) + "]";
           }
           str_row += " ";
        }
        qDebug() << str_row;
    }
}

// function to check whether Sudoku board is valid
// valid means all cells are assigned a number and numbers 1-9 occupy each row, column and box just once
bool SudokuBoard::isOk()
{
    for(int i=0; i<board.count();i++){
        for(int j=0;j<board[0].count();j++){
            QSet<val> cellNeighbors = getUniqueNeighborValues(i,j);
            if(cellNeighbors != QSet<val>{1,2,3,4,5,6,7,8,9} ){
                return false;
            }
        }
    }
    return true;
}

void SudokuBoard::solve()
{
//    Go through all unrevealed cells in a Sudoku board and store/update candidates for each unrevealed cell by inspecting its row, column and box neighbors.
//    If there are no cells with just one candidate, move on to step 4.
//    Complete the first cell with only one candidate. Go back to step 1.
//    If there are no rows, columns or boxes with missing numbers that can be placed only in one cell, then stop.
//    Take a missing number in a row, column or box that can be placed only in one cell in that particular row, column or box and place it there. Go back to step 1.

}

// function to get vector of neighbors of specific cell at coordinates (row,col)
QVector<CELL_INFO> SudokuBoard::getNeighbors(int row, int col){
    QVector<CELL_INFO> s;

    // get all column neighbors
    for(int i=0;i<board.count();i++){
        s.push_back(board.at(i).at(col));
    }
    // get all row neighbors
    for(int i=0;i<board.at(row).count();i++){
        s.push_back(board.at(row).at(i));
    }
    int box_r = floor(row/3.0)*3; // row index of the box top left position
    int box_c = floor(col/3.0)*3; // column index of the box top left position
    for(int i=box_r; i<box_r+3;i++){
        for(int j=box_c;j<box_c+3;j++){
            s.push_back(board.at(i).at(j));
        }
    }
    return s;
}

QSet<val> SudokuBoard::getUniqueNeighborValues(int row, int col)
{
    QVector<CELL_INFO> neighbors = getNeighbors(row,col);
    QSet<val> cellNeighborsSet;
    for(const CELL_INFO& e : neighbors){
        cellNeighborsSet.insert(e.value);
    }
    return cellNeighborsSet;
}

const QVector<QVector<CELL_INFO> > &SudokuBoard::getBoard() const
{
    return board;
}
