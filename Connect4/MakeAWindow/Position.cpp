#include "Position.h"

bool Position::isGameOver() {
    return gameover;
}

bool Position::canPlay(int col) const { //check if can play at that column
    return columnHeight[col] < HEIGHT-1;
};

void Position::play(int col) {  //Player plays a piece on that column
    columnHeight[col]++;
    board[columnHeight[col]][col] = PLAYER_PIECE;
    Pmoves++;
};

void Position::AIplay(int col) { //AI plays a piece on that column
    columnHeight[col]++;
    board[columnHeight[col]][col] = AI_PIECE;
    AImoves++;
};

void Position::Premove(int col) { //remove a Player piece
    board[columnHeight[col]][col] = 0;
    columnHeight[col]--;
    Pmoves--;
};

void Position::AIremove(int col) {  //remova a AI piece
    board[columnHeight[col]][col] = 0;
    columnHeight[col]--;
    AImoves--;
};

int Position::score_Position(Position& P, bool isPlayer) {  //scans whole board and adds point to manipulate AI behaviour
    int score = 0, me = 0, opp = 0;
    if (isPlayer) {  //used to check for 1 or -1 based on whose turn
        me = PLAYER_PIECE;
        opp = AI_PIECE;
    }
    else {
        me = AI_PIECE;
        opp = PLAYER_PIECE;
    }
    //preference for middle column
    for (int c = 1; c < WIDTH-1; c++) {
        for (int r = 0; r < HEIGHT; r++) {
            if (P.board[r][c] == me) {
                score += 1;
            }
        }
    }

    //horizontal 
    for (int r = 0; r < HEIGHT; r++) {
        int window[WIDTH] = { 0,0,0,0,0,0,0 };  //window to check for 4s or 3s in a 4 piece constraint
        for (int c = 0; c < WIDTH; c++) { //
            window[c] = (P.board[r][c]);
        }
        for (int c = 0; c < WIDTH - 3; c++) {
            if (count(window + c, window + (4 + c), me) == 4) { //4 in-a-row
                score += 100;
            }
            else if ((count(window + c, window + (4 + c), me) == 3) && (count(window + c, window + (4 + c), EMPTY_PIECE) == 1)) { //3 pieces within 4 pieces
                score += 20;
            }
            else if ((count(window + c, window + (4 + c), me) == 2) && (count(window + c, window + (4 + c), EMPTY_PIECE) == 2)) { //2 pieces within 4 pieces
                score += 10;
            }

            if ((count(window + c, window + (4 + c), opp) == 3) && (count(window + c, window + (4 + c), EMPTY_PIECE) == 1)) { //3 pieces within 4 pieces
                score -= 80;
            }
        }
    }
    //vertical
    for (int c = 0; c < WIDTH; c++) {
        int window[HEIGHT] = { 0,0,0,0,0,0 }; //window to check for 4s or 3s in a 4 piece constraint
        for (int r = 0; r < HEIGHT; r++) {
            window[r] = P.board[r][c];
        }
        for (int r = 0; r < HEIGHT - 3; r++) {
            if (count(window + r, window + (4+r), me) == 4) { //4 in-a-row
                score += 100;
            }
            else if ((count(window + r, window + (4 + r), me) == 3) && (count(window + r, window + (4 + r), EMPTY_PIECE) == 1)) { //3 pieces within 4 pieces
                score += 20;
            }
            else if ((count(window + r, window + (4 + r), me) == 2) && (count(window + r, window + (4 + r), EMPTY_PIECE) == 2)) { //2 pieces within 4 pieces
                score += 10;
            }
            if ((count(window + r, window + (4 + r), opp) == 3) && (count(window + r, window + (4 + r), EMPTY_PIECE) == 1)) { //3 pieces within 4 pieces
                score -= 80;
            }

        }
    }
    //diagonal bottom left to upper right
    for (int r = 0; r < HEIGHT - 3; r++) {
        for (int c = 0; c < WIDTH - 3; c++) {
            int window[DIAGONAL] = { 0,0,0,0,0,0 }; //window to check for 4s or 3s in a 4 piece constraint
            for (int d = 0; d < DIAGONAL; d++) {
                if (c + d < 7 && r + d < 6) {
                    window[d] = P.board[r + d][c + d];
                }
            }
            for (int d = 0; d < DIAGONAL - 3; d++) {
                if (c + d < 4 && r + d < 3) {
                    if (count(window + d, window + (4 + d), me) == 4) { //4 in-a-row
                        score += 100;
                    }
                    else if ((count(window + d, window + (4 + d), me) == 3) && (count(window + d, window + (4+d), EMPTY_PIECE) == 1)) { //3 pieces within 4 pieces
                        score += 20;
                    }
                    else if ((count(window + d, window + (4 + d), me) == 2) && (count(window + d, window + (4 + d), EMPTY_PIECE) == 2)) { //2 pieces within 4 pieces
                        score += 10;
                    }
                    if ((count(window + d, window + (4 + d), opp) == 3) && (count(window + d, window + (4 + d), EMPTY_PIECE) == 1)) { //3 pieces within 4 pieces
                        score -= 80;
                    }
                }
            }
        }
    }
    //diagonal upper left to bootom right
    for (int r = 5; r > 2; r--) {
        for (int c = 0; c < WIDTH - 3; c++) {
            int window[DIAGONAL] = { 0,0,0,0,0,0 }; //window to check for 4s or 3s in a 4 piece constraint
            for (int d = 0; d < DIAGONAL; d++) {
                if (c + d < 7 && r - d >= 0) {
                    window[d] = P.board[r - d][c + d];
                }
            }
            for (int d = 0; d < DIAGONAL - 3; d++) {
                if (c + d < 4 && r - d >= 3) {
                    if (count(window + d, window + (4 + d), me) == 4) { //4 in-a-row
                        score += 100;
                    }
                    else if ((count(window + d, window + (4 + d), me) == 3) && (count(window + d, window + (4+ d), EMPTY_PIECE) == 1)) {//3 pieces within 4 pieces
                        score += 20;
                    }
                    else if ((count(window + d, window + (4 + d), me) == 2) && (count(window + d, window + (4 + d), EMPTY_PIECE) == 2)) {//2 pieces within 4 pieces
                        score += 10;
                    }
                    if ((count(window + d, window + (4 + d), opp) == 3) && (count(window + d, window + (4 + d), EMPTY_PIECE) == 1)) { //3 pieces within 4 pieces
                        score -= 80;
                    }
                }
            }
        }
    }
    return score;
}

int Position::bestMove(Position& P) {
    int best_score = -1000;

    int best_column = rand() % 7;
    for (int c = 0; c < WIDTH; c++) {

        if (canPlay(c)) {
            Position P2(P);
            P2.AIplay(c);

            int score = score_Position(P2, false);
            
            if (score > best_score) {
                best_score = score;
                best_column = c;
            }
        }
    }
    return best_column;
}


bool Position::isWinningMove(bool isPlayer) { //check if current move can result in a win
    int count = 0; //count the number of connected token
    int ans = 0; // 1 if player turn 2 if AI turn
    if (isPlayer) {
        ans = PLAYER_PIECE;
    }
    if(!isPlayer){
        ans = AI_PIECE;
    }
    
    //horizontal
    for (int r = 0; r < HEIGHT; r++) {
        for (int c = 0; c < WIDTH-3; c++) {
            if (board[r][c] == ans && board[r][c + 1] == ans && board[r][c + 2] == ans && board[r][c + 3] == ans) {
                return true;
            }
        }
    }
    //vertical
    for (int c = 0; c < WIDTH; c++) {
        for (int r = 0; r < HEIGHT - 3; r++) {
            if (board[r][c] == ans && board[r+1][c] == ans && board[r+2][c] == ans && board[r+3][c] == ans) {
                return true;
            }
        }
    }
    //upper left to lower right
    for (int r = 5; r > 2; r--) {
        for (int c = 0; c < WIDTH - 3; c++) {
            if (board[r][c] == ans && board[r - 1][c+1] == ans && board[r - 2][c+2] == ans && board[r - 3][c+3] == ans) {
                return true;
            }
        }
    }
    
    for (int r = 0; r < HEIGHT - 3; r++) {
        for (int c = 0; c < WIDTH - 3; c++) {
            if (board[r][c] == ans && board[r + 1][c + 1] == ans && board[r + 2][c + 2] == ans && board[r + 3][c + 3] == ans) {
                return true;
            }
        }
    }
    
    /*for (int c = 0; c < WIDTH; c++) {
        if (board[columnHeight[col]][c] == ans) {
            count++;
            if (count == 4) {
                return true;
            }
        }
        else {
            count = 0;
        }
    }
    count = 0;
    //vertical

    for (int r = 0; r < HEIGHT; r++) {
        if (board[r][col] == ans) {
            count++;
            if (count == 4) {
                return true;
            }
        }
        else {
            count = 0;
        }
    }
    count = 0;*/
    //diagonal top left to bottom right
    //diagonal upper left to bootom right
    /*for (int r = 5; r > 2; r--) {
        for (int c = 0; c < WIDTH - 3; c++) {
            for (int d = 0; d < DIAGONAL; d++) {
                if (c + d < 7 && r - d >= 0) {
                    if (board[r - d][c + d] == ans) {
                        count++;
                        if (count == 4) {
                            return true;
                        }
                    }
                    else {
                        count = 0;
                    }
                }
            }
        }
    }*/
    /*if (columnHeight[col] <= HEIGHT - col) {  //check if in left side of diagonal board
        for (int row = 5; row >= HEIGHT - 3; row--) {
            for (int c = 0, r = row; c < WIDTH && r >= 0; c++, r--) {
                if (board[r][c] == ans) {
                    count++;
                    if (count == 4) {
                        return true;
                    }
                }
                else {
                    count = 0;
                }
            }
        }
    }
    else {   //right side of diagonal board
        for (int column = 1; column < WIDTH - 3; column++) {
            for (int c = column, r = 5; c < WIDTH && r >= 0; c++, r--) {
                if (board[r][c] == ans) {
                    count++;
                    if (count == 4) {
                        return true;
                    }
                }
                else {
                    count = 0;
                }
            }
        }
    }
    */
   /* count = 0;
    
    //diagonal bottom left to top right 
    //diagonal bottom left to upper right
    for (int r = 0; r < HEIGHT - 3; r++) {
        for (int c = 0; c < WIDTH - 3; c++) {
            for (int d = 0; d < DIAGONAL; d++) {
                if (c + d < 7 && r + d < 6) {
                    if (board[r + d][c + d] == ans) {
                        count++;
                        if (count == 4) {
                            return true;
                        }
                    }
                    else {
                        count = 0;
                    }
                   
                }
            }
        }
    }*/
    /*if (columnHeight[col] <= HEIGHT - (WIDTH - 1 - col)) {  //check if in left side of diagonal board
        for (int row = 0; row < HEIGHT - 3; row++) {
            for (int c = 0, r = row; c < WIDTH && r < HEIGHT; c++, r++) {
                if (board[r][c] == ans) {
                    count++;
                    if (count == 4) {
                        return true;
                    }
                }
                else {
                    count = 0;
                }
            }
        }
    }
    else {   //right side of diagonal board
        for (int column = 1; column < WIDTH; column++) {
            for (int c = column, r = 0; c < WIDTH && r < HEIGHT; c++, r++) {
                if (board[r][c] == ans) {
                    count++;
                    if (count == 4) {
                        return true;
                    }
                }
                else {
                    count = 0;
                }
            }
        }
    }*/

    return false;
};

unsigned int Position::nbMoves() const { //return number of moves played from the beginning of the game.
    return Pmoves + AImoves;
};



 
