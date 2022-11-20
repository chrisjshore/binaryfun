#include <stdio.h>
#include <math.h>

int square(int x)
{
    return x*x;
}

int main(int argc, char** argv)
{
    double PI = 3.141592654;
    double cosign = (30 * PI) / 180;
    double result = cos(cosign);
    fprintf(stdout, "Hello world!\n");
    fprintf(stdout, "fprintf=%p, stdout=%p\n", fprintf, stdout);
    fprintf(stdout, "square(10) = %d\n", square(10));
    fprintf(stdout, "cos(0.523) = %f\n", result);
    printf("Goodbye world!\n");
    return 0;
}