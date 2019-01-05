#include "sudokuboard.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <QDateTime>

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

    // set size of the auxiliary matrix with candidate information
    candidate_info.resize(s);
    for(auto& row : candidate_info){
        row.resize(s);
        for(auto& candidate_vector : row){
            candidate_vector.resize(CANDIDATE_COUNT);
            for(bool& c : candidate_vector){
                c = false;
            }
        }
    }

    // set size of the global auxiliary matrix with candidate information
    global_candidate_info.resize(s);
    for(auto& row : global_candidate_info){
        row.resize(s);
        for(auto& candidate_vector : row){
            candidate_vector.resize(CANDIDATE_COUNT);
            for(bool& c : candidate_vector){
                c = false;
            }
        }
    }

    // reset the board
    reset();
}

// function to generate solved Sudoku board (fill it with valid numbers) and reveal clues
void SudokuBoard::generate()
{
    if(board.isEmpty()){
        logMessage("Board has no dimensions.");
        return;
    }
    reset();
    generateCells();
    showClues();
    originalBoard = board;
    logMessage("RANDOM SUDOKU GENERATED "+QDateTime::currentDateTime().toString(QString("dd.MM.yyyy,hh:mm:ss")),qRgb(153, 235, 255),Qt::black);
}

// recursive function to generate solved Sudoku board cell by cell
bool SudokuBoard::generateCells(int row, int col)
{   
    QSet<val> valid_options = DEFAULT_CANDIDATES-getUniqueNeighborValues(row,col);
    QVector<val> v_options = valid_options.toList().toVector();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v_options.begin(),v_options.end(), g);
    for(int option : v_options) {
        // assign the current cell a valid option
        board[row][col].value = option;

        // current cell has no candidates
        board[row][col].candidates = NO_CANDIDATES;

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
    board[row][col].candidates = DEFAULT_CANDIDATES;

    return false;
}

// function to reveal clues on Sudoku board, requires already generated Sudoku board
// what really happens here is that some cells are set as revealed
void SudokuBoard::showClues()
{
    QVector<int> rand_indices(SUDOKU_BOARD_SIDE*SUDOKU_BOARD_SIDE);
    std::iota(rand_indices.begin(),rand_indices.end(),0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(rand_indices.begin(), rand_indices.end(), g);
    int c;
    for (int i = 0; i<CLUES_COUNT;i++) {
        c = rand_indices[i];
        board[c/SUDOKU_BOARD_SIDE][c%SUDOKU_BOARD_SIDE].revealed = true;
        board[c/SUDOKU_BOARD_SIDE][c%SUDOKU_BOARD_SIDE].candidates = NO_CANDIDATES;
    }
    updateCandidates();
    // the rest of the board is set to 0
    for (int i = CLUES_COUNT; i<SUDOKU_BOARD_SIDE*SUDOKU_BOARD_SIDE;i++) {
        c = rand_indices[i];
        board[c/SUDOKU_BOARD_SIDE][c%SUDOKU_BOARD_SIDE].value = 0;
    }
}

// function to reset the contents of Sudoku board to 0, set default candidates and set cell revelation status to false
// function also resets candidate board to unguessed status
void SudokuBoard::reset()
{
    // reset Sudoku board
    for(auto& row : board){
        for(CELL_INFO& cell : row){
            cell.value = 0;
            cell.candidates = DEFAULT_CANDIDATES;
            cell.revealed = false;
        }
    }

    // reset candidate info - guess status
    for(auto& row : candidate_info){
        for(auto& candidate_vector : row){
            for(bool& c : candidate_vector){
                c = false;
            }
        }
    }

    // reset global candidate info
    for(auto& row : global_candidate_info){
        for(auto& candidate_vector : row){
            for(bool& c : candidate_vector){
                c = false;
            }
        }
    }
    history.clear();
}

// function to display current Sudoku board
// * detailed false - only revealed values
// * detailed true - complete information for each cell
void SudokuBoard::print(bool detailed)
{
    QString buffer;
    QTextStream message(&buffer);
    int cnt=0;
    int cnt2=0;
    QString hline = " -------------------- ";
    QString str_row;
    for(const auto& row: board){
        str_row.clear();
        if(!detailed){
            if(cnt%SUDOKU_BOX_SIZE == 0){
                message << hline << "\n";
            }
            cnt++;
            cnt2=0;
        }

        for(const CELL_INFO& cell: row){
            if(!detailed){
                if(cnt2%SUDOKU_BOX_SIZE == 0){
                    str_row += "|";
                }
                cnt2++;
            }

            if(cell.revealed){
                str_row += QString::number(cell.value);
            }
            else{
                str_row+=".";
            }

            if(detailed){
                if(cell.revealed){
                    str_row += " (" + QString::number(cell.coord.x()) + "," + QString::number(cell.coord.y()) + ")";
                    str_row += " [" + QString::number(cell.revealed) + "]";
                }

                if(!cell.revealed){
                    str_row += " {";
                    for(const val& v : cell.candidates){
                        str_row += QString::number(v) + " ";
                    }
                    str_row += "} ";
                }
                str_row += "|";

            }
            str_row += " ";

        }
        if(!detailed){
            str_row +="|";
        }
        message << str_row << "\n";
    }
    if(!detailed){
        message << hline << "\n";
    }
    message << "\n";
    logMessage(message.readAll());
}

// function to display provided Sudoku board, only revealed cells
void SudokuBoard::printBoard(const QVector<QVector<CELL_INFO> > & board)
{
    QString buffer;
    QTextStream message(&buffer);
    for(int row = 0; row<board.count(); row++){
        if((row)%SUDOKU_BOX_SIZE == 0){
            message << "-------------------------\n";
        }
        for(int col = 0; col<board[row].count(); col++){
            if((col)%SUDOKU_BOX_SIZE==0){
                message << "| ";
            }
            if( board[row][col].revealed){
                message << board[row][col].value << " ";
            }
            else{
                message << ". ";
            }
        }
        message << "| ";
        message << "\n";

    }
    message << "-------------------------\n";
    logMessage(message.readAll());
}

// function to display which candidates of unrevealed cells have been guessed ('x' symbol)
void SudokuBoard::printGuessedCandidates(const QVector<QVector<CELL_INFO> > & board, const QVector<QVector<QVector<bool>>>& candidate_info)
{
    QString buffer;
    QTextStream message(&buffer);
    for(int row = 0; row<candidate_info.count(); row++){
        if((row)%SUDOKU_BOX_SIZE == 0){
            message << "------------------------------ ------------------------------ ------------------------------\n";
        }
        for(int col = 0; col<candidate_info[row].count(); col++){
            if((col)%SUDOKU_BOX_SIZE==0){
                message << "| ";
            }
            if(!board[row][col].revealed){
                for(int k =0; k<candidate_info[row][col].size(); k++){
                    message << (candidate_info[row][col][k]?"x":QString::number(k+1));
                }
            }
            else{
                for(int k =0; k<candidate_info[row][col].size(); k++){
                    message << " ";
                }
            }
            message << " ";
        }
        message << "| ";
        message << "\n";

    }
    message << "------------------------------ ------------------------------ ------------------------------\n";
    logMessage(message.readAll());
}

// function to check if Sudoku board is solved
bool SudokuBoard::isSolved()
{
    QString whatHappened;
    QVector<CELL_INFO> unsolved;

    // check if all cells are revealed
    for(int i=0; i<board.count();i++){
        for(int j = 0; j<board[i].count();j++){
            if(!board[i][j].revealed){
                unsolved.push_back(board[i][j]);
            }
        }
    }
    if(!unsolved.isEmpty()){
        whatHappened = "board is not solved because there are some unrevealed cells in it";
        return false;
    }

    // check if all cells contain numbers 1-9
    for(int i=0; i<board.count();i++){
        for(int j=0;j<board[i].count();j++){
            if(!(board[i][j].value >=1 && board[i][j].value <=9)){
                whatHappened = "invalid value: (" + QString::number(i) + "," + QString::number(j);
                return false;
            }
        }
    }

    // check row conflicts
    QSet<val> row;
    for(int i=0; i<board.count();i++){
        row.clear();
        for(int j=0;j<board[i].count();j++){
            row.insert(board[i][j].value);
        }
        if(row != DEFAULT_CANDIDATES){
            whatHappened = "row uniqueness: " + QString::number(i);
            return false;
        }
    }

    // check column conflicts
    QSet<val> col;
    for(int i=0; i<board[0].count();i++){
        col.clear();
        for(int j=0;j<board.count();j++){
            col.insert(board[j][i].value);
        }
        if(col != DEFAULT_CANDIDATES){
            whatHappened = "column uniqueness: " + QString::number(i);
            return false;
        }
    }

    // check box conflicts
    QSet<val> box;
    for(int i=0; i<SUDOKU_BOARD_SIDE;i+=SUDOKU_BOX_SIZE){
        for(int j=0; j<SUDOKU_BOARD_SIDE;j+=SUDOKU_BOX_SIZE){
            box.clear();
            int box_r = floor(i/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // row index of the box top left position
            int box_c = floor(j/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // column index of the box top left position
            for(int box_i = box_r;box_i<box_r+SUDOKU_BOX_SIZE;box_i++){
                for(int box_j = box_c;box_j<box_c+SUDOKU_BOX_SIZE;box_j++){
                    box.insert(board[box_i][box_j].value);
                }
            }
            if(box != DEFAULT_CANDIDATES){
                whatHappened = "box uniqueness: (" + QString::number(i) + "," + QString::number(j);
                return false;
            }
        }
    }
    return true;
}

// function to check whether the intermediate solution has no row, column and box conflicts
// and if there are no unrevealed cells with no candidates
bool SudokuBoard::isGood(QString& whatHappened)
{
    whatHappened = "all OK";

    // check if there are unrevealed cells with no candidates
    QVector<CELL_INFO> vv = getUnrevealedCellsWithNCandidates(0);
    QString buffer;
    QTextStream message(&buffer);
    if(!vv.isEmpty()){
        message << "ISGOOD = FAIL: ";
        message << "Cells with no candidate found:\n";
        for(auto b : vv){
            message << "* " << "(" << b.coord.x()+1 << "," << b.coord.y()+1 << ")";
            //emit highlightCellSignal(b.coord.x(),b.coord.y(),Qt::magenta,Qt::white);
        }
        whatHappened = "no candidate cell found";
        //logMessage(message.readAll());
        return false;
    }

    // check for row conflicts
    QSet<val> row;
    for(int i=0; i<board.count();i++){
        row.clear();
        for(int j=0;j<board[i].count();j++){
            if(board[i][j].revealed){
                if(row.contains(board[i][j].value)){
                    whatHappened = "row uniqueness: " + QString::number(i);
                    return false;
                }
                row.insert(board[i][j].value);
            }
        }
    }

    // check for column conflicts
    QSet<val> col;
    for(int i=0; i<board[0].count();i++){
        col.clear();
        for(int j=0;j<board.count();j++){
            if(board[j][i].revealed){
                if(col.contains(board[j][i].value)){
                    whatHappened = "column uniqueness: " + QString::number(i);
                    return false;
                }
                col.insert(board[j][i].value);
            }
        }
    }

    // check for box conflicts
    QSet<val> box;
    for(int i=0; i<SUDOKU_BOARD_SIDE;i+=SUDOKU_BOX_SIZE){
        for(int j=0; j<SUDOKU_BOARD_SIDE;j+=SUDOKU_BOX_SIZE){
            box.clear();
            int box_r = floor(i/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // row index of the box top left position
            int box_c = floor(j/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // column index of the box top left position
            for(int box_i = box_r;box_i<box_r+SUDOKU_BOX_SIZE;box_i++){
                for(int box_j = box_c;box_j<box_c+SUDOKU_BOX_SIZE;box_j++){
                    if(board[box_i][box_j].revealed){
                        if(box.contains(board[box_i][box_j].value)){
                            whatHappened = "box uniqueness: (" + QString::number(box_i) + "," + QString::number(box_j) + ")";
                            return false;
                        }
                        box.insert(board[box_i][box_j].value);
                    }
                }
            }
        }
    }
    return true;
}

// function to get vector of all neighbors of specific cell at coordinates (row,col)
QVector<CELL_INFO> SudokuBoard::getNeighbors(int row, int col){
    QVector<CELL_INFO> s;

    // get all row neighbors
    for(int i=0;i<board.at(row).count();i++){
        s.push_back(board.at(row).at(i));
    }
    // get all column neighbors
    for(int i=0;i<board.count();i++){
        s.push_back(board.at(i).at(col));
    }
    // get all box neighbors
    int box_r = floor(row/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // row index of the box top left position
    int box_c = floor(col/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // column index of the box top left position
    for(int i=box_r; i<box_r+SUDOKU_BOX_SIZE;i++){
        for(int j=box_c;j<box_c+SUDOKU_BOX_SIZE;j++){
            s.push_back(board.at(i).at(j));
        }
    }
    return s;
}

// function to obtain all unique neighbor values
QSet<val> SudokuBoard::getUniqueNeighborValues(int row, int col)
{
    QVector<CELL_INFO> neighbors = getNeighbors(row,col);
    QSet<val> cellNeighborsSet;
    for(const CELL_INFO& e : neighbors){
        cellNeighborsSet.insert(e.value);
    }
    return cellNeighborsSet;
}

// function to return reference to candidates of the cell (row,col)
const QSet<val> &SudokuBoard::getCandidates(int row, int col)
{
    return board[row][col].candidates;
}

// function to return reference to Sudoku board
const QVector<QVector<CELL_INFO> > &SudokuBoard::getBoard() const
{
    return board;
}

// function to obtain all unrevealed cells with specific number of candidates
QVector<CELL_INFO> SudokuBoard::getUnrevealedCellsWithNCandidates(int n)
{
    QVector<CELL_INFO> v;
    for(int i =0; i<board.count();i++){
        for(int j =0; j< board[i].count();j++){
            if(!board[i][j].revealed && board[i][j].candidates.size()==n){
                v.push_back(board[i][j]);
            }
        }
    }
    return v;
}

// function to return the number of unrevealed cells
int SudokuBoard::getNumberOfUnrevealedCells()
{
    int n=0;
    for(int i =0; i<board.count();i++){
        for(int j =0; j< board[i].count();j++){
            if(!board[i][j].revealed ){
                n++;
            }
        }
    }
    return n;
}

// function that sends text message to UI
void SudokuBoard::logMessage(QString message, QColor background, QColor foreground)
{
    emit debugPrint(message,background,foreground);
}

// function to obtain set of cell (row,col) candidates
QSet<val> SudokuBoard::computeCandidates(int row, int col)
{
    QVector<CELL_INFO> cellNeighbors = getNeighbors(row,col);
    QSet<val> cellCandidates;
    for(const CELL_INFO& cn : cellNeighbors){
        if(cn.revealed){
            cellCandidates.insert(cn.value);
        }
    }
    cellCandidates = DEFAULT_CANDIDATES-cellCandidates;
    return cellCandidates;
}

// function to update candidates for unrevealed cells
void SudokuBoard::updateCandidates()
{
    for(int i=0;i<board.count();i++){
        for(int j =0; j<board[i].count();j++){
            if(!board[i][j].revealed){
                board[i][j].candidates = computeCandidates(i,j);
            }
        }
    }
}

// function to solve the board using deduction techniques
// deduction may finish when there is nothing to solve or a failure occured
// during solving
void SudokuBoard::deduction()
{
    bool debugInfo = false;
    bool solve_continue = true;
    bool solve1, solve2, solve3, solve4;
    // deduction loop
    try{
        while(solve_continue){
            // Filling in 1-candidate cells
            solve1 = solveCellsWithOneCandidate(debugInfo);

            // Filling in numbers that can go only in one place in a row
            solve2 = solveCellsInRow(debugInfo);

            // Filling in numbers that can go only in one place in a column
            solve3 = solveCellsInColumn(debugInfo);

            // Filling in numbers that can go only in one place in a box
            solve4 = solveCellsInBox(debugInfo);

            solve_continue = solve1 || solve2 || solve3 || solve4;
        }
    }
    catch (QString e){
        e = " ---> FAILURE DURING DEDUCTION: " + e;
        throw e;
    }
}

// function to solve one cell by guessing
void SudokuBoard::guessing()
{
    // nothing is guessed at the beginning
    GUESS guess = INVALID_GUESS;

    // repeat until I guess the value in the current state
    while(!guess.value){
        try{
            // compute coordinate and value of the next valid guess in the current state
            guess = nextGuess();

            // continue if there is something to guess in the current state

            // flag the guessed candidate in the current candidate_info
            candidate_info[guess.row][guess.col][guess.value-1] = true;
            // also in global candidate info
            global_candidate_info[guess.row][guess.col][guess.value-1] = true;

            // push the current state to stack
            //  * board before solving with guessed value
            //  * candidate_info after flagging the guessed value in the current candidate_info
            history.push({board,candidate_info});

        }
        // if there was no valid guess left in the current state
        catch(QString e){
            // set the guess as invalid
            guess = INVALID_GUESS;

            // if history not empty
            if(!history.isEmpty()){
                // go to previous state, pop last state from stack
                board = history.top().board;
                candidate_info = history.top().candidate_info;
                history.pop();
            }
            else{
                // history is empty ... will I ever get here?
                logMessage("history is empty ... will I ever get here?");
                break;
            }
        }
    }

    // solving
    if(guess.value !=0){
        // can there be an exception when guessing is smart?
        solveCell(guess.row, guess.col, guess.value, "solving cells by guessing");
    }
}

// function answers the question if there are some
// unrevealed cells in the current board with unguessed candidates
bool SudokuBoard::isThereSomethingToGuess()
{
    for(int row =0; row<board.count();row++){
        for(int col =0; col<board[row].count();col++){
            if(!board[row][col].revealed){
                for(val candidate : board[row][col].candidates){
                    if(!candidate_info[row][col][candidate-1]){
                        return true;
                    }
                }

            }
        }
    }
    return false;
}

// function to compute the next guess in the current board
// (the next unguessed candidate of unrevealed cell)
// returns:
//  * value of guess
//  * cell row
//  * cell column
//
// if no guesses are available, it throws an exception
GUESS SudokuBoard::nextGuess()
{
    QVector<CELL_INFO> vc;
    for(int row =0; row<board.count();row++){
        for(int col =0; col<board[row].count();col++){
            if(!board[row][col].revealed){
                vc.push_back(board[row][col]);
            }
        }
    }
    std::sort(vc.begin(),vc.end(),[](CELL_INFO& c1, CELL_INFO& c2){return c1.candidates.size() < c2.candidates.size();});

    for(CELL_INFO& ci : vc){
        for(val candidate : ci.candidates){
            if(!candidate_info[ci.coord.x()][ci.coord.y()][candidate-1]){
                return {candidate,ci.coord.x(),ci.coord.y()};
            }
        }
    }
    throw QString("NO GUESS LEFT IN THE CURRENT STATE");
}


// ******         *******
// ****** SOLVING *******
// ******         *******

// function to solve Sudoku board
void SudokuBoard::solve()
{
    logMessage("Solving, please wait, backtracking may take some while... ");
    // while board is not solved
    while(!isSolved()){
        try{
            // DEDUCTION
            deduction();
        }
        // if failure during deduction
        catch(QString e){
            //logMessage(e);
            // go to previous state, pop last state from stack
            board = history.top().board;
            candidate_info = history.top().candidate_info;
            history.pop();
        }

        // GUESSING
        // can this raise an exception when guessing will always take the valid candidate?
        if(!isSolved()){
            guessing();
        }

    }

    // I will get here only when Sudoku is solved

    logMessage("SOLVED "+QDateTime::currentDateTime().toString(QString("dd.MM.yyyy,hh:mm:ss"))+"\n",qRgb(0, 143, 179), Qt::white);
}

// function to solve all cells with 1 candidate
// return true if at least one cell was solved
bool SudokuBoard::solveCellsWithOneCandidate(bool debugInfo)
{
    bool solved_at_least_one = false;
    QVector<CELL_INFO> vci = getUnrevealedCellsWithNCandidates(1);
    while(!vci.isEmpty()){
        if(solveCell(vci.first().coord.x(),vci.first().coord.y(),*(vci.first().candidates.begin()), "solving cells with one candidate")){
            if(debugInfo){
                logMessage( "Solved by 1-candidate cell elimination: [" +
                            QString::number(vci.first().coord.x()) +
                            "," +
                            QString::number(vci.first().coord.y()) +
                            "]" +
                            QString::number(board[vci.first().coord.x()][vci.first().coord.y()].value)
                        );
            }
            solved_at_least_one = true;
            vci.clear();
            vci = getUnrevealedCellsWithNCandidates(1);
        }
    }
    return solved_at_least_one;
}

// function to solve cells in row where missing values can go only in one place
// return true if at least one cell was solved
bool SudokuBoard::solveCellsInRow(bool debugInfo){
    bool solved_at_least_one = false;
    QSet<val> missingInRow;
    for(int i=0; i<board.count();i++){
        missingInRow.clear();
        // find missing numbers in a row
        for(int j=0;j<board[i].count();j++){
            if(board[i][j].revealed){
                missingInRow.insert(board[i][j].value);
            }
        }
        missingInRow = DEFAULT_CANDIDATES - missingInRow;

        // looking for the one place in a row for each of the missing numbers
        int cnt =0;
        int one_index = -1;
        for(const val& v : missingInRow){
            cnt = 0;
            one_index = -1;
            for(int j=0;j<board[i].count();j++){
                if(!board[i][j].revealed && board[i][j].candidates.contains(v)){
                    one_index = j;
                    cnt++;
                    if(cnt>1){
                        one_index = -1;
                        break;
                    }
                }
            }
            if(one_index != -1){
                if(solveCell(i,one_index,v,"solving cells in a row")){
                    if(debugInfo){
                        logMessage( "Solved by only place in row: [" +
                                    QString::number(i) +
                                    "," +
                                    QString::number(one_index) +
                                    "]" +
                                    QString::number(board[i][one_index].value)
                                    );
                    }
                    solved_at_least_one = true;
                }
            }
        }
    }
    return solved_at_least_one;
}

// function to solve cells in column where missing values can go only in one place
// return true if at least one cell was solved
bool SudokuBoard::solveCellsInColumn(bool debugInfo){
    bool solved_at_least_one = false;
    QSet<val> missingInCol;
    for(int i=0; i<board[0].count();i++){
        missingInCol.clear();
        // find missing numbers in a column
        for(int j=0;j<board.count();j++){
            if(board[j][i].revealed){
                missingInCol.insert(board[j][i].value);
            }
        }
        missingInCol = DEFAULT_CANDIDATES - missingInCol;

        // looking for the one place in a column for each of the missing numbers
        int cnt =0;
        int one_index = -1;
        for(const val& v : missingInCol){
            cnt = 0;
            one_index = -1;
            for(int j=0;j<board.count();j++){
                if(!board[j][i].revealed && board[j][i].candidates.contains(v)){
                    one_index = j;
                    cnt++;
                    if(cnt>1){
                        one_index = -1;
                        break;
                    }
                }
            }
            if(one_index != -1){
                if(solveCell(one_index,i,v, "solving cells in a column")){
                    if(debugInfo){
                        logMessage( "Solved by only place in column: [" +
                                    QString::number(one_index) +
                                    "," +
                                    QString::number(i) +
                                    "]" +
                                    QString::number(board[one_index][i].value)
                                    );
                    }
                    solved_at_least_one = true;
                }
            }
        }
    }
    return solved_at_least_one;
}

// function to solve cells in box where missing values can go only in one place
// return true if at least one cell was solved
bool SudokuBoard::solveCellsInBox(bool debugInfo){
    bool solved_at_least_one = false;
    QSet<val> missingInBox;
    for(int i=0; i<SUDOKU_BOARD_SIDE;i+=SUDOKU_BOX_SIZE){
        for(int j=0; j<SUDOKU_BOARD_SIDE;j+=SUDOKU_BOX_SIZE){
            missingInBox.clear();
            // find missing numbers in a box
            int box_r = floor(i/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // row index of the box top left position
            int box_c = floor(j/SUDOKU_BOX_SIZE)*SUDOKU_BOX_SIZE; // column index of the box top left position
            for(int box_i = box_r;box_i<box_r+SUDOKU_BOX_SIZE;box_i++){
                for(int box_j = box_c;box_j<box_c+SUDOKU_BOX_SIZE;box_j++){
                    if(board[box_i][box_j].revealed){
                        missingInBox.insert(board[box_i][box_j].value);
                    }
                }
            }
            missingInBox = DEFAULT_CANDIDATES - missingInBox;
            // looking for the one place in a box for each of the missing numbers
            int cnt =0;
            int one_index_i = -1;
            int one_index_j = -1;
            bool jump_out = false;
            for(const val& v : missingInBox){
                cnt = 0;
                one_index_i = -1;
                one_index_j = -1;
                for(int box_i = box_r;box_i<box_r+SUDOKU_BOX_SIZE;box_i++){
                    jump_out = false;
                    for(int box_j = box_c;box_j<box_c+SUDOKU_BOX_SIZE;box_j++){
                        if(!board[box_i][box_j].revealed && board[box_i][box_j].candidates.contains(v)){
                            one_index_i = box_i;
                            one_index_j = box_j;
                            cnt++;
                            if(cnt>1){
                                one_index_i = -1;
                                one_index_j = -1;
                                jump_out = true;
                                break;
                            }
                        }
                    }
                    if(jump_out){
                        break;
                    }
                }
                if(one_index_i != -1 && one_index_j != -1){
                    if(solveCell(one_index_i,one_index_j,v,"solving cells in a box")){
                        if(debugInfo){
                            logMessage( "Solved by only place in box: [" +
                                        QString::number(one_index_i) +
                                        "," +
                                        QString::number(one_index_j) +
                                        "]" +
                                        QString::number(board[one_index_i][one_index_j].value)
                                        );
                        }
                        solved_at_least_one = true;
                    }
                }
            }
        }
    }
    return solved_at_least_one;
}

// function to solve cell at coordinates ('row','col') with value 'value' and check if does not
// break the solution
bool SudokuBoard::solveCell(int row, int col, val value, QString description)
{
    QString whatHappened;

    // actual solving
    board[row][col].value = value;
    board[row][col].candidates = NO_CANDIDATES;
    board[row][col].revealed = true;
    updateCandidates();

    bool isgood = isGood(whatHappened);
    QString buffer;
    QTextStream message(&buffer);
    message << "SOLVE="
            << (isgood?"CORRECT: ":"ERROR: ")
            << "when solving cell"
            << "(" << row+1 << "," << col+1 << ") "
            << "with value " << value
            << ", DESCRIPTION: " << whatHappened + " | " + description;
    //logMessage(message.readAll());
    //emit redrawBoardSignal();

    if(!isgood){
        throw " solve failure: (" + whatHappened + ")";
    }
    return true;
}


