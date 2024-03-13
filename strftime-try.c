#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_TIME_BUF 255

#define POPULATE(time,cell,inx,limit,arg) \
    if( inx >= limit ) return; \
    time->cell = atoi( arg[ inx++ ] );

void populate( struct tm *time, int argc, char **argv ) {
    int inx = 1;
    POPULATE( time, tm_sec,   inx, argc, argv );
    POPULATE( time, tm_min,   inx, argc, argv );
    POPULATE( time, tm_hour,  inx, argc, argv );
    POPULATE( time, tm_mday,  inx, argc, argv );
    POPULATE( time, tm_mon,   inx, argc, argv );
    POPULATE( time, tm_year,  inx, argc, argv );
    POPULATE( time, tm_wday,  inx, argc, argv );
    POPULATE( time, tm_yday,  inx, argc, argv );
    POPULATE( time, tm_isdst, inx, argc, argv );
    return;
}

int main( int argc, char **argv ) {
    int status = 0;
    struct tm time = {
	0,
	30,
	2,
	10,
	2,
	124,
	-1,
	-1,
	-1
    };
    char time_buf[MAX_TIME_BUF];
    populate( &time, argc, argv );
    if ( strftime( time_buf, MAX_TIME_BUF, "%Y-%m-%d %H:%M:%S", &time ) ) {
	fprintf( stdout, "%s\n", time_buf );
	return 0;
    } else {
	fprintf( stderr, "Conversion failed\n" );
	return 1;
    }
}
