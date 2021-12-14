#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#define BOARD_SIZE 8

typedef enum status{ILLEGAL, LEGAL}STATUS;

typedef struct game_state{
	char wcheck;
	char bcheck;
	
	// 0 -> game ongoing, 1 -> white win, 2 black win, 3 -> draw
	char result; 	
	
	// 0 -> black, 1 -> white
	char turn;
	
	// if enpassant[0] != \0 then the pawn to be en passanted is store ie "a4"
	char enpassant[3]; 
	
	// 0 -> none, 1 -> pawn, 2 -> knight, 3 -> bishop, 4 -> rook. 5 -> queen, 6 -> king.
	// Positive values indicate white, negative for Black.
	int board[BOARD_SIZE][BOARD_SIZE];

}GameState;

typedef struct square_coord{
	char rank;
	char file;
	char boardr;
	char boardc;
}SquareCoord;


void init_game(GameState*);
void print_game(GameState*);
STATUS parse_input(char*, SquareCoord*, SquareCoord*);
STATUS check_legal(GameState*, SquareCoord, SquareCoord);
int  set_square(GameState*, SquareCoord, int);
int  get_square(GameState*, SquareCoord);
void update_state(GameState*);

/*
 * Initialize a starting board, and other GameState variables
 */

void init_game(GameState* gs){
	static const int first_rank[BOARD_SIZE] = {4, 2, 3, 5, 6, 3, 2, 4};
	static const int sec_rank[BOARD_SIZE] = {1, 1, 1, 1, 1, 1, 1, 1};
	static const int eighth_rank[BOARD_SIZE] = {-4, -2, -3, -5, -6, -3, -2, -4};
	static const int sev_rank[BOARD_SIZE] = {-1, -1, -1, -1, -1, -1, -1, -1};

	gs->wcheck = 0;
	gs->bcheck = 0;
	gs->result = 0;
	gs->turn = 1;
	gs->enpassant[0] = '\0';
	
	//set the ranks with pieces
	memcpy(gs->board[0], first_rank, sizeof first_rank);
	memcpy(gs->board[1], sec_rank, sizeof sec_rank);
	memcpy(gs->board[6], sev_rank, sizeof sev_rank);
	memcpy(gs->board[7], eighth_rank, sizeof eighth_rank);

	//set the empty squares to 0
	memset(gs->board[2], 0, (sizeof gs->board[0][0]) * BOARD_SIZE * 4);
}

/*
 * Print the board representation
 */

static const wchar_t unicode_pieces[] = L"♚ ♛ ♜ ♝ ♞ ♟   ♙ ♘ ♗ ♖ ♕ ♔ ";

void print_game(GameState* gs){
	int i,j;
	for(i = BOARD_SIZE-1; i >= 0; --i){
		for(j = 0 ; j < BOARD_SIZE; ++j){
			wprintf(L"%lc ", unicode_pieces[(gs->board[i][j] + 6)*2]);
			fflush(stdout);
		}
		wprintf(L"\n");
	}
	wprintf(L"\n");
}

int set_square(GameState* gs, SquareCoord sc, int pv){
	gs->board[sc.boardr][sc.boardc] = pv;
	return 1;
}

int get_square(GameState* gs, SquareCoord sc){
	printf("%d %d", sc.rank, sc.file);
	return gs->board[sc.boardr][sc.boardc];
}

void update_state(GameState* gs){
	//check for mate ..
	//check for check ..
	//update en passant ..
	gs->turn = !(gs->turn);
}

STATUS parse_input(char* move, SquareCoord* from, SquareCoord* to){
	int slen;

	from->file = move[0];
	from->rank = move[1];
	from->boardc = from->file - 'a';
	from->boardr = (from->rank - '0') -1;

	to->file = move[2];
	to->rank = move[3];
	to->boardc = to->file - 'a';
	to->boardr = (to->rank - '0') -1;

	//wprintf(L"IN: %s -> From %d %d ... To %d %d\n", move, from->file, from->rank, to->file, to->rank);
	
	return LEGAL;
}

STATUS check_legal(GameState* gs, SquareCoord from, SquareCoord to){
	return LEGAL;
}

int main(int argc, char** argv){
	char *locale;
    locale = setlocale(LC_ALL, "");

	GameState* gs = (GameState*)malloc(sizeof(GameState));
	init_game(gs);
	print_game(gs);

	char current_move[10];
	SquareCoord from,to;
	//game loop
	
	while(gs->result == 0){
		scanf("%9s", current_move);
		parse_input(current_move, &from, &to);
		set_square(gs, to, get_square(gs, from));
		set_square(gs, from, 0);
		print_game(gs);
		update_state(gs);
	}
	
	free(gs);	
	
	return 0;
}
