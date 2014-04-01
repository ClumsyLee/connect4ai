#include "solver.h"
#include "Point.h"

Solver::Solver(int M, int N, const int *top, int **board,
               int lastX, int lastY, int noX, int noY)
        : M_(M),
          N_(N),
          top_(top),
          board_(board),
          lastX_(lastX),
          lastY_(lastY),
          noX_(noX),
          noY_(noY)
{
}

Solver::~Solver()
{
}

Point Solver::FindBestMove()
{

}
