#include <iostream>
#include "BaseDataStructs.h"
#include "StorageBase.h"
#include "StorageMysql.h"
#include "BaseType.h"
#include "Config.h"
#include "File.h"
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

    if (0 != (ret = ut_init_params(argc, argv))) {
       printf("ut_init_params fail, return %d.\n", ret);
       return ret;
    }


    vector<TSecurity> vSecurity;
    CStorageMysql *mysql = CStorageMysql::Create();
    mysql->InitStorage();

    EECode eStatus = mysql->ReadFromDB(EDBDataType_Security, &vSecurity);


    cout<<"OK"<<endl;
    mysql->DeInitStorage();
    return 0;
}
