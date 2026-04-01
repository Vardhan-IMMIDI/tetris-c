#include <ncurses.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define ROWS 20
#define COLOUMS 10
#define ATTRIBUTES 2
#define BLOCK ' '
#define BLOCKS 7
#define F(x) (int)((x) * (3.92))

//  Make add_block
//  Make delete_rows
//  Make end_game


void printer();
void draw_block(int y, int x, int color_pair);
void print_border();
void gravity();
int random_num(int x, int y);
void add_block();
void delete_rows();
void block_move();
void block_rotate();

int blocks[7][5][2] = {{{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 2}},    // Square (2 x 2)
                       {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {1, 4}},    // Line (1 x 4)
                       {{0, 1}, {0, 2}, {1, 0}, {1, 1}, {3, 2}},    // S (3 x 2)
                       {{0, 0}, {0, 1}, {1, 1}, {1, 2}, {3, 2}},    // Z (3 x 2)
                       {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 3}},    // L (2 x 3)
                       {{0, 1}, {1, 1}, {2, 0}, {2, 1}, {2, 3}},    // J (2 x 3)
                       {{0, 1}, {1, 0}, {1, 1}, {1, 2}, {3, 2}}};   // T (3 x 2)
int arr[ROWS][COLOUMS][ATTRIBUTES];
int bc = 0;
int ch;

int main(void)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLOUMS; j++)
        {
            arr[i][j][0] = 0;
            arr[i][j][1] = 1;
        }
    }
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
    curs_set(0);
    srand(time(NULL));

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

    init_pair(1, 10, 9);    //  Black White
    init_pair(2, 9, 11);    //  Grey
    init_pair(3, 9, 12);    //  Red
    init_pair(4, 9, 13);    //  Green
    init_pair(5, 9, 14);    //  Blue
    init_pair(6, 9, 15);    //  Orange
    init_pair(7, 9, 16);    //  Sky Blue
    init_pair(8, 9, 17);    //  Purple
    init_pair(9, 9, 18);    //  Yellow

    bkgd(COLOR_PAIR(1));
    print_border();

    struct timeval begin, start, current;
    gettimeofday(&start, NULL);
    gettimeofday(&begin, NULL);
    int r = 0;
    for (;;)
    {
        int nch = getch();

        if (nch != ERR)
        {
            ch = nch;
        }
        gettimeofday(&current, NULL);

        long elapsed = current.tv_sec - start.tv_sec;
        if (elapsed >= 0.5)
        {
            gettimeofday(&start, NULL);
            block_move();
            block_rotate();
            if (r % 2 == 0)
            {
                gravity();
            }
            printer();
            move(0,0);
            refresh();
            r++;
        }
    }

    getch();
    endwin();
}

void block_rotate()
{
    if (ch != '\n')
    {
        return;
    }

    int a[4][4][2] = {{{3, 0}, {2, 0}, {1, 0}, {0, 0}},
                      {{3, 1}, {2, 1}, {1, 1}, {0, 1}},
                      {{3, 2}, {2, 2}, {1, 2}, {0, 2}},
                      {{3, 3}, {2, 3}, {1, 3}, {0, 3}}};
    int indexes[4][2];
    int index[4][2];
    int lc = 0;

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLOUMS; j++)
        {
            int a = i;
            int b = j;
            if (arr[i][j][0] == bc)
            {
                if (lc == 4)
                {
                    printf("Code Working Improperly!\n");
                }
                indexes[lc][0] = index[lc][0] = a;
                indexes[lc][1] = index[lc][1] = b;
                lc++;
            }
        }
    }
    int i_min = 9999;
    int i_max = 0;
    int j_min = 9999;
    int j_max = 0;
    for (int i = 0; i < lc; i++)
    {
        int x = i;
        if (i_min > indexes[x][0])
        {
            i_min = indexes[x][0];
        }
        if (i_max < indexes[x][0])
        {
            i_max = indexes[x][0];
        }
        if (j_min > indexes[x][1])
        {
            j_min = indexes[x][1];
        }
        if (j_max < indexes[x][1])
        {
            j_max = indexes[x][1];
        }
    }

    for (int i = 0; i < lc; i++)
    {
        int x = i;
        indexes[x][0] = indexes[x][0] - i_min;
        indexes[x][1] = indexes[x][1] - j_min;
    }
    for (int i = 0; i < lc; i++)
    {
        int x = i;
        indexes[x][0] = a[indexes[x][0]][indexes[x][1]][0];
        indexes[x][1] = a[indexes[x][0]][indexes[x][1]][1];
    }
    // int i_av = (i_min + i_max) / 2;
    for (int i = 0; i < lc; i++)
    {
        int x = i;
        indexes[x][0] = indexes[x][0] + i_min;
        indexes[x][1] = indexes[x][1] + j_min;
    }
    bool flag = true;
    for (int i = 0; i < lc; i++)
    {
        int x = i;
        if (indexes[x][0] > 19 || indexes[x][0] < 0)
        {
            flag = false;
        }
        if (indexes[x][1] > 9 || indexes[x][1] < 0)
        {
            flag = false;
        }
    }
    if (flag)
    {
        for (int i = 0; i < lc; i++)
        {
            int x = i;
            if (arr[indexes[x][0]][indexes[x][1]][0] != 0 && arr[indexes[x][0]][indexes[x][1]][0] != bc)
            {
                flag = false;
            }
        }
    }

    if(flag)
    {
        int c = arr[index[0][0]][index[0][1]][1];
        for (int i = 0; i < lc; i++)
        {
            int x = i;
            arr[index[x][0]][index[x][1]][0] = 0;
            arr[index[x][0]][index[x][1]][1] = 1;
        }
        for (int i = 0; i < lc; i++)
        {
            int x = i;
            arr[indexes[x][0]][indexes[x][1]][0] = bc;
            arr[indexes[x][0]][indexes[x][1]][1] = c;
        }
    }
    ch = ERR;
}

void print_border()
{
    for (int i = 0; i <= ROWS; i++)
    {
        draw_block(i, 0, 2);
        draw_block(i, COLOUMS + 1, 2);
    }
    for (int i = 0; i <= COLOUMS; i++)
    {
        draw_block(20, i, 2);
    }
}

void block_move()
{
    int x;
    if (ch == ERR)
    {
        return;
    }
    if (ch == KEY_RIGHT)
    {
        x = 1;
    }
    else if (ch == KEY_LEFT)
    {
        x = -1;
    }
    else
    {
        return;
    }
    int indexes[4][2];
    int l = 0;
    bool flag = true;
    for (int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLOUMS; j++)
        {
            int a = i;
            int b = j;
            if (arr[i][j][0] == bc)
            {
                if (l > 3)
                {
                    printf("Warning! Code Working Improperly\n");
                }
                indexes[l][0] = a;
                indexes[l][1] = b;
                l++;
            }
        }
    }
    for (int i = 0; i < l; i++)
    {
        int n = i;
        if (indexes[n][1] + x > 9 || indexes[n][1] + x < 0)
        {
            flag = false;
            break;
        }
    }
    if (flag)
    {
        for (int i = 0; i < l; i++)
        {
            int n = i;
            if (arr[indexes[n][0]][indexes[n][1] + x][0] != 0 && arr[indexes[n][0]][indexes[n][1] + x][0] != bc)
            {
                flag = false;
            }
        }
    }
    if (flag)
    {
        int c = arr[indexes[0][0]][indexes[0][1]][1];
        for (int i = 0; i < l; i++)
        {
            int n = i;
            arr[indexes[n][0]][indexes[n][1]][0] = 0;
            arr[indexes[n][0]][indexes[n][1]][1] = 1;
        }
        for (int i = 0; i < l; i++)
        {
            int n = i;
            arr[indexes[n][0]][indexes[n][1] + x][0] = bc;
            arr[indexes[n][0]][indexes[n][1] + x][1] = c;
        }
    }
    ch = ERR;
}


void printer()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLOUMS; j++)
        {
            draw_block(i, j + 1, arr[i][j][1]);
        }
    }
}

void add_block()
{
    int b = random_num(0,6);
    int n = COLOUMS - blocks[b][4][0] + 1;
    int ar[COLOUMS];
    int l = 0;
    for (int i = 0; i < n; i++)
    {
        int x = i;
        bool flag = true;
        for (int j = 0; j < 4; j++)
        {
            if (arr[blocks[b][j][0]][blocks[b][j][1] + x][0] != 0)
            {
                flag = false;
                break;
            }
        }
        if (flag)
        {
            ar[l] = x;
            l++;
        }
    }
    if (l)
    {
        bc++;
        int pos = random_num(0, l - 1);
        int c = random_num(3, 9);
        for (int i = 0; i < 4; i++)
        {
            arr[blocks[b][i][0]][blocks[b][i][1] + ar[pos]][0] = bc;
            arr[blocks[b][i][0]][blocks[b][i][1] + ar[pos]][1] = c;
        }
    }
}

void gravity()
{
    int c[100];
    int indexes[4][2];
    int lc = 0, li;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLOUMS; j++)
        {
            li = 0;
            int a = i;
            int b = j;
            int x = arr[a][b][0];
            if (x > 0)
            {
                bool flag = true;
                for (int k = 0; k < lc; k++)
                {
                    if (c[k] == x)
                    {
                        flag = false;
                        break;
                    }
                }

                if (flag)
                {
                    for (int m = 0; m < ROWS; m++)
                    {
                        for (int n = 0; n < COLOUMS; n++)
                        {
                            int p = m;
                            int q = n;
                            if (arr[p][q][0] == x)
                            {
                                indexes[li][0] = p;
                                indexes[li][1] = q;
                                li++;
                            }
                        }
                    }
                    bool fl = true;
                    for (int m = 0; m < li; m++)
                    {
                        int p = m;
                        if (indexes[p][0] == 19)
                        {
                            fl = false;
                        }
                    }
                    if (fl)
                    {
                        for (int m = 0; m < li; m++)
                        {
                            int p = m;
                            if (arr[indexes[p][0] + 1][indexes[p][1]][0] != 0 && arr[indexes[p][0] + 1][indexes[p][1]][0] != x)
                            {
                                fl = false;
                            }
                        }
                    }
                    if (fl)
                    {
                        int at = arr[indexes[0][0]][indexes[0][1]][1];
                        for (int m = 0; m < li; m++)
                        {
                            int p = m;
                            arr[indexes[p][0]][indexes[p][1]][0] = 0;
                            arr[indexes[p][0]][indexes[p][1]][1] = 1;
                        }
                        for (int m = 0; m < li; m++)
                        {
                            int p = m;
                            arr[indexes[p][0] + 1][indexes[p][1]][0] = x;
                            arr[indexes[p][0] + 1][indexes[p][1]][1] = at;
                        }
                        c[lc] = x;
                        lc++;
                    }
                }
            }
        }
    }
    if (lc == 0)
    {
        delete_rows();
        add_block();
    }
}

void delete_rows()
{
    int deleted = 0;
    for(int i = 0; i < ROWS; i++)
    {
        bool flag = true;
        for(int j = 0; j < COLOUMS; j++)
        {
            if(arr[i][j][0] == 0)
            {
                flag = false;
            }
        }

        if (flag)
        {
            for (int j = 0; j < COLOUMS; j++)
            {
                arr[i][j][0] = 0;
                arr[i][j][1] = 0;
                deleted++;
            }
        }
    }
    if (deleted)
    {
        for(int i = 0; i < deleted; i++)
        {
            gravity();
        }
    }
}

void draw_block(int y, int x, int color_pair)
{
    attron(COLOR_PAIR(color_pair));
    mvaddch(y + 10, x * 2 + 20, BLOCK);
    mvaddch(y + 10, x * 2 + 1 + 20, BLOCK);
    attroff(COLOR_PAIR(color_pair));
}

int random_num(int x, int y)
{
    if(x < 0 || y < 0)
    {
        exit(1);
    }
    if (x > y)
    {
        return rand() % (x - y + 1) + y;
    }
    else
    {
        return rand() % (y - x + 1) + x;
    }
}


// Make code working without any edgecases or issues
// Keep Score and print and update score during game
// Make the Next block / Upcoming Block
// Make the end screen with game score
// Make the start screen with game starts contdown from 3
// Make a custom colourful startup screen
// Make a way to keep score and high score
// Set minimum size of terminal required and add borders to screen
// Make a login and register screen
// Make it secure and make an windows compatible version
// look into multi core and multi thread processing
// Make an effecient version of it by using stacks and queues and by keeping track of each and every element getting deleted and added by storing indexes
