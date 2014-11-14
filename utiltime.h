#include <stdio.h>
#include <ctime>
#include "argtable2.h"
#include "taxi_types.h"

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

/**
	Return 0- Sunday, 6- Saturday
*/
int getWeekDay(time_t time);

/**
	Return true if it is Sunday, Saturday or holiday
*/
int isWeekend(int64_t cityid, time_t time);

/**
	Return true if it is holiday in the city
*/
int isHoliday(int64_t cityid, time_t time);

/**
	Return 0- January, 11- December
*/
int getMonth(time_t time);

/**
	Return hour, GMT!: 0 - 23
*/
int getGMTHour(time_t time);

/**
	Return current month number: 0 - 11
*/
taxi::Month::type getTaxiMonth(time_t time);

/**
	Return current year
*/
int getCurrentYear();

/**
	Return 2014
*/
int extractYear(time_t time);
