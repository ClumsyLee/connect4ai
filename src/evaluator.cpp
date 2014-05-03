#include "evaluator.h"

Evaluator::Evaluator(const GameGrid *grid)
        : grid_(grid),
          value_(0.0),
          our_space_(0),
          our_pieces_(0),
          our_pit_(0),
          enemy_space_(0),
          enemy_pieces_(0),
          enemy_pit_(0)
{
}


void Evaluator::Next(int x, int y)
{
    int piece =  grid_->board()[x][y];

    if (piece == GameGrid::FRIENDLY)
    {
        our_space_++;
        our_pieces_++;
        EnemyStop();
    }
    else if (piece == GameGrid::ENEMY)
    {
        enemy_space_++;
        enemy_pieces_++;
        WeStop();
    }
    else  // blank
    {
        if (x == grid_->noX() && y == grid_->noY())
        {
            WeStop();
            EnemyStop();
        }
        else  // true blank
        {
            Skip(x, y);
        }
    }
}


void Evaluator::Skip(int x, int y)
{
    our_space_++;
    enemy_space_++;

    int depth = grid_->top()[y] - x - 1;
    our_pit_ += depth;
    enemy_pit_ += depth;
}


void Evaluator::WeStop()
{
    if (our_pieces_ && our_space_ >= 4)
    {
        value_ += static_cast<double>(our_pieces_) /
                  static_cast<double>(our_pit_ + 1);
    }
    our_space_ = our_pieces_ = our_pit_ = 0;
}


void Evaluator::EnemyStop()
{
    if (enemy_pieces_ && enemy_space_ >= 4)
    {
        value_ -= static_cast<double>(enemy_pieces_) /
                  static_cast<double>(enemy_pit_ + 1);
    }
    enemy_space_ = enemy_pieces_ = enemy_pit_ = 0;
}
