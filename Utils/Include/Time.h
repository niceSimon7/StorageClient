#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
#include "BaseType.h"
#include <time.h>

////////////////////////////////////////////////////////////
//返回系统从某个时间点开始的时间计数
//nType为时间单位:
//   TICK_TYPE_SECOND      0   -- unit is second.        秒
//   TICK_TYPE_MILLISECOND 1   -- unit is millisecond. 毫秒
//   TICK_TYPE_MICROSECOND 2   -- unit is microsecond. 微秒
s64 gfGetTickCount(int nType);

time_t gfGetTrueTime(time_t *tTick);

#endif // TIME_H_INCLUDED
