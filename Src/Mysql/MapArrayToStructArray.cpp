#include "MapArrayToStructArray.h"
#include "BaseDataEnum.h"
#include <stdlib.h>

extern TStaticEnumMap g_tStaticEnumMap;

// 1. tSecurity -- db.security
void TSecurityMapArrayToStructArray(vector<map<string, s8 *> >& vResult, vector<TSecurity> &vSecurity)
{
    vSecurity.clear();
    vSecurity.reserve(vResult.size());
    vector<map<string, s8 *> >::iterator vIt;
    map<string, s8 *>::iterator mapIt;
    struct tm tb;
    TSecurity tSecurity;

    for(vIt = vResult.begin(); vIt != vResult.end(); vIt++)
    {
        {
            //Init tSecurity
            tSecurity.mdwSecurityID = 0;
            tSecurity.msSecuritySymbol = "";
            tSecurity.meSecurityType = (ESecurityType) 0;
            tSecurity.meListExchange = (EListExchange) 0;
            tSecurity.meTradingExchange = (ETradingExchange) 0;
            tSecurity.meCurrency = (ECurrency) 0;
            tSecurity.mtExpiration = {0, 0};
            tSecurity.mdfStrike = 0.0;
            tSecurity.msOptionType = "";
            tSecurity.mdwUnderlayingSecurityID = 0;
            tSecurity.mdwContractSize = 0;
            tSecurity.mdfTickSize = 0.0;
            tSecurity.mdfLongMarginRate = 0.0;
            tSecurity.mdfShortMarginRate = 0.0;
            tSecurity.mdfGeneralMarginRate = 0.0;
            tSecurity.msISIN = "";
            tSecurity.mbTradeAble = false;
            tSecurity.mbExpirAble = false;
        }
        for(mapIt = (*vIt).begin(); mapIt != (*vIt).end(); mapIt++)
        {
            //1. 标的ID
            if(0 == mapIt->first.compare("securityid"))
            {
                tSecurity.mdwSecurityID = atoi(mapIt->second);
                continue;
            }
            //2. 证券代码
            if(0 == mapIt->first.compare("securitysymbol"))
            {
                tSecurity.msSecuritySymbol = mapIt->second;
                continue;
            }
            //3. 证券类别
            if(0 == mapIt->first.compare("securitytype"))
            {
                tSecurity.meSecurityType = g_tStaticEnumMap.m_MapESecurityTypeString2Value[mapIt->second];
                continue;
            }
            //4. 上市交易所
            if(0 == mapIt->first.compare("listexchange"))
            {
                tSecurity.meListExchange = g_tStaticEnumMap.m_MapEListExchangeString2Value[mapIt->second];
                continue;
            }
            //5. 交易交易所
            if(0 == mapIt->first.compare("tradingexchange"))
            {
                tSecurity.meTradingExchange = g_tStaticEnumMap.m_MapETradingExchangeString2Value[mapIt->second];
                continue;
            }
            //6. Currency
            if(0 == mapIt->first.compare("currency"))
            {
                tSecurity.meCurrency = g_tStaticEnumMap.m_MapECurrencyString2Value[mapIt->second];
                continue;
            }
            //7. 到期时间
            if(0 == mapIt->first.compare("expiration"))
            {
                if(strptime(mapIt->second, "%Y-%m-%d %H:%M:%S", &tb) != NULL)
                {
                    tSecurity.mtExpiration.tv_sec = mktime(&tb);
                    tSecurity.mtExpiration.tv_usec = 0;
                }
                continue;
            }
            //8. 执行价
            if(0 == mapIt->first.compare("strike"))
            {
                tSecurity.mdfStrike = atof(mapIt->second);
                continue;
            }
            //9. 期权类型
            if(0 == mapIt->first.compare("optiontype"))
            {
                tSecurity.msOptionType = mapIt->second;
                continue;
            }
            //10. 标的
            if(0 == mapIt->first.compare("underlayingsecurityid"))
            {
                tSecurity.mdwUnderlayingSecurityID = atoi(mapIt->second);
                continue;
            }
            //11. 合约大小
            if(0 == mapIt->first.compare("contractsize"))
            {
                tSecurity.mdwContractSize = atoi(mapIt->second);
                continue;
            }
            //12. 最小报价单位
            if(0 == mapIt->first.compare("ticksize"))
            {
                tSecurity.mdfTickSize = atof(mapIt->second);
                continue;
            }
            //13. 多头保证金率
            if(0 == mapIt->first.compare(("longmarginrate")))
            {
                tSecurity.mdfLongMarginRate = atof(mapIt->second);
                continue;
            }
            //14. 空头保证金率
            if(0 == mapIt->first.compare("shortmarginrate"))
            {
                tSecurity.mdfShortMarginRate = atof(mapIt->second);
                continue;
            }
            //15. 总保证金率
            if(0 == mapIt->first.compare("gereralmarginrate"))
            {
                tSecurity.mdfGeneralMarginRate = atof(mapIt->second);
                continue;
            }
            //16. ISIN
            if(0 == mapIt->first.compare("isin"))
            {
                tSecurity.msISIN = mapIt->second;
                continue;
            }
            //17. 是否可交易
            if(0 == mapIt->first.compare("tradeable"))
            {
                if(0 == strcmp(mapIt->second, "\001"))
                {
                    tSecurity.mbTradeAble = 1;
                }
                else
                {
                    tSecurity.mbTradeAble = 0;
                }
                continue;
            }
            //18. 是否到期
            if(0 == mapIt->first.compare("expirable"))
            {
                if(0 == strcmp(mapIt->second, "\001"))
                {
                    tSecurity.mbExpirAble = 1;
                }
                else
                {
                    tSecurity.mbExpirAble = 0;
                }
                continue;
            }

        }
        vSecurity.push_back(tSecurity);
    }
}

// 2. TMysqlProcessList
void TMysqlProcessListMapArrayToStructArray(vector<map<string, s8 *> >& vResult, vector<TMysqlProcessList> &vMysqlProcessList)
{
    vMysqlProcessList.clear();
    vMysqlProcessList.reserve(vResult.size());
    vector<map<string, s8 *> >::iterator vIt;
    map<string, s8 *>::iterator mapIt;
    TMysqlProcessList tMysqlProcessList;

    for(vIt = vResult.begin(); vIt != vResult.end(); vIt++)
    {
        {
            //Init tMysqlProcessList
            tMysqlProcessList.sdwId = 0;
            tMysqlProcessList.sUser = "";
            tMysqlProcessList.sHost = "";
            tMysqlProcessList.sdb = "";
            tMysqlProcessList.sCommand = "";
            tMysqlProcessList.qwTime = 0;
            tMysqlProcessList.sState = "";
            tMysqlProcessList.sInfo = "";
            tMysqlProcessList.dfProgress = 0.0;
        }
        for(mapIt = (*vIt).begin(); mapIt != (*vIt).end(); mapIt++)
        {
            //1. ID
            if(0 == mapIt->first.compare("id"))
            {
                tMysqlProcessList.sdwId = atoi(mapIt->second);
                continue;
            }
            //2. User
            if(0 == mapIt->first.compare("user"))
            {
                tMysqlProcessList.sUser = mapIt->second;
                continue;
            }
            //3. Host
            if(0 == mapIt->first.compare("host"))
            {
                tMysqlProcessList.sHost = mapIt->second;
                continue;
            }
            //4. db
            if(0 == mapIt->first.compare("db"))
            {
                tMysqlProcessList.sdb= mapIt->second;
                continue;
            }
            //5. Command
            if(0 == mapIt->first.compare("command"))
            {
                tMysqlProcessList.sCommand = mapIt->second;
                continue;
            }
            //6. Time
            if(0 == mapIt->first.compare("time"))
            {
                tMysqlProcessList.qwTime = atoi(mapIt->second);
                continue;
            }
            //7. State
            if(0 == mapIt->first.compare("state"))
            {
                tMysqlProcessList.sState = mapIt->second;
                continue;
            }
            //8. Info
            if(0 == mapIt->first.compare("info"))
            {
                tMysqlProcessList.sInfo = mapIt->second;
                continue;
            }
            //9. Progress
            if(0 == mapIt->first.compare("progress"))
            {
                tMysqlProcessList.dfProgress = atof(mapIt->second);
                continue;
            }
        }
        vMysqlProcessList.push_back(tMysqlProcessList);
    }
}

// 3. TMysqlStatus
void TMysqlStatusMapArrayToStructArray(vector<map<string, s8 *> >& vResult, vector<TMysqlStatus> &vMysqlStatus)
{
    vMysqlStatus.clear();
    vMysqlStatus.reserve(vResult.size());
    vector<map<string, s8 *> >::iterator vIt;
    map<string, s8 *>::iterator mapIt;
    TMysqlStatus tMysqlStatus;

    for(vIt = vResult.begin(); vIt != vResult.end(); vIt++)
    {
        {
            //Init tMysqlProcessList
            tMysqlStatus.sName = "";
            tMysqlStatus.sValue = "";
        }
        for(mapIt = (*vIt).begin(); mapIt != (*vIt).end(); mapIt++)
        {
            //1. Variable_name
            if(0 == mapIt->first.compare("variable_name"))
            {
                tMysqlStatus.sName = mapIt->second;
                continue;
            }
            //2. Value
            if(0 == mapIt->first.compare("value"))
            {
                tMysqlStatus.sValue = mapIt->second;
                continue;
            }
        }
        vMysqlStatus.push_back(tMysqlStatus);
    }
}
