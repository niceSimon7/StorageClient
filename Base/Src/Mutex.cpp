#include <stdio.h>
#include "Mutex.h"


MyMutex::MyMutex()
{
    int nRet;
    nRet = pthread_mutex_init(&_cri, NULL);
    if (0!=nRet)
    {
        printf("pthread_mutex_init() error.\n");
    }
}

MyMutex::~MyMutex()
{
    pthread_mutex_destroy(&_cri);
}

void MyMutex::Lock()
{
    int nRet;
    nRet = pthread_mutex_lock(&_cri);
    if (0!=nRet)
    {
        printf("pthread_mutex_lock() error.\n");
    }
}

void MyMutex::Unlock()
{
    int nRet;
    nRet = pthread_mutex_unlock(&_cri);
    if (0!=nRet)
    {
        printf("pthread_mutex_unlock() error.\n");
    }
}
