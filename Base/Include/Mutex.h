#ifndef _MUTEX_H_
#define _MUTEX_H_


#include <pthread.h>

class MyMutex
{
public:
    MyMutex();
    virtual ~MyMutex();
    virtual void Lock();
    virtual void Unlock();
protected:
    pthread_mutex_t _cri;

};

#endif

