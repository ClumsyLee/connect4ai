#ifndef CONNECT4AI_EVALUATOR_H_
#define CONNECT4AI_EVALUATOR_H_

#include "gamegrid.h"

class Evaluator
{
 public:
    explicit Evaluator(const GameGrid *gird);

    void Next(int x, int y);
    void Skip(int x, int y);
    void End();

    double value() const { return value_; }

 private:
    void WeStop();
    void EnemyStop();

    const GameGrid *grid_;
    double value_;

    int our_space_;
    int our_pieces_;
    int our_pit_;
    int enemy_space_;
    int enemy_pieces_;
    int enemy_pit_;
};

inline void Evaluator::End()
{
    WeStop();
    EnemyStop();
}

#endif  // CONNECT4AI_EVALUATOR_H_
