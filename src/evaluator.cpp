#include "evaluator.h"

Evaluator::Evaluator(const GameGrid *grid)
        : grid_(grid),
          value_(0.0),
          our_space_(0),
          our_count_(0),
          our_pit_(0),
          enemy_space_(0),
          enemy_count_(0),
          enemy_pit_(0)
{
}

void Evaluator::Begin()
{
    our_space_ = our_count_ = our_pit_ =
    enemy_space_ = enemy_count_ = enemy_pit_ = 0;
}

void Evaluator::Next(int x, int y)
{
}

void Evaluator::End()
{
}
