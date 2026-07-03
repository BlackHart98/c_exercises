#include <stdio.h>
#include <stdint.h>

#define SIGNIFICANT_NUMBER 1000

typedef int16_t fixed_point_t;


fixed_point_t
float_to_fixed_point(float f);

float
fixed_point_to_float(fixed_point_t p);


int
main(void)
{
    float f = 1.5457;
    int16_t fixed = float_to_fixed_point(1.5457);
    printf("Floating point: %f, corresponding fixed point: %i\n", f, fixed);
    printf("Floating point after conversion: %f\n", fixed_point_to_float(fixed));
    return 0;
}


fixed_point_t
float_to_fixed_point(float f)
{
    return (fixed_point_t)(f * SIGNIFICANT_NUMBER + ((f >= 0)? 0.5 : -0.5));
}


float
fixed_point_to_float(fixed_point_t p)
{
    return (float)(p)/SIGNIFICANT_NUMBER;
}