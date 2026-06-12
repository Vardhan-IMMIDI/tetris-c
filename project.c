#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define HEIGHT 20
#define WIDTH 10
#define HEIGHT_INDEX 0
#define WIDTH_INDEX 1
#define BLOCK_SIZE 4
#define AXES 2
#define BLOCK "[]"
#define EMPTY_BLOCK ".."

void initialize(void);
void printer(void);
bool add_new(void);
bool gravity(void);
void add_block_to_board(int block[BLOCK_SIZE][AXES], int color);
void remove_block_from_board(int block[BLOCK_SIZE][AXES]);

int board[HEIGHT][WIDTH];
int current_block[BLOCK_SIZE][AXES];
int current_block_color;
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
    initialize();
    add_new();
    for (int i = 0; i < 100000; i++)
    {
        printer();
        if (!gravity())
        {
            add_new();
        }
        printf("%i\n", i);
        sleep(1);
    }
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
}


void printer(void)
{
    // Print every frame

    // print leading new lines
    for (int i = 0; i < 10; i++)
    {
        printf("\n");
    }

    // Print board and corresponding blocks
    for (int i = 0; i < HEIGHT; i++)
    {
        // Print Left Border
        printf("<!");
        for (int j = 0; j < WIDTH; j++)
        {
            // Print BLOCK if block present
            if (board[i][j] > 0)
            {
                // Print BLOCK if block present
                printf(BLOCK);
            }
            else
            {
                // Print EMPTY_BLOCK if block is not present
                printf(EMPTY_BLOCK);
            }
        }
        // Print Right Border
        printf("!>\n");
    }
}

bool add_new(void)
{
    int block_width = 2;
    int pos = rand() % (WIDTH - block_width);
    board[0][0 + pos] = 1;
    board[0][1 + pos] = 1;
    board[1][0 + pos] = 1;
    board[1][1 + pos] = 1;

    current_block[0][0] = 0;
    current_block[0][1] = 0 + pos;
    current_block[1][0] = 0;
    current_block[1][1] = 1 + pos;
    current_block[2][0] = 1;
    current_block[2][1] = 0 + pos;
    current_block[3][0] = 1;
    current_block[3][1] = 1 + pos;

    current_block_color = 1;
    current_block_rotation = 1;
    return true;
}



bool gravity(void)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (current_block[i][0] + 1 >=  HEIGHT)
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
        current_block[i][0] += 1;
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