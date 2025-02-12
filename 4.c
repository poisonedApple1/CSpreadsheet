#include <stdio.h>
int fibo(int c)
{
    if (c == 0)
        return 0;
    else if (c == 1)
        return 1;
    else
        return fibo(c - 1) + fibo(c - 2);
}
int main()
{
    printf("%d", fibo(46));
}