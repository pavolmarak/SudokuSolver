#ifndef SUDOKUBOARD_H
#define SUDOKUBOARD_H

#include <QObject>
#include <QVector>
#include <QStack>
#include <QSet>
#include <QDebug>
#include <QColor>

typedef unsigned char val;

typedef struct{
    int value;
    int row;
    int col;
} GUESS;

#define INVALID_GUESS GUESS{0,-1,-1}

typedef struct {
    QPoint coord;
    val value;
    bool revealed;
    QSet<val> candidates;
} CELL_INFO;

typedef struct{
    QVector<QVector<CELL_INFO>> board;
    QVector<QVector<QVector<bool>>> candidate_info;
} HISTORY_SNAPSHOT;

#define SUDOKU_BOARD_SIDE 9
#define SUDOKU_BOX_SIZE 3
#define CLUES_COUNT 30
#define CANDIDATE_COUNT 9

#define DEFAULT_CANDIDATES QSet<val>{1,2,3,4,5,6,7,8,9}
#define NO_CANDIDATES QSet<val>{}

class SudokuBoard : public QObject
{
    Q_OBJECT
public:
    SudokuBoard(int size = SUDOKU_BOARD_SIDE);

    // public API
    void generate();
    void reset();
    void solve();
    void printGenerated();
    void printBoard(const QVector<QVector<CELL_INFO>>&);
    void printGuessedCandidates(const QVector<QVector<CELL_INFO> > & board, const QVector<QVector<QVector<bool> > > &candidate_info);
    void print(bool detailed =false);
    bool isSolved();
    bool isGood(QString& whatHappened);
    
    const QVector<QVector<CELL_INFO>>& getBoard() const;
    const QSet<val>& getCandidates(int,int);
    QVector<CELL_INFO> getNeighbors(int,int);
    QSet<val> getUniqueNeighborValues(int,int);
    QVector<CELL_INFO> getUnrevealedCellsWithNCandidates(int n);
    int getNumberOfUnrevealedCells();

signals:
    void redrawBoardSignal();
    void highlightCellSignal(int,int,QColor,QColor);
    void debugPrint(QString message, QColor background = Qt::white, QColor foreground = Qt::black);

public slots:
private:
    // data members
    QStack<HISTORY_SNAPSHOT> history;
    QVector<QVector<CELL_INFO>> board;
    QVector<QVector<CELL_INFO>> originalBoard;
    QVector<QVector<QVector<bool>>> candidate_info;
    QVector<QVector<QVector<bool>>> global_candidate_info;

    // mesasge logging
    void logMessage(QString message, QColor background = Qt::white, QColor foreground = Qt::black);
    // generating the board
    bool generateCells(int row=0, int col=0);
    void showClues();

    // working with candidates
    QSet<val> computeCandidates(int,int);
    void updateCandidates();

    // solving
    void deduction();
    void guessing();
    bool isThereSomethingToGuess();
    GUESS nextGuess();
    bool solveCellsWithOneCandidate(bool debugInfo = false);
    bool solveCellsInRow(bool debugInfo = false);
    bool solveCellsInColumn(bool debugInfo = false);
    bool solveCellsInBox(bool debugInfo = false);
    bool solveCell(int, int, val, QString description = "");
};

#endif // SUDOKUBOARD_H
