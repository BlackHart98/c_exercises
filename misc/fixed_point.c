#include <stdio.h>
#include <stdint.h>

#define FRACTION_BITS 6
#define SCALE         (1 << FRACTION_BITS)

typedef int8_t fixed_point_t; // q1.6


fixed_point_t
float_to_fixed_point(float f);

float
fixed_point_to_float(fixed_point_t p);

int
main(void)
{
    float f = -1.597;
    fixed_point_t fixed = float_to_fixed_point(f);
    printf("Floating point: %f, corresponding fixed point: %i\n", f, fixed);
    printf("Floating point after conversion: %f\n", fixed_point_to_float(fixed));
    return 0;
}


fixed_point_t
float_to_fixed_point(float f)
{
    size_t size_of_int_rep = sizeof(fixed_point_t);
    float positive_clip = (1 << ((8 * size_of_int_rep) - 1)) - 1;
    float negative_clip = -1.0f * (1 << ((8 * size_of_int_rep) - 1));
    if (f > positive_clip / SCALE) f = positive_clip / SCALE;
    if (f < negative_clip / SCALE) f = negative_clip / SCALE;
    return (fixed_point_t)(f * SCALE + ((f >= 0)? 0.5 : -0.5));
}


float
fixed_point_to_float(fixed_point_t p)
{
    return (float)(p) / SCALE;
}