#include <stdio.h>
#include <ctime>
#include "argtable2.h"

time_t currentTime();

/* returns the utc timezone offset (e.g. -8 hours for PST) */
int get_utc_offset();

/* the utc analogue of mktime, (much like timegm on some systems) */
time_t tm2time_tUTC( struct tm * timeptr );

#ifdef WIN32
struct arg_date* arg_date0(const char* shortopts,
                           const char* longopts,
                           const char* format,
                           const char *datatype,
                           const char *glossary);
#endif
