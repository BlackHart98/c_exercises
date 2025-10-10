#include <arm_neon.h>
#include <stdio.h>
#include <stdlib.h>

#define len(T, obj) (sizeof(obj)/sizeof(T))

int add_intrinsic(int *, int *, int *, size_t);


int main(int argc, char *argv[]){
    int input_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int result_array[9] = {0};
    int event_array[] = {-1, -2, -1, -3, -1, -4, -1, -2, -1};
    size_t array_len = len(int, input_array);

    add_intrinsic(input_array, event_array, result_array, array_len);

    for (int i = 0; i < array_len; i++){
        printf("result_array[%d] : %d\n", i, result_array[i]);
    }
    return 0;
}



int add_intrinsic(int *input_array, int *event_array, int *result, size_t n){
#if defined(__ARM_NEON)
    int i = 0;
    for (; i + 4 < n; i += 4){
        int32x4_t l = vld1q_s32(&input_array[i]);
        int32x4_t r = vld1q_s32(&event_array[i]);
        l = vaddq_u32(l, r);
        vst1q_s32(result + i, l);
    }
    for (; i < n; i++){
        result[i] = input_array[i] + event_array[i];
    }
#else
    // loop unrolling
    int i = 0;
    for (; i + 4 < n; i += 4){
        result[i] = input_array[i] + event_array[i];
        result[i + 1] = input_array[i + 1] + event_array[i + 1];
        result[i + 2] = input_array[i + 2] + event_array[i + 2];
        result[i + 3] = input_array[i + 3] + event_array[i + 3];
    }
    for (; i < n; i++){
        result[i] = input_array[i] + event_array[i];
    }
#endif
    return 1;
}