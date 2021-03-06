#ifndef CHESS_UTILS_H
#define CHESS_UTILS_H

#define BOARD_SIZE 8

typedef enum status{ILLEGAL, LEGAL}STATUS;

enum abs_pieces{PAWN=1, KNIGHT, BISHOP, ROOK, QUEEN, KING};

typedef struct square_coord{
	char rank;
	char file;
	char boardr;
	char boardc;
}SquareCoord;

typedef struct square_coord_list{
	int size;
	SquareCoord list[8];
}SquareCoordList;

typedef struct move{
	//"nxe4 ..."
	char algnot[10];
	SquareCoord from;
	SquareCoord to;
	int piece;
	//normal, capture, en passant
	int move_type;
}Move;

typedef struct game_state{
	char wcheck;
	char bcheck;

	// 0 -> game ongoing, 1 -> white win, 2 black win, 3 -> draw
	char result;    

	// 0 -> black, 1 -> white
	char turn;

	// if enpassant[0] != \0 then the pawn to be en passanted is store ie "a4"
	char enpassant[3]; 

	//keep track of the kings to make certain checks easier
	SquareCoord bking;
	SquareCoord wking;

	// 0 -> none, 1 -> pawn, 2 -> knight, 3 -> bishop, 4 -> rook. 5 -> queen, 6 -> king.
	// Positive values indicate white, negative for Black.
	int board[BOARD_SIZE][BOARD_SIZE];

}GameState;


void init_game(GameState*);
void print_game(GameState*);
STATUS check_legal(GameState*, SquareCoord, SquareCoord, char*);
int is_square_attacked(GameState*, SquareCoord, int, SquareCoordList*);
int in_check(GameState*, char);
int  set_square(GameState*, SquareCoord, int);
int  get_square(GameState*, SquareCoord);
void update_state(GameState*);
int in_board(int,int);
void print_game_state(GameState*);
int check_line(GameState*, SquareCoord, int, int, int, SquareCoordList*);
int check_square(GameState*, int, int, int, SquareCoordList*);
void alg_to_coord(SquareCoord*);
int is_square_attacked_color(GameState*, SquareCoord, char, SquareCoordList*);
int in_check_mate(GameState*, char);
#endif
