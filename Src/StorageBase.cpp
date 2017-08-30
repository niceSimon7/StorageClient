#include "BaseType.h"
#include "StorageBase.h"

extern char g_szDataDB[100];//行情数据库
extern BOOL gfTimeValSys2Mysql(IN timeval sysTime, IN OUT s8 *sbyMysqlDATETIME3, IN u32 dwBufLen);

CStorageBase* CStorageBase::Create()
{
    CStorageBase *result = new CStorageBase();
    if ((result) && (EECode_OK != result->Construct()))
    {
        result->clearData();
        result->Delete();
        result = NULL;
    }
    return result;
}

CStorageBase::CStorageBase()
{

}

EECode CStorageBase::Construct()
{
    return EECode_OK;
}

CStorageBase::~CStorageBase()
{
    clearData();
}


void CStorageBase::Delete()
{
    clearData();
    delete this;
}

// general functions
EECode CStorageBase::InitStorage()
{
    return EECode_NoImplement;
}

EECode CStorageBase::DeInitStorage()
{
    return EECode_NoImplement;
}

EECode CStorageBase::ReadFromDB(IN EDBDataType eDBDataType, IN u32 dwMaxRowCount, IN OUT void* pData)
{
    return EECode_NoImplement;
}

EECode CStorageBase::WriteToDB(IN EDBDataType eDBDataType, IN s32 sdwWriteMode, IN void* pData)
{
    return EECode_NoImplement;
}

void CStorageBase::clearData()
{

}
