#include "timing.h"

// Define the CLOCK macro based on available clocks
#if defined(CLOCK_HIGHRES)
#define CLOCK CLOCK_HIGHRES
#elif defined(CLOCK_REALTIME)
#define CLOCK CLOCK_REALTIME
#elif defined(USE_CLOCK)
// If the user has specified a clock, use it
/* Nothing */
#else
// If no suitable clock is found, generate a compile-time error
#error "No suitable clock found. Check docs for clock_gettime."
#endif

#ifndef USE_CLOCK
#endif

// Convert a timespec struct to a long double value
long double timespec_to_ldbl(struct timespec x)
{
    return x.tv_sec + 1.0E-9 * x.tv_nsec;
}

// Compute the difference between two timespec structs as a long double value
long double timespec_diff(struct timespec start, struct timespec finish)
{
    long double out;
    out = finish.tv_nsec - (double)start.tv_nsec;
    out *= 1.0E-9L;
    out += finish.tv_sec - (double)start.tv_sec;
    return out;
}

// Get the resolution of the CLOCK in seconds as a long double value
long double timer_resolution()
{
    struct timespec x;
    clock_getres(CLOCK, &x);
    return timespec_to_ldbl(x);
}

// Get the current time as a timespec struct
void get_time(struct timespec *x)
{
    clock_gettime(CLOCK, x);
}