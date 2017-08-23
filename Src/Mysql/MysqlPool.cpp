#include <string.h>
#include "MysqlPool.h"

extern time_t gfGetTrueTime(time_t *ptTick);

MysqlPool::MysqlPool(const char *host, unsigned short port, const char *user, const char *password, const char *dbname, int idle_timeout)
:m_currentCount(0)
{
	strncpy(m_host,host,sizeof(m_host));
	m_host[sizeof(m_host)-1] = 0;

	m_port = port;

	strncpy(m_user,user,sizeof(m_user));
	m_user[sizeof(m_user)-1] = 0;

	strncpy(m_password,password,sizeof(m_password));
	m_password[sizeof(m_password)-1] = 0;

	if (dbname)
	{
		strncpy(m_dbname, dbname, sizeof(m_dbname)-1);
		m_dbname[sizeof(m_dbname)-1] = 0;
	}
	m_idleTimeout = idle_timeout;
}

MysqlPool::~MysqlPool()
{
	int i;

	m_mutex.Lock();
	for(i=0;i<m_currentCount;i++){
		mysql_close(m_hmypool[i]->pmysql);
		delete m_hmypool[i];
	}
	m_currentCount=0;
	m_mutex.Unlock();
}

void MysqlPool::Drop(HMYCONNECTION hpool)
{
	hpool->valid = 0;
}

void MysqlPool::Release(HMYCONNECTION hpool)
{
	hpool->idle = 1;
}

HMYCONNECTION MysqlPool::Get()
{
	int i;
	HMYCONNECTION hpool = NULL;
	time_t tnow;

	gfGetTrueTime(&tnow);
	//try to find a valid and idle connection.
	m_mutex.Lock();
	//close valid and timeout connections.
	CloseConnections();
	for(i=0;i<m_currentCount;i++){
		if(m_hmypool[i]->idle==1 && m_hmypool[i]->valid==1){
			//use this.
			hpool = m_hmypool[i];
			hpool->idle = 0;
			hpool->last_active = tnow;
			break;
		}
	}
	m_mutex.Unlock();
	if(hpool){
		return hpool;
	}

	if (m_currentCount >= DMYSQLPOOL_MAX_CONNECTION_NUM)//2017.03.03 sq mdf, fix potential logic bug
	{
//		LogMessage("no idle connection and total connections=%d reach limit, can not create anymore.", m_currentCount);
		return NULL;
	}
//	LogMessage("MysqlPool::Get, no idle connection, create new connection, total=%d", m_currentCount);
	hpool = NewConnection();
	if(hpool){
		hpool->idle = 0;
		hpool->valid = 1;
		hpool->last_active = tnow;

		m_mutex.Lock();
		m_hmypool[m_currentCount] = hpool;
		m_currentCount++;
		m_mutex.Unlock();
	}else{
//		LogMessage("MysqlPool::NewConnection() failed!");
	}

	return hpool;
}

HMYCONNECTION MysqlPool::NewConnection()
{
	HMYCONNECTION hpool = new struct tagMYCONNECTION;
	if(hpool){
		hpool->idle = 1;
		hpool->valid = 1;
		hpool->pmysql = mysql_init(NULL);
		if(hpool->pmysql){
			int timeout;
			timeout = 15;
			mysql_options(hpool->pmysql,MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout);
			mysql_options(hpool->pmysql,MYSQL_SET_CHARSET_NAME, "utf8");
			if(mysql_real_connect(hpool->pmysql,m_host,m_user,m_password,m_dbname,m_port,NULL,0)){
				//do nothing.
			}else{
//				LogMessage("mysql_real_connect() failed: %s", mysql_error(hpool->pmysql));
				mysql_close(hpool->pmysql);
				delete hpool;
				hpool = NULL;
			}
		}else{
//			LogMessage("mysql_init() failed.");
			delete hpool;
			hpool = NULL;
		}
	}else{
//		LogMessage("Can not allocate memory for hpool in NewConnection()");
	}
	return hpool;
}

void MysqlPool::CloseConnections()
{
	int i,j;

	time_t tnow;
	gfGetTrueTime(&tnow);

	//valid timeout connections.
	for(i=0;i<m_currentCount;i++){
		if(m_hmypool[i]->idle==1 && m_hmypool[i]->valid==1){
			if(tnow-m_hmypool[i]->last_active > m_idleTimeout){
				m_hmypool[i]->valid = 0;
			}
		}
	}
	//close all failed connections.
	for(i=0;i<m_currentCount;i++){
		if(m_hmypool[i]->valid == 0){
			mysql_close(m_hmypool[i]->pmysql);
			for(j=i+1;j<m_currentCount;j++){
				m_hmypool[j-1] = m_hmypool[j];
			}
			m_currentCount--;
		}
	}
}

