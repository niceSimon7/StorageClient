#ifndef CSTORAGEMYSQL_H_INCLUDED
#define CSTORAGEMYSQL_H_INCLUDED

#define MAX_REMOTE_MYSQL_COUNT 16
#define MAX_QUEUED_SQL_TASK_COUNT 4000
#define MAX_COLUME_NUM 100

#define SQL_MODE_SYNC 1
#define SQL_MODE_ASYNC 2

#include <pthread.h>
#include <string.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <mysql/errmsg.h>

#include "Time.h"
#include "Char.h"
#include "time.h"
#include "BaseType.h"
#include "MysqlPool.h"
#include "Event.h"
#include "Mutex.h"
#include "Config.h"
#include "StorageBase.h"

using namespace std;

class CStorageMysql : public CStorageBase
{
public:
    EECode ReadFromDB(IN EDBDataType eDBDataType, IN OUT void* pData, u32 dwMaxRowCount);
    EECode WriteToDB(IN EDBDataType eDBDataType, IN s32 sdwWriteMode, IN void* pData);
    EECode DeleteFromDB(IN s8* sbySql);

public:
    static CStorageMysql* Create();

    EECode InitStorage();
    EECode DeInitStorage();

    inline BOOL GetExitFlag()
    {
        return m_bExitFlag;
    }
    inline void SetExitFlag(BOOL bExitFlag)
    {
        m_bExitFlag = bExitFlag;
    }

private:
    CStorageMysql();
    ~CStorageMysql();

    EECode Mysql_Query(const s8* pszSql, vector<map<string,string> >& vResult, u32 dwMaxRowCount);
    EECode Mysql_Execute(const s8* pszUpdateSql, s32 nLocalRunMode);

    BOOL mysqlCheckReady();
//    void connectMysql();
    struct TMysqlWorkArea* createMysqlWorkArea(const s8 *szHost, unsigned short usPort, const s8 * szUser, const s8 *szPassword, const s8 *szDefaultDBName);
    void destroyMysqlWorkArea(struct TMysqlWorkArea *pMysqlWorkArea);
    s8 *mysqlReplaceSupportedPatterns(const s8 *szOriginalSQL);


private:
    s8 m_szDefaultDBName[80];
    struct TMysqlWorkArea *m_localdb;
    struct TMysqlWorkArea *m_remotedbs[MAX_REMOTE_MYSQL_COUNT];

    u32 m_nRemoteCount;
    u32 m_nMaxRowCount; //Query MaxRowCount
    BOOL m_bExitFlag;
    BOOL m_bLocalDBReady;
};

struct TMysqlWorkArea
{
    CStorageMysql *m_pmysql; //每一个工作区记录一个所属CStorageMysql对象的指针
    MysqlPool *pMysqlPool;

    pthread_t hThread;
    MyEvent taskEvent;
    MyMutex taskMutex;

    list<s8*> tasks;
    u32 nTaskCount;
};

//void* MysqlWorkThreadProc(void* lpParameter);

#endif // CSTORAGEMYSQL_H_INCLUDED
