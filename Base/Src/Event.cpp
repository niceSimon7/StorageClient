#include "Event.h"
#include <errno.h>
#include <string.h>


MyEvent::MyEvent()
{
	int iRet;
	iRet = pthread_mutex_init(&_mutex, NULL);
	if(iRet!=0)
	{
//		LogMessage("pthread_mutex_init() failed in MyEvent::MyEvent()");
		return;
	}

	iRet=pthread_cond_init(&_cond, NULL);
	if(iRet!=0)
	{
//		LogMessage("pthread_cond_init() failed 1 in MyEvent::MyEvent()");
		pthread_mutex_destroy(&_mutex);
		return;
	}
}

MyEvent::~MyEvent()
{
	pthread_mutex_destroy(&_mutex);
	pthread_cond_destroy(&_cond);
}

void MyEvent::Maketimeout(struct timespec *tsp,int nTimeout)
{
	struct timeval now;
	long add_sec,add_msec;
	long end_sec,end_nsec;
	add_sec = (int)(nTimeout/1000);
	add_msec = nTimeout - add_sec*1000;

	gettimeofday(&now,NULL);

	end_sec = now.tv_sec + add_sec;
	end_nsec = now.tv_usec*1000 + add_msec*1000000;

	if (end_nsec>1000000000) {
		end_sec ++;
		end_nsec = (end_nsec%1000000000);
	}
	tsp->tv_sec = end_sec;
	tsp->tv_nsec = end_nsec;
}



//Wait a signal to be signed.
//return:
//  0 -- timeout.
//  1 -- have a event.
int MyEvent::Wait(int nTimeout)
{
	int nRet,nStatus;
	time_t now;
	timespec _tsp;

	if (nTimeout>15000)
	{
//		LogMessage("Wait() nTimeout =%d",nTimeout);
		nTimeout = 15000;
	}
	if (nTimeout<0)
	{
//		LogMessage("Wait() nTimeout =%d",nTimeout);
		nTimeout = 0;
	}
    Maketimeout(&_tsp,nTimeout);
    if (_tsp.tv_sec<1320318247)
	{
//		LogMessage("Wait() time failure");
		time(&now);
		_tsp.tv_sec = now+1;
		_tsp.tv_nsec = (_tsp.tv_nsec%1000000000);
	}

	nRet = pthread_mutex_lock(&_mutex);
	if (0!=nRet)
	{
//		LogMessage("MyEvent::Wait() error 1 occured.err=%s",strerror(nRet));
		return 0;
	}

	nStatus = pthread_cond_timedwait(&_cond, &_mutex,&_tsp);


	nRet = pthread_mutex_unlock(&_mutex);
	if (0!=nRet)
	{
//		LogMessage("MyEvent::Wait() error 2 occured.err=%s",strerror(nRet));
	}
	if (0==nStatus)
	{
		//have a signal.
		return 1;
	}
	//timeout or other error.
	//LogMessage("MyEvent::Wait() pthread_cond_timedwait error occured.err=%s",strerror(nStatus));
	return 0;
}


//Tiger a signal.
void MyEvent::Post()
{
	int nRet;
	nRet = pthread_cond_signal(&_cond);
	if (0!=nRet)
	{
//		LogMessage("MyEvent::Post() error occured.,errno=%d",nRet);
	}
}
