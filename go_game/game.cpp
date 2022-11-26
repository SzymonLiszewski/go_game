#include<stdio.h>
#include"conio2.h"
#include<iostream>

#define SIZE 19
#define BOARD_POSITION_X 100	
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
	int board[SIZE][SIZE];
	int chain[SIZE][SIZE];
	int on_move;
	int score[3];
};

void draw_board(int board[][SIZE], int pos_x, int pos_y) {
	textcolor(0);
	textbackground(GREEN);

	for (int i = 0; i < SIZE; i++) {
		gotoxy(BOARD_POSITION_X, BOARD_POSITION_Y + i);
		for (int j = 0; j < SIZE; j++) {
			if (board[j][i] == EMPTY || board[j][i] == KO_WHITE || board[j][i] == KO_BLACK) {
				if (i == 0 && j != 0 && j != SIZE - 1) putch(194);
				else if (j == 0 && i != 0 && i != SIZE - 1) putch(195);
				else if (j == SIZE - 1 && i != 0 && i != SIZE - 1) putch(180);
				else if (i == SIZE - 1 && j != 0 && j != SIZE - 1) putch(193);
				else if (j == 0 && i == 0) putch(218);
				else if (j == SIZE - 1 && i == 0) putch(191);
				else if (j == 0 && i == SIZE - 1) putch(192);
				else if (j == SIZE - 1 && i == SIZE - 1) putch(217);
				else putch(197);

			}
			else if (board[j][i] == WHITE) {
				textcolor(15);
				cputs("O");
				textcolor(0);
			}
			else if (board[j][i] == BLACK) {
				cputs("O");
			}
		}
	}
	textcolor(15);
	textbackground(0);
}
void draw_menu() {
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
}

void new_game(game_t* game) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			game->board[i][j] = EMPTY;
		}
	}
	game->on_move = WHITE;
	game->score[WHITE] = 0;
	game->score[BLACK] = 0;
}
void find_chain(game_t* game, int pos_x, int pos_y, int color) {
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y;
	if (game->board[x][y] != EMPTY && game->board[x][y] == color) {
		game->chain[x][y] = 1;
		if (x != SIZE - 1 && game->board[x + 1][y] == color) {
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
		if (y != SIZE - 1 && game->board[x][y + 1] == color) {
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
int liberties_new(game_t* game, int pos_x, int pos_y) {
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y;
	int liberties = 0;
	if ((game->board[x + 1][y] == EMPTY && x != SIZE - 1) || game->board[x + 1][y] == KO_BLACK || game->board[x + 1][y] == KO_WHITE) {
		liberties += 1;
	}
	if ((game->board[x - 1][y] == EMPTY && x != 0) || game->board[x - 1][y] == KO_BLACK || game->board[x - 1][y] == KO_WHITE) {
		liberties += 1;
	}
	if ((game->board[x][y + 1] == EMPTY && y != SIZE - 1) || game->board[x][y + 1] == KO_BLACK || game->board[x][y + 1] == KO_WHITE) {
		liberties += 1;
	}
	if ((game->board[x][y - 1] == EMPTY && y != 0) || game->board[x][y - 1] == KO_BLACK || game->board[x][y - 1] == KO_WHITE) {
		liberties += 1;
	}
	return liberties;
}
int chain_liberties(game_t* game) {
	int liberties = 0;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (game->chain[j][i] == 1) {
				liberties += liberties_new(game, j + BOARD_POSITION_X, i + BOARD_POSITION_Y);
			}
		}
	}
	return liberties;
}

void check_captures(game_t* game, int pos_x, int pos_y) {
	if (chain_liberties(game) == 0) {
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				if (game->chain[j][i] == 1) {
					if (game->on_move == WHITE) game->board[j][i] = KO_BLACK;
					else if (game->on_move == BLACK) game->board[j][i] = KO_WHITE;
					game->chain[j][i] = EMPTY;
				}
			}
		}
	}
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			game->chain[j][i] = EMPTY;
		}
	}
}
void capture(game_t* game, int pos_x, int pos_y) {
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y, color;
	if (game->board[x][y] == WHITE) color = BLACK;
	else if (game->board[x][y] == BLACK) color = WHITE;
	else color = EMPTY;
	find_chain(game, pos_x + 1, pos_y, color);
	check_captures(game, pos_x, pos_y);
	find_chain(game, pos_x - 1, pos_y, color);
	check_captures(game, pos_x, pos_y);
	find_chain(game, pos_x, pos_y + 1, color);
	check_captures(game, pos_x, pos_y);
	find_chain(game, pos_x, pos_y - 1, color);
	check_captures(game, pos_x, pos_y);
}

int can_place(game_t* game, int pos_x, int pos_y) {             //checking if the stone can be placed (need at least one liberty after placement, checking for ko first, then checking for captures)
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y, color = game->board[x][y];
	int lib = 0;
	if ((game->on_move == WHITE && game->board[x][y] == KO_WHITE) || (game->on_move == BLACK && game->board[x][y] == KO_BLACK)) {
		return 0;
	}
	game->board[x][y] = game->on_move;
	capture(game, pos_x, pos_y);
	lib = liberties_new(game, pos_x, pos_y);
	game->board[x][y] = color;
	return lib;
}

void change_ko(game_t* game) {
	if (game->on_move == WHITE) {
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				if (game->board[i][j] == KO_WHITE) {
					game->board[i][j] = EMPTY;
				}
			}
		}
	}
	else if (game->on_move == BLACK) {
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				if (game->board[i][j] == KO_BLACK) {
					game->board[i][j] = EMPTY;
				}
			}
		}
	}
}

int placement(game_t* game, int pos_x, int pos_y) {
	game_t previous_state = *game;
	if (can_place(game, pos_x, pos_y) != 0) {
		if (game->on_move == WHITE && game->board[pos_x - BOARD_POSITION_X][pos_y - BOARD_POSITION_Y] == EMPTY) {
			game->board[pos_x - BOARD_POSITION_X][pos_y - BOARD_POSITION_Y] = WHITE;
			draw_board(game->board, BOARD_POSITION_X, BOARD_POSITION_Y);
			int key = getch();
			while (key != ESC && key != ENTER) {
				key = getch();
			}
			if (key == ESC) {
				*game = previous_state;
				draw_board(game->board, BOARD_POSITION_X, BOARD_POSITION_Y);
				return 0;
			}
			if (key == ENTER) {
				change_ko(game);
				game->on_move = BLACK;
				return 1;
			}

		}
		else if (game->on_move == BLACK && game->board[pos_x - BOARD_POSITION_X][pos_y - BOARD_POSITION_Y] == EMPTY) {
			game->board[pos_x - BOARD_POSITION_X][pos_y - BOARD_POSITION_Y] = BLACK;
			draw_board(game->board, BOARD_POSITION_X, BOARD_POSITION_Y);
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
				*game = previous_state;
				draw_board(game->board, BOARD_POSITION_X, BOARD_POSITION_Y);
				return 0;
			}
		}
		else {
			return 0;
		}
		return 1;
	}
}

void save(game_t* game) {
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
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			fprintf(f, "%d ", game->board[i][j]);
		}
	}
	fprintf(f, "%d ", game->on_move);
	fprintf(f, "%d ", game->score[WHITE]);
	fprintf(f, "%d ", game->score[BLACK]);
	fclose(f);
}

void load(game_t* game) {
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
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			fscanf(f, "%d", &s);
			game->board[i][j] = s;
		}
	}
	printf("%d", game->board[0][0]);
	fscanf(f, "%d", &s);
	game->on_move = s;
	fscanf(f, "%d", &s);
	game->score[WHITE] = s;
	fscanf(f, "%d", &s);
	game->score[BLACK] = s;
	fclose(f);
}

void move(char key, int* pos_x, int* pos_y, game_t* game) {
	if (key == 0) {
		key = getch();
		if (key == ARROW_UP) {
			(*pos_y) -= 1;
			if (*pos_y == BOARD_POSITION_Y - 1) {
				*pos_y = BOARD_POSITION_Y + SIZE - 1;
			}
		}
		else if (key == ARROW_DOWN) (*pos_y) = (*pos_y + 1 - BOARD_POSITION_Y) % (SIZE)+BOARD_POSITION_Y;
		else if (key == ARROW_LEFT) {
			(*pos_x) -= 1;
			if (*pos_x == BOARD_POSITION_X - 1) {
				*pos_x = BOARD_POSITION_X + SIZE - 1;
			}
		}
		else if (key == ARROW_RIGHT) (*pos_x) = (*pos_x + 1 - BOARD_POSITION_X) % (SIZE)+BOARD_POSITION_X;
	}
	else if (key == 'i') {
		if (placement(game, *pos_x, *pos_y) == 1) {
			capture(game, *pos_x, *pos_y);

			//check_captures(game, *pos_x, *pos_y);
		}
		/*else {
			//placement(game, *pos_x, *pos_y);                 //???????
		}*/
	}
	else if (key == 'n') {
		new_game(game);
	}
	else if (key == 's') {
		save(game);
	}
	else if (key == 'l') {
		load(game);
	}
	else if (key == ENTER) {
		//
	}
	else if (key == ESC) {
		//
	}
}

void round(game_t* game, int* pos_x, int* pos_y, int* key) {

	clrscr();
	draw_menu();
	draw_board(game->board, BOARD_POSITION_X, BOARD_POSITION_Y);
	printf("%d %d", game->score[WHITE], game->score[BLACK]);             //!!!!!!
	gotoxy(*pos_x, *pos_y);
	*key = getch();
	move(*key, pos_x, pos_y, game);

}

int main() {
#ifndef __cplusplus
	Conio2_Init();
#endif
	settitle("First name, Last Name, Student number");

	int pos_x = BOARD_POSITION_X + 2, pos_y = BOARD_POSITION_Y + 2, key = 0;
	struct game_t game = { {EMPTY},{EMPTY}, WHITE, {0,0} };

	do {
		round(&game, &pos_x, &pos_y, &key);
	} while (key != 'q');
}
