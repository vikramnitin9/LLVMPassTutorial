#include <stdio.h>
#include "arithmetic.h"

int main() {
    int a = 5;
    int b = 3;
    printf("Addition: %d\n", add(a, b));
    printf("Subtraction: %d\n", sub(a, b));
    printf("Multiplication: %d\n", mul(a, b));
    return 0;
}