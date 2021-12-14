#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include "chess_utils.h"

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
