#include <cstring>
#include "gamegrid.h"

///////////////////////// helper functions /////////////////////////

namespace {

struct EvaluationStatus
{
    EvaluationStatus();
    void Reset();

    int piece_here;
    int friendly_continuous_space;
    int friendly_count;
    int enemy_continuous_space;
    int enemy_count;
    double evaluation_point;
};


EvaluationStatus::EvaluationStatus() : piece_here(0),
                                       friendly_continuous_space(0),
                                       friendly_count(0),
                                       enemy_continuous_space(0),
                                       enemy_count(0),
                                       evaluation_point(0.0)
{
}


inline void EvaluationStatus::Reset()
{
    friendly_continuous_space = friendly_count =
    enemy_continuous_space = enemy_count = 0;
}


void DealWithPiece(EvaluationStatus &status)
{
    if (status.piece_here == GameGrid::ENEMY)
    {
        // for friendly
        // add if there are enough spaces before this square
        if (status.friendly_continuous_space >= 4)
            status.evaluation_point += status.friendly_count;
        status.friendly_continuous_space = 0;
        status.friendly_count = 0;
        // for enemy
        status.enemy_continuous_space++;
        status.enemy_count++;
    }
    else if (status.piece_here == GameGrid::FRIENDLY)
    {
        // for friendly
        status.friendly_continuous_space++;
        status.friendly_count++;
        // for enemy
        // add if there are enough spaces before this square
        if (status.enemy_continuous_space >= 4)
            status.evaluation_point -= status.enemy_count;
        status.enemy_continuous_space = 0;
        status.enemy_count = 0;
    }
    else  // blank square
    {
        //TODO: what if cannot place
        status.friendly_continuous_space++;
        status.enemy_continuous_space++;
    }
}

}  // namespace


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
    // adjust the top of the col
    // jump over if the new top is (noX, noY)
    if (this_y == noY_ && this_x == noX_ + 1 && noX_ != 0)
        top_[this_y] -= 2;
    else
        top_[this_y]--;

    // add the new piece
    board_[this_x][this_y] = piece_type;

    Reevaluate(this_x, this_y);
}



