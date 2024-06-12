#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct board {
    char  g_board[9];
};

struct player {
    char name[20];
    char *mark;  // will either be X or O
};

void print_board(struct board game_board);
struct player * setup_player(char *mark);
int is_valid_move(struct board game_board, int chosen_sq);
void update_board(struct board * game_board, int chosen_sq, char *player_mark);

int main() {
    int game_over = 0;
    struct board game_board = {"000000000"};
    print_board(game_board);
    struct player *player1;
    player1 = setup_player("X");
    struct player *player2;
    player2 = setup_player("O");
    //printf("players name is %s", player1->name);
    while(game_over == 0){
        int chosen_sq = 0;
        print_board(game_board);
        while(chosen_sq == 0){
            int valid = 0;
            printf("Enter which square you want (1-9) : ");
            scanf("%d", &chosen_sq);
            valid = is_valid_move(game_board, chosen_sq);
            if (valid == 0)
                chosen_sq = 0;
        }
        update_board(&game_board, chosen_sq, player2->mark);
        game_over = 1;
    }
    return 0;
}

void print_board(struct board game_board){
    for(int i = 1; i < 10; i++){
        if (game_board.g_board[i-1]=='0')
            printf("|   ");
        else
            printf("| %c ",game_board.g_board[i-1]);
        if(i%3 == 0){
            printf("|\t\t| %d | %d | %d | \n", i-2,i-1,i);
        }
    }
    printf("\n");
}

struct player * setup_player( char *mark){
    struct player *plyr;
    plyr = (struct player *)malloc(sizeof(struct player));
    char name[20] = "Sam";
    //char name[20];
    //printf("Please enter the players name : ");
    //scanf("%19s", name); // cut off at 19 to prevent buffer overflow
    strcpy(plyr->name, name);
    plyr->mark = mark;
    return plyr;
}

int is_valid_move(struct board game_board, int chosen_sq){
    if (chosen_sq < 1 || chosen_sq > 9)
        return 0;
    if ((game_board.g_board[chosen_sq - 1])=='0')
        return 1;
    else
        return 0;
    
}

void update_board(struct board * game_board, int chosen_sq, char *player_mark){
    game_board->g_board[chosen_sq-1] = *player_mark;
    print_board(*game_board);
    
}