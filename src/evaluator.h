#ifndef CONNECT4AI_EVALUATOR_H_
#define CONNECT4AI_EVALUATOR_H_

#include "gamegrid.h"

class Evaluator
{
 public:
    Evaluator(const GameGrid *gird);

    void Begin();
    void Next(int x, int y);
    void End();

    double value() const { return value_; }

 private:
    const GameGrid *grid_;
    double value_;

    int our_space_;
    int our_count_;
    int our_pit_;
    int enemy_space_;
    int enemy_count_;
    int enemy_pit_;
};


#endif  // CONNECT4AI_EVALUATOR_H_
