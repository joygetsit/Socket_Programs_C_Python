/* To check how to convert float to string in C
for sending acaross a socket */
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *a = "36.86";
    float pi = atof(a);
    printf("Value of pi = %f\n", pi);
    return 0;
}

