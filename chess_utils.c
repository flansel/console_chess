#include "chess_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#define WHITE 1
#define BLACK 0

enum pieces{B_KING=-6, B_QUEEN, B_ROOK, B_BISHOP, B_KNIGHT, B_PAWN, NONE,
				W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING};

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

/*
 * GameState, targeted square, piece type to check for, color that is attacking, optional loco
 * that is filled with the position of the attacking piece
 * 
 * check_line & check_square are helper fns to check a square, row, col, or diagonal
 * 
 * eg of is_square_attacked:
 *
 * is_s_a(gs,tg,2,NULL) -> is a white knight attacking square tg in game gs, dont bother given me 
 * the coords
 */
int check_line(GameState* gs, SquareCoord st, int pc, int rowf, int colf, SquareCoord* loco){
	int row, col, piece;
	for(row = st.boardr+rowf, col = st.boardc+colf; in_board(row,col); row+=rowf, col+=colf){
		piece = gs->board[row][col];
		if(piece != 0 && piece != pc){
			return 0;
		}else if(piece == pc){
			if(loco){
				loco->boardr = row;
				loco->boardr = col;
			}
			return 1;
		}
	}
return 0;
}

int check_square(GameState* gs, int row, int col, int pc, SquareCoord* loco){
	if(in_board(row, col) && gs->board[row][col] == pc){
		if(loco){
			loco->boardr = row;
			loco->boardc = col;
		}
		return 1;
	}
	return 0;
}

int is_square_attacked(GameState* gs, SquareCoord tg, int pc, SquareCoord* loco){
	int i, row, col, ptype;
	SquareCoord check_pos;

	ptype = abs(pc);

	switch(ptype){
		//check for knights
		case 2:
			for(i = 0; i < 8; ++i){
				check_pos.boardc = tg.boardc + knight_offsets[i][0];
				check_pos.boardr = tg.boardr + knight_offsets[i][1];
				if(in_board(check_pos.boardc, check_pos.boardr)){
					if(gs->board[check_pos.boardr][check_pos.boardc] == pc){
						if(loco){
							loco->boardr = check_pos.boardr;
							loco->boardc = check_pos.boardc;
						}
						return 1;
					}
				}
			}
			break;
		case 4:
		case 5:
			if(check_line(gs, tg, pc, 0, 1, loco))
				return 1;
			if(check_line(gs, tg, pc, 0, -1, loco))
				return 1;
			if(check_line(gs, tg, pc, 1, 0, loco))
				return 1;
			if(check_line(gs, tg, pc, -1, 0, loco))
				return 1;
			if(ptype == 4)
				break;
		case 3:
			if(check_line(gs, tg, pc, 1, 1, loco))
				return 1;
			else if(check_line(gs, tg, pc, 1, -1, loco))
				return 1;
			else if(check_line(gs, tg, pc, -1, 1, loco))
				return 1;
			else if(check_line(gs, tg, pc, -1, -1, loco))
				return 1;
			break;
		case 6:
			row = tg.boardr;
			col = tg.boardc;
			if(check_square(gs, row+1, col, pc, loco))
				return 1;
			if(check_square(gs, row-1, col, pc, loco))
				return 1;
			if(check_square(gs, row, col+1, pc, loco))
				return 1;
			if(check_square(gs, row, col-1, pc, loco))
				return 1;
		case 1:
			row = tg.boardr;
			col = tg.boardc;
			if(pc != -1){
				if(check_square(gs, row-1, col-1, pc, loco))
					return 1;
				if(check_square(gs, row-1, col+1, pc, loco))
					return 1;
			}
			if(pc != 1){
				if(check_square(gs, row+1, col-1, pc, loco))
					return 1;
				if(check_square(gs, row+1, col+1, pc, loco))
					return 1;
			}
			break;
		default:
			break;

	}
	
	return 0;
}

int in_check(GameState* gs, char color){
	SquareCoord kpos;
	int i,pc;

	if(color == WHITE){
		wprintf(L"Checking for White king\n");
		kpos = gs->wking;
		pc = -1;
	}else if(color == BLACK){
		wprintf(L"Checking for Black king\n");
		kpos = gs->bking;
		pc = 1;
	}

	for(i = 1; i < 6; ++i){
		if(is_square_attacked(gs, kpos, i*pc, NULL))
			return 1;
	}

	return 0;
}

void print_game_state(GameState* gs){
	wprintf(L"W_CHECK= %d\nB_CHECK= %d\n", gs->wcheck, gs->bcheck);
}
