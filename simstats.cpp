#include <math.h>
#include "lcgrand.cpp" /* Header for random number generator */

#define NUM_AGENTS 1

long factorial(long num);

float erlang_c(int num_of_clients, float mean_service)
{
    float traffic_intensity = num_of_clients * (mean_service / 60); // Traffic intensity in Erlangs unit = clients * hour

    float erlang_numerator = (pow(traffic_intensity, NUM_AGENTS) / factorial(NUM_AGENTS)) * (NUM_AGENTS / (NUM_AGENTS - traffic_intensity));

    float erlang_denominator_sum = 0.0;
    for (size_t i = 0; i < NUM_AGENTS; i++)
    {
        erlang_denominator_sum += (pow(traffic_intensity, i) / (factorial(i)));
    }

    return erlang_numerator / (erlang_denominator_sum + erlang_numerator);
}

long factorial(long num)
{
    if (num == 0 || num == 1)
        return 1;
    return (num * factorial(num - 1));
}

float exponential(float mean) /* Exponential random variable generator function */
{
    /* Return an exponential random variable with mean "mean" */

    return -mean * log(lcgrand(1));
}