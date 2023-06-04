/* External definitions for simple queue system */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simstats.h"
#include "reports.h"

#define QUEUE_LIMIT 100 /* Maximum queue capacity */
#define BUSY 1          /* Server busy indicator */
#define IDLE 0          /* Server idle indicator */

int next_event_type, num_customers_waiting, required_wait_count, num_events,
    num_arrivals_queue, server_status;
float area_num_arrivals_queue, area_server_status, mean_interarrival, mean_service,
    simulation_time, arrival_time[QUEUE_LIMIT + 1], last_event_time, next_event_time[3],
    total_wait_time, erlang_c_value;
FILE *parameters, *results;

void initialize(void);
void timing(void);
void arrival(void);
void departure(void);
float *compute_metrics(void);
void update_average_wait_time(void);

int main(void) /* Main function */
{
    /* Open input and output files */
    printf("Reading input parameters from param.txt and writing results to result.txt\n");
    parameters = fopen("param.txt", "r");
    results = fopen("result.txt", "w");
    printf("Successfully opened files\n");

    /* Set the number of events for the timing function */
    num_events = 2;

    /* Read the input parameters */
    fscanf(parameters, "%f %f %d", &mean_interarrival, &mean_service,
           &required_wait_count);

    /* Write the report headers and initial parameters to the output file */
    init_report(results, mean_interarrival, mean_service, required_wait_count);

    /* Initialize the simulation */
    initialize();

    /* Run the simulation until the required number of customers arrive */
    while (num_customers_waiting < required_wait_count)
    {
        /* Determine the next event */
        timing();

        /* Update average timing statistics */
        update_average_wait_time();

        /* Invoke the appropriate event function */
        switch (next_event_type)
        {
        case 1:
            arrival();
            break;
        case 2:
            departure();
            break;
        }
    }

    erlang_c_value = erlang_c(num_customers_waiting, mean_service);
    /* Generate the report and end the simulation */
    report_metrics(results, compute_metrics());

    fclose(parameters);
    fclose(results);

    printf("Successfully closed files\n");

    return 0;
}

void initialize(void) /* Initialization function */
{
    /* Initialize the simulation clock */
    simulation_time = 0.0;

    /* Initialize the state variables */
    server_status = IDLE;
    num_arrivals_queue = 0;
    last_event_time = 0.0;

    /* Initialize the statistical counters */
    num_customers_waiting = 0;
    total_wait_time = 0.0;
    area_num_arrivals_queue = 0.0;
    area_server_status = 0.0;

    /* Initialize the event list. Since there are no customers, the departure
       event (end of service) is not considered */
    next_event_time[1] = simulation_time + exponential(mean_interarrival);
    next_event_time[2] = 1.0e+30;
}

void timing(void) /* Timing function */
{
    int i;
    float min_next_event_time = 1.0e+29;

    next_event_type = 0;

    /* Determine the type of event that should occur next */
    for (i = 1; i <= num_events; ++i)
    {
        if (next_event_time[i] < min_next_event_time)
        {
            min_next_event_time = next_event_time[i];
            next_event_type = i;
        }
    }

    /* Check if the event list is empty */
    if (next_event_type == 0)
    {
        /* The event list is empty, stop the simulation */
        fprintf(results, "\nThe event list is empty at time %f", simulation_time);
        exit(1);
    }

    /* The event list is not empty, advance the simulation clock */
    simulation_time = min_next_event_time;
}

void arrival(void) /* Arrival function */
{
    float wait_time;

    /* Schedule the next arrival */
    next_event_time[1] = simulation_time + exponential(mean_interarrival);

    /* Check if the server is BUSY */
    if (server_status == BUSY)
    {
        /* Server is BUSY, increase the number of customers in the queue */

        ++num_arrivals_queue;

        /* Check for overflow condition */
        if (num_arrivals_queue > QUEUE_LIMIT)
        {
            /* The queue has overflowed, stop the simulation */
            fprintf(results, "\nOverflow of arrival time array at time");
            fprintf(results, " %f", simulation_time);
            exit(2);
        }

        /* There is still space in the queue, store the arrival time of the
            customer at the (new) end of arrival_time array */
        arrival_time[num_arrivals_queue] = simulation_time;
    }
    else
    {
        /* The server is IDLE, so the arriving customer has wait time = 0
           (The following two lines of code are for clarity and do not affect
           the result of the simulation) */
        wait_time = 0.0;
        total_wait_time += wait_time;

        /* Increase the number of customers waiting and make the server BUSY */
        ++num_customers_waiting;
        server_status = BUSY;

        /* Schedule a departure (end of service) */
        next_event_time[2] = simulation_time + exponential(mean_service);
    }
}

void departure(void) /* Departure function */
{
    int i;
    float wait_time;

    /* Check if the queue is empty */
    if (num_arrivals_queue == 0)
    {
        /* The queue is empty, make the server IDLE and
        do not consider the departure event */
        server_status = IDLE;
        next_event_time[2] = 1.0e+30;
    }
    else
    {
        /* The queue is not empty, decrease the number of customers in the queue */
        --num_arrivals_queue;

        /* Calculate the wait time for the customer being served and
        update the wait time accumulator */
        wait_time = simulation_time - arrival_time[1];
        total_wait_time += wait_time;

        /* Increase the number of customers waiting and schedule the departure */
        ++num_customers_waiting;
        next_event_time[2] = simulation_time + exponential(mean_service);

        /* Move each customer in the queue (if any) one position forward */
        for (i = 1; i <= num_arrivals_queue; ++i)
            arrival_time[i] = arrival_time[i + 1];
    }
}

// Compute metrics returns the array of metrics for reporting
float *compute_metrics()
{
    float *metrics = static_cast<float *>(malloc(sizeof(float) * 4));
    metrics[0] = total_wait_time / num_customers_waiting;
    metrics[1] = area_num_arrivals_queue / simulation_time;
    metrics[2] = area_server_status / simulation_time;
    metrics[3] = simulation_time;
    metrics[4] = erlang_c_value;
    return metrics;
}

void update_average_wait_time(void) /* Update area accumulators for average statistics */
{
    float time_since_last_event;

    /* Calculate the time since the last event, and update the last event marker */
    time_since_last_event = simulation_time - last_event_time;
    last_event_time = simulation_time;

    /* Update the area under the queue length function */
    area_num_arrivals_queue += num_arrivals_queue * time_since_last_event;

    /* Update the area under the server status indicator function */
    area_server_status += server_status * time_since_last_event;
}
