#ifndef POSITION_H
#define POSITION_H

#include<random>
#include<time.h>

using namespace std;

#ifndef PLAYER_PIECE
#define PLAYER_PIECE 1
#endif // !PLAYER_PIECE

#ifndef AI_PIECE
#define AI_PIECE 2
#endif // !AI_PIECE

#ifndef EMPTY_PIECE
#define EMPTY_PIECE 0
#endif // !EMPTY_PIECE

class Position {
public:
    static const int WIDTH = 7;  // Width of the board
    static const int HEIGHT = 6; // Height of the board
    static const int DIAGONAL = 6; //max diagonal of board
    int board[HEIGHT][WIDTH] = { 0 };  //board 00 is bottom left 06 is bottom right
    int columnHeight[WIDTH] = { -1,-1,-1,-1,-1,-1,-1 };
    int Pmoves = 0;
    int AImoves = 0;
    bool gameover = false;
   

    bool isGameOver();

    bool canPlay(int col) const; //check if can play at that column

    void play(int col);  //Player plays a piece on that column
        
    void AIplay(int col); //AI plays a piece on that column
        
    void Premove(int col);//remove a Player piece
        
    void AIremove(int col);  //remova a AI piece

    int score_Position(Position& P, bool isPlayer); //scans whole board and adds point to manipulate AI behaviour
        
    int bestMove(Position& P);//finds best move based on score

    bool isWinningMove(bool isPlayer);

    unsigned int nbMoves() const; //return number of moves played from the beginning of the game.
       
};

#endif