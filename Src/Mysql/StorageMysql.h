#ifndef CSTORAGEMYSQL_H_INCLUDED
#define CSTORAGEMYSQL_H_INCLUDED

#define MAX_REMOTE_MYSQL_COUNT 16      //同步远程数据库数量限制
#define MAX_QUEUED_SQL_TASK_COUNT 4000 //SQL工作线程积压的任务限制
#define SQL_MODE_SYNC   1              //SQL执行模式：同步
#define SQL_MODE_ASYNC  2              //SQL执行模式：异步

#define TICK_TYPE_SECOND      0
#define TICK_TYPE_MILLISECOND 1
#define TICK_TYPE_MICROSECOND 2
#define TICK_TYPE_TENSECOND   3

#include <list>
#include "MysqlPool.h"
#include "Event.h"
#include "Mutex.h"
#include <pthread.h>
#include "StorageBase.h"
#include <string.h>
#include <mysql/errmsg.h>
#include <pthread.h>
#include "CommCodec.h"
#include "time.h"
#include "BaseType.h"
#include <vector>
#include <map>
#include <algorithm>
#include "Time.h"

using namespace std;

class CStorageMysql : public CStorageBase
{
    friend class TestCStorageMysql;
public:
    EECode ReadFromDB(IN EDBDataType eDBDataType, IN OUT void* pData);
    EECode WriteToDB(IN EDBDataType eDBDataType, IN s32 sdwWriteMode, IN void* pData);

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

    EECode Mysql_Query(const s8* pszSql, s8 **ppResult, s32 *pdwRowCount);
    EECode Mysql_Execute(const s8* pszUpdateSql, s32 nLocalRunMode);
    EECode DeleteFromDB(IN s8* sbySql);

    BOOL mysqlCheckReady();
    struct _S_MysqlWorkArea* createMysqlWorkArea(const s8 *szHost, unsigned short usPort, const s8 * szUser, const s8 *szPassword, const s8 *szDefaultDBName);
    void destroyMysqlWorkArea(struct _S_MysqlWorkArea *pMysqlWorkArea);
    s8 *mysqlReplaceSupportedPatterns(const s8 *szOriginalSQL);

private:
    s8 m_szDefaultDBName[80];
    struct _S_MysqlWorkArea *m_localdb;
    struct _S_MysqlWorkArea *m_remotedbs[MAX_REMOTE_MYSQL_COUNT];
    s32 m_nRemoteCount;
    s32 m_nMaxRowCount; //Query MaxRowCount

    BOOL m_bExitFlag;
    BOOL m_bLocalDBReady;
    int64_t m_qwLastCheckTick; //记录上次check时间，秒检测一次，避免浪费资源

    f64 dfBegin, dfFinish;
};

struct _S_MysqlWorkArea
{
    CStorageMysql *m_pmysql; //每一个工作区记录一个所属CStorageMysql对象的指针
    MysqlPool *pMysqlPool;

    pthread_t hThread;
    MyEvent taskEvent;
    MyMutex taskMutex;

    list<s8*> tasks;
    s32 nTaskCount;
};

//void* MysqlWorkThreadProc(void* lpParameter);

#endif // CSTORAGEMYSQL_H_INCLUDED
