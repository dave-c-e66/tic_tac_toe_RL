#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "test_framework.h"

#define STATE_TABLE_SIZE 9973  // larger then optimal, use prime number to reduce colisions

struct board {
    char  g_board[10];
};

struct player {
    char name[20];
    char *mark;  // will either be X or O
    int player_num;
    int type;  // 0 human, 1 Qlearning AI, 2, random AI
};

struct key_value {
    int key;
    float value;
    struct key_value* next_node;
    int key_initialized;
};

void print_board(struct board game_board);
struct player * setup_player(char *mark, int player_num);
int is_valid_move(struct board game_board, int chosen_sq);
void update_board(struct board * game_board, int chosen_sq, char *player_mark);
int is_game_over(struct board game_board, int turn_num);
struct player * switch_player(struct player *player1, struct player *player2, struct player *curr_player);
void print_winner(int winner);
int human_game_loop(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player);
int human_game_loop2(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player);
int hash_function(int key);
void update_value(int key, float value, struct key_value state_action_table[]);
void insert_value(int key, struct key_value state_action_table[], int index);
float get_value(int key, struct key_value state_action_table[]);
struct key_value *get_node(int key, struct key_value state_action_table[], int index);
void insert_node(int key, struct key_value *current_node);
int board_to_key(struct board game_board);
int q_learn_main_loop(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player, int num_games, struct key_value state_action_table[]);
int next_available_ai(struct board game_board);
int q_learn_ai_move(struct board game_board, struct key_value state_action_table[], int epsilon, struct player *curr_player);
void test_state_action_table(struct key_value state_action_table[]);
void print_key_value(struct key_value *kv);
void update_QSA(struct board game_board, struct key_value state_action_table[], int last_state_key, int return_val, struct player *curr_player);
void get_available_actions(struct board game_board, int *available_actions);
int get_max_qsa(struct board game_board, struct key_value state_action_table[], struct player *curr_player);
int test_get_available_actions();
int test_get_max_qsa();
int test_update_QSA();
int test_q_learn_ai_move();
int test_functions();

int main() {
    int winner = 0;    
    struct board game_board = {"333333333"};
    //print_board(game_board);
    struct player *player1;
    player1 = setup_player("X", 1);
    struct player *player2;
    player2 = setup_player("O", 2);
    struct player *curr_player;
    curr_player = player1;
    //printf("players name is %s", player1->name);
    struct key_value state_action_table[STATE_TABLE_SIZE];
    for(int i=0; i<10;i++){
        printf("key %d value %f \n",state_action_table[i].key,state_action_table[i].value);
    }
    test_functions();
    //winner = human_game_loop2(game_board, player1, player2, curr_player);
    //winner = q_learn_main_loop(game_board, player1, player2, curr_player, 1, state_action_table);
    //print_winner(winner);
    //test_state_action_table(state_action_table);
    return 0;
    free(player1);
    free(player2);
    // need a function to search through state_action_table to find any nodes that have been added and free that memory.
}

void test_state_action_table(struct key_value state_action_table[]){
    float value;
    int index = 0;
    int key = 9974;
    value = get_value(key, state_action_table);
    index = hash_function(key);
    printf("\n value %f index %d \n", value, index);
    print_key_value(&state_action_table[index]);
    key = 19947;
    value = get_value(key, state_action_table);
    index = hash_function(key);
    printf("\n value %f index %d \n", value, index);
    print_key_value(&state_action_table[index]);
    key = 29920;
    value = get_value(key, state_action_table);
    index = hash_function(key);
    printf("\n value %f index %d \n", value, index);
    print_key_value(&state_action_table[index]);
    printf("\n Start of hash table %p \n", (void*)&state_action_table[0]);
    update_value(key=9974,1.3,state_action_table);
    update_value(key=19947,2.4,state_action_table);
    update_value(key=29920,3.5,state_action_table);

    printf("\nFinal Key: %d Initialized: %d Value: %f  next node %p \n", state_action_table[1].key, state_action_table[1].key_initialized, state_action_table[1].value, (void*)(&(state_action_table[1].next_node)));
    struct key_value *new_key_value = state_action_table[1].next_node;
    printf("\n Next node Final Key: %d Initialized: %d Value: %f  next node %p \n", new_key_value->key, new_key_value->key_initialized, new_key_value->value, (void*)(&(new_key_value->next_node)));
    new_key_value = new_key_value->next_node;
    printf("\n Final node Final Key: %d Initialized: %d Value: %f  next node %p \n", new_key_value->key, new_key_value->key_initialized, new_key_value->value, (void*)(&(new_key_value->next_node)));
    printf("\n Final node Final Key: %d Initialized: %d Value: %f  next node %p \n", new_key_value->key, new_key_value->key_initialized, new_key_value->value, (void*)(&(new_key_value->next_node)));
    new_key_value = new_key_value->next_node;
    printf("\n Final Final node Final Key: %d Initialized: %d Value: %f  next node %p \n", new_key_value->key, new_key_value->key_initialized, new_key_value->value, (void*)(&(new_key_value->next_node)));
}

void print_key_value(struct key_value *kv){
    int next_node = (kv->next_node != NULL);
    printf("\nKey: %d Initialized: %d Value: %f next node: %d  \n", kv->key, kv->key_initialized, kv->value, next_node);
}



void print_board(struct board game_board){
    for(int i = 1; i < 10; i++){
        if (isdigit(game_board.g_board[i-1]))
            printf("|   ");
        else
            printf("| %c ",game_board.g_board[i-1]);
        if(i%3 == 0){
            printf("|\t\t| %d | %d | %d | \n", i-2,i-1,i);
        }
    }
    printf("\n");
}

struct player * setup_player( char *mark, int player_num){
    int player_type;
    struct player *plyr;
    plyr = (struct player *)malloc(sizeof(struct player));
    char name[20] = "Sam";
    plyr->player_num = player_num;
    //char name[20];
    //printf("Please enter the players name : ");
    //scanf("%19s", name); // cut off at 19 to prevent buffer overflow
    strcpy(plyr->name, name);
    plyr->mark = mark;
    printf("Which type of player is this 0=human 1=q_learn ai 2=next_sq_ai :");
    scanf("%d", &player_type);
    plyr->type = player_type;
    return plyr;
}

int is_valid_move(struct board game_board, int chosen_sq){
    int board_sq = (int)game_board.g_board[chosen_sq - 1] - (int)'0';  // get int value of square 
    if (chosen_sq < 1 || chosen_sq > 9)
        return 0;
    if (board_sq == 3)   // blank squares are 3 as board in created filled with 3's
        return 1;
    else
        return 0;
    
}

void update_board(struct board * game_board, int chosen_sq, char *player_mark){
    game_board->g_board[chosen_sq-1] = *player_mark;
    //print_board(*game_board);
    
}

/** Checks if game is over eithe because board is filled or someone has 3 in a row */
int is_game_over(struct board game_board, int turn_num){
    int winner = 0;
    char winner_mark;
    for (int i = 0; i <3; i++){     
        int sq = i*3; 
        if(game_board.g_board[sq] == game_board.g_board[sq+1] && game_board.g_board[sq] == game_board.g_board[sq+2]) // first check horizontal squares
            winner_mark = game_board.g_board[sq];
        if(game_board.g_board[i] == game_board.g_board[i+3] && game_board.g_board[i] == game_board.g_board[i+6]) // next check vertical squares
            winner_mark = game_board.g_board[i];
        }
    // finally we need to check the 2 diagonals
    if(game_board.g_board[0] == game_board.g_board[4] && game_board.g_board[0] == game_board.g_board[8])
        winner_mark = game_board.g_board[0];
    if(game_board.g_board[2] == game_board.g_board[4] && game_board.g_board[2] == game_board.g_board[6])
        winner_mark = game_board.g_board[2];
    if (winner == 0 && turn_num == 9)  // no empty spaces so game is over and no winner
        winner = 3; 
    //printf("\n winner mark is %c \n", winner_mark);
    if (winner_mark == 'X')
        winner = 1;
    else if (winner_mark == 'O')
        winner = 2;
    return winner;  
           
}

/** Switches current player by returning pointer to player which is not current player */
struct player * switch_player(struct player *player1, struct player *player2, struct player *curr_player){
    if (curr_player == player1)
        return player2;
    else
        return player1;

}

void print_winner(int winner){
    if(winner == 3)
        printf("The game is a draw/ \n");
    else
        printf("winner is Player %d \n", winner);

}

int human_game_loop(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player){
    int turn_num = 0;
    int game_over = 0;
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
            else{
                turn_num++;
                update_board(&game_board, chosen_sq, curr_player->mark);
                game_over = is_game_over(game_board, turn_num);
                //printf("\n Game over is %d", game_over);
                curr_player = switch_player(player1, player2, curr_player);
            }
                
        }
        
        //printf("game over %d", game_over);
    }
    print_board(game_board);
    return game_over;

}

int hash_function(int key) {
    return key % STATE_TABLE_SIZE;
}

void update_value(int key, float value, struct key_value state_action_table[]) {
    // get value which adds new nodes is always be run before update_value so the index will always exist
    int index = hash_function(key);
    if (state_action_table[index].key == key) {
        state_action_table[index].value = value;
    }
    else{         // have a collision need to find right node before updating value. 
        struct key_value *current_node = get_node(key, state_action_table, index);
        current_node->value = value;    
        }
        
}



void insert_value(int key, struct key_value state_action_table[], int index){
    if(state_action_table[index].key_initialized == 0){    // new value to add directly to state_action_table
        state_action_table[index].key = key;
        state_action_table[index].value = 0.0;
        state_action_table[index].next_node = NULL; 
        state_action_table[index].key_initialized = 1;
    }
    
    
}

void insert_node(int key, struct key_value *current_node){
    struct key_value *new_key_value = (struct key_value*)malloc(sizeof(struct key_value));
    new_key_value->key = key;
    new_key_value->value = 0.0;
    new_key_value->next_node = NULL;
    new_key_value->key_initialized = 1;
    current_node->next_node = new_key_value;
    //printf("\nKey: %d Initialized: %d Value: %f next node: %p  \n", current_node.key, current_node.key_initialized, current_node.value, current_node.next_node);

}

float get_value(int key, struct key_value state_action_table[]) {
    int index = hash_function(key);
    if (state_action_table[index].key == key) {
        return state_action_table[index].value;
    }
    else if(state_action_table[index].key_initialized == 0){    // need to initialize key value
        insert_value(key, state_action_table, index);
        return state_action_table[index].value;
    }
    else{         // have a collision need to see if node exists, if not add it. 
        struct key_value *current_node = get_node(key, state_action_table, index);
        if(current_node->key != key){    // node needs to be added already existed so return value
            insert_node(key, current_node);
            printf("\n in get_value collision Key: %d Initialized: %d Value: %f next node: %p  \n", current_node->key, current_node->key_initialized, current_node->value, (void*)current_node->next_node);
            current_node = current_node->next_node;
        }
        return current_node->value;
    }
}

struct key_value *get_node(int key, struct key_value state_action_table[], int index){
    
    struct key_value *current_node;
    struct key_value *next_node;
    current_node = &state_action_table[index];
    next_node = current_node->next_node;
    while( next_node != NULL){
        if (next_node->key == key)
            return next_node;                // if node with key already exists return it when found
        current_node = next_node;
        next_node = next_node->next_node;
    }
    return current_node;  // if not found return current node      

}

int board_to_key(struct board game_board){
    char digit_board[10] = "333333333";
    for (int i=0; i < 9; i++){
        if(isdigit(game_board.g_board[i])){
            digit_board[i] = game_board.g_board[i];}
        else if(game_board.g_board[i] == 'X')
            digit_board[i] = '1';
        else
            digit_board[i] = '2';
    }
    printf("\n in board to key - digit borad %s ", digit_board);
    int board_num = atoi(digit_board);
    return board_num;
}

int q_learn_main_loop(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player, int num_games, struct key_value state_action_table[]){   
    int epsilon = 10; // represents percent i.e. 10 = 10%
    for(int i =0; i < num_games; i++){   // number of learning rounds
        int game_over = 0;
        int turn_num = 0;
        int last_action = 0;
        int last_state_key = 0;  // will store board key for what board looks like after ai players last move
        while(game_over == 0){
            int chosen_sq = 0;
            while(chosen_sq == 0){
                int valid = 0;
                if (curr_player->type == 1)
                    chosen_sq = q_learn_ai_move(game_board, state_action_table, epsilon, curr_player);
                else if(curr_player->type == 2)
                    chosen_sq = next_available_ai(game_board);
                printf("chosen sdq is %d", chosen_sq);
                valid = is_valid_move(game_board, chosen_sq);
                if (valid == 0)
                    chosen_sq = 0;
                else{
                    turn_num++;
                    update_board(&game_board, chosen_sq, curr_player->mark);
                    print_board(game_board);
                    game_over = is_game_over(game_board, turn_num);                    
                    if (curr_player->type == 1)
                        last_state_key = board_to_key(game_board);
                    if (game_over == 1)  // ai won
                        update_QSA(game_board, state_action_table, last_state_key, 1, curr_player); // reward = 1
                    else if (game_over == 2)
                        update_QSA(game_board, state_action_table, last_state_key, -1, curr_player); // reward = -1
                    else if ((game_over == 3) || ((curr_player->player_num == 2) && game_over ==0))  
                        update_QSA(game_board, state_action_table, last_state_key, 0, curr_player);  // reward = 0
                    curr_player = switch_player(player1, player2, curr_player);
                }
                    
            }
        
        //printf("game over %d", game_over);
        }
    return game_over;
    }
    

}

void get_available_actions(struct board game_board, int *available_actions){
    for(int i=0; i<9; i++){
        if (isdigit(game_board.g_board[i])){
            available_actions[i] = 1;
            //printf("\n board %c ", game_board.g_board[i]);
        }
        else
            available_actions[i] = 0;
    }
}

int get_max_qsa(struct board game_board, struct key_value state_action_table[], struct player *curr_player){
    int greedy_action = -1;
    float greedy_action_value = -2;
    float state_action_values[9] = {-2,-2,-2,-2,-2,-2,-2,-2,-2};
    int available_actions[9] = {0,0,0,0,0,0,0,0,0};
    get_available_actions(game_board, available_actions);
    printf("available actions [0] %d", available_actions[0]);
    for(int i=0; i<9; i++){
        if (available_actions[i] == 1){
            struct board after_state = game_board;
            update_board(&after_state, i+1, curr_player->mark);
            print_board(after_state);
            int board_int = board_to_key(after_state);
            printf("\nboard int %d \n", board_int);
            state_action_values[i] = get_value(board_int, state_action_table);
            printf("\n state action values [i] is %f \n", state_action_values[i]);
            if (state_action_values[i] > greedy_action_value){
                greedy_action_value = state_action_values[i];
                greedy_action = i+1;
            }
        }
        else{
            printf("\nSomething has gone wrong, no available actions ");
        }
    }
    return greedy_action;
}

void update_QSA(struct board game_board, struct key_value state_action_table[], int last_state_key, int return_val, struct player *curr_player){
    float gamma = .5;
    float alpha = .10;
    float new_value = 0;
    float old_value =  get_value(last_state_key,state_action_table);
    printf("\n old value %f \n", old_value);
    int best_move = get_max_qsa(game_board, state_action_table, curr_player);
    printf("\n best move %d \n ", best_move);
    struct board max_state = game_board;
    update_board(&max_state, best_move, curr_player->mark);        
    int board_int = board_to_key(max_state);
    float max_sa = get_value(board_int, state_action_table); 
    float adjustment = return_val + (gamma * (max_sa)) - old_value;
    new_value = old_value + (alpha * adjustment);
    update_value(last_state_key, new_value, state_action_table);
}

int next_available_ai(struct board game_board){
    for(int i = 1; i < 10; i++)
        if(is_valid_move(game_board, i))
            return i;
    return -1;  // seomthing went wrong if it gets to here
}

int q_learn_ai_move(struct board game_board, struct key_value state_action_table[], int epsilon, struct player *curr_player){
    int new_state;
    int key;
    int available_actions[9] = {0,0,0,0,0,0,0,0,0};
    float state_action_values[9] = {-2,-2,-2,-2,-2,-2,-2,-2,-2};
    int greedy_action = 0;
    // chose action based on e-greedy
    int e_greedy_num = rand() % 100;  // generates number from 0 to 99
    if(e_greedy_num < epsilon){   
        printf("\nrandom move\n");
        int move = 0;
        while(move == 0){
            int random_move = rand() % 9;
            if( available_actions[random_move] == 1)
                return random_move + 1;        // need to add 1 since random move is based on index but squares are labled 1-9
        }
    }
    else{                    // we will pick greedy move
        greedy_action = get_max_qsa(game_board, state_action_table, curr_player);
        return greedy_action;
    }
    
    return -1;  // seomthing went wrong if it gets to here

}















int human_game_loop2(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player){
    int turn_num = 0;
    int game_over = 0;
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
            else{
                turn_num++;
                update_board(&game_board, chosen_sq, curr_player->mark);
                printf("\n game board num is %d \n", board_to_key(game_board));
                game_over = is_game_over(game_board, turn_num);
                //printf("\n Game over is %d", game_over);
                curr_player = switch_player(player1, player2, curr_player);
            }
                
        }
        
        //printf("game over %d", game_over);
    }
    print_board(game_board);
    return game_over;

}

int test_get_available_actions() {
    struct board game_board;
    int available_actions[9] = {0};

    // Case 1: Empty board
    strcpy(game_board.g_board, "333333333");
    get_available_actions(game_board, available_actions);
    for (int i = 0; i < 9; i++) {
        ASSERT_EQUAL(available_actions[i], 1);
        printf("%d", available_actions[i]);
    }
    
    // Case 2: Some moves made
    strcpy(game_board.g_board, "X33X33O33");
    print_board(game_board);
    int expected_actions[9] = {0, 1, 1, 0, 1, 1, 0, 1, 1};
    get_available_actions(game_board, available_actions);
    printf(" \n example 2 \n");
    for (int i = 0; i < 9; i++) {
        printf("%d", available_actions[i]);
        ASSERT_EQUAL(available_actions[i], expected_actions[i]);
    }

    return 0;
}

int test_get_max_qsa() {
    struct board game_board;
    struct key_value state_action_table[STATE_TABLE_SIZE]; // Define SIZE appropriately
    
    struct board max_state;
    struct board before_move;
    int best_move;
    float value;
    int last_state_key;
    struct player *player1;
    player1 = setup_player("X", 1);
    struct player *player2;
    player2 = setup_player("O", 2);
    struct player *curr_player;
    curr_player = player1;
    // Initialize state_action_table and game_board as needed
    // Test case
    strcpy(game_board.g_board, "333333333");
    last_state_key = board_to_key(game_board);
    value = get_value(last_state_key, state_action_table);
    float new_value = 0.0;
    
    //last_state_key = board_to_key(game_board);
    before_move = game_board;

    strcpy(game_board.g_board, "33X333333");
    last_state_key = board_to_key(game_board);
    value = get_value(last_state_key, state_action_table);
    printf("\n last state key is %d \n", last_state_key);
    new_value = 1.3;
    update_value(last_state_key, new_value, state_action_table);
    //best_move = get_max_qsa(game_board, state_action_table, &curr_player);
    //printf("\n Best move %d \n", best_move);
    new_value = get_value(last_state_key, state_action_table);
    printf("\n last state key is %d value is %f \n", last_state_key, new_value);
    strcpy(game_board.g_board, "3333X3333");
    last_state_key = board_to_key(game_board);
    value = get_value(last_state_key, state_action_table);
    new_value = 1.4;
    update_value(last_state_key, new_value, state_action_table);

    strcpy(game_board.g_board, "33X3333X3");
    last_state_key = board_to_key(game_board);
    value = get_value(last_state_key, state_action_table);
    new_value = 1.8;
    update_value(last_state_key, new_value, state_action_table);
    
    
    best_move = get_max_qsa(before_move, state_action_table, curr_player);
    printf("\n Best move %d \n", best_move);
    max_state = before_move;
    update_board(&max_state, best_move, curr_player->mark);        
    int board_int = board_to_key(max_state);
    float max_sa = get_value(board_int, state_action_table); 
    printf("\n RESULT %f  new_vale %f  ", max_sa, new_value);
    //ASSERT_EQUAL(result, new_value);


    return 0;
}

int test_update_QSA() {
    struct board game_board;
    struct key_value state_action_table[STATE_TABLE_SIZE]; // Define SIZE appropriately
       
    int last_state_key;
    int first_state_key;
    int value;
    float new_value;
    int available_actions[9] = {0};
    struct player *player1;
    player1 = setup_player("X", 1);
    struct player *player2;
    player2 = setup_player("O", 2);
    struct player *curr_player;
    curr_player = player1;
    // Initialize state_action_table and game_board as needed

    // Test case
    strcpy(game_board.g_board, "333333333");
    first_state_key = board_to_key(game_board);
    value = get_value(first_state_key, state_action_table);
    strcpy(game_board.g_board, "33X33OX33");
    last_state_key = board_to_key(game_board);
    value = get_value(first_state_key, state_action_table);
    new_value = 3.0;
    update_value(last_state_key, new_value, state_action_table); 
    strcpy(game_board.g_board, "33X333333");
    last_state_key = board_to_key(game_board);
    value = get_value(last_state_key, state_action_table);
    new_value = .4;
    get_available_actions(game_board, available_actions);
    update_value(last_state_key, new_value, state_action_table); 
    update_QSA(game_board, state_action_table, last_state_key, 1.0, curr_player);
    float updated_value = get_value(last_state_key, state_action_table);
    printf("\n updated value %f \n", updated_value);
    ASSERT_TRUE(updated_value != 0); // Check if the value was updated

    return 0;
}

int test_q_learn_ai_move() {
    struct board game_board;
    struct key_value state_action_table[STATE_TABLE_SIZE]; // Define SIZE appropriately
    struct player curr_player;
    curr_player.mark = "X";

    // Initialize state_action_table and game_board as needed

    // Test case
    strcpy(game_board.g_board, "333333333");
    int move = q_learn_ai_move(game_board, state_action_table, 50, &curr_player);
    ASSERT_TRUE(move >= 1 && move <= 9);

    return 0;
}

int test_functions(){
    //RUN_TEST(test_get_available_actions);
    //RUN_TEST(test_get_max_qsa);
    RUN_TEST(test_update_QSA);
    //RUN_TEST(test_q_learn_ai_move);
    return 0;
}

