#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define HEIGHT 20
#define WIDTH 10
#define BLOCK "[]"
#define EMPTY_BLOCK ".."


int board[HEIGHT][WIDTH];


void initialize(void);
void printer(void);
bool add_new(void);


int main(void)
{
    initialize();
    for (int i = 0; i < 100000; i++)
    {
        if (i == 0)
        {
            add_new();
        }
        printer();
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

    return true;
}