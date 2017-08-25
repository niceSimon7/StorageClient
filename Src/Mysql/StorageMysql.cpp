#include "StorageMysql.h"
#include "MapArrayToStructArray.h"
#include <iostream>

using namespace std;

extern TStaticEnumMap g_tStaticEnumMap;
extern BOOL gfTimeValSys2Mysql(IN timeval sysTime, IN OUT s8 *sbyMysqlDATETIME3, IN u32 dwBufLen);


CStorageMysql* CStorageMysql::Create()
{
    CStorageMysql *result = new CStorageMysql();
    if ((result) && (EECode_OK != result->Construct()))
    {
        delete result;
        result = NULL;
    }
    return result;
}


EECode CStorageMysql::InitStorage()
{
    s32 i;
    s8 szHost[80], szUser[80], szPassword[80];
    s8 szBuf[512];
    unsigned short usPort;


    Cfg_SectionItem("global_server", "default_dbname", m_szDefaultDBName, sizeof(m_szDefaultDBName));
    Cfg_MysqlServer("mysql_local", szHost, sizeof(szHost), &usPort, szUser, sizeof(szUser), szPassword, sizeof(szPassword));
    m_localdb = createMysqlWorkArea(szHost, usPort, szUser, szPassword, m_szDefaultDBName);
    if(!m_localdb)
    {
//        LogMessage("CStorageMysql::InitStorage, Cannot create localdb!");
        return EECode_NotInitilized;
    }

    for(i=0; i<(s32)(sizeof(m_remotedbs)/sizeof(m_remotedbs[0])); i++)
    {
        sprintf(szBuf, "mysql_remote%d", i+1);
        Cfg_MysqlServer(szBuf, szHost, sizeof(szHost), &usPort, szUser, sizeof(szUser), szPassword, sizeof(szPassword));
        if(strlen(szHost) == 0)
        {
            break;
        }
        m_remotedbs[i] = createMysqlWorkArea(szHost, usPort, szUser, szPassword, m_szDefaultDBName);
        if(!m_remotedbs[i])
        {
//            LogMessage("CStorageMysql::InitStorage, Create remote db%u workarea failed", i);
            return EECode_NotInitilized;
        }
        m_nRemoteCount++;
    }

    SetExitFlag(false);
    m_bLocalDBReady = false;
    mysqlCheckReady();

    return EECode_OK;
}


EECode CStorageMysql::DeInitStorage()
{
    s32 i;
    //Close Pool
    SetExitFlag(true);
    m_bLocalDBReady = false;

    destroyMysqlWorkArea(m_localdb);
    m_localdb = NULL;

    for(i=0; i<m_nRemoteCount; i++)
    {
        destroyMysqlWorkArea(m_remotedbs[i]);
        m_remotedbs[i] = NULL;
    }
    m_nRemoteCount = 0;
    return EECode_OK;
}


CStorageMysql::CStorageMysql() :
    m_localdb(NULL),
    m_bLocalDBReady(false),
    m_nRemoteCount(0),
    m_nMaxRowCount(5000)
{
}

CStorageMysql::~CStorageMysql()
{
}


EECode CStorageMysql::Mysql_Query(const s8* pszSql, vector<map<string,string> >& vResult, u32 dwMaxRowCount)
{
    s32 i, nRet;
    BOOL bReady;
    HMYCONNECTION hconn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    MYSQL_FIELD *fields;
    uint32_t nFieldCount,nRowCount;
    char* szFieldNames[MAX_COLUME_NUM];
    s8 *pszGBKSQL,*pszFinalSQL;
    map<string, string > mapRow;
    vResult.clear();
    vResult.reserve(dwMaxRowCount);
    EECode eStatus = EECode_OK;

    bReady = mysqlCheckReady();
    if (!bReady)
    {
//        LogMessage("CStorageMysql::Mysql_Query, database is not ready!");
        return EECode_NotRunning;
    }
    hconn = m_localdb->pMysqlPool->Get();
    if (!hconn)
    {
//        LogMessage("CStorageMysql::Mysql_Query, get database connection failed!");
        return EECode_NotInitilized;
    }

    pszGBKSQL = UTF8ToGBK(pszSql);

    if (!pszGBKSQL)
    {
//        LogMessage("CStorageMysql::Mysql_Query, utf8 to gbk failed!");
        m_localdb->pMysqlPool->Release(hconn);
        return EECode_Error;
    }
    pszFinalSQL = mysqlReplaceSupportedPatterns(pszGBKSQL);
    delete pszGBKSQL;

    //先分配4096字节
    nRet = mysql_query(hconn->pmysql, pszSql);
    if (0==nRet)
    {
        res = mysql_use_result(hconn->pmysql);
        if (res)
        {
            //第一行，写入列名。假设列名不含中文，可以视为utf8编码，不用转换字符集。
            nFieldCount = mysql_num_fields(res);
            fields = mysql_fetch_fields(res);
            nRet = 0;
            for (i=0; i<nFieldCount; i++)
            {
                szFieldNames[i] = StringToLower(fields[i].name);
            }

            //写入数据（后续N行）。
            nRowCount = 0;
            while((row=mysql_fetch_row(res))!=NULL)
            {
                if (nRowCount>=dwMaxRowCount)
                {
//                    LogMessage("CStorageMysql::Mysql_Query, rows too many!");
                    eStatus = EECode_TooMany;
                    break;
                }

                mapRow.clear();
                for (i=0; i<(s32)nFieldCount; i++)
                {
                    mapRow[szFieldNames[i]] = row[i]?row[i]:"";
                }
                vResult.push_back(mapRow);
                nRowCount++;
            }
            mysql_free_result(res);
        }
    }
    else
    {
        if(CR_SERVER_GONE_ERROR==nRet || CR_SERVER_LOST==nRet || CR_COMMANDS_OUT_OF_SYNC==nRet || CR_UNKNOWN_ERROR==nRet)
        {
//            LogMessage("CStorageMysql::Mysql_Query, mysql_query() connection error!");
            m_localdb->pMysqlPool->Drop(hconn);
        }
        else
        {
//            LogMessage("CStorageMysql::Mysql_Query, mysql_query() failed: Error %u (%s)", mysql_errno(hconn->pmysql), mysql_error(hconn->pmysql));
            cout << "CStorageMysql::Mysql_Query, mysql_query() failed: Error "<< mysql_error(hconn->pmysql) << endl;
            m_localdb->pMysqlPool->Release(hconn);
        }
        hconn = NULL;
        delete pszFinalSQL;
        return EECode_Error;
    }
    delete pszFinalSQL;

    if (hconn)
    {
        m_localdb->pMysqlPool->Release(hconn);
    }

    return eStatus;
}


EECode CStorageMysql::Mysql_Execute(const s8* pszUpdateSql, s32 nLocalRunMode)
{
    s32 i,nLen,nRet=-1;
    s8 *sql;
    HMYCONNECTION hconn = NULL;

    if (!mysqlCheckReady())
    {
//        LogMessage("CStorageMysql::Mysql_Execute, database is not ready!");
        return EECode_NotRunning;
    }

    //更新远程库
    nLen = strlen(pszUpdateSql);
    for (i=0; i<m_nRemoteCount; i++)
    {
        m_remotedbs[i]->taskMutex.Lock();
        if (m_remotedbs[i]->nTaskCount < MAX_QUEUED_SQL_TASK_COUNT)
        {
            sql = new s8[nLen+1];
            if (!sql)
            {
                m_remotedbs[i]->taskMutex.Unlock();
//				LogMessage("can not allocate memory for sql in Mysql_Execute().");
                return EECode_Error;
            }
            memcpy(sql, pszUpdateSql, nLen+1);

            m_remotedbs[i]->taskEvent.Post();
            m_remotedbs[i]->tasks.push_back(sql);
            m_remotedbs[i]->nTaskCount ++;
        }
        else
        {
//			LogMessage("too many queued task in remotedb %d in Mysql_Execute().", i);
        }
        m_remotedbs[i]->taskMutex.Unlock();
    }


    //更新本地库
    EECode eStatus = EECode_OK;
    if (SQL_MODE_ASYNC==nLocalRunMode)
    {
        //异步更新即可
        m_localdb->taskMutex.Lock();
        if (m_localdb->nTaskCount < MAX_QUEUED_SQL_TASK_COUNT)
        {
            sql = new s8[nLen+1];
            if (!sql)
            {
                m_localdb->taskMutex.Unlock();
//				LogMessage("can not allocate memory for sql 2 in Mysql_Execute().");
                return EECode_Error;
            }
            memcpy(sql, pszUpdateSql, nLen+1);
            m_localdb->taskEvent.Post();
            m_localdb->tasks.push_back(sql);
            m_localdb->nTaskCount++;
            nRet = 0;
        }
        else
        {
//			LogMessage("too many queued task in m_localdb in Mysql_Execute().");
            eStatus = EECode_TooMany;
        }
        m_localdb->taskMutex.Unlock();
    }
    else if(SQL_MODE_SYNC==nLocalRunMode)
    {
        //同步更新，马上执行该SQL语句
        hconn = m_localdb->pMysqlPool->Get();
        if (hconn)
        {
            nRet = mysql_query(hconn->pmysql, pszUpdateSql);
            if (0!=nRet)
            {
//                LogMessage("CStorageMysql::Mysql_Execute, mysql_query() failed: Error %u (%s)", mysql_errno(hconn->pmysql), mysql_error(hconn->pmysql));

                if (CR_SERVER_GONE_ERROR==nRet || CR_SERVER_LOST==nRet || CR_COMMANDS_OUT_OF_SYNC==nRet || CR_UNKNOWN_ERROR==nRet)
                {
                    m_localdb->pMysqlPool->Drop(hconn);
                }
                else
                {
                    m_localdb->pMysqlPool->Release(hconn);
                }
                hconn = NULL;
                return EECode_Error;
            }
            if (hconn)
            {
                m_localdb->pMysqlPool->Release(hconn);
            }
        }
        else
        {
//            LogMessage("CStorageMysql::Mysql_Execute, workarea->pMysqlPool->Get failed!");
            eStatus = EECode_NotInitilized;
        }
    }
    else
    {
        eStatus = EECode_BadParam;
    }
    return eStatus;
}


EECode CStorageMysql::ReadFromDB(IN EDBDataType eDBDataType, IN OUT void* pData, u32 dwMaxRowCount)
{
    s8* sbySql = NULL;
    s8 * pszOut = NULL;
    vector<map<string, string> > vResult;
    EECode eStatus;

    if(pData == NULL)
    {
//        LogMessage("CStorageMysql::ReadFromDB, pData is NULL!");
        return EECode_BadParam;
    }

    switch(eDBDataType)
    {
    case EDBDataType_Security:
    {
        sbySql = (s8*)"select * from security;";
        eStatus = Mysql_Query(sbySql, vResult, dwMaxRowCount);
        if(eStatus != EECode_OK)
        {
            return eStatus;
        }

        TSecurityMapArrayToStructArray(vResult, *(vector<TSecurity>*)pData);
        break;
    }
    case EDBDataType_ProcessList:
    {
        sbySql = (s8*)"show processlist;";
        eStatus = this->Mysql_Query(sbySql, vResult, dwMaxRowCount);
        if(eStatus != EECode_OK)
        {
            return eStatus;
        }

        TMysqlProcessListMapArrayToStructArray(vResult, *(vector<TMysqlProcessList>*)pData);
        break;
    }
    case EDBDataType_Status:
    {
        sbySql = (s8*)"show status;";
        eStatus = this->Mysql_Query(sbySql, vResult, dwMaxRowCount);
        if(eStatus != EECode_OK)
        {
            return eStatus;
        }

        TMysqlStatusMapArrayToStructArray(vResult, *(vector<TMysqlStatus>*)pData);
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
    // check params
    if(!pData)
    {
//        LogMessage("CStorageMysql::WriteToDB, pData is NULL!");
        return EECode_BadParam;
    }


    // prepare variables
    s8 szSql[1024];
    BOOL bRet;
    EECode eStatus;


    // write into database
    switch(eDBDataType)
    {
    case EDBDataType_Security:
    {
        vector<TSecurity> vSecurity = *(vector<TSecurity> *)pData;
        vector<TSecurity>::iterator vIt;
        const s8 *pbySecuritySymbol = NULL;
        const s8 *pbySecurityType = NULL;
        const s8 *pbyListExchange = NULL;
        const s8 *pbyTradingExchange = NULL;
        const s8 *pbyCurrency = NULL;
        const s8 *pbyOptionType = NULL;
        const s8 *pbyISIN = NULL;
        s8 sbyExpiration[32];

        for( vIt = vSecurity.begin(); vIt != vSecurity.end(); vIt++ )
        {
            bRet = gfTimeValSys2Mysql(vIt->mtExpiration, sbyExpiration, sizeof(sbyExpiration));
            if(!bRet)
            {
//                LogMessage("CStorageMysql::WriteToDB, TSecurity gfTimeValSys2Mysql failed for vIt->mtExpiration(mdwSecurityID=%u)", vIt->mdwSecurityID);
                return EECode_Error;
            }

            pbySecuritySymbol = vIt->msSecuritySymbol.empty()? "" : vIt->msSecuritySymbol.c_str();
            pbySecurityType = g_tStaticEnumMap.m_MapESecurityTypeValue2String[vIt->meSecurityType].empty()? "" : g_tStaticEnumMap.m_MapESecurityTypeValue2String[vIt->meSecurityType].c_str();
            pbyListExchange = g_tStaticEnumMap.m_MapEListExchangeValue2String[vIt->meListExchange].empty()? "" : g_tStaticEnumMap.m_MapEListExchangeValue2String[vIt->meListExchange].c_str();
            pbyTradingExchange = g_tStaticEnumMap.m_MapETradingExchangeValue2String[vIt->meTradingExchange].empty()? "" : g_tStaticEnumMap.m_MapETradingExchangeValue2String[vIt->meTradingExchange].c_str();
            pbyCurrency = g_tStaticEnumMap.m_MapECurrencyValue2String[vIt->meCurrency].empty()? "" : g_tStaticEnumMap.m_MapECurrencyValue2String[vIt->meCurrency].c_str();
            pbyOptionType = vIt->msOptionType.empty()? "" : vIt->msOptionType.c_str();
            pbyISIN = vIt->msISIN.empty()? "" : vIt->msISIN.c_str();

            // szSQL
            snprintf(szSql, sizeof(szSql), "insert into %s.security(securityid, securitysymbol, securitytype, listexchange, tradingexchange, currency, expiration, strike, optiontype, underlayingsecurityid, contractsize, ticksize, longmarginrate, shortmarginrate, generalmarginrate, isin, tradeable, expirable) \
values('%u', '%s', '%s', '%s', '%s', '%s', '%s', '%lf', '%s', '%u', '%u', '%lf', '%lf', '%lf', '%lf', '%s', b'%u', b'%u')", m_szDefaultDBName,
                     vIt->mdwSecurityID,
                     pbySecuritySymbol,
                     pbySecurityType,
                     pbyListExchange,
                     pbyTradingExchange,
                     pbyCurrency,
                     sbyExpiration,
                     vIt->mdfStrike,
                     pbyOptionType,
                     vIt->mdwUnderlayingSecurityID,
                     vIt->mdwContractSize,
                     vIt->mdfTickSize,
                     vIt->mdfLongMarginRate,
                     vIt->mdfShortMarginRate,
                     vIt->mdfGeneralMarginRate,
                     pbyISIN,
                     vIt->mbTradeAble,
                     vIt->mbExpirAble);

            eStatus = this->Mysql_Execute(szSql, sdwWriteMode);
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


EECode CStorageMysql::DeleteFromDB(IN s8* sbySql)
{
    BOOL bReady;
    HMYCONNECTION hconn;
    EECode eStatus;

    bReady = mysqlCheckReady();
    if (!bReady)
    {
//        LogMessage("CStorageMysql::DeleteFromDB, database is not ready!");
        return EECode_NotRunning;
    }
    hconn = m_localdb->pMysqlPool->Get();
    if (!hconn)
    {
//        LogMessage("CStorageMysql::DeleteFromDB, get database connection failed!");
        return EECode_NotInitilized;
    }

    s32 nRet = mysql_query(hconn->pmysql, sbySql);

    if(nRet == 0)
    {
        eStatus = EECode_OK;
    }
    else
    {
//        LogMessage("CStorageMysql::DeleteFromDB, mysql_query() failed: Error %u (%s)", mysql_errno(hconn->pmysql), mysql_error(hconn->pmysql));
        eStatus = EECode_Error;
    }

    if (hconn)
    {
        m_localdb->pMysqlPool->Release(hconn);
    }
    return eStatus;

}


//职能：重新启动后，检查数据库是否已同步过。
//约定：同步完成以后向数据库插入一条记录：
//      insert into t_para (Fmod_name,Fpara_name,Fpara_value,Fpara_memo) values ('dbsync','sync_end_time',Now(),'数据同步完成');
//      因此只要检查该记录是否存在即可。
//返回值： true   -- 完成同步
//         false  -- 未完成同步
BOOL CStorageMysql::mysqlCheckReady()
{
    s8 szError[1024];
    s8 szSQL[512];
    s32 nRet;
    HMYCONNECTION hconn = NULL;
    MYSQL_RES *res;
    MYSQL_ROW row;
    s8 szSyncEndTime[80];
    BOOL bFound;
    int64_t qwNowTick;

    if(m_localdb == NULL)
    {
        return false;
    }

    if (m_bLocalDBReady)
    {
        //一旦ready，永远有效。
        return true;
    }

    hconn = m_localdb->pMysqlPool->Get();
    if (!hconn)
    {
        //数据库无法连接
        return false;
    }

    bFound = false;
    m_bLocalDBReady = true;

    if (hconn)
    {
        m_localdb->pMysqlPool->Release(hconn);
    }
    return m_bLocalDBReady;
}

void* _MysqlWorkThreadProc(void* lpParameter)
{
    struct TMysqlWorkArea *workarea;
    s8 *sql;
    s32 nRet;
    s8 szError[512];
    HMYCONNECTION hconn = NULL;
    BOOL bHaveTask;

    workarea = (struct TMysqlWorkArea*)lpParameter;

    while(!workarea->m_pmysql->GetExitFlag())
    {
        nRet = workarea->taskEvent.Wait(1000);
        if (0==nRet)
        {
            //未等到事件，继续等
            continue;
        }

        do
        {
            bHaveTask = false;
            sql = NULL;
            workarea->taskMutex.Lock();
            if (!workarea->tasks.empty())
            {
                sql = workarea->tasks.front();
                workarea->tasks.pop_front();
                workarea->nTaskCount--;
                bHaveTask = true;
            }
            workarea->taskMutex.Unlock();

            if (!sql)
            {
                if (bHaveTask)
                {
                    //一定是要退出了。
                    bHaveTask = false;
                }
                continue;
            }

            //执行该SQL语句
            hconn = workarea->pMysqlPool->Get();
            if (hconn)
            {
                nRet = mysql_query(hconn->pmysql, sql);
                if (0!=nRet)
                {
                    strncpy(szError,mysql_error(hconn->pmysql),sizeof(szError));
                    szError[sizeof(szError)-1] = 0;
                    if (1==nRet && strncmp(szError,"Duplicate entry",15)==0)
                    {
                        //忽略 "Duplicate entry" 错误
                    }
                    else
                    {
//                        LogMessage("_MysqlWorkThreadProc, mysql_query() failed: Error %u (%s)", mysql_errno(hconn->pmysql), mysql_error(hconn->pmysql));
                    }
                    if (CR_SERVER_GONE_ERROR==nRet || CR_SERVER_LOST==nRet || CR_COMMANDS_OUT_OF_SYNC==nRet || CR_UNKNOWN_ERROR==nRet)
                    {
                        workarea->pMysqlPool->Drop(hconn);
                    }
                    else
                    {
                        workarea->pMysqlPool->Release(hconn);
                    }

                    hconn = NULL;
                }
                if (hconn)
                {
                    workarea->pMysqlPool->Release(hconn);
                }
            }
            else
            {
//                LogMessage("_MysqlWorkThreadProc, workarea->pMysqlPool->Get() failed");
            }
            delete sql;
        }
        while (bHaveTask);
    }

    return 0;
}


struct TMysqlWorkArea* CStorageMysql::createMysqlWorkArea(const s8 *szHost, unsigned short usPort, const s8 *szUser, const s8 *szPassword, const s8 *szDefaultDBName)
{
    struct TMysqlWorkArea *workarea;
    workarea = new struct TMysqlWorkArea;
    if(!workarea)
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, Cannot allocate memory for workarea");
        return NULL;
    }
    workarea->nTaskCount = 0;

    workarea->m_pmysql = this;  //保存当前CStorageMysql的指针
    workarea->pMysqlPool = new MysqlPool(szHost, usPort, szUser, szPassword, szDefaultDBName);
    if(!workarea->pMysqlPool)
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, Cannot allocate memory for workarea->pMysqlPool");
        delete workarea;
        return NULL;
    }

    s32 nRet = pthread_create(&workarea->hThread, NULL, _MysqlWorkThreadProc, workarea);
    if(0 != nRet)
    {
//        LogMessage("CStorageMysql::createMysqlWorkArea, Cannot create mysql thread, szHost = %s", szHost);
        delete workarea->pMysqlPool;
        delete workarea;
        return NULL;
    }
    return workarea;
}


void CStorageMysql::destroyMysqlWorkArea(struct TMysqlWorkArea *pMysqlWorkArea)
{
    s8 *sql;
    if(!pMysqlWorkArea)
    {
        return ;
    }

    pMysqlWorkArea->taskMutex.Lock();
    pMysqlWorkArea->taskEvent.Post();
    pMysqlWorkArea->tasks.push_back(NULL);
    pMysqlWorkArea->nTaskCount++;//fix potential bug
    pMysqlWorkArea->taskMutex.Unlock();

    pthread_join(pMysqlWorkArea->hThread, NULL);

    while(!pMysqlWorkArea->tasks.empty())
    {
        sql = pMysqlWorkArea->tasks.front();
        pMysqlWorkArea->tasks.pop_front();
        if (sql)//fix potential bug
        {
            delete sql;
        }
    }

    delete pMysqlWorkArea->pMysqlPool;
    delete pMysqlWorkArea;
}


s8* CStorageMysql::mysqlReplaceSupportedPatterns(const s8 *szOriginalSQL)
{
    s32 nLen;
    s8 szNow[80];
    s8 *pszNewSQL;
    time_t tNow;

    gfGetTrueTime(&tNow);
    szNow[0] = '\'';
    //GetTimeString(tNow, szNow+1, sizeof(szNow)-1);
    struct tm ct;
    localtime_r(&tNow, &ct);
    snprintf(szNow+1, sizeof(szNow)-1, "%04d-%02d-%02d %02d:%02d:%02d",
             (s32)(ct.tm_year +1900), (s32)(ct.tm_mon +1), (s32)(ct.tm_mday),
             (s32)(ct.tm_hour), (s32)(ct.tm_min), (s32)(ct.tm_sec)
            );

    szNow[20] = '\'';
    szNow[21] = 0;
    pszNewSQL = ReplacePattern(szOriginalSQL, "{$NOW$}", szNow);
    if(NULL == pszNewSQL)
    {
        nLen = strlen(szOriginalSQL);
        pszNewSQL = new s8[nLen+1];
        if(pszNewSQL)
        {
            strcpy(pszNewSQL, szOriginalSQL);
        }
        else
        {
//            LogMessage("CStorageMysql::mysqlReplaceSupportedPatterns, Allocate memory for pszNew failed!");
        }
    }
    return pszNewSQL;
}
