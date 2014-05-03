#ifndef CONNECT4AI_EVALUATOR_H_
#define CONNECT4AI_EVALUATOR_H_

#include "gamegrid.h"

class Evaluator
{
 public:
    Evaluator(const GameGrid *gird);
    ~Evaluator();

    void Begin();
    void Next(int x, int y);
    double End() { return value_; };

 private:
    const GameGrid *grid_;
    double value_;

    int our_space;
    int our_count;
    int enemy_space;
    int enemy_count;
    int our_pit;
    int enemy_pit;
};

#endif  // CONNECT4AI_EVALUATOR_H_
