#ifndef _BASEDATAENUM_H_
#define _BASEDATAENUM_H_

typedef enum
{
    ESecurityType_STK = 1,  //using 0 to denote all enum NULL(Init).
    ESecurityType_OPT,
    ESecurityType_IND,
    ESecurityType_FUT,
    ESecurityType_FOP,
    ESecurityType_CASH,
    ESecurityType_BAG,
} ESecurityType;


typedef enum
{
    EListExchange_SZSE = 30,//use sting as "SZE" instead of "SZSE"
    EListExchange_SSE,
    EListExchange_SHFE,
    EListExchange_DCE,
    EListExchange_CZCE,
    EListExchange_CFFEX,
    EListExchange_HKFE,
    EListExchange_SEHK,
    EListExchange_AMEX,
    EListExchange_AUTO,
    EListExchange_CBOE,
    EListExchange_CBOT,
    EListExchange_CFE,
    EListExchange_CME,
    EListExchange_DTB,
    EListExchange_GLOBEX,
    EListExchange_ECBOT,
    EListExchange_IDEALPRO,
    EListExchange_NYBOT,
    EListExchange_NYMEX,
    EListExchange_NASDAQ,
    EListExchange_NYSE,
    EListExchange_OTCCBB,
    EListExchange_PINK,
    EListExchange_SMART,
    EListExchange_SOFFEX,
    EListExchange_LMAX,
} EListExchange;


typedef enum
{
    ETradingExchange_SZSE = 70,//use sting as "SZE" instead of "SZSE"
    ETradingExchange_SSE,
    ETradingExchange_SHFE,
    ETradingExchange_DCE,
    ETradingExchange_CZCE,
    ETradingExchange_CFFEX,
    ETradingExchange_HKFE,
    ETradingExchange_SEHK,
    ETradingExchange_AMEX,
    ETradingExchange_AUTO,
    ETradingExchange_CBOE,
    ETradingExchange_CBOT,
    ETradingExchange_CFE,
    ETradingExchange_CME,
    ETradingExchange_DTB,
    ETradingExchange_GLOBEX,
    ETradingExchange_ECBOT,
    ETradingExchange_IDEALPRO,
    ETradingExchange_NYBOT,
    ETradingExchange_NYMEX,
    ETradingExchange_NASDAQ,
    ETradingExchange_NYSE,
    ETradingExchange_OTCCBB,
    ETradingExchange_PINK,
    ETradingExchange_SMART,
    ETradingExchange_SOFFEX,
    ETradingExchange_LMAX,
} ETradingExchange;


typedef enum
{
    ECurrency_CHF= 110,
    ECurrency_EUR,
    ECurrency_USD,
    ECurrency_GBP,
    ECurrency_JPY,
    ECurrency_CNY,
    ECurrency_CNH,
    ECurrency_HKD,
    ECurrency_AUD,
    ECurrency_CAD,
    ECurrency_NZD,
    ECurrency_SGD,
    ECurrency_SEK,
    ECurrency_NOK,
    ECurrency_MXN,
    ECurrency_ZAR,
    ECurrency_DKK,
    ECurrency_PLN,
    ECurrency_RUB,
    ECurrency_BRL,
    ECurrency_CZK,
} ECurrency;


//数据库数据类型
typedef enum
{
    EDBDataType_Security,		//db.security
    EDBDataType_ProcessList, //show processlist
    EDBDataType_Status //show status
} EDBDataType;


#endif//_BASEDATAENUM_H_
