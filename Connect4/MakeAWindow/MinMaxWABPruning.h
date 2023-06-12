#ifndef MINMAXWABPRUNING_H
#define MINMAXWABPRUNING_H
#include<random>
#include "Position.h"
using namespace std;
#ifndef PLAYER_PIECE
#define PLAYER_PIECE 1
#endif // !PLAYER_PIECE

#ifndef AI_PIECE
#define AI_PIECE -1
#endif // !AI_PIECE

#ifndef EMPTY_PIECE
#define EMPTY_PIECE 0
#endif // !EMPTY_PIECE

struct Col_Score {
    int COL;
    int score;
};

Col_Score MinMaxWABPruning(Position& P, int alpha, int beta, bool isMaxPlayer, int depth);

#endif