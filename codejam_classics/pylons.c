#include <stdio.h>
#include <stdlib.h>

#define ARRAY_LEN(arr, T) (sizeof(arr)/sizeof(T))

int next_cell_v2(int crow, int ccol, int prow, int pcol, int *visited_grid, int max_row, int max_col);

enum {
    EMPTY_STATE = 0,
    CHOSE_STATE = 1,
    UNLIST_STATE = 2
};

void evade_pylons(int rrow, int *visited_grid, int max_row, int max_col, int **result);

int main(int argc, char *argv[]){
    int visited_grid[2 * 5] = {0};
    int max_row = 2;
    int max_col = 5;

    int result[2 * 5][2] = {0};
    int temp_[2 * 5][2] = {0};


    for (int i = 0; i < max_row; i++){
        for (int j = 0; j < max_col; j++){
            temp_[j + (max_col * i)][0] = i;
            temp_[j + (max_col * i)][1] = j;
        }
    }
    int top = 0;
    result[top][0] = temp_[0][0];
    result[top][1] = temp_[0][1];
    top++;
    // for (int i = 0; i < max_row; i++){
    //     for (int j = 0; j < max_col; j++){

    //         // if (top == -1){
    //         //     printf("IMPOSSIBLE\n");
    //         //     goto end;
    //         // } else {
    //         //     if (top >= ARRAY_LEN(visited_grid, int)){
    //         //         goto end;
    //         //     } else if ((i == result[top][0]) && (j == result[top][1])){
    //         //         continue;
    //         //     } else if (next_cell_v2(i, j, result[top][0], result[top][1], visited_grid, max_row, max_col)){
    //         //         printf("=============+\n");
    //         //         result[top][0] = temp_[j + (max_col * i)][0];
    //         //         result[top][1] = temp_[j + (max_col * i)][1];
    //         //         top++;
    //         //     } else {
    //         //         printf("Oh no\n");
    //         //         top--;
    //         //     }
    //         // }
    //     }
    //     printf("+++++++++++\n");
    // }
    for (int i = 1; i < ARRAY_LEN(visited_grid, int); i++){
        if (top == -1){
            goto end;
        } else {
            break;
        }
    }
    end:
        if (top > -1){
            for (int i = 0; i < top; i++){
                printf("result[%d]: <%d, %d>\n", i, result[i][0], result[i][1]);
            }
        }
        printf("+++++++++++\n");
        for (int i = 0; i < ARRAY_LEN(visited_grid, int); i++){
            printf("temp_[%d]: <%d, %d>\n", i, temp_[i][0], temp_[i][1]);
        }
        return 0;
}


int next_cell_v2(int crow, int ccol, int prow, int pcol, int *visited_grid, int max_row, int max_col){
    if (crow == prow){}
    else if (ccol == pcol){}
    else if ((crow - ccol) == (prow - pcol)){}
    else if ((crow + ccol) == (prow + pcol)){}
    else {
        if (visited_grid[pcol + (max_col * prow)] == 0){
            return 1;
        }
    }
    return 0;
}