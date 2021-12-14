#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#define BOARD_SIZE 8
#define WHITE 1
#define BLACK 0

typedef enum status{ILLEGAL, LEGAL}STATUS;

typedef struct square_coord{
	char rank;
	char file;
	char boardr;
	char boardc;
}SquareCoord;

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
STATUS parse_input(char*, SquareCoord*, SquareCoord*);
STATUS check_legal(GameState*, SquareCoord, SquareCoord, char*);
int in_check(GameState* gs, char color);
int  set_square(GameState*, SquareCoord, int);
int  get_square(GameState*, SquareCoord);
void update_state(GameState*);
int in_board(int,int);
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

	//set king positions
	gs->wking.rank = 1;
	gs->wking.file = 'e';
	gs->wking.boardc = 4;
	gs->wking.boardr = 0;

	gs->bking.rank = 8;
	gs->bking.file = 'e';
	gs->bking.boardc = 4;
	gs->bking.boardr = 7;
	
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
	//wprintf(L"wcheck = %d\n", in_check(gs, 1));
	//wprintf(L"bcheck = %d\n", in_check(gs, 0));
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
	gs->wcheck = in_check(gs, WHITE);
	gs->bcheck = in_check(gs, BLACK);
	
	//update en passant ..
	gs->turn = !(gs->turn);
}

STATUS parse_input(char* move, SquareCoord* from, SquareCoord* to){
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

STATUS check_legal(GameState* gs, SquareCoord from, SquareCoord to, char* err){
	int piece;
	
	if(!in_board(from.boardr, from.boardc) || !in_board(to.boardr, to.boardc))
		return ILLEGAL;
	
	piece = gs->board[from.boardr][from.boardc];
	if(piece == 0 || (piece < 0 && gs->turn) || (piece > 0 && !gs->turn))
		return ILLEGAL;

	gs->board[to.boardr][to.boardc] = piece;
	gs->board[from.boardr][from.boardc] = 0;
	
	if(in_check(gs, gs->turn))
		return ILLEGAL;

	return LEGAL;
}

//(+/- x, +/- y)
const int knight_offsets[8][2] = {{1,2}, {2,1}, {2,-1}, {1, -2}, {-1,-2}, {-2,-1}, {-2,1}, {-1,2}};

int in_board(int row, int col){
	if(row >= 0 && row < 8 && col >= 0 && col < 8)
		   return 1;
	return 0;	
}

int in_check(GameState* gs, char color){
	int i, row, col, piece;
	SquareCoord kpos;
	SquareCoord check_pos;

	if(color){
		kpos = gs->wking;
	}else{
		kpos = gs->bking;
	}

	//check for knights
	for(i = 0; i < 8; ++i){
		check_pos.boardc = kpos.boardc + knight_offsets[i][0];
		check_pos.boardr = kpos.boardr + knight_offsets[i][1];
		if(in_board(check_pos.boardc, check_pos.boardr))
			//wprintf(L"Knight check(%d, %d) -> %d\n", check_pos.boardr, check_pos.boardc, gs->board[check_pos.boardr][check_pos.boardc]);
			//this 2 + (-4 * color) -> -2(black knight) when king color is white and vice versa
			if(gs->board[check_pos.boardr][check_pos.boardc] == (2 + (-4 * color)))
				return 1;
	}
	
	
	//check the diagonals
	for(row = kpos.boardr+1, col = kpos.boardc+1; in_board(row,col); ++row, ++col){
		piece = gs->board[row][col];		
		//wprintf(L"(%d, %d) -> %d\n", row, col, piece);
		//check for friendly piece blocking the way
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 3)
			return 1;
	}
	
	for(row = kpos.boardr+1, col = kpos.boardc-1; in_board(row,col); ++row, --col){
		piece = gs->board[row][col];
		//wprintf(L"(%d, %d) -> %d\n", row, col, piece);
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 3)
			return 1;
	}
	
	for(row = kpos.boardr-1, col = kpos.boardc+1; in_board(row,col); --row, ++col){
		piece = gs->board[row][col];
		//wprintf(L"(%d, %d) -> %d\n", row, col, piece);
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 3)
			return 1;
	}
	
	for(row = kpos.boardr-1, col = kpos.boardc-1; in_board(row,col); --row, --col){
		piece = gs->board[row][col];
		//wprintf(L"(%d, %d) -> %d\n", row, col, piece);
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 3)
			return 1;
	}
	
	//check the rank
	for(row = kpos.boardr, col = kpos.boardc+1; in_board(row,col); ++col){
		piece = gs->board[row][col];
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 4)
			return 1;
	}

	for(row = kpos.boardr, col = kpos.boardc-1; in_board(row,col); --col){
		piece = gs->board[row][col];
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 4)
			return 1;
	}
	
	//check the file
	for(row = kpos.boardr+1, col = kpos.boardc; in_board(row,col); ++row){
		piece = gs->board[row][col];
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 4)
			return 1;
	}

	for(row = kpos.boardr-1, col = kpos.boardc; in_board(row,col); --row){
		piece = gs->board[row][col];
		if((piece < 0 && !color) || (piece > 0 && color))
			break;
		if(piece == 0)
			continue;
		piece = abs(piece);
		if(piece == 5 || piece == 4)
			return 1;
	}

	//check pawns seperately
	//wprintf(L"kpos -> (%d, %d)\n", kpos.boardr, kpos.boardc);
	if(color){
		if(in_board(kpos.boardr+1, kpos.boardc-1) && gs->board[kpos.boardr+1][kpos.boardc-1] == -1)
			return 1;
		if(in_board(kpos.boardr+1, kpos.boardc+1) && gs->board[kpos.boardr+1][kpos.boardc+1] == -1)
			return 1;
	}else{
		if(in_board(kpos.boardr-1, kpos.boardc-1) && gs->board[kpos.boardr-1][kpos.boardc-1] == 1)
			return 1;
		if(in_board(kpos.boardr-1, kpos.boardc+1) && gs->board[kpos.boardr-1][kpos.boardc+1] == 1)
			return 1;
	}

	return 0;
}

int main(int argc, char** argv){
	char *locale;
	locale = setlocale(LC_ALL, "");

	GameState* gs = (GameState*)malloc(sizeof(GameState));
	init_game(gs);
	print_game(gs);

	char current_move[10];
	char error_msg[30];
	SquareCoord from,to;
	//game loop
	
	while(gs->result == 0){
		scanf("%9s", current_move);
		parse_input(current_move, &from, &to);
		if(!check_legal(gs, from, to, error_msg)){
			wprintf(L"Error: ILLEGAL MOVE fnd\n");
			exit(1);
		}
		//set_square(gs, to, get_square(gs, from));
		//set_square(gs, from, 0);
		print_game(gs);
		update_state(gs);
	}
	
	free(gs);	
	
	return 0;
}
