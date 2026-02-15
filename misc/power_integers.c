#include <stdio.h>

int power_integer_max(int n){
    int k, max, count;
    k = 1; max = 1; count = 2;
    while (k < n){
        int temp = count;
        while (temp > 1 && (temp % 2) == 0){
            temp /= 2;
        }
        while (temp > 1 && (temp % 3) == 0){
            temp /= 3;
        }
        while (temp > 1 && (temp % 5) == 0){
            temp /= 5;
        }
        if (temp == 1){
            max = count; k++;
        }
        count ++;
    }
    return max;
}


int main(int argc, char **argv){
    printf("Solution is %d\n", power_integer_max(6));
    return 0;
}