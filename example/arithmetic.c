int add(int a, int b) {
    return a + b;
}

int sub(int a, int b) {
    return a - b;
}

int mul(int a, int b) {
    if (b == 0) {
        return 0;
    }
    return add(a, mul(a, sub(b, 1)));
}