#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#include "chess_utils.h"

STATUS parse_input(GameState*, char*, SquareCoord*, SquareCoord*);
void debug(int);

void debug(int n){
	wprintf(L"Debug bp=%d\n", n);
}

STATUS parse_input(GameState* gs, char* move, SquareCoord* from, SquareCoord* to){
	int i, len, pc, ret=-1;
	Move current_move;
	
	
	if(strcmp(move, "resign") == 0){
		gs->result = gs->turn ? 2 : 1;
		return -1;
	}
	
	
	SquareCoordList* possible_moves = (SquareCoordList*)malloc(sizeof(SquareCoordList));
	possible_moves->size = 0;

	len = strlen(move);
	to->rank = move[len-1];
	to->file = move[len-2];
	alg_to_coord(to);
	
	if(gs->turn)
		pc = 1;
	else
		pc = -1;

	//fall-throughs are used for upper and lower in all cases except Bishop and pawns
	switch(move[0]){
		case 'B':
			if(!is_square_attacked(gs, *to, BISHOP * pc, possible_moves))
				ret = ILLEGAL;
			break;
		case 'n':
		case 'N':
			if(!is_square_attacked(gs, *to, KNIGHT * pc, possible_moves))
				ret = ILLEGAL;
			break;
		case 'k':
		case 'K':
			if(!is_square_attacked(gs, *to, KING * pc, possible_moves))
				ret = ILLEGAL;
			break;
		case 'q':
		case 'Q':
			if(!is_square_attacked(gs, *to, QUEEN * pc, possible_moves))
				ret = ILLEGAL;
			break;
		case 'r':
		case 'R':
			if(!is_square_attacked(gs, *to, ROOK * pc, possible_moves))
				ret = ILLEGAL;
			break;
		default:
			if(move[1] == 'x'){
				if(!is_square_attacked(gs, *to, PAWN * pc, possible_moves)){
					ret = ILLEGAL;
					break;
				}
				for(i = 0; i < possible_moves->size; ++i){
					if(possible_moves->list[i].boardc + 'a' == move[0]){
						from->boardr = possible_moves->list[i].boardr;
						from->boardc = possible_moves->list[i].boardc;
						ret = LEGAL;
						break;
					}
				}
			}else if(gs->board[to->boardr + (pc * -1)][to->boardc] == PAWN * pc){
				from->boardr = to->boardr + (pc * -1);
				from->boardc = to->boardc;
				ret = LEGAL;
			}else if(gs->board[to->boardr + (pc * -2)][to->boardc] == PAWN * pc){
				if(!(move[1] == '4' && pc == 1) && !(move[1] == '5' && pc == -1)){
					ret = ILLEGAL;
				}
				from->boardr = to->boardr + (pc * -2);
				from->boardc = to->boardc;
				ret = LEGAL;
			}

			if(ret == -1)
				ret = ILLEGAL;

			break;
	}
	
	if(ret != -1){
		free(possible_moves);
		return ret;
	}
	
	//debug(12345);

	if(possible_moves->size == 1 && (len < 4 || move[1] == 'x')){
		from->boardr = possible_moves->list[0].boardr;
		from->boardc = possible_moves->list[0].boardc;
		ret = LEGAL;
	}else if(move[1] > '0' && move[1] < '9'){
		for(i = 0; i < possible_moves->size; ++i){
			if(possible_moves->list[i].boardr == (move[1] - '1')){
				from->boardr = possible_moves->list[i].boardr;
				from->boardc = possible_moves->list[i].boardc;
				ret = LEGAL;
				break;
			}
		}
	}else if(move[1] >= 'a' && move[1] <= 'h'){
		for(i = 0; i < possible_moves->size; ++i){
			if(possible_moves->list[i].boardc == (move[1] - 'a')){
				from->boardr = possible_moves->list[i].boardr;
				from->boardc = possible_moves->list[i].boardc;
				ret = LEGAL;
				break;
			}
		}
	}
	
	if(ret == -1){
		ret = ILLEGAL;
	}

	free(possible_moves);

	return ret;
}

int main(int argc, char** argv){
	char *locale;
	locale = setlocale(LC_ALL, "");

	GameState* gs = (GameState*)malloc(sizeof(GameState));
	init_game(gs);
	system("clear");
	print_game(gs);

	char current_move[10];
	char error_msg[30];
	SquareCoord from,to;
	int res;
	//game loop

	while(1){
		scanf("%9s", current_move);
		
		res = parse_input(gs, current_move, &from, &to);
		if(res == 0){
			wprintf(L"Error: ILLEGAL MOVE fnd, in move parser\n");
			exit(1);
		}else if(res == -1){
			wprintf(L"Player %d has resigned\n", gs->turn ? 1 : 2);
			exit(1);
		}

		if(!check_legal(gs, from, to, error_msg)){
			wprintf(L"Error: ILLEGAL MOVE fnd, in Legality check\n");
			exit(1);
		}
		//set_square(gs, to, get_square(gs, from));
		//set_square(gs, from, 0);
		update_state(gs);
		system("clear");
		print_game(gs);
		//print_game_state(gs);

	}
	
	free(gs);
	
	return 0;
}
