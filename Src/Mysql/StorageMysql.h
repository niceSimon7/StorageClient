#ifndef CSTORAGEMYSQL_H_INCLUDED
#define CSTORAGEMYSQL_H_INCLUDED

#define DMAX_REMOTE_MYSQL_COUNT 16
#define DMAX_QUEUED_SQL_TASK_COUNT 100000

#define SQL_MODE_SYNC 1
#define SQL_MODE_ASYNC 2

#include <pthread.h>
#include <memory>
#include <sstream>
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
#include "Config.h"
#include "StorageBase.h"

using namespace std;

class CStorageMysql : public CStorageBase
{
public:
    CStorageMysql();
    ~CStorageMysql();
    EECode InitStorage();
    EECode DeInitStorage();
    static CStorageMysql* Create();
    EECode ReadFromDB(IN EDBDataType eDBDataType, IN u32 dwMaxRowCount, IN OUT void* pData);
    EECode WriteToDB(IN EDBDataType eDBDataType, IN s32 sdwWriteMode, IN void* pData);
    EECode mysqlDelete(string& pbySql);

private:
    BOOL mysqlCheckReady();
    EECode mysqlQuery(string& pbySql, vector<map<string,string> >& vResultMap, u32 dwMaxRowCount);
    EECode mysqlExecute(string& pbySql, s32 sdwWriteMode);

    struct TMysqlWorkArea* createMysqlWorkArea(const s8* pbyHost, u16 wPort, const s8* pbyUser, const s8* pbyPassword, const s8* pbyDefaultDBName);
    void destroyMysqlWorkArea(struct TMysqlWorkArea* ptMysqlWorkArea);

    s8 mabyDefaultDBName[80] = {0, };
    struct TMysqlWorkArea* mptLocalDatabase = NULL;
    u32 mdwRemoteCount;
    struct TMysqlWorkArea* maptRemoteDatabase[DMAX_REMOTE_MYSQL_COUNT] = {NULL, };
};

struct TMysqlWorkArea
{
    MysqlPool* mpMysqlPool;
    pthread_t mhThread;
    BOOL mbExitFlag;

    list<unique_ptr<string> > mlistTasks;
    pthread_mutex_t mTaskMutex;
    pthread_cond_t mTaskReadyEvent;

    BOOL mbDatabaseReady;
    pthread_mutex_t mDatabaseReadyMutex;
};

//void* MysqlWorkThreadProc(void* lpParameter);

#endif
