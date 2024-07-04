#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
void update_value(int key, float value, struct key_value hash_table[], int new_key);
void insert_value(int key, struct key_value hash_table[], int index);
float get_value(int key, struct key_value hash_table[]);
struct key_value *get_node(int key, struct key_value hash_table[], int index);
void insert_node(int key, struct key_value *current_node);
int board_to_key(struct board game_board);
int q_learning(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player, int num_games, struct key_value hash_table[]);
int next_available_ai(struct board game_board);
int q_learn_ai(struct board game_board, struct key_value hash_table[], int epsilon, struct player *curr_player);
void test_hash_table(struct key_value hash_table[]);
void print_key_value(struct key_value *kv);

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
    struct key_value hash_table[STATE_TABLE_SIZE];
    for(int i=0; i<10;i++){
        printf("key %d value %f \n",hash_table[i].key,hash_table[i].value);
    }
    //winner = human_game_loop2(game_board, player1, player2, curr_player);
    //winner = q_learning(game_board, player1, player2, curr_player, 1, hash_table);
    //print_winner(winner);
    //insert(42000, 100, hash_table);
    //insert(17000, 200, hash_table);
    test_hash_table(hash_table);
    //printf("Value for key 42: %d\n", search(42000, hash_table));
    //printf("Value for key 17: %d\n", search(17000, hash_table));
    return 0;
    free(player1);
    free(player2);
    // need a function to search through hash_table to find any nodes that have been added and free that memory.
}

void test_hash_table(struct key_value hash_table[]){
    float value;
    int index = 0;
    int key = 9974;
    value = get_value(key, hash_table);
    index = hash_function(key);
    printf("\n value %f index %d \n", value, index);
    print_key_value(&hash_table[index]);
    key = 19947;
    value = get_value(key, hash_table);
    index = hash_function(key);
    printf("\n value %f index %d \n", value, index);
    print_key_value(&hash_table[index]);
    key = 29920;
    value = get_value(key, hash_table);
    index = hash_function(key);
    printf("\n value %f index %d \n", value, index);
    print_key_value(&hash_table[index]);
    printf("\n Start of hash table %p \n", (void*)&hash_table[0]);
    

    printf("\nFinal Key: %d Initialized: %d Value: %f  next node %p \n", hash_table[1].key, hash_table[1].key_initialized, hash_table[1].value, (void*)(&(hash_table[1].next_node)));
    struct key_value *new_key_value = hash_table[1].next_node;
    printf("\n Next node Final Key: %d Initialized: %d Value: %f  next node %p \n", new_key_value->key, new_key_value->key_initialized, new_key_value->value, (void*)(&(new_key_value->next_node)));
    new_key_value = new_key_value->next_node;
    printf("\n Final node Final Key: %d Initialized: %d Value: %f  next node %p \n", new_key_value->key, new_key_value->key_initialized, new_key_value->value, (void*)(&(new_key_value->next_node)));
    printf("\n Final node Final Key: %d Initialized: %d Value: %f  next node %p \n", new_key_value->key, new_key_value->key_initialized, new_key_value->value, new_key_value->next_node);
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

void update_value(int key, float value, struct key_value hash_table[], int new_key) {
    int index = hash_function(key);
    if (new_key){                              // if this is a new key value pair set next node equal to NULL
        hash_table[index].next_node = NULL;    
        hash_table[index].key = key;
        hash_table[index].value = value;
    }
    else if(hash_table[index].key == key)      // if key already exists and matches what is stored in table update value
        hash_table[index].value = value;
    else{                                     // this is case where there is collision in hash table need to add or update node
        //struct key_value *node_hash_table = get_node(key, hash_table, index);
        //node_hash_table->value = value;
    }
    //hash_table[index].key = key;
    //hash_table[index].value = value;

    printf("\n added value %f to state %d in set of states \n", value, key);
}

void insert_value(int key, struct key_value hash_table[], int index){
    if(hash_table[index].key_initialized == 0){    // new value to add directly to hash_table
        hash_table[index].key = key;
        hash_table[index].value = 0.0;
        hash_table[index].next_node = NULL; 
        hash_table[index].key_initialized = 1;
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

float get_value(int key, struct key_value hash_table[]) {
    int index = hash_function(key);
    if (hash_table[index].key == key) {
        return hash_table[index].value;
    }
    else if(hash_table[index].key_initialized == 0){    // need to initialize key value
        insert_value(key, hash_table, index);
        return hash_table[index].value;
    }
    else{         // have a collision need to see if node exists, if not add it. 
        struct key_value *current_node = get_node(key, hash_table, index);
        if(current_node->key != key){    // node needs to be added already existed so return value
            insert_node(key, current_node);
            printf("\n in get_value collision Key: %d Initialized: %d Value: %f next node: %p  \n", current_node->key, current_node->key_initialized, current_node->value, (void*)current_node->next_node);
            current_node = current_node->next_node;
        }
        return current_node->value;
    }
    // next 
    // Handle collisions (e.g., search through linked list)
    //update_value(key, 0, hash_table, 1);  // initiate value equal to 0
    return -1; // Not found
}

struct key_value *get_node(int key, struct key_value hash_table[], int index){
    
    struct key_value *current_node;
    struct key_value *next_node;
    current_node = &hash_table[index];
    next_node = current_node->next_node;
    while( next_node != NULL){
        if (next_node->key == key)
            return next_node;                // if node with key already exists return it when found
        current_node = next_node;
        next_node = next_node->next_node;
    }
    return current_node;  // if not found return current node    
     // This is the first collision for this key so need to add new node
    //struct key_value *new_key_value;
    //new_key_value->key = key;
    //new_key_value->value = value;
    //new_key_value->next_node = NULL;
    //return new_key_value;
    


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
    printf("\n digit borad %s ", digit_board);
    int board_num = atoi(digit_board);
    return board_num;
}

int q_learning(struct board game_board, struct player *player1, struct player *player2, struct player *curr_player, int num_games, struct key_value hash_table[]){
    //float gamma;
    int epsilon = 10; // represents percent i.e. 10 = 10%
    //float alpha;
    
    for(int i =0; i < num_games; i++){   // number of learning rounds
        int game_over = 0;
        int turn_num = 0;    
        while(game_over == 0){
            int chosen_sq = 0;
            while(chosen_sq == 0){
                int valid = 0;
                if (curr_player->type == 1)
                    chosen_sq = q_learn_ai(game_board, hash_table, epsilon, curr_player);
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
                    //printf("\n Game over is %d", game_over);
                    //if (curr_player->type == 1)
                    //    update_QSA();
                    curr_player = switch_player(player1, player2, curr_player);
                }
                    
            }
        
        //printf("game over %d", game_over);
        }
    return game_over;
    }
    

}


int next_available_ai(struct board game_board){
    for(int i = 1; i < 10; i++)
        if(is_valid_move(game_board, i))
            return i;
    return -1;  // seomthing went wrong if it gets to here
}

int q_learn_ai(struct board game_board, struct key_value hash_table[], int epsilon, struct player *curr_player){
    int new_state;
    int key;
    int available_actions[9] = {0,0,0,0,0,0,0,0,0};
    float state_action_values[9] = {-2,-2,-2,-2,-2,-2,-2,-2,-2};
    int greedy_action = -1;
    float greedy_action_value = -2;
    for(int i=0; i<9; i++){
        if (isdigit(game_board.g_board[i])){
            available_actions[i] = 1;
            struct board after_state = game_board;
            update_board(&after_state, i+1, curr_player->mark);
            print_board(after_state);
            int board_int = board_to_key(after_state);
            printf("\nboard int %d \n", board_int);
            state_action_values[i] = get_value(board_int, hash_table);
            printf("\n state action values [i] is %f \n", state_action_values[i]);
            if (state_action_values[i] > greedy_action_value){
                greedy_action_value = state_action_values[i];
                greedy_action = i+1;
            }
        }
    }
    // chose action based on e-greedy
    int e_greedy_num = rand() % 100;  // generates number from 0 to 99
        if(e_greedy_num < epsilon){   // since num is 0 to 99 and epsilon will be 0 to 100 need to use less than
            printf("\nrandom move\n");
            int move = 0;
            while(move == 0){
                int random_move = rand() % 9;
                if( available_actions[random_move] == 1)
                    return random_move + 1;        // need to add 1 since random move is based on index but squares are labled 1-9
            }
        }
        else                    // we will pick greedy move
            return greedy_action;
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
