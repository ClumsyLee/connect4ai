#ifndef CONNECT4AI_SOLVER_H_
#define CONNECT4AI_SOLVER_H_

#include "Point.h"

class Solver
{
 public:
    Solver(int M, int N, const int *top, int **board,
           int lastX, int lastY, int noX, int noY);
    ~Solver();

    Point FindBestMove();

 private:
    const int M_, N_;
    const int *top_;
    int **board_;
    const int lastX_, lastY_;
    const int noX_, noY_;
};

#endif  // CONNECT4AI_SOLVER_H_
