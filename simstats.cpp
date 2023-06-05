#include <math.h>
#include "lcgrand.cpp" /* Header for random number generator */

#define NUM_AGENTS 1

long factorial(int num);

// num agents is 1
float erlang_c(int numClients, float meanService)
{
    // Convert mean service time to hours
    float serviceHours = meanService / 60.0;

    // Calculate traffic intensity
    float trafficIntensity = numClients * serviceHours;

    // Calculate numerator
    float numerator = std::pow(trafficIntensity, 1) / factorial(numClients);
    numerator *= (numClients / (numClients - trafficIntensity));

    // Calculate denominator
    float denominatorSum = 0.0;
    for (int i = 0; i < 1; ++i)
    {
        denominatorSum += std::pow(trafficIntensity, i) / factorial(i);
    }

    // Calculate Erlang C
    float erlangC = (numerator / (denominatorSum + numerator)) * 100;

    return erlangC;
}

long factorial(int num)
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