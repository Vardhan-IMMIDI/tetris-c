#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>



int main(void)
{
    struct timeval begin, start, current;
    gettimeofday(&start, NULL);
    gettimeofday(&begin, NULL);

    while(1)
    {
        gettimeofday(&current, NULL);
        unsigned long long current_ms = 1000000 * current.tv_sec + current.tv_usec;
        unsigned long long start_ms = 1000000 * start.tv_sec + start.tv_usec;
        unsigned long long elapsed = current.tv_usec - start.tv_usec;
        // printf("%ld, %ld, %lf\n", current.tv_usec, start.tv_usec, elapsed);
        if (elapsed > 500000)
        {  
            start = current;
            printf("Hello\n");
        }
    }
}