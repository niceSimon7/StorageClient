#ifndef _BASEDATASTRUCT_H_
#define _BASEDATASTRUCT_H_

#include "BaseType.h"
#include "BaseDataEnum.h"
#include <map>
#include <string>
using namespace std;

////////////////////////////////////////////////////////////// ENUM <--> STRING begin ///////////////////////////////////////////////////////

struct TStaticEnumMap
{
    map<string, ESecurityType> m_MapESecurityTypeString2Value; //(1)
    map<ESecurityType, string> m_MapESecurityTypeValue2String;

    map<string, EListExchange> m_MapEListExchangeString2Value; //(2)
    map<EListExchange, string> m_MapEListExchangeValue2String;

    map<string, ETradingExchange> m_MapETradingExchangeString2Value; //(3)
    map<ETradingExchange, string> m_MapETradingExchangeValue2String;

    map<string, ECurrency> m_MapECurrencyString2Value; //(4)
    map<ECurrency, string> m_MapECurrencyValue2String;



    TStaticEnumMap()
    {
        //(1)
        m_MapESecurityTypeString2Value["STK"] = ESecurityType_STK;
        m_MapESecurityTypeString2Value["OPT"] = ESecurityType_OPT;
        m_MapESecurityTypeString2Value["IND"] = ESecurityType_IND;
        m_MapESecurityTypeString2Value["FUT"] = ESecurityType_FUT;
        m_MapESecurityTypeString2Value["FOP"] = ESecurityType_FOP;
        m_MapESecurityTypeString2Value["CASH"] = ESecurityType_CASH;
        m_MapESecurityTypeString2Value["BAG"] = ESecurityType_BAG;

        m_MapESecurityTypeValue2String[ESecurityType_STK] = "STK";
        m_MapESecurityTypeValue2String[ESecurityType_OPT] = "OPT";
        m_MapESecurityTypeValue2String[ESecurityType_IND] = "IND";
        m_MapESecurityTypeValue2String[ESecurityType_FUT] = "FUT";
        m_MapESecurityTypeValue2String[ESecurityType_FOP] = "FOP";
        m_MapESecurityTypeValue2String[ESecurityType_CASH] = "CASH";
        m_MapESecurityTypeValue2String[ESecurityType_BAG] = "BAG";

        //(2)
        m_MapEListExchangeString2Value["SZE"] = EListExchange_SZSE;
        m_MapEListExchangeString2Value["SSE"] = EListExchange_SSE;
        m_MapEListExchangeString2Value["SHFE"] = EListExchange_SHFE;
        m_MapEListExchangeString2Value["DCE"] = EListExchange_DCE;
        m_MapEListExchangeString2Value["CZCE"] = EListExchange_CZCE;
        m_MapEListExchangeString2Value["CFFEX"] = EListExchange_CFFEX;
        m_MapEListExchangeString2Value["HKFE"] = EListExchange_HKFE;
        m_MapEListExchangeString2Value["SEHK"] = EListExchange_SEHK;
        m_MapEListExchangeString2Value["AMEX"] = EListExchange_AMEX;
        m_MapEListExchangeString2Value["AUTO"] = EListExchange_AUTO;
        m_MapEListExchangeString2Value["CBOE"] = EListExchange_CBOE;
        m_MapEListExchangeString2Value["CBOT"] = EListExchange_CBOT;
        m_MapEListExchangeString2Value["CFE"] = EListExchange_CFE;
        m_MapEListExchangeString2Value["CME"] = EListExchange_CME;
        m_MapEListExchangeString2Value["DTB"] = EListExchange_DTB;
        m_MapEListExchangeString2Value["GLOBEX"] = EListExchange_GLOBEX;
        m_MapEListExchangeString2Value["ECBOT"] = EListExchange_ECBOT;
        m_MapEListExchangeString2Value["IDEALPRO"] = EListExchange_IDEALPRO;
        m_MapEListExchangeString2Value["NYBOT"] = EListExchange_NYBOT;
        m_MapEListExchangeString2Value["NYMEX"] = EListExchange_NYMEX;
        m_MapEListExchangeString2Value["NASDAQ"] = EListExchange_NASDAQ;
        m_MapEListExchangeString2Value["NYSE"] = EListExchange_NYSE;
        m_MapEListExchangeString2Value["OTCCBB"] = EListExchange_OTCCBB;
        m_MapEListExchangeString2Value["PINK"] = EListExchange_PINK;
        m_MapEListExchangeString2Value["SMART"] = EListExchange_SMART;
        m_MapEListExchangeString2Value["SOFFEX"] = EListExchange_SOFFEX;
        m_MapEListExchangeString2Value["LMAX"] = EListExchange_LMAX;

        m_MapEListExchangeValue2String[EListExchange_SZSE] = "SZE";
        m_MapEListExchangeValue2String[EListExchange_SSE] = "SSE";
        m_MapEListExchangeValue2String[EListExchange_SHFE] = "SHFE";
        m_MapEListExchangeValue2String[EListExchange_DCE] = "DCE";
        m_MapEListExchangeValue2String[EListExchange_CZCE] = "CZCE";
        m_MapEListExchangeValue2String[EListExchange_CFFEX] = "CFFEX";
        m_MapEListExchangeValue2String[EListExchange_HKFE] = "HKFE";
        m_MapEListExchangeValue2String[EListExchange_SEHK] = "SEHK";
        m_MapEListExchangeValue2String[EListExchange_AMEX] = "AMEX";
        m_MapEListExchangeValue2String[EListExchange_AUTO] = "AUTO";
        m_MapEListExchangeValue2String[EListExchange_CBOE] = "CBOE";
        m_MapEListExchangeValue2String[EListExchange_CBOT] = "CBOT";
        m_MapEListExchangeValue2String[EListExchange_CFE] = "CFE";
        m_MapEListExchangeValue2String[EListExchange_CME] = "CME";
        m_MapEListExchangeValue2String[EListExchange_DTB] = "DTB";
        m_MapEListExchangeValue2String[EListExchange_GLOBEX] = "GLOBEX";
        m_MapEListExchangeValue2String[EListExchange_ECBOT] = "ECBOT";
        m_MapEListExchangeValue2String[EListExchange_IDEALPRO] = "IDEALPRO";
        m_MapEListExchangeValue2String[EListExchange_NYBOT] = "NYBOT";
        m_MapEListExchangeValue2String[EListExchange_NYMEX] = "NYMEX";
        m_MapEListExchangeValue2String[EListExchange_NASDAQ] = "NASDAQ";
        m_MapEListExchangeValue2String[EListExchange_NYSE] = "NYSE";
        m_MapEListExchangeValue2String[EListExchange_OTCCBB] = "OTCCBB";
        m_MapEListExchangeValue2String[EListExchange_PINK] = "PINK";
        m_MapEListExchangeValue2String[EListExchange_SMART] = "SMART";
        m_MapEListExchangeValue2String[EListExchange_SOFFEX] = "SOFFEX";
        m_MapEListExchangeValue2String[EListExchange_LMAX] = "LMAX";

        //(3)
        m_MapETradingExchangeString2Value["SZE"] = ETradingExchange_SZSE;
        m_MapETradingExchangeString2Value["SSE"] = ETradingExchange_SSE;
        m_MapETradingExchangeString2Value["SHFE"] = ETradingExchange_SHFE;
        m_MapETradingExchangeString2Value["DCE"] = ETradingExchange_DCE;
        m_MapETradingExchangeString2Value["CZCE"] = ETradingExchange_CZCE;
        m_MapETradingExchangeString2Value["CFFEX"] = ETradingExchange_CFFEX;
        m_MapETradingExchangeString2Value["HKFE"] = ETradingExchange_HKFE;
        m_MapETradingExchangeString2Value["SEHK"] = ETradingExchange_SEHK;
        m_MapETradingExchangeString2Value["AMEX"] = ETradingExchange_AMEX;
        m_MapETradingExchangeString2Value["AUTO"] = ETradingExchange_AUTO;
        m_MapETradingExchangeString2Value["CBOE"] = ETradingExchange_CBOE;
        m_MapETradingExchangeString2Value["CBOT"] = ETradingExchange_CBOT;
        m_MapETradingExchangeString2Value["CFE"] = ETradingExchange_CFE;
        m_MapETradingExchangeString2Value["CME"] = ETradingExchange_CME;
        m_MapETradingExchangeString2Value["DTB"] = ETradingExchange_DTB;
        m_MapETradingExchangeString2Value["GLOBEX"] = ETradingExchange_GLOBEX;
        m_MapETradingExchangeString2Value["ECBOT"] = ETradingExchange_ECBOT;
        m_MapETradingExchangeString2Value["IDEALPRO"] = ETradingExchange_IDEALPRO;
        m_MapETradingExchangeString2Value["NYBOT"] = ETradingExchange_NYBOT;
        m_MapETradingExchangeString2Value["NYMEX"] = ETradingExchange_NYMEX;
        m_MapETradingExchangeString2Value["NASDAQ"] = ETradingExchange_NASDAQ;
        m_MapETradingExchangeString2Value["NYSE"] = ETradingExchange_NYSE;
        m_MapETradingExchangeString2Value["OTCCBB"] = ETradingExchange_OTCCBB;
        m_MapETradingExchangeString2Value["PINK"] = ETradingExchange_PINK;
        m_MapETradingExchangeString2Value["SMART"] = ETradingExchange_SMART;
        m_MapETradingExchangeString2Value["SOFFEX"] = ETradingExchange_SOFFEX;
        m_MapETradingExchangeString2Value["LMAX"] = ETradingExchange_LMAX;

        m_MapETradingExchangeValue2String[ETradingExchange_SZSE] = "SZE";
        m_MapETradingExchangeValue2String[ETradingExchange_SSE] = "SSE";
        m_MapETradingExchangeValue2String[ETradingExchange_SHFE] = "SHFE";
        m_MapETradingExchangeValue2String[ETradingExchange_DCE] = "DCE";
        m_MapETradingExchangeValue2String[ETradingExchange_CZCE] = "CZCE";
        m_MapETradingExchangeValue2String[ETradingExchange_CFFEX] = "CFFEX";
        m_MapETradingExchangeValue2String[ETradingExchange_HKFE] = "HKFE";
        m_MapETradingExchangeValue2String[ETradingExchange_SEHK] = "SEHK";
        m_MapETradingExchangeValue2String[ETradingExchange_AMEX] = "AMEX";
        m_MapETradingExchangeValue2String[ETradingExchange_AUTO] = "AUTO";
        m_MapETradingExchangeValue2String[ETradingExchange_CBOE] = "CBOE";
        m_MapETradingExchangeValue2String[ETradingExchange_CBOT] = "CBOT";
        m_MapETradingExchangeValue2String[ETradingExchange_CFE] = "CFE";
        m_MapETradingExchangeValue2String[ETradingExchange_CME] = "CME";
        m_MapETradingExchangeValue2String[ETradingExchange_DTB] = "DTB";
        m_MapETradingExchangeValue2String[ETradingExchange_GLOBEX] = "GLOBEX";
        m_MapETradingExchangeValue2String[ETradingExchange_ECBOT] = "ECBOT";
        m_MapETradingExchangeValue2String[ETradingExchange_IDEALPRO] = "IDEALPRO";
        m_MapETradingExchangeValue2String[ETradingExchange_NYBOT] = "NYBOT";
        m_MapETradingExchangeValue2String[ETradingExchange_NYMEX] = "NYMEX";
        m_MapETradingExchangeValue2String[ETradingExchange_NASDAQ] = "NASDAQ";
        m_MapETradingExchangeValue2String[ETradingExchange_NYSE] = "NYSE";
        m_MapETradingExchangeValue2String[ETradingExchange_OTCCBB] = "OTCCBB";
        m_MapETradingExchangeValue2String[ETradingExchange_PINK] = "PINK";
        m_MapETradingExchangeValue2String[ETradingExchange_SMART] = "SMART";
        m_MapETradingExchangeValue2String[ETradingExchange_SOFFEX] = "SOFFEX";
        m_MapETradingExchangeValue2String[ETradingExchange_LMAX] = "LMAX";

        //(4)
        m_MapECurrencyString2Value["CHF"] = ECurrency_CHF;
        m_MapECurrencyString2Value["EUR"] = ECurrency_EUR;
        m_MapECurrencyString2Value["USD"] = ECurrency_USD;
        m_MapECurrencyString2Value["GBP"] = ECurrency_GBP;
        m_MapECurrencyString2Value["JPY"] = ECurrency_JPY;
        m_MapECurrencyString2Value["CNY"] = ECurrency_CNY;
        m_MapECurrencyString2Value["CNH"] = ECurrency_CNH;
        m_MapECurrencyString2Value["HKD"] = ECurrency_HKD;
        m_MapECurrencyString2Value["AUD"] = ECurrency_AUD;
        m_MapECurrencyString2Value["CAD"] = ECurrency_CAD;
        m_MapECurrencyString2Value["NZD"] = ECurrency_NZD;
        m_MapECurrencyString2Value["SGD"] = ECurrency_SGD;
        m_MapECurrencyString2Value["SEK"] = ECurrency_SEK;
        m_MapECurrencyString2Value["NOK"] = ECurrency_NOK;
        m_MapECurrencyString2Value["MXN"] = ECurrency_MXN;
        m_MapECurrencyString2Value["ZAR"] = ECurrency_ZAR;
        m_MapECurrencyString2Value["DKK"] = ECurrency_DKK;
        m_MapECurrencyString2Value["PLN"] = ECurrency_PLN;
        m_MapECurrencyString2Value["RUB"] = ECurrency_RUB;
        m_MapECurrencyString2Value["BRL"] = ECurrency_BRL;
        m_MapECurrencyString2Value["CZK"] = ECurrency_CZK;

        m_MapECurrencyValue2String[ECurrency_CHF] = "CHF";
        m_MapECurrencyValue2String[ECurrency_EUR] = "EUR";
        m_MapECurrencyValue2String[ECurrency_USD] = "USD";
        m_MapECurrencyValue2String[ECurrency_GBP] = "GBP";
        m_MapECurrencyValue2String[ECurrency_JPY] = "JPY";
        m_MapECurrencyValue2String[ECurrency_CNY] = "CNY";
        m_MapECurrencyValue2String[ECurrency_CNH] = "CNH";
        m_MapECurrencyValue2String[ECurrency_HKD] = "HKD";
        m_MapECurrencyValue2String[ECurrency_AUD] = "AUD";
        m_MapECurrencyValue2String[ECurrency_CAD] = "CAD";
        m_MapECurrencyValue2String[ECurrency_NZD] = "NZD";
        m_MapECurrencyValue2String[ECurrency_SGD] = "SGD";
        m_MapECurrencyValue2String[ECurrency_SEK] = "SEK";
        m_MapECurrencyValue2String[ECurrency_NOK] = "NOK";
        m_MapECurrencyValue2String[ECurrency_MXN] = "MXN";
        m_MapECurrencyValue2String[ECurrency_ZAR] = "ZAR";
        m_MapECurrencyValue2String[ECurrency_DKK] = "DKK";
        m_MapECurrencyValue2String[ECurrency_PLN] = "PLN";
        m_MapECurrencyValue2String[ECurrency_RUB] = "RUB";
        m_MapECurrencyValue2String[ECurrency_BRL] = "BRL";
        m_MapECurrencyValue2String[ECurrency_CZK] = "CZK";

    }
};

////////////////////////////////////////////////////////////// ENUM <--> STRING end ///////////////////////////////////////////////////////

typedef struct
{
    u32	mdwSecurityID;//Unique的标的ID，由数据库生成
    string msSecuritySymbol;//例如ru1605中的ru
    ESecurityType meSecurityType;
    EListExchange meListExchange;//标的上市 所在交易所
    ETradingExchange meTradingExchange;//标的交易 所在交易所
    ECurrency meCurrency;
    timeval mtExpiration;//到期时间，如ru1605中的1605，统一格式为4位数字串YYmm
    f64 mdfStrike;//执行价
    string msOptionType; //期权类型
    u32 mdwUnderlayingSecurityID;//标的 的基础标的ID（如指数）
    u32 mdwContractSize;// 合约规模，交易单位
    f64 mdfTickSize;//最小报价单位
    f64 mdfLongMarginRate;//多头保证金率
    f64 mdfShortMarginRate;//空头保证金率
    f64 mdfGeneralMarginRate;//总保证金率
    string msISIN;//国际证券识别编码
    BOOL mbTradeAble;  //可交易
    BOOL mbExpirAble;  //是否到期限制
}TSecurity;


#endif//_BASEDATASTRUCT_H_
