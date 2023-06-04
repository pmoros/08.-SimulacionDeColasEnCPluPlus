#include <math.h>
#include "lcgrand.cpp" /* Header for random number generator */

float exponential(float mean) /* Exponential random variable generator function */
{
    /* Return an exponential random variable with mean "mean" */

    return -mean * log(lcgrand(1));
}