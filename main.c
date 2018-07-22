// a very simple snake game in for ncurses written in C

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


typedef struct{
    int x;
    int y;
} Pos;

// positions the snake to the middle of the screen with a length of 3
void startPositions( Pos* p, int max_x, int max_y){
    int size = max_x*max_y;

    for(int i = 0; i < 3; ++i){
        p->x = max_x/2 + i;
        p->y = max_y/2;
        ++p;
    }

    for(int i = 3; i < size; ++i){
        p->x = 0;
        p->y = 0;
        ++p;
    }
}
// Checks that the snakes head isn't hitting the snakes body
int checkMovement(Pos* snakePos, int size){
    int x = snakePos->x;
    int y = snakePos->y;
    
    for(int i = 0; i < size; ++i){
        snakePos++;
        if(snakePos->x == x && snakePos->y == y){
            return 0;
        }
    }
    return 1;

}

// Check for food existance in the position p
int check_food(Pos* foodPos, Pos* p){
    if(foodPos->x == p->x && foodPos->y == p->y) return 1;

    return 0;
}

// Print the positions of the snake and the food to the screen
void printPositions( Pos* snake, int size, Pos *foodPos){

    clear();
            
    for(int i = 0; i < size; ++i){
        mvprintw(snake->y, snake->x, "*");
        ++snake;
    }
    
    mvprintw(foodPos->y, foodPos->x, "F");
    
    refresh();

}

//  Checks if the moveTime variable has been passed
// if so, adjusts the moveTime and returns 1
int check_time(clock_t *moveTime, clock_t movePeriod){
    if(clock() > *moveTime) {
        *moveTime = (clock() + movePeriod);
        return 1;
    }
    return 0;
}
// checks that the dir key is a valid direction for the snake to go
int check_key(char *dir, char prevDir, int *running){
        
        // Prevent 180 degree turns
        if(*dir == 'a' && prevDir == 'd'){
            *dir = 'd';
        }
        else if(*dir == 'd' && prevDir == 'a'){
            *dir = 'a';
        }
        else if(*dir == 'w' && prevDir == 's'){
            *dir = 's';
        }
        else if(*dir == 's' && prevDir == 'w'){
            *dir = 'w';
        }

        // Prevent keys that aren't used
        if( (*dir != 'a') & (*dir != 's') & (*dir != 'd') 
                & (*dir != 'w') & (*dir != 'q')){
            *dir = prevDir;
            return 0;
        }
        if(*dir == prevDir){
            return 0;
        }
        if(*dir == 'q'){
            *running = 0;
        }
        return 1;
}

// creates a random position for the food between the maximium values
void makeFood(Pos *p, int max_x, int max_y){

    p->x = rand()%(max_x - 1) + 1;
    p->y = rand()%(max_y - 1) + 1;

}

// moves the snake, and grows it if it hits a position with food in it
int moveSnake( Pos* snakePos, int *size, Pos* foodPos, char dir, int max_x, int max_y, int* running){

    Pos temp = *snakePos, temp2;

    switch (dir){
        case 'a':
            snakePos->x--;
            break;
        case 'd':
            snakePos->x++;
            break;
        case 's':
            snakePos->y++;
            break;
        case 'w':
            snakePos->y--;
            break;
        default:
            break;
    }
         

    if(snakePos->x > max_x) snakePos->x = 0;
    if(snakePos->x < 0) snakePos->x = max_x;

    if(snakePos->y > max_y) snakePos->y = 0;
    if(snakePos->y < 0) snakePos->y = max_y;
    
    if(check_food(foodPos, snakePos)){
        *size += 1;
        foodPos->x = 0;
        foodPos->y = 0;
    }
    // Check that the snake hasn't hit itself
    if(!checkMovement(snakePos, *size)){
        *running = 0;
        sleep(3);
        return 0;
    }

    for(int i = 0; i < *size; ++i){
        ++snakePos;
        temp2 = *snakePos;
        *snakePos = temp;
        temp = temp2;
    }
    
}

// initializes the ncurses mode and gets the screen width and height
void init_all(int* max_x, int* max_y, int* pos_x, int* pos_y){

    initscr();			// Start curses mode 
    noecho();           // don't print pressed keys
    curs_set(0);         // Hide cursor

    getmaxyx(stdscr, *max_y, *max_x);
    *pos_x = *max_x/2;
    *pos_y = *max_y/2;

    mvprintw(*pos_y, (*pos_x - 6), "Move with wasd");	/* Print Hello World		  */
	refresh();			/* Print it on )to the real screen */
    
	getch();			/* Wait for user input */
    nodelay(stdscr, TRUE);
    cbreak();

}

// checks for keyboard hit
int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

//  returns the first char in the buffer and moves the later chars forward
char get_buffer(char dir_buffer[], int *buff_size){
    char dir = dir_buffer[0];
    for(int i = 0; i < *buff_size - 1; ++i){
        dir_buffer[i] = dir_buffer[i + 1];
    }
    (*buff_size)--;
    return dir;
}


int main(){

    int max_x = 0 , max_y = 0, pos_x = 0, pos_y = 0, running = 1, snakeSize = 3;
    //  Buffer for user inputs
    char dir_buffer[5] = {'a'};
    int buff_size = 1;
    //starting directions
    char dir = 'a', prevDir = 'a';


    clock_t timePeriod = 2*CLOCKS_PER_SEC/10;
    clock_t moveTime = clock() + timePeriod;

    init_all(&max_x, &max_y, &pos_x, &pos_y);
    clear();

     Pos* snakePos = malloc(sizeof(Pos)*max_x*max_y);
     Pos foodPos = {5, 5};
     makeFood(&foodPos, max_x, max_y);

    //  Create the starting positions
    startPositions(snakePos, max_x, max_y);
    printPositions(snakePos, snakeSize, &foodPos);

    while(running){


        if(kbhit()){
            
            dir = getch();
            if(buff_size != 0) prevDir = dir_buffer[buff_size-1];

            if(check_key(&dir, prevDir, &running) && buff_size < 5){

                dir_buffer[buff_size] = dir;
                buff_size++;
            }


        }

        if(check_time(&moveTime, timePeriod)){

            if(buff_size == 0){
                dir = prevDir;
            }
            else{
                dir = get_buffer(dir_buffer, &buff_size);
            }
            moveSnake(snakePos, &snakeSize, &foodPos, dir, max_x, max_y, &running);
            if(foodPos.x == 0 && foodPos.y == 0){
                makeFood(&foodPos, max_x, max_y);
            }
            printPositions(snakePos, snakeSize, &foodPos);
            prevDir = dir;
        }


        usleep(10000);
        moveTime -= CLOCKS_PER_SEC/100;
    }

    free(snakePos);
	endwin();			/* End curses mode		  */


	return 0;
}

