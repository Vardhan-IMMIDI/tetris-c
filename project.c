#include <stdio.h>

#define HEIGHT 20
#define WIDTH 10
#define BLOCK "[]"
#define EMPTY_BLOCK ".."


int board[HEIGHT][WIDTH];


void initialize(void);
void printer(void);


int main(void)
{
    printf("Hello, World!\n");
    initialize();
    printer();
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
        printf("<!");
        for (int j = 0; j < WIDTH; j++)
        {
            if (board[i][j] > 0)
            {
                printf(BLOCK);
            }
            else
            {
                printf(EMPTY_BLOCK);
            }
        }
        printf("!>\n");
    }
}