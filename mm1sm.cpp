/* External definitions for simple queue system */

#include "erlang.cpp"  /* Erlang C function estimator */
#include "lcgrand.cpp" /* Header for random number generator */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define QUEUE_LIMIT 100 /* Maximum queue capacity */
#define BUSY 1          /* Server busy indicator */
#define IDLE 0          /* Server idle indicator */

int next_event_type, num_customers_waiting, required_wait_count, num_events,
    num_arrivals_queue, server_status;
float area_num_arrivals_queue, area_server_status, mean_interarrival, mean_service,
    simulation_time, arrival_time[QUEUE_LIMIT + 1], last_event_time, next_event_time[3],
    total_wait_time, erlang_c_estimation;
FILE *parameters, *results;

void initialize(void);
void timing(void);
void arrival(void);
void departure(void);
void reports(void);
void update_average_wait_time(void);
float exponential(float mean);

int main(void) /* Main function */
{
    /* Open input and output files */
    parameters = fopen("param.txt", "r");
    results = fopen("result.txt", "w");

    /* Set the number of events for the timing function */
    num_events = 2;

    /* Read the input parameters */
    fscanf(parameters, "%f %f %d", &mean_interarrival, &mean_service,
           &required_wait_count);

    /* Write the report headers and initial parameters to the output file */
    fprintf(results, "Simple Queue System\n\n");
    fprintf(results, "Mean interarrival time%11.3f minutes\n\n",
            mean_interarrival);
    fprintf(results, "Mean service time%16.3f minutes\n\n", mean_service);
    fprintf(results, "Number of customers%14d\n\n", required_wait_count);

    /* Initialize the simulation */
    initialize();

    /* Run the simulation until the required number of customers arrive */
    while (num_customers_waiting < required_wait_count) {
        /* Determine the next event */
        timing();

        /* Update average timing statistics */
        update_average_wait_time();

        /* Invoke the appropriate event function */
        switch (next_event_type) {
        case 1:
            arrival();
            break;
        case 2:
            departure();
            break;
        }
    }

    printf("asdasdasd");

    erlang_c_estimation = erlang_c(required_wait_count, mean_service);

    printf("aaaa");

    /* Generate the reports and end the simulation */
    reports();

    fclose(parameters);
    fclose(results);

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
    erlang_c_estimation = 0.0;

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
    for (i = 1; i <= num_events; ++i) {
        if (next_event_time[i] < min_next_event_time) {
            min_next_event_time = next_event_time[i];
            next_event_type = i;
        }
    }

    /* Check if the event list is empty */
    if (next_event_type == 0) {
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
    if (server_status == BUSY) {
        /* Server is BUSY, increase the number of customers in the queue */

        ++num_arrivals_queue;

        /* Check for overflow condition */
        if (num_arrivals_queue > QUEUE_LIMIT) {
            /* The queue has overflowed, stop the simulation */
            fprintf(results, "\nOverflow of arrival time array at time");
            fprintf(results, " %f", simulation_time);
            exit(2);
        }

        /* There is still space in the queue, store the arrival time of the
            customer at the (new) end of arrival_time array */
        arrival_time[num_arrivals_queue] = simulation_time;
    } else {
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
    if (num_arrivals_queue == 0) {
        /* The queue is empty, make the server IDLE and
        do not consider the departure event */
        server_status = IDLE;
        next_event_time[2] = 1.0e+30;
    } else {
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
void reports(void) /* Report generation function */
{
    /* Calculate and estimate the desired performance measures */
    fprintf(results, "\n\nAverage wait time in the queue%11.3f minutes\n\n",
            total_wait_time / num_customers_waiting);
    fprintf(results, "Average number in the queue%10.3f\n\n",
            area_num_arrivals_queue / simulation_time);
    fprintf(results, "Server utilization%15.3f\n\n",
            area_server_status / simulation_time);
    fprintf(results, "Time simulation ended%12.3f minutes", simulation_time);
    fprintf(results, "Erlang C function estimation is %1.3f", erlang_c_estimation);
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

float exponential(float mean) /* Exponential random variable generator function */
{
    /* Return an exponential random variable with mean "mean" */

    return -mean * log(lcgrand(1));
}
