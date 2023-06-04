#include <stdio.h>

void init_report(_IO_FILE *results, float mean_interarrival, float mean_service, int required_wait_count)
{
        fprintf(results, "Simple Queue System\n\n");
        fprintf(results, "Mean interarrival time%11.3f minutes\n\n",
                mean_interarrival);
        fprintf(results, "Mean service time%16.3f minutes\n\n", mean_service);
        fprintf(results, "Number of customers%14d\n\n", required_wait_count);
}

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
        fprintf(results, "\n\nErlang C value%20.3f\n\n", metrics[4]);
}