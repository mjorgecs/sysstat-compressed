#include "sa.h"

// Minimal implementation of get_itv_value
void get_itv_value(struct record_header *record_hdr_curr,
                   struct record_header *record_hdr_prev,
                   unsigned long long *itv)
{
    if (record_hdr_curr->uptime_cs < record_hdr_prev->uptime_cs) {
        /* Counter wrapped */
        *itv = (unsigned long long) (~0ULL - record_hdr_prev->uptime_cs) + 
               record_hdr_curr->uptime_cs + 1;
    }
    else {
        *itv = record_hdr_curr->uptime_cs - record_hdr_prev->uptime_cs;
    }
}
