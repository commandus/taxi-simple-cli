#include "utiltime.h"

#include <stdio.h>
#include <ctime>

#ifdef WIN32
#include <string>
#include <sstream>
#include <iomanip>
#endif

#include "boost/detail/endian.hpp"

using namespace ::taxi;


bool isBigEndian()
{
    union 
	{
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

int BoostEndian()
{
	int r = 0;
#ifdef BOOST_BIG_ENDIAN
	r = 1;
#endif
#ifdef BOOST_LITTLE_ENDIAN
	r = 2;
#endif
#ifdef BOOST_PDP_ENDIAN
	r = 3;
#endif
	return r;
}

time_t currentTime()
{
	time_t r(time(NULL));
	return r;
}

/*
  returns the utc timezone offset
  (e.g. -8 hours for PST)
*/
int get_utc_offset() {
	time_t zero = 24*60*60L;
	struct tm * timeptr;
	int gmtime_hours;

	/* get the local time for Jan 2, 1900 00:00 UTC */
	timeptr = localtime(&zero);
	gmtime_hours = timeptr->tm_hour;

	/* if the local time is the "day before" the UTC, subtract 24 hours from the hours to get the UTC offset */
	if( timeptr->tm_mday < 2 )
		gmtime_hours -= 24;
	return gmtime_hours;
}

/*
	the utc analogue of mktime, (much like timegm on some systems)
*/
time_t tm2time_tUTC(struct tm* tm) 
{
	// printf("%d %d %d %d %d\n", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
	/* gets the epoch time relative to the local time zone, and then adds the appropriate number of seconds to make it UTC */	
	time_t r = mktime(tm) + (get_utc_offset() * 3600);
	return r;
}

#ifdef WIN32
/**
	argtable does not provide arg_dateX functions 
	strptime implementation
*/
char *strptime(const char *buf, const char *format, struct tm *tm)
{
	// std::tm tm;
	std::stringstream ss(buf);
	ss >> std::get_time(tm, format);
	tm2time_tUTC(tm);
	return NULL;
}

/* local error codes  */
enum {EMINCOUNT=1,EMAXCOUNT,EBADDATE};

static void resetfn(struct arg_date *parent)
    {
    /*printf("%s:resetfn(%p)\n",__FILE__,parent);*/
    parent->count=0;
    }

static int scanfn(struct arg_date *parent, const char *argval)
    {
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount )
        errorcode = EMAXCOUNT;
    else if (!argval)
        {
        /* no argument value was given, leave parent->tmval[] unaltered but still count it */
        parent->count++;
        }
    else 
        {
        const char *pend;
        struct tm tm = parent->tmval[parent->count];

        /* parse the given argument value, store result in parent->tmval[] */
        pend = strptime(argval, parent->format, &tm);
        if (pend && pend[0]=='\0')
            parent->tmval[parent->count++] = tm;
        else
            errorcode = EBADDATE;
        }

    /*printf("%s:scanfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
    }

static int checkfn(struct arg_date *parent)
    {
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;
     
    /*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
    }

static void errorfn(struct arg_date *parent, FILE *fp, int errorcode, const char *argval, const char *progname)
    {
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp,"%s: ",progname);
    switch(errorcode)
        {
        case EMINCOUNT:
            fputs("missing option ",fp);
            arg_print_option(fp,shortopts,longopts,datatype,"\n");
            break;

        case EMAXCOUNT:
            fputs("excess option ",fp);
            arg_print_option(fp,shortopts,longopts,argval,"\n");
            break;

        case EBADDATE:
            {
            struct tm tm;
            char buff[200];           

            fprintf(fp,"illegal timestamp format \"%s\"\n",argval);
            memset(&tm, 0, sizeof(tm));
            strptime("1999-12-31 23:59:59","%F %H:%M:%S",&tm);
            strftime(buff, sizeof(buff), parent->format, &tm);
            printf("correct format is \"%s\"\n", buff);                        
            break;
            }
        }
    }

struct arg_date* arg_date0(const char* shortopts,
                           const char* longopts,
                           const char* format,
                           const char *datatype,
                           const char *glossary)
{
	struct arg_date *result;
	size_t nbytes = sizeof(struct arg_date) + sizeof(struct tm);
    result = (struct arg_date*) malloc(nbytes);

    if (result)
	{
        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : format;
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = 0;
        result->hdr.maxcount  = 1;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn*)resetfn;
        result->hdr.scanfn    = (arg_scanfn*)scanfn;
        result->hdr.checkfn   = (arg_checkfn*)checkfn;
        result->hdr.errorfn   = (arg_errorfn*)errorfn;

        /* store the tmval[maxcount] array immediately after the arg_date struct */
        result->tmval  = (struct tm*)(result+1);

        /* init the remaining arg_date member variables */
        result->count = 0;
        result->format = format;
	}
	return result;
}
#endif


/**
	Return 0- Sunday, 6- Saturday
*/
int getWeekDay(time_t time)
{
	struct tm *timeptr = localtime(&time);
	return timeptr->tm_wday;
}

/**
	Return 0- January, 11- December
*/
int getMonth(time_t time)
{
	struct tm *timeptr = localtime(&time);
	return timeptr->tm_mon;
}

/**
	Return current month number: 0 - 11
*/
taxi::Month::type getTaxiMonth(time_t time)
{
	taxi::Month::type r;
	struct tm *tm = localtime(&time);
	r = static_cast<taxi::Month::type> (tm->tm_mon);
	return r;
}


/**
	Return hour, GMT!: 0 - 23
*/
int getGMTHour(time_t time)
{
	struct tm *tm = gmtime(&time);
	return (tm->tm_hour); 
}


/**
	Return current year
*/
int getCurrentYear()
{
	time_t r(time(NULL));
	struct tm *tm = localtime(&r);
	return tm->tm_year + 1900;
}

/**
	Return true if it is Sunday, Saturday or holiday
*/
int isWeekend(int64_t cityid, time_t time)
{
	int wd = getWeekDay(time);
	return ((wd == 0) || (wd == 6)) || isHoliday(cityid, time);
}

/**
	Return true if it is holiday in the city
*/
int isHoliday(int64_t cityid, time_t time)
{
	return false;
}
