#include <stdio.h>

void report_metrics(_IO_FILE *results, float *metrics)
{
    /* Calculate and estimate the desired performance measures */
    fprintf(results, "\n\nAverage wait time in the queue%11.3f minutes\n\n",
            metrics[0]);
    fprintf(results, "Average number in the queue%10.3f\n\n",
            metrics[1]);
    fprintf(results, "Server utilization%15.3f\n\n",
            metrics[2]);
    fprintf(results, "Time simulation ended%12.3f minutes", metrics[3]);
}