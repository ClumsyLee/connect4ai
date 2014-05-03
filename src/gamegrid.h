#ifndef CONNECT4AI_GAMEGRID_H_
#define CONNECT4AI_GAMEGRID_H_

class GameGrid
{
 public:
    enum { BLANK, ENEMY, FRIENDLY };

    // construct from a initial state, will set static members.
    GameGrid(int M, int N, const int *top, int **board,
             int lastX, int lastY, int noX, int noY);
    // construct from parent.
    GameGrid(const GameGrid &parent);
    ~GameGrid();

    void Place(int this_x, int this_y, int piece_type);

    static int M() { return M_; }
    static int N() { return N_; }
    static int noX() { return noX_; }
    static int noY() { return noY_; }
    const int * top() const { return top_; }

    int ** board() const { return board_; }
    double value() const { return value_; }

 private:
    void Reevaluate(int this_x, int this_y);

    // row and col of the grid
    static int M_;
    static int N_;
    static int noX_;
    static int noY_;

    int *top_;
    int **board_;

    double value_;
};


#endif  // CONNECT4AI_GAMEGRID_H_
