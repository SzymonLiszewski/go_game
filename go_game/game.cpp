#include<stdio.h>
#include"conio2.h"
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define SIZE 9
#define BOARD_POSITION_X 50
#define BOARD_POSITION_Y 5
#define MENU_POSITION_X 5
#define MENU_POSITION_Y 5

#define EMPTY 0
#define WHITE 1
#define BLACK 2
#define KO_WHITE 3
#define KO_BLACK 4

#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4b
#define ARROW_RIGHT 0x4d
#define ENTER 0x0d
#define ESC 0x1b

struct game_t {
	int size;
	int** board;
	int** chain;
	int on_move;
	int score[3];
};

void draw_board(struct game_t* game, int pos_x, int pos_y) {
	textcolor(0);
	textbackground(BROWN);

	for (int i = 0; i < game->size; i++) {
		gotoxy(BOARD_POSITION_X, BOARD_POSITION_Y + i);
		for (int j = 0; j < game->size; j++) {
			if (game->board[j][i] == EMPTY || game->board[j][i] == KO_WHITE || game->board[j][i] == KO_BLACK) {
				if (i == 0 && j != 0 && j != game->size - 1) putch(194);
				else if (j == 0 && i != 0 && i != game->size - 1) putch(195);
				else if (j == game->size - 1 && i != 0 && i != game->size - 1) putch(180);
				else if (i == game->size - 1 && j != 0 && j != game->size - 1) putch(193);
				else if (j == 0 && i == 0) putch(218);
				else if (j == game->size - 1 && i == 0) putch(191);
				else if (j == 0 && i == game->size - 1) putch(192);
				else if (j == game->size - 1 && i == game->size - 1) putch(217);
				else putch(197);

			}
			else if (game->board[j][i] == WHITE) {
				textcolor(15);
				cputs("O");
				textcolor(0);
			}
			else if (game->board[j][i] == BLACK) {
				cputs("O");
			}
		}
	}
	textcolor(15);
	textbackground(0);
}
void draw_menu(struct game_t* game, int pos_x, int pos_y) {
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y);
	char tab[10][100] = { "Szymon Liszewski 193477",
	"arrows: moving the cursor over the board",
	"q : quit the program",
	"n : start a new game",
	"enter : confirm choiceand end player’s turn",
	"esc : cancel current action",
	"i : place a stone on the board",
	"s : save the game state",
	"l : load the game state",
	"f : finish the game."

	};
	for (int i = 0; i < 10; i++) {
		gotoxy(MENU_POSITION_X, MENU_POSITION_Y + i);
		cputs(tab[i]);
	}
	char buffer[100], buffer2[100];
	snprintf(buffer, 100, "white: %d black: %d", game->score[WHITE], game->score[BLACK]);
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y + 10);
	cputs(buffer);
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y + 11);
	snprintf(buffer2, 100, "current position: %d %d ", pos_x - BOARD_POSITION_X, pos_y - BOARD_POSITION_Y);
	cputs(buffer2);
}

void new_game(struct game_t* game) {
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			game->board[i][j] = EMPTY;
		}
	}
	game->on_move = BLACK;
	game->score[WHITE] = 0;
	game->score[BLACK] = 0;
}
void find_chain(struct game_t* game, int pos_x, int pos_y, int color) {
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y;
	if (game->board[x][y] != EMPTY && game->board[x][y] == color) {
		game->chain[x][y] = 1;
		if (x < game->size - 1 && game->board[x + 1][y] == color) {
			if (game->chain[x + 1][y] == 0) {
				game->chain[x + 1][y] = 1;
				find_chain(game, pos_x + 1, pos_y, color);
			}
		}
		if (x != 0 && game->board[x - 1][y] == color) {
			if (game->chain[x - 1][y] == 0) {
				game->chain[x - 1][y] = 1;
				find_chain(game, pos_x - 1, pos_y, color);
			}
		}
		if (y != game->size - 1 && game->board[x][y + 1] == color) {
			if (game->chain[x][y + 1] == 0) {
				game->chain[x][y + 1] = 1;
				find_chain(game, pos_x, pos_y + 1, color);
			}
		}
		if (y != 0 && game->board[x][y - 1] == color) {
			if (game->chain[x][y - 1] == 0) {
				game->chain[x][y - 1] = 1;
				find_chain(game, pos_x, pos_y - 1, color);
			}
		}
	}
}
int liberties_new(struct game_t* game, int pos_x, int pos_y) {
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y;
	int liberties = 0;
	if ((x != game->size - 1) && (game->board[x + 1][y] == EMPTY || game->board[x + 1][y] == KO_BLACK || game->board[x + 1][y] == KO_WHITE)) {
		liberties += 1;
	}
	if ((x != 0) && (game->board[x - 1][y] == EMPTY || game->board[x - 1][y] == KO_BLACK || game->board[x - 1][y] == KO_WHITE)) {
		liberties += 1;
	}
	if ((y != game->size - 1) && (game->board[x][y + 1] == EMPTY || game->board[x][y + 1] == KO_BLACK || game->board[x][y + 1] == KO_WHITE)) {
		liberties += 1;
	}
	if ((y != 0) && (game->board[x][y - 1] == EMPTY || game->board[x][y - 1] == KO_BLACK || game->board[x][y - 1] == KO_WHITE)) {
		liberties += 1;
	}
	return liberties;
}
int chain_liberties(struct game_t* game) {
	int liberties = 0;
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			if (game->chain[j][i] == 1) {
				liberties += liberties_new(game, j + BOARD_POSITION_X, i + BOARD_POSITION_Y);
			}
		}
	}
	return liberties;
}

void check_captures(struct game_t* game, int pos_x, int pos_y) {
	int chain_size = 0;
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			if (game->chain[j][i] != EMPTY) chain_size++;
		}
	}
	if (chain_liberties(game) == 0) {
		for (int i = 0; i < game->size; i++) {
			for (int j = 0; j < game->size; j++) {
				if (game->chain[j][i] == 1) {
					if (game->on_move == WHITE) {
						if (chain_size == 1) game->board[j][i] = KO_BLACK;
						else game->board[j][i] = EMPTY;
						game->score[WHITE] += 1;
					}
					else if (game->on_move == BLACK) {
						if (chain_size == 1) game->board[j][i] = KO_WHITE;
						else game->board[j][i] = EMPTY;
						game->score[BLACK] += 1;
					}
					game->chain[j][i] = EMPTY;
				}
			}
		}
	}
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			game->chain[j][i] = EMPTY;
		}
	}
}
void capture(struct game_t* game, int pos_x, int pos_y) {
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y, color;
	if (game->board[x][y] == WHITE) color = BLACK;
	else if (game->board[x][y] == BLACK) color = WHITE;
	else color = EMPTY;
	if (x < game->size - 1) {                                       //przeniesc ify do find_chain
		find_chain(game, pos_x + 1, pos_y, color);
		check_captures(game, pos_x, pos_y);
	}
	if (x > 0) {
		find_chain(game, pos_x - 1, pos_y, color);
		check_captures(game, pos_x, pos_y);
	}
	if (y < game->size - 1) {
		find_chain(game, pos_x, pos_y + 1, color);
		check_captures(game, pos_x, pos_y);
	}
	if (y > 0) {
		find_chain(game, pos_x, pos_y - 1, color);
		check_captures(game, pos_x, pos_y);
	}
}

int can_place(struct game_t* game, int pos_x, int pos_y) {             //checking if the stone can be placed (need at least one liberty after placement, checking for ko first, then checking for captures)
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y, color = game->board[x][y];
	int lib = 0;
	if ((game->on_move == WHITE && game->board[x][y] == KO_WHITE) || (game->on_move == BLACK && game->board[x][y] == KO_BLACK)) {
		return 0;
	}
	game->board[x][y] = game->on_move;
	capture(game, pos_x, pos_y);
	find_chain(game, pos_x, pos_y, game->on_move);
	lib = chain_liberties(game);
	game->board[x][y] = color;
	return lib;
}

void change_ko(struct game_t* game) {
	if (game->on_move == WHITE) {
		for (int i = 0; i < game->size; i++) {
			for (int j = 0; j < game->size; j++) {
				if (game->board[i][j] == KO_WHITE) {
					game->board[i][j] = EMPTY;
				}
			}
		}
	}
	else if (game->on_move == BLACK) {
		for (int i = 0; i < game->size; i++) {
			for (int j = 0; j < game->size; j++) {
				if (game->board[i][j] == KO_BLACK) {
					game->board[i][j] = EMPTY;
				}
			}
		}
	}
}

int placement(struct game_t* game, int* pos_x, int* pos_y) {
	int* ptr, ** previous_board;
	previous_board = (int**)malloc(sizeof(int*) * game->size + sizeof(int) * game->size * game->size);

	ptr = (int*)(previous_board + game->size);
	for (int i = 0; i < game->size; i++) {
		previous_board[i] = (ptr + game->size * i);
	}

	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			previous_board[i][j] = game->board[i][j];
		}
	}
	int previous_score[3] = { 0,game->score[WHITE],game->score[BLACK] };
	if (can_place(game, *pos_x, *pos_y) != 0) {
		if (game->on_move == WHITE && game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == EMPTY) {
			game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = WHITE;
			draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
			int key = getch();
			while (key != ESC && key != ENTER) {
				key = getch();
			}
			if (key == ESC) {
				for (int i = 0; i < game->size; i++) {
					for (int j = 0; j < game->size; j++) {
						game->board[i][j] = previous_board[i][j];
					}
				}
				game->score[WHITE] = previous_score[WHITE];
				draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
				return 0;
			}
			if (key == ENTER) {
				change_ko(game);
				game->on_move = BLACK;
				return 1;
			}
		}

		else if (game->on_move == BLACK && game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == EMPTY) {
			game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = BLACK;
			draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
			int key = getch();
			while (key != ENTER && key != ESC) {
				key = getch();
			}
			if (key == ENTER) {
				change_ko(game);
				game->on_move = WHITE;
				return 1;
			}
			else if (key == ESC) {
				for (int i = 0; i < game->size; i++) {
					for (int j = 0; j < game->size; j++) {
						game->board[i][j] = previous_board[i][j];
					}
				}
				game->score[BLACK] = previous_score[BLACK];
				draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
				return 0;
			}
		}
		else {
			return 0;
		}
		return 1;
	}
	return 0;
}

void save(struct game_t* game) {
	FILE* f;
	char name[100] = { 0 }, buffer[100] = { 0 };
	int key, i = 0;
	clrscr();
	gotoxy(1, 1);
	cputs("Write name of the file: ");
	key = getch();
	while (key != ENTER) {
		putch(key);
		name[i] = key;
		i++;
		key = getch();
	}
	snprintf(buffer, sizeof(name), "%s.txt", name);
	f = fopen(buffer, "w");
	fprintf(f, "%d ", game->size);
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			fprintf(f, "%d ", game->board[i][j]);
		}
	}
	fprintf(f, "%d ", game->on_move);
	fprintf(f, "%d ", game->score[WHITE]);
	fprintf(f, "%d ", game->score[BLACK]);
	fclose(f);
}

void load(struct game_t* game) {
	int s;
	FILE* f;
	char name[100] = { 0 }, buffer[100] = { 0 };
	int key, i = 0;
	clrscr();
	gotoxy(1, 1);
	cputs("Write name of the file: ");
	key = getch();
	while (key != ENTER) {
		putch(key);
		name[i] = key;
		i++;
		key = getch();
	}
	snprintf(buffer, sizeof(name), "%s.txt", name);
	f = fopen(buffer, "r");
	fscanf(f, "%d", &s);
	game->size = s;
	free(game->board);
	game->board = (int**)malloc(sizeof(int*) * game->size + sizeof(int) * game->size * game->size);
	game->chain = (int**)malloc(sizeof(int*) * game->size + sizeof(int) * game->size * game->size);

	int* ptr = (int*)(game->board + game->size);
	int* ptr2 = (int*)(game->chain + game->size);

	for (int i = 0; i < game->size; i++) {
		game->board[i] = (ptr + game->size * i);
		game->chain[i] = (ptr2 + game->size * i);
	}

	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			game->board[i][j] = 0;
			game->chain[i][j] = 0;
		}
	}

	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			fscanf(f, "%d", &s);
			game->board[i][j] = s;
		}
	}
	//printf("%d", game->board[0][0]);
	fscanf(f, "%d", &s);
	game->on_move = s;
	fscanf(f, "%d", &s);
	game->score[WHITE] = s;
	fscanf(f, "%d", &s);
	game->score[BLACK] = s;
	fclose(f);
}
void arrows(char* key, int* pos_x, int* pos_y, struct game_t* game) {
	*key = getch();
	if (*key == ARROW_UP) {
		(*pos_y) -= 1;
		if (*pos_y == BOARD_POSITION_Y - 1) {
			*pos_y = BOARD_POSITION_Y + game->size - 1;
		}
	}
	else if (*key == ARROW_DOWN) (*pos_y) = (*pos_y + 1 - BOARD_POSITION_Y) % (game->size) + BOARD_POSITION_Y;
	else if (*key == ARROW_LEFT) {
		(*pos_x) -= 1;
		if (*pos_x == BOARD_POSITION_X - 1) {
			*pos_x = BOARD_POSITION_X + game->size - 1;
		}
	}
	else if (*key == ARROW_RIGHT) (*pos_x) = (*pos_x + 1 - BOARD_POSITION_X) % (game->size) + BOARD_POSITION_X;
}

void game_state_editor(struct game_t* game, char* key, int* pos_x, int* pos_y) {
	clrscr();
	draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y);
	cputs("arrows: move");
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y + 1);
	cputs("i: put/delete stone");
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y + 2);
	cputs("enter: confirm and start game");
	gotoxy(*pos_x, *pos_y);
	//key = getch();
	while (*key != ENTER) {
		//key = getch();
		arrows(key, pos_x, pos_y, game);
		gotoxy(*pos_x, *pos_y);
		if (*key == 'i') {
			if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == EMPTY) {
				game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = BLACK;
			}
			else if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == BLACK) {
				game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = EMPTY;
			}
			draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
		}
	}

}

void move(char* key, int* pos_x, int* pos_y, struct game_t* game) {
	if (*key == 0) {
		arrows(key, pos_x, pos_y, game);
	}
	else if (*key == 'i') {
		//placement(game, pos_x, pos_y);

		if (placement(game, pos_x, pos_y) == 1) {
			capture(game, *pos_x, *pos_y);
		}
		/*else {
			//placement(game, *pos_x, *pos_y);                 //???????
		}*/
	}
	else if (*key == 'n') {
		new_game(game);
	}
	else if (*key == 's') {
		save(game);
	}
	else if (*key == 'l') {
		load(game);
	}
	else if (*key == 'e') {
		int start_game = 0;
		for (int i = 0; i < game->size; i++) {
			for (int j = 0; j < game->size; j++) {
				if (game->board[j][i] != 0)
					start_game = 1;
			}
		}
		if (start_game == 0) game_state_editor(game, key, pos_x, pos_y);
	}
}

void round(struct game_t* game, int* pos_x, int* pos_y, char* key) {

	clrscr();
	draw_menu(game, *pos_x, *pos_y);
	draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
	gotoxy(*pos_x, *pos_y);

	if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == WHITE) {                  //changing background color
		textbackground(LIGHTRED);
		textcolor(15);
		cputs("O");
		textbackground(0);
	}
	else if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == BLACK) {
		textbackground(LIGHTRED);
		textcolor(0);
		cputs("O");
		textbackground(0);
		textcolor(15);
	}
	else {
		textbackground(LIGHTRED);
		cputs(" ");
		textbackground(0);
	}

	*key = getch();
	move(key, pos_x, pos_y, game);


}
void test_func(int size, struct game_t* game) {
	game->board[2][3] = 8;
	game->board[3][3] = 5;
	//board[0][0] = 1;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%d ", game->board[i][j]);
		}
		printf("\n");
	}
}

int change_size() {
	int key;
	int s[100], i = 0, size = 0;
	clrscr();
	gotoxy(1, 1);
	cputs("choose board size");
	gotoxy(1, 2);
	cputs("1: 9x9");
	gotoxy(1, 3);
	cputs("2: 13x13");
	gotoxy(1, 4);
	cputs("3: 19x19");
	gotoxy(1, 5);
	cputs("4: custom size");
	key = getch();
	while (key != '1' && key != '2' && key != '3' && key != '4') {
		key = getch();
	}
	if (key == '1') return 9;
	else if (key == '2') return 13;
	else if (key == '3') return 19;
	else if (key == '4') {
		struct text_info info;
		gettextinfo(&info);
		clrscr();
		gotoxy(1, 1);
		cputs("enter size of the board: ");
		while (key != ENTER) {
			key = getche();
			if (key != ENTER) {
				s[i] = key;
				i++;
			}
		}
		for (int j = 0; j < i; j++) {
			size += ((int)s[i - j - 1] - 48) * pow(10, (double)j);
		}
		return size;
	}
}

int main() {
#ifndef __cplusplus
	Conio2_Init();
#endif
	settitle("First name, Last Name, Student number");
	_setcursortype(_NOCURSOR);

	int size = change_size();
	int* ptr, * ptr2, ** board, ** chain;
	board = (int**)malloc(sizeof(int*) * size + sizeof(int) * size * size);
	chain = (int**)malloc(sizeof(int*) * size + sizeof(int) * size * size);

	ptr = (int*)(board + size);
	ptr2 = (int*)(chain + size);
	for (int i = 0; i < size; i++) {
		board[i] = (ptr + size * i);
		chain[i] = (ptr2 + size * i);
	}

	int pos_x = BOARD_POSITION_X + 2, pos_y = BOARD_POSITION_Y + 2;
	char key = 0;

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			board[i][j] = 0;
			chain[i][j] = 0;
		}
	}
	//board[1][1] = WHITE;
	//board[2][2] = BLACK;
	struct game_t game = { size, board, chain, BLACK, {0,0} };
	//test_func(size, &game);
	do {
		round(&game, &pos_x, &pos_y, &key);
	} while (key != 'q');

	/*for (int i = 0; i < size; i++) {
		free(board[i]);
	}*/
	//free(board);
}
