#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <sys/time.h>

#define HEIGHT 20
#define WIDTH 10
#define HEIGHT_INDEX 0
#define WIDTH_INDEX 1
#define BLOCK_SIZE 4
#define AXES 2
#define BLOCKS 7
#define COLORS 7
#define ROTATIONS 4
#define BLOCK ' '
#define EMPTY_BLOCK ".."
#define F(x) (int)((x) * (3.92))


void initialize(void);
void printer(void);
bool add_new(void);
bool gravity(void);
void add_block_to_board(int block[BLOCK_SIZE][AXES], int color);
void remove_block_from_board(int block[BLOCK_SIZE][AXES]);
int runner(void);
void draw_block(int y, int x, int color_pair);
void print_border(void);
void move_block(int input);


int board[HEIGHT][WIDTH];
int current_block[BLOCK_SIZE][AXES];
int current_block_color;
int current_block_index;
int current_block_rotation;



const int blocks[7][4][2] = {{{0, 0}, {0, 1}, {1, 0}, {1, 1}},  // Square
                            {{0, 0}, {1, 0}, {2, 0}, {3, 0}},  // Line
                            {{0, 1}, {1, 1}, {2, 0}, {2, 1}},  // J
                            {{0, 0}, {1, 0}, {2, 0}, {2, 1}},  // L
                            {{0, 1}, {0, 2}, {1, 0}, {1, 1}},  // S
                            {{0, 1}, {1, 0}, {1, 1}, {1, 2}},  // T {In reverse}
                            {{0, 0}, {0, 1}, {1, 1}, {1, 2}}};  // Z


const int rotations[6][4][4][2] = {
                                    // 0: Line block, 2 rotations
                                    {
                                        {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
                                        {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
                                        {{2, 0}, {2, 1}, {2, 2}, {2, 3}},   // Duplicated 
                                        {{0, 1}, {1, 1}, {2, 1}, {3, 1}}    // Due to uncompatible jagged array in c
                                    },
                                    // 1: J block, 4 rotations
                                    {
                                        {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
                                        {{0, 1}, {1, 1}, {2, 0}, {2, 1}},
                                        {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
                                        {{0, 1}, {0, 2}, {1, 1}, {2, 1}}
                                    },
                                    // 2: L block, 4 rotations
                                    {
                                        {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
                                        {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
                                        {{0, 2}, {1, 0}, {1, 1}, {1, 2}},
                                        {{0, 1}, {1, 1}, {2, 1}, {2, 2}}
                                    },
                                    // 3: S block, 2 rotations
                                    {
                                        {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
                                        {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
                                        {{1, 1}, {1, 2}, {2, 0}, {2, 1}},   // Duplicated 
                                        {{0, 0}, {1, 0}, {1, 1}, {2, 1}}    // Due to uncompatible jagged array in c
                                    },
                                    // 4: T block, 4 rotations
                                    {
                                        {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
                                        {{0, 1}, {1, 0}, {1, 1}, {2, 1}},
                                        {{0, 1}, {1, 0}, {1, 1}, {1, 2}},
                                        {{0, 1}, {1, 1}, {1, 2}, {2, 1}}
                                    },
                                    // 5: Z block, 2 rotations
                                    {
                                        {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
                                        {{0, 1}, {1, 0}, {1, 1}, {2, 0}},
                                        {{1, 0}, {1, 1}, {2, 1}, {2, 2}},   // Duplicated 
                                        {{0, 1}, {1, 0}, {1, 1}, {2, 0}}    // Due to uncompatible jagged array in c
                                    }
                                };


const int sizes[7][2] = {{2, 2}, {4, 1}, {2, 3}, {2, 3}, {3, 2}, {3, 2}, {2, 3}};


int main(void)
{
    runner();
}


int runner(void)
{
    initialize();
    add_new();
    
    struct timeval begin, start1, start2, current;
    gettimeofday(&start1, NULL);
    gettimeofday(&start2, NULL);
    gettimeofday(&begin, NULL);

    int key_stroke;

    for (;;)
    {
        gettimeofday(&current, NULL);

        unsigned long long current_ms = 1000000 * current.tv_sec + current.tv_usec;
        unsigned long long start_ms = 1000000 * start1.tv_sec + start1.tv_usec;
        unsigned long long elapsed1 = current.tv_usec - start1.tv_usec;
        unsigned long long elapsed2 = current.tv_usec - start2.tv_usec;

        int input = getch();

        if (input != ERR)
        {
            key_stroke = input;
        }

        if (elapsed1 > 50000)
        {
            start1 = current;
            printer();
            move_block(key_stroke);
            key_stroke = ERR;
            if (elapsed2 > 500000)
            {
                start2 = current;
                if (!gravity())
                {
                    if (!add_new())
                    {
                        break;
                    }
                }
            }
        }
        

        // if (elapsed > 500000)
        // {
        //     start = current;   
        //     printer();
        //     if (!gravity())
        //     {
        //         if (!add_new())
        //         {
        //             break;
        //         }
        //     }
        // }
    }
    getch();
    endwin();
    return 0;
}

void initialize(void)
{
    // Initialize board array to 0
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0;j < WIDTH; j++)
        {
            board[i][j] = 0;
        }
    }

    initscr();
    // cbreak();
    noecho();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    start_color();
    curs_set(0);

    init_color(9, 0, 0, 0); //  Black
    init_color(10, F(255), F(255), F(255)); //   White
    init_color(11, F(120), F(120), F(120));  //  Grey (borders)
    init_color(12, F(205), 0, 0);  //  Red
    init_color(13, 0, F(205), 0);  //  Green
    init_color(14, 0, 0, F(205));  //  Blue
    init_color(15, F(205), F(102), 0); //  Orange
    init_color(16, 0, F(205), F(205)); //  Sky Blue
    init_color(17, F(154), 0, F(205)); //  Purple
    init_color(18, F(205), F(205), 0);  //  Yellow

    init_pair(0, 9, 9);    //  Black White
    init_pair(1, 9, 11);    //  Grey
    init_pair(2, 9, 12);    //  Red
    init_pair(3, 9, 13);    //  Green
    init_pair(4, 9, 14);    //  Blue
    init_pair(5, 9, 15);    //  Orange
    init_pair(6, 9, 16);    //  Sky Blue
    init_pair(7, 9, 17);    //  Purple
    init_pair(8, 9, 18);    //  Yellow

    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            draw_block(i, j, 0);
        }
    }

    return;
}


void printer(void)
{
    // Print every frame
    move(0,0);

    int horizontal_padding = (COLS / 2) - (WIDTH);
    int vertical_padding = (LINES / 2) - (HEIGHT / 2);
    // Print board and corresponding blocks
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0 ; j < WIDTH; j++)
        {
            // Print BLOCK if block present
            if (board[i][j] > 0)
            {
                // Print BLOCK if block present
                draw_block(i + vertical_padding, (j * 2) + horizontal_padding, board[i][j]);
                draw_block(i + vertical_padding, (j * 2) + horizontal_padding + 1, board[i][j]);
            }
            else
            {
                // Print EMPTY_BLOCK if block is not present
                draw_block(i + vertical_padding, (j * 2) + horizontal_padding, 0);
                draw_block(i + vertical_padding, (j * 2) + horizontal_padding + 1, 0);
            }
        }
    }
    print_border();
    refresh();

}

void print_border(void)
{
    for (int i = 0; i <= HEIGHT; i++)
    {
        draw_block(i + ((LINES - HEIGHT) / 2), ((COLS / 2) - WIDTH) - 1, 1);
        draw_block(i + ((LINES - HEIGHT) / 2), ((COLS / 2) - WIDTH) - 2, 1);
        draw_block(i + ((LINES - HEIGHT) / 2), ((COLS / 2) + WIDTH), 1);
        draw_block(i + ((LINES - HEIGHT) / 2), ((COLS / 2) + WIDTH) + 1, 1);
    }
    
    for (int i = 0; i <= WIDTH; i++)
    {
        draw_block((LINES + HEIGHT) / 2, (i * 2) + (COLS / 2) -  WIDTH, 1);
        draw_block((LINES + HEIGHT) / 2, (i * 2) + (COLS / 2) -  WIDTH + 1, 1);
    }
}

bool add_new(void)
{
    srand(time(NULL));

    int block = rand() % BLOCKS;
    int rotation = rand() % ROTATIONS;
    int position = rand() % (WIDTH - sizes[block][WIDTH_INDEX]);

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (board[blocks[block][i][0]][blocks[block][i][1] + position] != 0)
        {
            return false;
        }
    }

    current_block_color = rand() % COLORS + 2;

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        current_block[i][0] = blocks[block][i][0];
        current_block[i][1] = blocks[block][i][1] + position;
    }

    add_block_to_board(current_block, current_block_color);

    return true;
}



bool gravity(void)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (current_block[i][HEIGHT_INDEX] + 1 >=  HEIGHT)
        {
            return false;
        }
    }

    remove_block_from_board(current_block);

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (board[current_block[i][HEIGHT_INDEX] + 1][current_block[i][WIDTH_INDEX]] > 0)
        {
            add_block_to_board(current_block, current_block_color);
            return false;
        }
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        current_block[i][HEIGHT_INDEX] += 1;
    }

    add_block_to_board(current_block, current_block_color);

    return true;
}

void add_block_to_board(int block[BLOCK_SIZE][AXES], int color)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        board[block[i][HEIGHT_INDEX]][block[i][WIDTH_INDEX]] = color;
    }
}

void remove_block_from_board(int block[BLOCK_SIZE][AXES])
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        board[block[i][HEIGHT_INDEX]][block[i][WIDTH_INDEX]] = 0;
    }
}

void draw_block(int x, int y, int color_pair)
{
    attron(COLOR_PAIR(color_pair));
    mvaddch(x, y, ' ');
    // mvaddch(y, x, ' ');
    // mvaddch(y + 10, x * 2 + 20, BLOCK);
    // mvaddch(y + 10, x * 2 + 1 + 20, BLOCK);
    attroff(COLOR_PAIR(color_pair));
}

void move_block(int input)
{
    if (input == KEY_DOWN)
    {
        // Move faster
    }
    else if (input == KEY_UP)
    {
        // Rotate Block
    }

    int to_add;
    if (input == KEY_LEFT)
    {
        to_add = -1;
    }
    else if (input == KEY_RIGHT)
    {
        to_add = 1;
    }
    else
    {
        return;
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (current_block[i][WIDTH_INDEX] + to_add >= WIDTH || current_block[i][WIDTH_INDEX] + to_add < 0)
        {
            return;
        }
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if ((board[current_block[i][HEIGHT_INDEX]][current_block[i][WIDTH_INDEX] + to_add] != 0) && (board[current_block[i][HEIGHT_INDEX]][current_block[i][WIDTH_INDEX] + to_add] != current_block_color))
        {
            bool flag = true;
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                if (current_block[i][HEIGHT_INDEX] == current_block[j][HEIGHT_INDEX] && current_block[j][WIDTH_INDEX] == current_block[i][WIDTH_INDEX] + to_add)
                {
                    flag = false;
                }
            }
            // attron(COLOR_PAIR(0));
            // char str[4];
            // sprintf(str, "%i %i", board[current_block[i][HEIGHT_INDEX]][current_block[i][WIDTH_INDEX] + to_add], to_add);
            // mvaddstr(0, 0, str);
            // attroff(COLOR_PAIR(0));
            if (flag)
            {
                return;
            }
        }
    }
    
    remove_block_from_board(current_block);
        
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        current_block[i][WIDTH_INDEX] += to_add;
    }

    add_block_to_board(current_block, current_block_color);
}