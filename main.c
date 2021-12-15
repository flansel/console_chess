#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include "chess_utils.h"

STATUS parse_input(GameState*, char*, SquareCoord*, SquareCoord*);

STATUS parse_input(GameState* gs, char* move, SquareCoord* from, SquareCoord* to){
	int i, len, pc;
	Move current_move;

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
			if(!is_square_attacked(gs, *to, BISHOP * pc, from))
				return ILLEGAL;
			break;
		case 'n':
		case 'N':
			if(!is_square_attacked(gs, *to, KNIGHT * pc, from))
				return ILLEGAL;
			break;
		case 'k':
		case 'K':
			if(!is_square_attacked(gs, *to, KING * pc, from))
				return ILLEGAL;
			break;
		case 'q':
		case 'Q':
			if(!is_square_attacked(gs, *to, QUEEN * pc, from))
				return ILLEGAL;
			break;
		case 'r':
		case 'R':
			if(!is_square_attacked(gs, *to, ROOK * pc, from))
				return ILLEGAL;
			break;
		default:
			return ILLEGAL;
			break;

	}
	
	//Old method "e2e4 -> move from : e2, to : e4"	
	/*from->file = move[0];
	from->rank = move[1];
	from->boardc = from->file - 'a';
	from->boardr = (from->rank - '0') -1;

	to->file = move[2];
	to->rank = move[3];
	to->boardc = to->file - 'a';
	to->boardr = (to->rank - '0') -1;
	*/
	//wprintf(L"IN: %s -> From %d %d ... To %d %d\n", move, from->file, from->rank, to->file, to->rank);
	
	return LEGAL;
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
		if(!parse_input(gs, current_move, &from, &to)){
			wprintf(L"Error: ILLEGAL MOVE fnd\n");
			exit(1);
		}
		if(!check_legal(gs, from, to, error_msg)){
			wprintf(L"Error: ILLEGAL MOVE fnd\n");
			exit(1);
		}
		//set_square(gs, to, get_square(gs, from));
		//set_square(gs, from, 0);
		update_state(gs);
		print_game(gs);
		print_game_state(gs);

	}
	
	free(gs);
	
	return 0;
}
