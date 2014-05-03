#include <cstring>
#include "gamegrid.h"

///////////////////////// helper functions /////////////////////////

namespace {

void DealWithPiece(int piece_here,
                   int &friendly_continuous_space, int &friendly_count,
                   int &enemy_continuous_space, int &enemy_count,
                   int &evaluation_point)
{
    if (piece_here == GameGrid::ENEMY)
    {
        // for friendly
        // add if there are enough spaces before this square
        if (friendly_continuous_space >= 4)
            evaluation_point += friendly_count;
        friendly_continuous_space = 0;
        friendly_count = 0;
        // for enemy
        enemy_continuous_space++;
        enemy_count++;
    }
    else if (piece_here == GameGrid::FRIENDLY)
    {
        // for friendly
        friendly_continuous_space++;
        friendly_count++;
        // for enemy
        // add if there are enough spaces before this square
        if (enemy_continuous_space >= 4)
            evaluation_point -= enemy_count;
        enemy_continuous_space = 0;
        enemy_count = 0;
    }
    else  // blank square
    {
        friendly_continuous_space++;
        enemy_continuous_space++;
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
          board_(new int *[M])
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
          board_(new int *[M_])
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
    delete top_;

    // delete board
    for (int row = 0; row < M_; row++)
        delete board_[row];
    delete board_;
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
}

int GameGrid::Evaluate() const
{
    int evaluation_point = 0;
    // for every row
    for (int row = 0; row < M_; row++)
    {
        int friendly_continuous_space = 0;
        int enemy_continuous_space = 0;
        int friendly_count = 0;
        int enemy_count = 0;
        for (int col = 0; col < N_; col++)
        {
            int piece_here = board_[row][col];
            DealWithPiece(piece_here,
                          friendly_continuous_space, friendly_count,
                          enemy_continuous_space, enemy_count,
                          evaluation_point);
        }
        // deal with last squares
        if (friendly_continuous_space >= 4)
            evaluation_point += friendly_count;
        if (enemy_continuous_space >= 4)
            evaluation_point -= enemy_count;
    }
    // for every column
    for (int col = 0; col < N_; col++)
    {
        int friendly_continuous_space = 0;
        int enemy_continuous_space = 0;
        int friendly_count = 0;
        int enemy_count = 0;
        for (int row = 0; row < M_; row++)
        {
            int piece_here = board_[row][col];
            DealWithPiece(piece_here,
                          friendly_continuous_space, friendly_count,
                          enemy_continuous_space, enemy_count,
                          evaluation_point);
        }
        // deal with last squares
        if (friendly_continuous_space >= 4)
            evaluation_point += friendly_count;
        if (enemy_continuous_space >= 4)
            evaluation_point -= enemy_count;
    }
    // for every diagonal
    // from left side, the last three diagonals can be ignored
    for (int row = M_ - 4; row >= 0; row--)
    {
        int friendly_continuous_space = 0;
        int enemy_continuous_space = 0;
        int friendly_count = 0;
        int enemy_count = 0;
        for (int x = row, y = 0; x < M_ && y < N_; x++, y++)
        {
            int piece_here = board_[x][y];
            DealWithPiece(piece_here,
                          friendly_continuous_space, friendly_count,
                          enemy_continuous_space, enemy_count,
                          evaluation_point);
        }
        // deal with last squares
        if (friendly_continuous_space >= 4)
            evaluation_point += friendly_count;
        if (enemy_continuous_space >= 4)
            evaluation_point -= enemy_count;
    }
    // from up side, the last three diagonals can be ignored
    for (int col = N_ - 4; col > 0; col--)
    {
        int friendly_continuous_space = 0;
        int enemy_continuous_space = 0;
        int friendly_count = 0;
        int enemy_count = 0;
        for (int x = 0, y = col; x < M_ && y < N_; x++, y++)
        {
            int piece_here = board_[x][y];
            DealWithPiece(piece_here,
                          friendly_continuous_space, friendly_count,
                          enemy_continuous_space, enemy_count,
                          evaluation_point);
        }
        // deal with last squares
        if (friendly_continuous_space >= 4)
            evaluation_point += friendly_count;
        if (enemy_continuous_space >= 4)
            evaluation_point -= enemy_count;
    }
    // for every clinodiagoal
    // from up side, the first three can be ignored
    for (int col = 3; col < N_; col++)
    {
        int friendly_continuous_space = 0;
        int enemy_continuous_space = 0;
        int friendly_count = 0;
        int enemy_count = 0;
        for (int x = 0, y = col; x < M_ && y >= 0 ; x++, y--)
        {
            int piece_here = board_[x][y];
            DealWithPiece(piece_here,
                          friendly_continuous_space, friendly_count,
                          enemy_continuous_space, enemy_count,
                          evaluation_point);
        }
        // deal with last squares
        if (friendly_continuous_space >= 4)
            evaluation_point += friendly_count;
        if (enemy_continuous_space >= 4)
            evaluation_point -= enemy_count;
    }
    // from right side, the last three can be ignored
    for (int row = M_ - 4; row > 0; row--)
    {
        int friendly_continuous_space = 0;
        int enemy_continuous_space = 0;
        int friendly_count = 0;
        int enemy_count = 0;
        for (int x = row, y = N_ - 1; x < M_ && y >= 0; x++, y--)
        {
            int piece_here = board_[x][y];
            DealWithPiece(piece_here,
                          friendly_continuous_space, friendly_count,
                          enemy_continuous_space, enemy_count,
                          evaluation_point);
        }
        // deal with last squares
        if (friendly_continuous_space >= 4)
            evaluation_point += friendly_count;
        if (enemy_continuous_space >= 4)
            evaluation_point -= enemy_count;
    }
    return evaluation_point;
}

