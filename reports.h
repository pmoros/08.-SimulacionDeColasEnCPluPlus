#include <stdio.h>
void init_report(_IO_FILE *results, float mean_interarrival, float mean_service, int required_wait_count);
void report_metrics(_IO_FILE *results, float *metrics);