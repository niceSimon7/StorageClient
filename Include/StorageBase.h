/*****************************************************************************
模块名      : Storage
文件名      : StorageBase.h
相关文件    : 存储模块的开发文件
文件实现功能: 提供存储的基本数据结构定义
作者        : sq
版本        : V1.0  Copyright(C) 2016-2021 Boguan Investment, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2016/08/24  1.0         sq          创建
******************************************************************************/

#ifndef _STORAGEBASE_H_
#define _STORAGEBASE_H_

#include "BaseDataStructs.h"

class CStorageBase
{
public:
    static CStorageBase* Create();

protected:
    CStorageBase();
    EECode Construct();
    virtual ~CStorageBase();

public:
    virtual void Delete();

public:
    // general functions:
    /*===========================================================
    函数名：InitStorage
    功能：初始化存储模块
    算法实现：需子类具体实现
    引用全局变量：无
    输入参数：无
    返回值：成功返回EECode_OK，失败返回错误码
    ============================================================*/
    virtual EECode InitStorage();

    /*===========================================================
    函数名：DeInitStorage
    功能：反初始化存储模块
    算法实现：需子类具体实现
    引用全局变量：无
    输入参数：无
    返回值：成功返回EECode_OK，失败返回错误码
    ============================================================*/
    virtual EECode DeInitStorage();//正常停止/紧急停止（完成最后一个回调数据的保存，关闭数据库)//could refer to func DestroyThread()  [TODO]

    /*===========================================================
    函数名：ReadFromDB
    功能：读取指定类型的数据（得到特定表里的数据）
    算法实现：需子类具体实现
    引用全局变量：无
    输入参数：  EDBDataType eDBDataType，数据类型
    			void* pData，用于数据输出的存储空间指针
    			u32 dwMaxRowCount，最大读取行数
    返回值：成功返回EECode_OK，失败返回错误码
    ============================================================*/
    virtual EECode ReadFromDB(IN EDBDataType eDBDataType, IN u32 dwMaxRowCount, IN OUT void* pData);


    /*===========================================================
    函数名：WriteToDB
    功能：存储指定类型的数据（写入特定的数据到表里）
    算法实现：需子类具体实现
    引用全局变量：无
    输入参数：  EDBDataType eDBDataType，数据类型
    			s32 sdwWriteMode，存储模式（SQL_MODE_SYNC为同步模式，SQL_MODE_ASYNC为异步模式）
    			void* pData，输入数据的指针
    返回值：成功返回EECode_OK，失败返回错误码
    ============================================================*/
    virtual EECode WriteToDB(IN EDBDataType eDBDataType, IN s32 sdwWriteMode, IN void* pData);


private:
    void clearData();

};


#endif//_STORAGEBASE_H_
