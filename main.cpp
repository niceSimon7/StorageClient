#include <iostream>
#include "BaseDataStructs.h"
#include "StorageBase.h"
#include "StorageMysql.h"
#include "BaseType.h"
#include "Config.h"
#include "File.h"
#include "Time.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

//Global Vars
TStaticEnumMap g_tStaticEnumMap;
s8 g_szDataDB[100];//行情数据库

//Init Config
s32 ut_init_params(s32 argc, const s8 **argv)
{
    char szBuf[1024];
    bool bRet = false;
    if(argc < 2)
    {
        printf("no input config file path, use local Config/Config.xml as default.\n");
        gfGetConfigFile(szBuf, sizeof(szBuf));
    }
    else
    {
        snprintf(szBuf, sizeof(szBuf), "%s", argv[1]);
    }
    bRet = Cfg_Load(szBuf);
    if (!bRet)
    {
        printf("load %s failed.\n", szBuf);
        return -1;
    }

    Cfg_SectionItem("mysql_local", "datadb", szBuf, sizeof(szBuf));
    snprintf(g_szDataDB, sizeof(g_szDataDB), "%s", szBuf);

    return 0;
}

int main(int argc, const char *argv[])
{

    int ret = -1;

    if (0 != (ret = ut_init_params(argc, argv)))
    {
        printf("ut_init_params fail, return %d.\n", ret);
        return ret;
    }


    vector<TSecurity> vSecurity;
    f64 dfBegin, dfFinish;
    CStorageMysql *mysql = CStorageMysql::Create();
    mysql->InitStorage();
    /////////////////////////////////////////////////////////
// Write 100000 items
    EECode eStatus1 = mysql->ReadFromDB(EDBDataType_Security, 3500, &vSecurity);
    TSecurity tSecurity = vSecurity[0];
    vSecurity.clear();

    for(int i=3120; i<100000; i++)
    {
        tSecurity.mdwSecurityID = i+100000;
        tSecurity.mdwUnderlayingSecurityID = 2750;
        tSecurity.msSecuritySymbol = to_string(i+100000);
        tSecurity.meListExchange = EListExchange_SSE;
        tSecurity.meTradingExchange = ETradingExchange_SSE;
        vSecurity.push_back(tSecurity);
    }
    dfBegin = getWallSec();
    EECode eStatus2 = mysql->WriteToDB(EDBDataType_Security, SQL_MODE_SYNC, &vSecurity);
    dfFinish = getWallSec();
    cout << "Security WriteToDB(9.7W Lines): " <<dfFinish - dfBegin << "s" << endl;

//    // read 100000 items
//    vSecurity.clear();
//    dfBegin = getWallSec();
//    EECode eStatus3 = mysql->ReadFromDB(EDBDataType_Security, &vSecurity, 3120);
//    dfFinish = getWallSec();
//    cout << "Security ReadFromDB(10W Lines): "<<dfFinish - dfBegin << "s" << endl;

//    // delete 100000 items
//    EECode eStatus4;
//    for(int i=3120; i<100000; i++)
//    {
//        s8 sbySql[512] = {0, };
//        snprintf(sbySql, sizeof(sbySql), "delete from security where securityid = %u;", i+100000);
//        eStatus4 = mysql->mysqlDelete(sbySql);
//    }
    ////////////////////////////////////////////////////////////
    mysql->DeInitStorage();
    return 0;
}
