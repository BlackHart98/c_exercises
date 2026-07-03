#include <stdio.h>
#include <string.h>
#include <assert.h>

#define SLICE_IMPLEMENTATION
#include "../data_structures/slice.c"

typedef int8_t fixed_point_t;

typedef struct min_max_t {
    float min;
    float max;
} min_max_t;

typedef struct quantized_slice_t {
    fixed_point_t zero_point;
    float         scale;
    slice_t       slice; // []fixed_point_t;
} quantized_slice_t;


min_max_t
min_max_float(float *arr, size_t size)
{
    assert((0 != size)&&"size should be greater than zero");
    size_t i = 0; float min = arr[i]; float max = arr[i];
    for (; i < size; i++){
        if (min > arr[i]) min = arr[i];
        if (max < arr[i]) max = arr[i];
    }
    return (min_max_t) { .max = max, .min = min };
}

float 
get_scale(min_max_t min_max)
{
    size_t size_of_int_rep = sizeof(fixed_point_t);
    float positive_clip    = (1 << ((8 * size_of_int_rep) - 1)) - 1;
    float negative_clip    = -1.0f * (1 << ((8 * size_of_int_rep) - 1));
    float scale            = (min_max.max - min_max.min) / (positive_clip - negative_clip);
    return scale;
}


fixed_point_t
get_zero_point(min_max_t min_max, float scale)
{
    size_t size_of_int_rep   = sizeof(fixed_point_t);
    float negative_clip      = -1.0f * (1 << ((8 * size_of_int_rep) - 1));
    fixed_point_t zero_point = (fixed_point_t)((negative_clip - (min_max.min / scale)) - 0.5);
    return zero_point;
}

int quantize(float f, float scale, fixed_point_t zero_point)
{
    size_t size_of_int_rep = sizeof(fixed_point_t);
    float positive_clip    = (1 << ((8 * size_of_int_rep) - 1)) - 1;
    float negative_clip    = -1.0f * (1 << ((8 * size_of_int_rep) - 1));
    if (f > positive_clip / scale) f = positive_clip / scale;
    if (f < negative_clip / scale) f = negative_clip / scale;
    return (fixed_point_t)((f / scale) + ((f >= 0) ? 0.5 : -0.5)) + zero_point;
}

float dequantize(fixed_point_t p, float scale, fixed_point_t zero_point)
{
    return scale * (float)(p - zero_point);
}


quantized_slice_t
quantize_float_slice(slice_t float_slice, slice_t slice)
{
    fixed_point_t *ptr = slice.ptr;
    float *fptr = float_slice.ptr;
    size_t Nf   = float_slice.len_in_bytes / sizeof(*fptr);
    size_t Nq   = slice.len_in_bytes / sizeof(*ptr);
    assert((Nq == Nf)&&"Nq should be equal to Nf");
    min_max_t min_max        = min_max_float(float_slice.ptr, float_slice.len_in_bytes / sizeof(float));
    float scale              = get_scale(min_max);
    fixed_point_t zero_point = get_zero_point(min_max, scale);
    for (size_t i = 0; i < Nq; i++) {
        ptr[i] = quantize(fptr[i], scale, zero_point);
    }
    return (quantized_slice_t){
        .scale      = scale,
        .zero_point = zero_point,
        .slice      = slice,
    };
}


slice_t
dequantize_slice(quantized_slice_t q, slice_t float_slice)
{
    fixed_point_t *ptr = q.slice.ptr;
    float *fptr = float_slice.ptr;
    size_t Nf   = float_slice.len_in_bytes / sizeof(*fptr);
    size_t Nq   = q.slice.len_in_bytes / sizeof(*ptr);
    assert((Nq == Nf)&&"Nq should be equal to Nf");
    for (size_t i = 0; i < Nq; i++) {
        fptr[i] = dequantize(ptr[i], q.scale, q.zero_point);
    }
    return float_slice;
}

int
main(void)
{
    float float_arr[] = {
        -2.00f, -1.50f, -1.00f, -0.75f, -0.50f, -0.25f,
        -0.10f,  0.00f,  0.10f,  0.25f,  0.50f,  0.75f,
        1.00f,  1.25f,  1.50f,  2.00f,  4.00f,  8.00f
    };
    fixed_point_t integer_buf[18] = {0};
    float float_buf[18]           = {0};
    quantized_slice_t q = quantize_float_slice(
        make_slice(float_arr, sizeof(float_arr)), make_slice(integer_buf, sizeof(integer_buf)));
    printf("scale: %f, zero point: %d\n", q.scale, q.zero_point);
    fixed_point_t *ptr = q.slice.ptr;
    size_t Nq          = q.slice.len_in_bytes / sizeof(*ptr);
    for (size_t i = 0; i < Nq; i++) {
        printf("q[%zu]: %d\n", i, ptr[i]);
    }
    printf("========================\n");
    printf("Dequantize\n");
    printf("========================\n");
    slice_t f   = dequantize_slice(q, make_slice(float_buf, sizeof(float_buf)));
    float *fptr = f.ptr;
    size_t Nf   = f.len_in_bytes / sizeof(*fptr);
    for (size_t i = 0; i < Nf; i++) {
        printf("f[%zu]: %f\n", i, fptr[i]);
    }
    printf("Size -> float: %zu, int: %zu\n", 
        sizeof(float_arr), q.slice.len_in_bytes + sizeof(quantized_slice_t));
    return 0;
}