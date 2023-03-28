#ifndef TIMING_H_ /* Include guard */
#endif /* TIMING_H_ */
#define TIMING_H_ /* Include guard */

#include <time.h>

#ifdef USE_CLOCK /* Use clock() instead of clock_gettime() */

// Define the timing_t type to be clock_t when using clock()
typedef clock_t timing_t;

// Macro to get the current time using clock()
#define get_time(tref) ((*tref) = clock())

// Macro to compute the difference between two clock_t values as a double
#define timespec_diff(t1, t2) ((double)(t2 - t1) / CLOCKS_PER_SEC)

#else

// Define the timing_t type to be a timespec struct when using clock_gettime()
typedef struct timespec timing_t;

// Convert a timespec struct to a long double value
long double timespec_to_dbl(struct timespec x);

// Compute the difference between two timespec structs as a long double value
long double timespec_diff(struct timespec start, struct timespec finish);

// Get the resolution of the CLOCK in seconds as a long double value
long double timer_resolution(void);

// Get the current time as a timespec struct
void get_time(struct timespec *);

#endif /* TIMING_H_ */