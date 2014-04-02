#ifndef CONNECT4AI_SOLVER_H_
#define CONNECT4AI_SOLVER_H_

#include "Point.h"

class Solver
{
 public:
    class GameGrid;

    Solver(int M, int N, const int *top, int **board,
           int lastX, int lastY, int noX, int noY,
           int depth);
    ~Solver();

    Point FindBestMove();

 private:
    class AlphaNode;
    class BetaNode;

    Solver(const Solver &);
    void operator=(const Solver &);

    const GameGrid *initial_state_;
    int depth_;
};

#endif  // CONNECT4AI_SOLVER_H_
