
#include <stdio.h>
#define LIMIT 50

int main()
{
    int x = 20;
    int y = 8;
    int total = 0;

    // Arithmetic operations
    total = x + y;
    int remainder = x % y;

    /* Relational and logical operations */
    if(x > y && y != 0)
    {
        printf("X is greater than Y\n");
        printf("Testing operators: + - * / %\n");
    }

    // Increment and decrement
    x++;
    y--;

    // Compound assignment
    total += 10;
    total -= 2;

    // Bitwise operations
    int result = (x & y) | (x ^ y);
    int shifted = x >> 2;

    char grade = 'A';

    float average = 25.5f;

    printf("Total = %d\n", total);
    printf("Grade = %c\n", grade);
    printf("Average = %.2f\n", average);

    return 0;
}
