#include "utiltime.h"

#include <stdio.h>
#include <ctime>

#ifdef WIN32
#include <string>
#include <sstream>
#include <iomanip>
#endif

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
	timeptr = localtime( &zero );
	gmtime_hours = timeptr->tm_hour;

	/* if the local time is the "day before" the UTC, subtract 24 hours from the hours to get the UTC offset */
	if( timeptr->tm_mday < 2 )
		gmtime_hours -= 24;
	return gmtime_hours;
}

/*
	the utc analogue of mktime, (much like timegm on some systems)
*/
time_t tm2time_tUTC(struct tm* timeptr) {
  /* gets the epoch time relative to the local time zone, and then adds the appropriate number of seconds to make it UTC */
  return mktime(timeptr) + get_utc_offset() * 3600;
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
