#include "MinMaxWABPruning.h"
#include <random>
#include <time.h>

using namespace std;

Col_Score MinMaxWABPruning(Position& P, int alpha, int beta, bool isMaxPlayer, int depth) {
    srand(time(NULL));
    
    Col_Score Terminal_Node;
   // check if is winning state
    if (isMaxPlayer == false) {
        if (P.isWinningMove(false)) {
            Terminal_Node = { NULL, 100000000 };
            return Terminal_Node;
        }
    }
    if (isMaxPlayer == true) {
        if (P.isWinningMove(true)) {
            Terminal_Node = { NULL, -100000000 };
            return Terminal_Node;
        }
    }

    if (depth == 0) {
        Terminal_Node = { P.bestMove(P), P.score_Position(P,isMaxPlayer) };
        return Terminal_Node;
    }

    if (P.nbMoves() == Position::WIDTH * Position::HEIGHT) { // check for draw game
        Terminal_Node = { P.bestMove(P), 0 };
        return Terminal_Node;
    }

    if (isMaxPlayer) { //AI is Max
        int maxScore = -10000000; //score of this node/position/state
        Col_Score MaxPlayer = { rand() % 7, maxScore }; //col = choose a random column
        for (int x = 0; x < Position::WIDTH; x++) {
            if (P.canPlay(x)) {
                Position P2(P);
                P2.AIplay(x);
                Col_Score current;
                current = MinMaxWABPruning(P2, alpha, beta, false, depth - 1); //recursive call to search into search tree
                if (current.score > maxScore) { //remember that move if child node has better score for AI
                    MaxPlayer.COL = x;
                    maxScore = current.score;
                    MaxPlayer.score = maxScore;
                }
                alpha = max(alpha, MaxPlayer.score);
                if (alpha >= beta) {
                    break;  //cutoff 
                }
            }
        }
        return MaxPlayer;
    }

    if (!isMaxPlayer) { //Player is Min

        int minScore = 10000000;  //score of this node/position/state
        Col_Score MinPlayer = { rand() % 7 , minScore };
        for (int x = 0; x < Position::WIDTH; x++) {
            if (P.canPlay(x)) {
                Position P2(P);
                P2.play(x);
                Col_Score current;
                current = MinMaxWABPruning(P2, alpha, beta, true, depth - 1); //recursive call to search into search tree
                if (current.score < minScore) { //remember that move if child node has better score for Player
                    MinPlayer.COL = x;
                    minScore = current.score;
                    MinPlayer.score = minScore;
                }
                beta = min(beta, MinPlayer.score);
                if (alpha >= beta) {
                    break;  //cutoff 
                }
            }
        }
        return MinPlayer;
    }
}
