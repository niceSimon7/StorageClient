#include "StorageMysql.h"
#include "MapArrayToStructArray.h"
#include "String.h"
#include <iostream>

using namespace std;

extern TStaticEnumMap g_tStaticEnumMap;
extern BOOL gfTimeValSys2Mysql(IN timeval sysTime, IN OUT s8 *sbyMysqlDATETIME3, IN u32 dwBufLen);


CStorageMysql* CStorageMysql::Create()
{
    CStorageMysql* pStorageMysql = new CStorageMysql();
    if ((pStorageMysql) && (EECode_OK != pStorageMysql->Construct()))
    {
        delete pStorageMysql;
        pStorageMysql = NULL;
    }
    return pStorageMysql;
}


EECode CStorageMysql::InitStorage()
{
    s8 abyHost[80], abyUser[80], abyPassword[80];
    s8 abyBuf[80];
    u16 wPort;

    Cfg_SectionItem("global_server", "default_dbname", mabyDefaultDBName, sizeof(mabyDefaultDBName));
    Cfg_MysqlServer("mysql_local", abyHost, sizeof(abyHost), &wPort, abyUser, sizeof(abyUser), abyPassword, sizeof(abyPassword));
    mptLocalDatabase = createMysqlWorkArea(abyHost, wPort, abyUser, abyPassword, mabyDefaultDBName);
    if(!mptLocalDatabase)
    {
//        LogMessage("CStorageMysql::InitStorage, Cannot create localdb!");
        return EECode_NotInitilized;
    }

    for(s32 i=0; i<(s32)(sizeof(maptRemoteDatabase)/sizeof(maptRemoteDatabase[0])); i++)
    {
        sprintf(abyBuf, "mysql_remote%d", i+1);
        Cfg_MysqlServer(abyBuf, abyHost, sizeof(abyHost), &wPort, abyUser, sizeof(abyUser), abyPassword, sizeof(abyPassword));
        if(strlen(abyHost) == 0)
        {
            break;
        }
        maptRemoteDatabase[i] = createMysqlWorkArea(abyHost, wPort, abyUser, abyPassword, mabyDefaultDBName);
        if(!maptRemoteDatabase[i])
        {
//            LogMessage("CStorageMysql::InitStorage, Create remote db%u workarea failed", i);
            return EECode_NotInitilized;
        }
        mdwRemoteCount++;
    }

    return EECode_OK;
}


EECode CStorageMysql::DeInitStorage()
{
    destroyMysqlWorkArea(mptLocalDatabase);
    mptLocalDatabase = NULL;

    for(s32 i=0; i<mdwRemoteCount; i++)
    {
        destroyMysqlWorkArea(maptRemoteDatabase[i]);
        maptRemoteDatabase[i] = NULL;
    }
    mdwRemoteCount = 0;
    return EECode_OK;
}


CStorageMysql::CStorageMysql() :
    mdwRemoteCount(0)
{
}

CStorageMysql::~CStorageMysql()
{
}

EECode CStorageMysql::mysqlQuery(string& pbySql, vector<map<string,string> >& vResultMap, u32 dwMaxRowCount)
{
    s32 i, sdwReturn;
    HMYCONNECTION hconn;
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;
    MYSQL_FIELD *fields = NULL;
    u32 dwFieldCount, dwRowCount;
    vector<string> vFieldNames;
    map<string, string > mapRow;
    vResultMap.clear();
    vResultMap.reserve(dwMaxRowCount);
    EECode eStatus = EECode_OK;

    if (!mysqlCheckReady())
    {
//        LogMessage("CStorageMysql::mysqlQuery, database is not ready!");
        return EECode_NotRunning;
    }
    hconn = mptLocalDatabase->mpMysqlPool->Get();
    if (!hconn)
    {
//        LogMessage("CStorageMysql::mysqlQuery, get database connection failed!");
        return EECode_NotInitilized;
    }

    DatabaseReplaceSupportedPatterns(pbySql);

    //执行查询
    sdwReturn = mysql_query(hconn->pmysql, pbySql.c_str());
    if (0==sdwReturn)
    {
        res = mysql_use_result(hconn->pmysql);
        if (res)
        {
            //第一行，写入列名。假设列名不含中文，可以视为utf8编码，不用转换字符集。
            dwFieldCount = mysql_num_fields(res);
            vFieldNames.reserve(dwFieldCount);
            fields = mysql_fetch_fields(res);
            for (i=0; i<dwFieldCount; i++)
            {
                vFieldNames.push_back(StringToLower(fields[i].name));
            }

            //写入数据（后续N行）。
            dwRowCount = 0;
            while((row=mysql_fetch_row(res))!=NULL)
            {
                if (dwRowCount>=dwMaxRowCount)
                {
//                    LogMessage("CStorageMysql::mysqlQuery, rows too many!");
                    eStatus = EECode_TooMany;
                    break;
                }

                mapRow.clear();
                for (i=0; i<(s32)dwFieldCount; i++)
                {
                    mapRow[vFieldNames[i]] = row[i]?row[i]:"";
                }
                vResultMap.push_back(mapRow);
                dwRowCount++;
            }
            mysql_free_result(res);
        }
    }
    else
    {
        if(CR_SERVER_GONE_ERROR==sdwReturn || CR_SERVER_LOST==sdwReturn || CR_COMMANDS_OUT_OF_SYNC==sdwReturn || CR_UNKNOWN_ERROR==sdwReturn)
        {
//            LogMessage("CStorageMysql::mysqlQuery, mysql_query() connection error!");
            mptLocalDatabase->mpMysqlPool->Drop(hconn);
        }
        else
        {
//            LogMessage("CStorageMysql::mysqlQuery, mysql_query() failed: Error %u (%s)", mysql_errno(hconn->pmysql), mysql_error(hconn->pmysql));
            mptLocalDatabase->mpMysqlPool->Release(hconn);
        }
        hconn = NULL;
        return EECode_Error;
    }

    if (hconn)
    {
        mptLocalDatabase->mpMysqlPool->Release(hconn);
    }

    return eStatus;
}

EECode CStorageMysql::mysqlExecute(string& pbySql, s32 sdwWriteMode)
{
    HMYCONNECTION hconn = NULL;

    if (!mysqlCheckReady())
    {
//        LogMessage("CStorageMysql::mysqlExecute, database is not ready!");
        return EECode_NotRunning;
    }

    //更新远程库
    for (s32 i=0; i<mdwRemoteCount; i++)
    {
        pthread_mutex_lock(&(maptRemoteDatabase[i]->mTaskMutex));
        if (maptRemoteDatabase[i]->mlistTasks.size() < DMAX_QUEUED_SQL_TASK_COUNT)
        {
            unique_ptr<string> pSqlTask(new string(pbySql));
            if (!pSqlTask)
            {
                pthread_mutex_unlock(&(maptRemoteDatabase[i]->mTaskMutex));
//                LogMessage("can not allocate memory for pbySqlTask in mysqlExecute().");
                return EECode_Error;
            }
            if(maptRemoteDatabase[i]->mlistTasks.empty())
            {
                pthread_cond_signal(&(maptRemoteDatabase[i]->mTaskReadyEvent));
            }
            maptRemoteDatabase[i]->mlistTasks.push_back(move(pSqlTask));
        }
        else
        {
//            LogMessage("too many queued task in remotedb %d in mysqlExecute().", i);
        }
        pthread_mutex_unlock(&(maptRemoteDatabase[i]->mTaskMutex));
    }


    //更新本地库
    EECode eStatus = EECode_OK;
    if (SQL_MODE_ASYNC==sdwWriteMode)
    {
        //异步更新即可
        pthread_mutex_lock(&(mptLocalDatabase->mTaskMutex));
        if (mptLocalDatabase->mlistTasks.size() < DMAX_QUEUED_SQL_TASK_COUNT)
        {
            unique_ptr<string> pSqlTask(new string(pbySql));
            if (!pSqlTask)
            {
                pthread_mutex_unlock(&(mptLocalDatabase->mTaskMutex));
//                LogMessage("can not allocate memory for pbySqlTask 2 in mysqlExecute().");
                return EECode_Error;
            }
            if(mptLocalDatabase->mlistTasks.empty())
            {
                pthread_cond_signal(&(mptLocalDatabase->mTaskReadyEvent));
            }
            mptLocalDatabase->mlistTasks.push_back(move(pSqlTask));
        }
        else
        {
//            LogMessage("too many queued task in mptLocalDatabase in mysqlExecute().");
            eStatus = EECode_TooMany;
        }
        pthread_mutex_unlock(&(mptLocalDatabase->mTaskMutex));
    }
    else if(SQL_MODE_SYNC==sdwWriteMode)
    {
        //同步更新，马上执行该SQL语句
        hconn = mptLocalDatabase->mpMysqlPool->Get();
        if (hconn)
        {
            s32 sdwReturn = mysql_query(hconn->pmysql, pbySql.c_str());
            if (0!=sdwReturn)
            {
//                LogMessage("CStorageMysql::mysqlExecute, mysql_query() failed: Error %u (%s)", mysql_errno(hconn->pmysql), mysql_error(hconn->pmysql));

                if (CR_SERVER_GONE_ERROR==sdwReturn || CR_SERVER_LOST==sdwReturn || CR_COMMANDS_OUT_OF_SYNC==sdwReturn || CR_UNKNOWN_ERROR==sdwReturn)
                {
                    mptLocalDatabase->mpMysqlPool->Drop(hconn);
                }
                else
                {
                    mptLocalDatabase->mpMysqlPool->Release(hconn);
                }
                hconn = NULL;
                return EECode_Error;
            }
            mptLocalDatabase->mpMysqlPool->Release(hconn);
        }
        else
        {
//            LogMessage("CStorageMysql::mysqlExecute, workarea->mpMysqlPool->Get failed!");
            eStatus = EECode_NotInitilized;
        }
    }
    else
    {
        eStatus = EECode_BadParam;
    }
    return eStatus;
}


EECode CStorageMysql::ReadFromDB(IN EDBDataType eDBDataType, IN u32 dwMaxRowCount, IN OUT void* pData)
{
    vector<map<string, string> > vResultMap;


    if(!pData)
    {
//        LogMessage("CStorageMysql::ReadFromDB, pData is NULL!");
        return EECode_BadParam;
    }

    switch(eDBDataType)
    {
    case EDBDataType_Security:
    {
        string sSql = "select * from security;";
        EECode eStatus = mysqlQuery(sSql, vResultMap, dwMaxRowCount);
        if(eStatus != EECode_OK)
        {
            return eStatus;
        }

        TSecurityMapArrayToStructArray(vResultMap, *(vector<TSecurity>*)pData);
        break;
    }
    case EDBDataType_ProcessList:
    {
        string sSql = "show processlist;";
        EECode eStatus = mysqlQuery(sSql, vResultMap, dwMaxRowCount);
        if(eStatus != EECode_OK)
        {
            return eStatus;
        }

        TMysqlProcessListMapArrayToStructArray(vResultMap, *(vector<TMysqlProcessList>*)pData);
        break;
    }
    case EDBDataType_Status:
    {
        string sSql = "show status;";
        EECode eStatus = mysqlQuery(sSql, vResultMap, dwMaxRowCount);
        if(eStatus != EECode_OK)
        {
            return eStatus;
        }

        TMysqlStatusMapArrayToStructArray(vResultMap, *(vector<TMysqlStatus>*)pData);
        break;
    }
    default:
    {
        return EECode_BadParam;
    }
    }

    return EECode_OK;
}


EECode CStorageMysql::WriteToDB(IN EDBDataType eDBDataType, IN s32 sdwWriteMode, IN void* pData)
{
    s8 abySql[1024];
    BOOL bReturn;
    EECode eStatus;

    if(!pData)
    {
//        LogMessage("CStorageMysql::WriteToDB, pData is NULL!");
        return EECode_BadParam;
    }

    // write into database
    switch(eDBDataType)
    {
    case EDBDataType_Security:
    {
        vector<TSecurity> vSecurity = *(vector<TSecurity> *)pData;
        vector<TSecurity>::iterator vIt;
        s8 abyExpiration[32];

        for( vIt = vSecurity.begin(); vIt != vSecurity.end(); vIt++ )
        {
            bReturn = gfTimeValSys2Mysql(vIt->mtExpiration, abyExpiration, sizeof(abyExpiration));
            if(!bReturn)
            {
//                LogMessage("CStorageMysql::WriteToDB, TSecurity gfTimeValSys2Mysql failed for vIt->mtExpiration(mdwSecurityID=%u)", vIt->mdwSecurityID);
                return EECode_Error;
            }

            snprintf(abySql, sizeof(abySql), "insert into %s.security(securityid, securitysymbol, securitytype, listexchange, tradingexchange, currency, expiration, strike, optiontype, underlayingsecurityid, contractsize, ticksize, longmarginrate, shortmarginrate, generalmarginrate, isin, tradeable, expirable) \
values('%u', '%s', '%s', '%s', '%s', '%s', '%s', '%lf', '%s', '%u', '%u', '%lf', '%lf', '%lf', '%lf', '%s', b'%u', b'%u')", mabyDefaultDBName,
                     vIt->mdwSecurityID,
                     vIt->msSecuritySymbol.c_str(),
                     g_tStaticEnumMap.m_MapESecurityTypeValue2String[vIt->meSecurityType].c_str(),
                     g_tStaticEnumMap.m_MapEListExchangeValue2String[vIt->meListExchange].c_str(),
                     g_tStaticEnumMap.m_MapETradingExchangeValue2String[vIt->meTradingExchange].c_str(),
                     g_tStaticEnumMap.m_MapECurrencyValue2String[vIt->meCurrency].c_str(),
                     abyExpiration,
                     vIt->mdfStrike,
                     vIt->msOptionType.c_str(),
                     vIt->mdwUnderlayingSecurityID,
                     vIt->mdwContractSize,
                     vIt->mdfTickSize,
                     vIt->mdfLongMarginRate,
                     vIt->mdfShortMarginRate,
                     vIt->mdfGeneralMarginRate,
                     vIt->msISIN.c_str(),
                     vIt->mbTradeAble,
                     vIt->mbExpirAble);

            string sSql(abySql);
            eStatus = mysqlExecute(sSql, sdwWriteMode);
            if(EECode_OK != eStatus)
            {
//                LogMessage("CStorageMysql::WriteToDB, TSecurity mdwSecurityID = %u update db.security failed!", vIt->mdwSecurityID);
                return eStatus;
            }
            else
            {
                //LogMessage("CStorageMysql::WriteToDB, TSecurity mdwSecurityID = %u saving to sql...", vIt->mdwSecurityID);
            }
        }
        break;
    }
    default:
    {
        return EECode_BadParam;
    }
    }

    return EECode_OK;
}


EECode CStorageMysql::mysqlDelete(string& sSql)
{
    EECode eStatus = mysqlExecute(sSql, SQL_MODE_ASYNC);

    return eStatus;
}


//职能：重新启动后，检查数据库是否已同步过。
//返回值： true   -- 完成同步
//         false  -- 未完成同步
BOOL CStorageMysql::mysqlCheckReady()
{
    HMYCONNECTION hconn = NULL;
    BOOL bDBReady = true;

    if(!mptLocalDatabase)
    {
        return false;
    }

    for(int i=0; i< mdwRemoteCount; i++)
    {
        if(maptRemoteDatabase[i] == NULL)
        {
            return false;
        }
    }

    //Check localdb
    pthread_mutex_lock(&(mptLocalDatabase->mDatabaseReadyMutex));
    if(mptLocalDatabase->mbDatabaseReady == true)
    {
        //pass
    }
    else
    {
        hconn = mptLocalDatabase->mpMysqlPool->Get();
        if(!hconn)
        {
            mptLocalDatabase->mbDatabaseReady = false;
            bDBReady = false;
        }
        else
        {
            mptLocalDatabase->mpMysqlPool->Release(hconn);
            hconn = NULL;
        }
    }
    pthread_mutex_unlock(&(mptLocalDatabase->mDatabaseReadyMutex));

    //Check remote
    for(int i=0; i<mdwRemoteCount; i++)
    {
        pthread_mutex_lock(&(maptRemoteDatabase[i]->mDatabaseReadyMutex));
        if(maptRemoteDatabase[i]->mbDatabaseReady == true)
        {
            //pass
        }
        else
        {
            hconn = maptRemoteDatabase[i]->mpMysqlPool->Get();
            if(!hconn)
            {
                maptRemoteDatabase[i]->mbDatabaseReady = false;
                bDBReady = false;
            }
            else
            {
                maptRemoteDatabase[i]->mpMysqlPool->Release(hconn);
                hconn = NULL;
            }
        }
        pthread_mutex_unlock(&(maptRemoteDatabase[i]->mDatabaseReadyMutex));
    }

    return bDBReady;
}

void* _MysqlWorkThreadProc(void* lpParameter)
{
    struct TMysqlWorkArea *ptMysqlWorkArea;
    unique_ptr<string> pSql;
    s32 sdwReturn;
    s8 abyError[512];
    HMYCONNECTION hconn = NULL;

    ptMysqlWorkArea = (struct TMysqlWorkArea*)lpParameter;

    while(true)
    {
        // While mlistTasks empty, thread condition wait...
        pthread_mutex_lock(&(ptMysqlWorkArea->mTaskMutex));
        while(ptMysqlWorkArea->mlistTasks.empty())
        {
            pthread_cond_wait(&(ptMysqlWorkArea->mTaskReadyEvent), &(ptMysqlWorkArea->mTaskMutex));
        }

        // Get a task from mlistTasks
        pSql = move(ptMysqlWorkArea->mlistTasks.front());
        ptMysqlWorkArea->mlistTasks.pop_front();

        // Exit thread
        if(ptMysqlWorkArea->mbExitFlag == true) //Most situation not satisfied
        {
            // Finish all tasks
            if("Exit" == *pSql)
            {
                pthread_mutex_unlock(&(ptMysqlWorkArea->mTaskMutex));
                break;
            }
            else
            {
//                LogMessage("_MysqlWorkThreadProc, The remaining tasks are being processed.");
            }
        }
        pthread_mutex_unlock(&(ptMysqlWorkArea->mTaskMutex));

//        // When pbySql is NULL, continue next
//        if(!pbySql)
//        {
//            continue;
//        }

        // Execute task
        hconn = ptMysqlWorkArea->mpMysqlPool->Get();
        if (hconn)
        {
            sdwReturn = mysql_query(hconn->pmysql, pSql->c_str());
            if (0!=sdwReturn)
            {
                string sError = mysql_error(hconn->pmysql);
                if (1==sdwReturn && sError.compare(0, 15, "Duplicate entry")==0)
                {
                    //忽略 "Duplicate entry" 错误
                }
                else
                {
//                    LogMessage("_MysqlWorkThreadProc, mysqlQuery() failed: Error %u (%s)", mysql_errno(hconn->pmysql), mysql_error(hconn->pmysql));
                }
                if (CR_SERVER_GONE_ERROR==sdwReturn || CR_SERVER_LOST==sdwReturn || CR_COMMANDS_OUT_OF_SYNC==sdwReturn || CR_UNKNOWN_ERROR==sdwReturn)
                {
                    ptMysqlWorkArea->mpMysqlPool->Drop(hconn);
                }
                else
                {
                    ptMysqlWorkArea->mpMysqlPool->Release(hconn);
                }

                hconn = NULL;
            }
            if (hconn)
            {
                ptMysqlWorkArea->mpMysqlPool->Release(hconn);
            }
        }
        else
        {
//            LogMessage("_MysqlWorkThreadProc, ptMysqlWorkArea->mpMysqlPool->Get() failed");
        }
//        if(pbySql)
//        {
//            delete pbySql;
//        }
    }

    return 0;
}


struct TMysqlWorkArea* CStorageMysql::createMysqlWorkArea(const s8 *pbyHost, u16 wPort, const s8 *pbyUser, const s8 *pbyPassword, const s8 *pbyDefaultDBName)
{
    struct TMysqlWorkArea *tMysqlWorkArea;
    tMysqlWorkArea = new struct TMysqlWorkArea;
    if(!tMysqlWorkArea)
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, Cannot allocate memory for tMysqlWorkArea");
        return NULL;
    }

    tMysqlWorkArea->mbExitFlag = false; //置位
    tMysqlWorkArea->mbDatabaseReady = false;   //置位

    if(0 != pthread_mutex_init(&(tMysqlWorkArea->mTaskMutex), NULL))
    {
//        LogMessage("CStorageMysql::createMysqltMysqlWorkArea, pthread_mutex_init() for tMysqlWorkArea->mTaskMutex failed.");
        delete tMysqlWorkArea;
        return NULL;
    }


    if(0 != pthread_mutex_init(&(tMysqlWorkArea->mDatabaseReadyMutex), NULL))
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, pthread_mutex_init() for tMysqlWorkArea->mDatabaseReadyMutex failed.");
        pthread_mutex_destroy(&(tMysqlWorkArea->mTaskMutex));
        delete tMysqlWorkArea;
        return NULL;
    }

    if(0 != pthread_cond_init(&(tMysqlWorkArea->mTaskReadyEvent), NULL))
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, pthread_mutex_init() for tMysqlWorkArea->mDatabaseReadyMutex failed.");
        pthread_mutex_destroy(&(tMysqlWorkArea->mTaskMutex));
        pthread_mutex_destroy(&(tMysqlWorkArea->mDatabaseReadyMutex));
        delete tMysqlWorkArea;
        return NULL;
    }

    tMysqlWorkArea->mpMysqlPool = new MysqlPool(pbyHost, wPort, pbyUser, pbyPassword, pbyDefaultDBName);
    if(!tMysqlWorkArea->mpMysqlPool)
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, Cannot allocate memory for tMysqlWorkArea->mpMysqlPool");
        pthread_mutex_destroy(&(tMysqlWorkArea->mTaskMutex));
        pthread_mutex_destroy(&(tMysqlWorkArea->mDatabaseReadyMutex));
        pthread_cond_destroy(&(tMysqlWorkArea->mTaskReadyEvent));
        delete tMysqlWorkArea;
        return NULL;
    }

    s32 sdwReturn = pthread_create(&tMysqlWorkArea->mhThread, NULL, _MysqlWorkThreadProc, tMysqlWorkArea);
    if(0 != sdwReturn)
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, Cannot create mysql thread, pbyHost = %s", pbyHost);
        pthread_mutex_destroy(&(tMysqlWorkArea->mTaskMutex));
        pthread_mutex_destroy(&(tMysqlWorkArea->mDatabaseReadyMutex));
        pthread_cond_destroy(&(tMysqlWorkArea->mTaskReadyEvent));
        delete tMysqlWorkArea->mpMysqlPool;
        delete tMysqlWorkArea;
        return NULL;
    }
    return tMysqlWorkArea;
}


void CStorageMysql::destroyMysqlWorkArea(struct TMysqlWorkArea *ptMysqlWorkArea)
{
    if(!ptMysqlWorkArea)
    {
        return ;
    }

    ptMysqlWorkArea->mbExitFlag = true;     //退出工作区
    pthread_mutex_lock(&(ptMysqlWorkArea->mTaskMutex));
    unique_ptr<string> pExit(new string("Exit"));
    if(!pExit){
//        LogMessage("CStorageMysql::createMysqlWorkArea, new pExit failed!");
    }
    ptMysqlWorkArea->mlistTasks.push_back(move(pExit));
    pthread_cond_signal(&(ptMysqlWorkArea->mTaskReadyEvent)); //触发事件信号
    pthread_mutex_unlock(&(ptMysqlWorkArea->mTaskMutex));

    pthread_join(ptMysqlWorkArea->mhThread, NULL);  //工作区线程退出


    if(ptMysqlWorkArea->mpMysqlPool)
    {
        delete ptMysqlWorkArea->mpMysqlPool;
    }

    pthread_mutex_destroy(&(ptMysqlWorkArea->mTaskMutex));
    pthread_mutex_destroy(&(ptMysqlWorkArea->mDatabaseReadyMutex));
    pthread_cond_destroy(&(ptMysqlWorkArea->mTaskReadyEvent));

    delete ptMysqlWorkArea;
}
