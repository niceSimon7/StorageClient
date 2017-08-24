#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
#include "BaseType.h"
#include <time.h>

time_t gfGetTrueTime(time_t *tTick);

BOOL gfTimeValSys2Mysql(IN timeval sysTime, IN OUT s8 *sbyMysqlDATETIME3, IN u32 dwBufLen);
#endif // TIME_H_INCLUDED
