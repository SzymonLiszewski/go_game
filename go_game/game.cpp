#include<stdio.h>
#include"conio2.h"
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define _CRT_SECURE_NO_WARNINGS

//coordinates
#define BOARD_POSITION_X 60
#define BOARD_POSITION_Y 5
#define MENU_POSITION_X 1
#define MENU_POSITION_Y 1

//board state
#define EMPTY 0
#define WHITE_STONE 1
#define BLACK_STONE 2
#define KO_WHITE 3
#define KO_BLACK 4

//keys
#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4b
#define ARROW_RIGHT 0x4d
#define ENTER 0x0d
#define ESC 0x1b
#define BACKSPACE '\b'

struct game_t {
	int size;
	int** board;
	int** chain;			//stores recently found chain 1-stone in chain, 0-empty or not in chain
	int on_move;
	int score[3];
	int start_x;			//if board is too big - displaying from start_x,start_y instead of 0,0
	int start_y;
};

void draw_board(struct game_t* game, int pos_x, int pos_y) {
	textcolor(0);
	textbackground(BROWN);
	struct text_info info;
	gettextinfo(&info);
	int x = game->size, y = game->size;
	if (game->size + BOARD_POSITION_X > info.screenwidth) x = info.screenwidth - BOARD_POSITION_X + 1;
	if (game->size + BOARD_POSITION_Y > info.screenheight) y = info.screenheight - BOARD_POSITION_Y + 1;
	for (int i = game->start_y, k = 0; i < (y + game->start_y); i++, k++) {
		gotoxy(BOARD_POSITION_X, BOARD_POSITION_Y + k);
		for (int j = game->start_x; j < (x + game->start_x); j++) {
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
			else if (game->board[j][i] == WHITE_STONE) {
				textcolor(15);
				cputs("O");
				textcolor(0);
			}
			else if (game->board[j][i] == BLACK_STONE) {
				cputs("O");
			}
		}
	}
	textcolor(15);
	textbackground(0);
}
void draw_menu(struct game_t* game, int pos_x, int pos_y) {
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y);
	char tab[11][100] = { "Szymon Liszewski 193477",
	"arrows: moving the cursor over the board",
	"q : quit the program",
	"n : start a new game",
	"enter : confirm choice and end player's turn",
	"esc : cancel current action",
	"i : place a stone on the board",
	"s : save the game state",
	"l : load the game state",
	"f : finish the game",
	"implemented functionalities: a,b,c,d,e,f,g,h,i,j,k"

	};
	for (int i = 0; i < 11; i++) {
		gotoxy(MENU_POSITION_X, MENU_POSITION_Y + i);
		cputs(tab[i]);
	}
	char buffer[100], buffer2[100];
	snprintf(buffer, 100, "white: %d black: %d", game->score[WHITE_STONE], game->score[BLACK_STONE]);
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y + 12);
	cputs(buffer);
	gotoxy(MENU_POSITION_X, MENU_POSITION_Y + 13);
	snprintf(buffer2, 100, "current position: %d %d ", pos_x - BOARD_POSITION_X, pos_y - BOARD_POSITION_Y);
	cputs(buffer2);
}

void new_game(struct game_t* game) {
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			game->board[i][j] = EMPTY;
		}
	}
	game->on_move = BLACK_STONE;
	game->score[WHITE_STONE] = 0;
	game->score[BLACK_STONE] = 0;
}
void find_chain(struct game_t* game, int pos_x, int pos_y, int color) {     //stones which belong to found chain are marked as '1' in game.chain
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y;			//pos_x and pos_y are first stones of chain
	if (game->board[x][y] != EMPTY && game->board[x][y] == color) {			//color - color of chain
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
int liberties_new(struct game_t* game, int pos_x, int pos_y) {         //function return number of liberties (stones of the same color doesn't count as liberties)
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

void check_captures(struct game_t* game, int pos_x, int pos_y) {        //removin chain from board if chain liberties are 0
	int chain_size = 0;													//if there is only one stone in chain applying ko rule
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			if (game->chain[j][i] != EMPTY) chain_size++;
		}
	}
	if (chain_liberties(game) == 0) {
		for (int i = 0; i < game->size; i++) {
			for (int j = 0; j < game->size; j++) {
				if (game->chain[j][i] == 1) {
					if (game->on_move == WHITE_STONE) {
						if (chain_size == 1) game->board[j][i] = KO_BLACK;
						else game->board[j][i] = EMPTY;
						game->score[WHITE_STONE] += 1;
					}
					else if (game->on_move == BLACK_STONE) {
						if (chain_size == 1) game->board[j][i] = KO_WHITE;
						else game->board[j][i] = EMPTY;
						game->score[BLACK_STONE] += 1;
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
void capture(struct game_t* game, int pos_x, int pos_y) {					//checking if there is posibility of capturing on squares next to recently placed stone
	int x = pos_x - BOARD_POSITION_X, y = pos_y - BOARD_POSITION_Y, color;
	if (game->board[x][y] == WHITE_STONE) color = BLACK_STONE;
	else if (game->board[x][y] == BLACK_STONE) color = WHITE_STONE;
	else color = EMPTY;
	if (x < game->size - 1) {
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
	if ((game->on_move == WHITE_STONE && game->board[x][y] == KO_WHITE) || (game->on_move == BLACK_STONE && game->board[x][y] == KO_BLACK)) {
		return 0;
	}
	game->board[x][y] = game->on_move;
	capture(game, pos_x, pos_y);
	find_chain(game, pos_x, pos_y, game->on_move);
	lib = chain_liberties(game);
	for (int i = 0; i < game->size; i++) {
		for (int j = 0; j < game->size; j++) {
			game->chain[j][i] = EMPTY;
		}
	}
	game->board[x][y] = color;
	return lib;
}

void change_ko(struct game_t* game) {
	if (game->on_move == WHITE_STONE) {
		for (int i = 0; i < game->size; i++) {
			for (int j = 0; j < game->size; j++) {
				if (game->board[i][j] == KO_WHITE) {
					game->board[i][j] = EMPTY;
				}
			}
		}
	}
	else if (game->on_move == BLACK_STONE) {
		for (int i = 0; i < game->size; i++) {
			for (int j = 0; j < game->size; j++) {
				if (game->board[i][j] == KO_BLACK) {
					game->board[i][j] = EMPTY;
				}
			}
		}
	}
}

int placement(struct game_t* game, int* pos_x, int* pos_y) {			//place stone; return 1 when stone is placed, 0 when can't place stone
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
	int previous_score[3] = { 0,game->score[WHITE_STONE],game->score[BLACK_STONE] };
	if (can_place(game, *pos_x, *pos_y) != 0) {
		if (game->on_move == WHITE_STONE && game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == EMPTY) {
			game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = WHITE_STONE;
			draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
			int key = getch();
			while (key != ESC && key != ENTER) {
				key = getch();
			}
			if (key == ESC) {										//returning to previous state if key = ESC
				for (int i = 0; i < game->size; i++) {
					for (int j = 0; j < game->size; j++) {
						game->board[i][j] = previous_board[i][j];
					}
				}
				game->score[WHITE_STONE] = previous_score[WHITE_STONE];
				draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
				return 0;
			}
			if (key == ENTER) {                                   //confirm move
				change_ko(game);
				game->on_move = BLACK_STONE;
				return 1;
			}
		}

		else if (game->on_move == BLACK_STONE && game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == EMPTY) {
			game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = BLACK_STONE;
			draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
			int key = getch();
			while (key != ENTER && key != ESC) {
				key = getch();
			}
			if (key == ENTER) {
				change_ko(game);
				game->on_move = WHITE_STONE;
				return 1;
			}
			else if (key == ESC) {
				for (int i = 0; i < game->size; i++) {
					for (int j = 0; j < game->size; j++) {
						game->board[i][j] = previous_board[i][j];
					}
				}
				game->score[BLACK_STONE] = previous_score[BLACK_STONE];
				draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
				free(previous_board);
				return 0;
			}
		}
		else {
			return 0;
		}
		free(previous_board);
		return 1;
	}
	free(previous_board);
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
		if (key == BACKSPACE && i > 0) {
			name[i - 1] = 0;
			gotoxy(wherex() - 1, wherey());
			cputs(" ");
			gotoxy(wherex() - 1, wherey());
			i--;
		}
		else {
			name[i] = key;
			putch(name[i]);
			i++;
		}
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
	fprintf(f, "%d ", game->score[WHITE_STONE]);
	fprintf(f, "%d ", game->score[BLACK_STONE]);
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
		if (key == BACKSPACE && i > 0) {
			name[i - 1] = 0;
			gotoxy(wherex() - 1, wherey());
			cputs(" ");
			gotoxy(wherex() - 1, wherey());
			i--;
		}
		else {
			name[i] = key;
			putch(name[i]);
			i++;
		}
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
	fscanf(f, "%d", &s);
	game->on_move = s;
	fscanf(f, "%d", &s);
	game->score[WHITE_STONE] = s;
	fscanf(f, "%d", &s);
	game->score[BLACK_STONE] = s;
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

void cursor(struct game_t* game,int* pos_x, int* pos_y) {											//changing background color in cursor position
	if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == WHITE_STONE) {
		textbackground(LIGHTRED);
		textcolor(15);
		cputs("O");
		textbackground(0);
	}
	else if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == BLACK_STONE) {
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
	while (*key != ENTER) {
		arrows(key, pos_x, pos_y, game);
		draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
		gotoxy(*pos_x, *pos_y);
		cursor(game, pos_x, pos_y);
		if (*key == 'i') {
			if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == EMPTY) {
				game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = BLACK_STONE;
			}
			else if (game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] == BLACK_STONE) {
				game->board[*pos_x - BOARD_POSITION_X][*pos_y - BOARD_POSITION_Y] = EMPTY;
			}
			draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
		}
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
	}
	return size;
}

void finish(struct game_t* game, char* key) {
	clrscr();
	gotoxy(1, 1);
	char buffer[100];
	snprintf(buffer, 100, "white: %d black: %d", game->score[WHITE_STONE], game->score[BLACK_STONE]);
	cputs(buffer);
	while (*key != 'n' && *key != 'q') {
		*key = getchar();
	}
}

void move(char* key, int* pos_x, int* pos_y, struct game_t* game) {
	if (*key == 0) {
		arrows(key, pos_x, pos_y, game);
	}
	else if (*key == 'i') {

		if (placement(game, pos_x, pos_y) == 1) {
			capture(game, *pos_x, *pos_y);
		}
	}
	else if (*key == 'n') {
		new_game(game);
		game->size = change_size();
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
	else if (*key == 'f') {
		finish(game, key);
	}
}

void scroll(struct game_t* game, int pos_x, int pos_y) {
	struct text_info info;
	gettextinfo(&info);
	if (pos_x > info.screenwidth) game->start_x = pos_x - info.screenwidth;
	else game->start_x = 0;
	if (pos_y > info.screenheight) game->start_y = pos_y - info.screenheight;
	else game->start_y = 0;
}

void round(struct game_t* game, int* pos_x, int* pos_y, char* key) {
	struct text_info info;
	gettextinfo(&info);
	clrscr();
	draw_menu(game, *pos_x, *pos_y);
	draw_board(game, BOARD_POSITION_X, BOARD_POSITION_Y);
	if (*pos_x > info.screenwidth && *pos_y > info.screenheight) gotoxy(info.screenwidth, info.screenheight);
	else if (*pos_x > info.screenwidth) gotoxy(info.screenwidth, *pos_y);
	else if (*pos_y > info.screenheight) gotoxy(*pos_x, info.screenheight);
	else gotoxy(*pos_x, *pos_y);
	cursor(game, pos_x, pos_y);

	*key = getch();
	move(key, pos_x, pos_y, game);
	scroll(game, *pos_x, *pos_y);

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
	struct game_t game = { size, board, chain, BLACK_STONE, {0,0}, 0,0 };
	do {
		round(&game, &pos_x, &pos_y, &key);
	} while (key != 'q');

	free(game.board);
}
