#include <cstring>
#include <climits>
#include <iostream>
#include "solver.h"
#include "Point.h"
#include "Judge.h"

///////////////////////// classes declarations /////////////////////////

class Solver::GameGrid
{
 public:
    enum { BLANK, ENEMY, FRIENDLY };

    GameGrid(int M, int N, const int *top, int **board,
             int lastX, int lastY, int noX, int noY);
    GameGrid(const GameGrid &parent);
    ~GameGrid();

    void Place(int this_x, int this_y, int piece_type);
    int Evaluate() const;

    static int M() { return M_; }
    static int N() { return N_; }
    static int noX() { return noX_; }
    static int noY() { return noY_; }
    const int * top() const { return top_; }
    //
    int ** board() const { return board_; }

 private:
    // row and col of the grid
    static int M_;
    static int N_;
    static int noX_;
    static int noY_;

    int *top_;
    int **board_;
};

class Solver::AlphaNode
{
 public:
    explicit AlphaNode(const GameGrid & board);
    AlphaNode(const BetaNode &parent, int this_x, int this_y);

    int FindMax(int depth);

    const GameGrid & board() const { return board_; }
    int alpha_max() const { return alpha_max_; }
    int beta_min() const { return beta_min_; }
    int best_col() const { return best_col_; }

 private:
    GameGrid board_;
    int alpha_max_;
    int beta_min_;
    int best_col_;
};

class Solver::BetaNode
{
 public:
    BetaNode(const AlphaNode &parent, int this_x, int this_y);

    int FindMin(int depth);

    const GameGrid & board() const { return board_; }
    int alpha_max() const { return alpha_max_; }
    int beta_min() const { return beta_min_; }
    int best_col() const { return best_col_; }

 private:
    GameGrid board_;
    int alpha_max_;
    int beta_min_;
    int best_col_;
};



///////////////////////// helper functions /////////////////////////

namespace {

void DealWithPiece(int piece_here,
                   int &friendly_continuous_space, int &friendly_count,
                   int &enemy_continuous_space, int &enemy_count,
                   int &evaluation_point)
{
    if (piece_here == Solver::GameGrid::ENEMY)
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
    else if (piece_here == Solver::GameGrid::FRIENDLY)
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


///////////////////////// functions of GameGrid /////////////////////////

// static members
int Solver::GameGrid::M_ = -1;
int Solver::GameGrid::N_ = -1;
int Solver::GameGrid::noX_ = -1;
int Solver::GameGrid::noY_ = -1;

Solver::GameGrid::GameGrid(int M, int N, const int *top, int **board,
                           int lastX, int lastY, int noX, int noY)
        : top_(new int[N]),
          board_(new int *[M])
{
    std::clog << "Building GameGrid from origin arguments...";

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

    std::clog << "Done\n";
}

Solver::GameGrid::GameGrid(const GameGrid &parent)
        : top_(new int[N_]),
          board_(new int *[M_])
{
    std::clog << "Building GameGrid from parent GameGrid...";

    // copy the top
    std::memcpy(top_, parent.top_, N_ * sizeof(top_[0]));

    // copy the board
    for (int row = 0; row < M_; row++)
    {
        board_[row] = new int[N_];
        std::memcpy(board_[row], parent.board_[row],
                    N_ * sizeof(board_[0][0]));
    }

    std::clog << "Done\n";
}

Solver::GameGrid::~GameGrid()
{
    std::clog << "Destructing GameGrid...";
    // delete top
    delete top_;

    // delete board
    for (int row = 0; row < M_; row++)
        delete board_[row];
    delete board_;

    std::clog << "Done\n";
}

void Solver::GameGrid::Place(int this_x, int this_y, int piece_type)
{
    std::clog << "Placing " << piece_type << " to (" << this_x << ", "
                                                     << this_y << ")...";

    // adjust the top of the col
    // jump over if the new top is (noX, noY)
    if (this_y == noY_ && this_x == noX_ + 1 && noX_ != 0)
        top_[this_y] -= 2;
    else
        top_[this_y]--;

    std::clog << "top adjusted...";
    // add the new piece
    board_[this_x][this_y] = piece_type;

    std::clog << "Done\n";
}

int Solver::GameGrid::Evaluate() const
{
    std::clog << "Evaluating...\n";

    for (int x = 0; x < M_; x++)
    {
        for (int y = 0; y < N_; y++)
            std::clog << board_[x][y];
        std::clog << std::endl;
    }

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
    std::clog << "Evaluation point: " << evaluation_point << std::endl;
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
    std::clog << "Evaluation point: " << evaluation_point << std::endl;
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
    std::clog << "Evaluation point: " << evaluation_point << std::endl;
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
    std::clog << "Evaluation point: " << evaluation_point << std::endl;
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
    std::clog << "Evaluation point: " << evaluation_point << std::endl;
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
    std::clog << "Evaluation point: " << evaluation_point << std::endl;
    return evaluation_point;
}



///////////////////////// functions of Solver /////////////////////////

Solver::Solver(int M, int N, const int *top, int **board,
               int lastX, int lastY, int noX, int noY,
               int depth)
        : initial_state_(new GameGrid(M, N, top, board,
                        lastX, lastY, noX, noY)),
          depth_(depth)
{
    std::clog << "A solver built\n";
}

Solver::~Solver()
{
    delete initial_state_;
    std::clog << "A solver destructed\n";
}

Point Solver::FindBestMove()
{
    AlphaNode head_node(*initial_state_);
    head_node.FindMax(depth_);
    int best_col = head_node.best_col();

    if (best_col == -1)  // no solutions
        return Point(-1, -1);

    return Point(initial_state_->top()[best_col] - 1, best_col);
}



///////////////////////// functions of AlphaNode /////////////////////////

Solver::AlphaNode::AlphaNode(const GameGrid & board)
        : board_(board),
          alpha_max_(INT_MIN),
          beta_min_(INT_MAX),
          best_col_(-1)
{
}


Solver::AlphaNode::AlphaNode(const BetaNode &parent, int this_x, int this_y)
        : board_(parent.board()),
          alpha_max_(parent.alpha_max()),
          beta_min_(parent.beta_min()),
          best_col_(-1)
{
    board_.Place(this_x, this_y, GameGrid::FRIENDLY);
}

int Solver::AlphaNode::FindMax(int depth)
{
    std::clog << "FindMax called for a alpha node (depth: " << depth << ")\n";
    if (depth <= 0)  // depth limit reached
        return board_.Evaluate();

    int max_value = INT_MIN;
    for (int col = 0; col < GameGrid::N(); col++)
    {
        int top_this_col = board_.top()[col];
        if (top_this_col == 0)  // cannot put pieces here
            continue;

        // construct the child and find min
        BetaNode child(*this, top_this_col - 1, col);
        int child_value;
        // If AI wins, there is no need to keep finding
        // so the child node do not need to expend.
        if (machineWin(top_this_col - 1, col, GameGrid::M(), GameGrid::N(),
            child.board().board()))
            child_value = INT_MAX;
        else
            child_value = child.FindMin(depth - 1);

        if (child_value > max_value)  // a better child found
        {
            max_value = child_value;
            best_col_ = col;

            if (max_value > alpha_max_)
            {
                if (max_value > beta_min_)  // beta cut
                    return max_value;
                alpha_max_ = max_value;  // update alpha_max_
            }
        }
    }
    return max_value;
}



///////////////////////// functions of BetaNode /////////////////////////

Solver::BetaNode::BetaNode(const AlphaNode &parent, int this_x, int this_y)
        : board_(parent.board()),
          alpha_max_(parent.alpha_max()),
          beta_min_(parent.beta_min()),
          best_col_(-1)
{
    board_.Place(this_x, this_y, GameGrid::ENEMY);
}


int Solver::BetaNode::FindMin(int depth)
{
    std::clog << "FindMin called for a beta node (depth: " << depth << ")\n";
    if (depth <= 0)  // depth limit reached
        return board_.Evaluate();

    int min_value = INT_MAX;
    for (int col = 0; col < GameGrid::N(); col++)
    {
        int top_this_col = board_.top()[col];
        if (top_this_col == 0)  // cannot put pieces here
            continue;

        // construct the child and find max
        AlphaNode child(*this, top_this_col - 1, col);
        int child_value;
        // If user wins, there is no need to keep finding
        // so the child node do not need to expend.
        if (userWin(top_this_col - 1, col, GameGrid::M(), GameGrid::N(),
            child.board().board()))
            child_value = INT_MIN;
        else
            child_value = child.FindMax(depth - 1);

        if (child_value < min_value)  // a better child found
        {
            min_value = child_value;
            best_col_ = col;

            if (min_value < beta_min_)
            {
                if (min_value < alpha_max_)  // alpha cut
                    return min_value;
                beta_min_ = min_value;  // update beta_min_
            }
        }
    }
    return min_value;
}
