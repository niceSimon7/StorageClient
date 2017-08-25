#if !defined(_MYEVENT_H_)
#define _MYEVENT_H_
#include <sys/time.h>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <iostream>

class MyEvent
{
public:
    MyEvent();
    virtual ~MyEvent();

    //Wait a signal to be signed.
    //return:
    //  0 -- timeout.
    //  1 -- have a event.
    int Wait(int nTimeout);  //单位：毫秒


    //Tiger a signal.
    void Post();

protected:
    pthread_mutex_t _mutex;
    pthread_cond_t  _cond;
    //timespec _tsp;

private:
    void Maketimeout(struct timespec *tsp,int nTimeout); //单位：毫秒

};

#endif
