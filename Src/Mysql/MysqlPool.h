#ifndef _ORACLE_POOL_H_
#define _ORACLE_POOL_H_

#include <string>
#include <time.h>
#include <mysql/mysql.h>
#include "Mutex.h"

typedef struct tagMYCONNECTION{
	MYSQL *pmysql;
	time_t last_active;
	int valid; //1--valid, 0--invalid.
	int idle;  //1--idle,  0--busy.
}*HMYCONNECTION;
#define DMYSQLPOOL_MAX_CONNECTION_NUM 200

class MysqlPool{
public:
	MysqlPool(const char *host, unsigned short port, const char *user, const char *password,const char *dbname, int idle_timeout=600); //µ•Œª£∫√Î
	virtual ~MysqlPool();
	void Drop(HMYCONNECTION hpool);
	HMYCONNECTION Get();
	void Release(HMYCONNECTION hpool);
	
protected:
	HMYCONNECTION NewConnection();
	void CloseConnections();

	char m_host[128],m_user[128],m_password[128],m_dbname[80];
	unsigned short m_port;
	int m_idleTimeout;
	HMYCONNECTION m_hmypool[DMYSQLPOOL_MAX_CONNECTION_NUM];
	int m_currentCount;
	MyMutex m_mutex;
};

#endif

