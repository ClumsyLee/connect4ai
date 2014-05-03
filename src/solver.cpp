#include <cstdlib>
#include <climits>
#include <iostream>
#include "gamegrid.h"
#include "Judge.h"
#include "Point.h"
#include "solver.h"

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



///////////////////////// functions of Solver /////////////////////////

Solver::Solver(int M, int N, const int *top, int **board,
               int lastX, int lastY, int noX, int noY,
               int depth)
        : initial_state_(new GameGrid(M, N, top, board,
                                      lastX, lastY, noX, noY)),
          depth_(depth)
{
}


Solver::~Solver()
{
    delete initial_state_;
}


Point Solver::FindBestMove()
{
// TODO: make N_ beta node, evaluate them fisrt, to find quick win
    AlphaNode head_node(*initial_state_);
    head_node.FindMax(depth_);
    int best_col = head_node.best_col();

    if (best_col == -1)  // no solutions
    {
        // pick a column randomly
        do
        {
            best_col = std::rand() % GameGrid::N();
        } while (initial_state_->top()[best_col] == 0);
    }

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
    board_.Place(this_x, this_y, GameGrid::ENEMY);
}


int Solver::AlphaNode::FindMax(int depth)
{
    if (depth <= 0)  // depth limit reached
        return board_.value();

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
    board_.Place(this_x, this_y, GameGrid::FRIENDLY);
}


int Solver::BetaNode::FindMin(int depth)
{
    if (depth <= 0)  // depth limit reached
        return board_.value();

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
