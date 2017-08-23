#include "Time.h"

////////////////////////////////////////////////////////////
//返回系统从某个时间点开始的时间计数
//nType为时间单位:
//   TICK_TYPE_SECOND      0   -- unit is second.        秒
//   TICK_TYPE_MILLISECOND 1   -- unit is millisecond. 毫秒
//   TICK_TYPE_MICROSECOND 2   -- unit is microsecond. 微秒
s64 gfGetTickCount(int nType)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    if (0 == nType)
    {
        return ((s64)tp.tv_sec);
    }

    if (1 == nType)
    {
        return ((s64)tp.tv_sec) * 1000 + ((s64)tp.tv_nsec) / 1000000;
    }

    if (2 == nType)
    {
        return ((s64)tp.tv_sec) * 1000000 + ((s64)tp.tv_nsec) / 1000;
    }

    if (3 == nType)
    {
        return ((s64)tp.tv_sec) * 10 + ((s64)tp.tv_nsec) / 100000000;
    }

    return 0;
}

time_t gfGetTrueTime(time_t *tTick)
{
    return time(tTick);
}

void gfGetLocalTime(timeval &localtime)
{
    struct tm tTm;
    time_t tNow;
    time(&tNow);
    localtime_r(&tNow, &tTm);
    localtime.tv_sec = mktime(&tTm);
    localtime.tv_usec = 0;
    return;
}
