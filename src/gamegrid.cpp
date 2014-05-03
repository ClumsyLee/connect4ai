#include <cstring>
#include "evaluator.h"
#include "gamegrid.h"

// static members
int GameGrid::M_ = -1;
int GameGrid::N_ = -1;
int GameGrid::noX_ = -1;
int GameGrid::noY_ = -1;


GameGrid::GameGrid(int M, int N, const int *top, int **board,
                   int lastX, int lastY, int noX, int noY)
        : top_(new int[N]),
          board_(new int *[M]),
          value_(0.0)
{
    // static members
    M_ = M;
    N_ = N;
    noX_ = noX;
    noY_ = noY;

    // non-static members
    std::memcpy(top_, top, N_ * sizeof(top[0]));

    for (int row = 0; row < M_; row++)
    {
        board_[row] = new int[N_];
        std::memcpy(board_[row], board[row], N_ * sizeof(board_[0][0]));
    }
    // We just drop lastX and lastY here, but keep them in the parameters
    // so that it can be used later if needed.
}


GameGrid::GameGrid(const GameGrid &parent)
        : top_(new int[N_]),
          board_(new int *[M_]),
          value_(parent.value_)
{
    // copy the top
    std::memcpy(top_, parent.top_, N_ * sizeof(top_[0]));

    // copy the board
    for (int row = 0; row < M_; row++)
    {
        board_[row] = new int[N_];
        std::memcpy(board_[row], parent.board_[row],
                    N_ * sizeof(board_[0][0]));
    }
}


GameGrid::~GameGrid()
{
    // delete top
    delete [] top_;

    // delete board
    for (int row = 0; row < M_; row++)
        delete [] board_[row];
    delete [] board_;
}


void GameGrid::Place(int this_x, int this_y, int piece_type)
{
    double value_before = EvaluateRalatedValue(this_x, this_y);

    // adjust the top of the col
    // jump over if the new top is (noX, noY)
    if (this_y == noY_ && this_x == noX_ + 1 && noX_ != 0)
        top_[this_y] -= 2;
    else
        top_[this_y]--;

    // add the new piece
    board_[this_x][this_y] = piece_type;

    double value_after = EvaluateRalatedValue(this_x, this_y);
    value_ += (value_after - value_before);
}


double GameGrid::EvaluateRalatedValue(int this_x, int this_y)
{
    Evaluator evaluator(this);

    // same row
    for (int y = 0; y < N_; y++)
        evaluator.Next(this_x, y);
    evaluator.End();

    // same col
    for (int x = 0; x < M_; x++)
        evaluator.Next(x, this_y);
    evaluator.End();

    // same diagonal
    if (this_x >= this_y)
    {
        int init_x = this_x - this_y;
        if (init_x <= M_ - 4)
        {
            for (int x = init_x, y = 0; x < M_ && y < N_; x++, y++)
                evaluator.Next(x, y);
            evaluator.End();
        }
    }
    else
    {
        int init_y = this_y - this_x;
        if (init_y <= N_ - 4)
        {
            for (int x = 0, y = init_y; x < M_ && y < N_; x++, y++)
                evaluator.Next(x, y);
            evaluator.End();
        }
    }

    // same clinodiagoal
    int init_y = this_x + this_y;
    if (init_y >= N_)
    {
        int init_x = init_y - N_ + 1;
        for (int x = init_x, y = N_ - 1; x < M_ && y >= 0; x++, y--)
            evaluator.Next(x, y);
        evaluator.End();
    }
    else
    {
        for (int x = 0, y = init_y; x < M_ && y >= 0; x++, y--)
            evaluator.Next(x, y);
        evaluator.End();
    }

    return evaluator.value();
}
