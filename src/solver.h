#ifndef CONNECT4AI_SOLVER_H_
#define CONNECT4AI_SOLVER_H_

#include "gamegrid.h"
#include "Point.h"

class Solver
{
 public:
    Solver(int M, int N, const int *top, int **board,
           int lastX, int lastY, int noX, int noY,
           int depth);
    ~Solver();

    Point FindBestMove();

 private:
    class AlphaNode;
    class BetaNode;

    // disable copying
    Solver(const Solver &);
    void operator=(const Solver &);

    const GameGrid *initial_state_;
    int depth_;
};

#endif  // CONNECT4AI_SOLVER_H_
