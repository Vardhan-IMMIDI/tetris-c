#include <stdio.h>

#define HEIGHT 20
#define WIDTH 10

int board[HEIGHT][WIDTH];

int main(void)
{
    printf("Hello, World!\n");
}

void initialize(void)
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0;j < WIDTH; j++)
        {
            board[i][j] = 0;
        }
    }
}