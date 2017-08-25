#include "Time.h"
#include <stdio.h>

time_t gfGetTrueTime(time_t *tTick)
{
    return time(tTick);
}

//[TODO] for other storages, need add gfTimeValSys2xxx
BOOL gfTimeValSys2Mysql(IN timeval sysTime, IN OUT s8 *sbyMysqlDATETIME3, IN u32 dwBufLen)
{
    if (!sbyMysqlDATETIME3 || 0==dwBufLen)
    {
        return FALSE;
    }
    struct tm tTm;
    localtime_r(&(sysTime.tv_sec), &tTm);
    snprintf(sbyMysqlDATETIME3, dwBufLen, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             ((s32)(tTm.tm_year + 1900)), (s32)(tTm.tm_mon + 1), tTm.tm_mday, tTm.tm_hour, tTm.tm_min, tTm.tm_sec, sysTime.tv_usec/1000);

    return TRUE;
}

// 得到挂钟时间
double getWallSec(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (tv.tv_sec + (double)tv.tv_usec/1000000);
}
