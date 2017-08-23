#ifndef _BASETYPE_H_
#define _BASETYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------
//
//  General types
//
//-----------------------------------------------------------------------

//整型定义
#ifdef _OSBITS64_
typedef  unsigned int      u32;
#else
typedef  unsigned long      u32;
#endif
typedef  unsigned short		u16;
typedef  unsigned char		u8;

#ifdef _OSBITS64_
typedef signed int         s32;
#else
typedef signed long			s32;
#endif
typedef signed short		s16;


#ifdef __cplusplus

typedef char				s8;

#else

#ifdef _LINUX_

typedef char				s8;

#else

typedef signed char			s8;

#endif //_LINUX_

#endif //__cplusplus

#ifdef _MSC_VER

typedef signed __int64		s64;
typedef unsigned _int64		u64;

#else

typedef signed	long long	s64;
typedef unsigned long long	u64;

#endif //_MSC_VER


//兼容其它风格的整型定义

#ifndef _MSC_VER     //msc 已定义的UINT32类型

typedef  u64				UINT64;
typedef  u32				UINT32;
typedef  u16				UINT16;
typedef  u8					UINT8;

typedef  s64				INT64;
typedef  s32				INT32;
typedef  s16				INT16;
typedef  s8					INT8;

#endif //_MSC_VER




//兼容其它风格的整型定义
#ifdef _COMPATIBLE_TYPE      //由于某些模块调用的系统库如db已有 uint32_t 的定义，所以此处用宏来避免冲突

typedef  u64				uint64_t;
typedef  u32				uint32_t;
typedef  u16				uint16_t;
typedef  u8					uint8_t;

typedef  s64				int64_t;
typedef  s32				int32_t;
typedef  s16				int16_t;
typedef  s8					int8_t;

#endif //_COMPATIBLE_TYPE


//浮点型定义
#ifndef f32
typedef	float				f32;
#endif

#ifndef f64
typedef double				f64;
#endif


//BOOL 定义
typedef  int				BOOL;


// 字符指针
#ifndef _MSC_VER

#ifndef LPSTR
#define LPSTR   char *
#endif

#ifndef LPCSTR
#define LPCSTR  const char *
#endif

#endif //_MSC_VER


//句柄
#ifndef _MSC_VER

typedef void *  HANDLE;

#endif //_MSC_VER


//BOOL常量
#ifndef TRUE
#define TRUE				(BOOL)1
#endif

#ifndef FALSE
#define FALSE				(BOOL)0
#endif


//-----------------------------------------------------------------------
//
// Macros
//
//-----------------------------------------------------------------------

//参数标识定义
#ifndef IN
#define IN
#endif /* for Input */

#ifndef OUT
#define OUT
#endif /* for Output */


//文件名、目录名最大长度
#define MAX_FILENAME_LEN        (u8)64


//-----------------------------------------------------------------------
//
//  Error code
//
//-----------------------------------------------------------------------

#define DERRBASE_SET(base)       ((u32)base << 16)  //错误码的基数放在高16位，低16位为各模块内部具体的错误码
#define DERRBASE_GET(err)        (err & 0xffff0000) //得到具体错误码的基数
#define DERRLOWBITS_GET(err)     (err & 0x0000ffff) //得到具体错误码的低16位

/*================================= 错误码分配 ================================*/
#define DOAL_ERRBASE                 DERRBASE_SET(200)    //OAL
#define DOTL_ERRBASE                 DERRBASE_SET(210)    //OTL

typedef enum
{
    EECode_OK = 0,

    EECode_NotInitilized = 0x001,
    EECode_NotRunning = 0x002,

    EECode_Error = 0x003,
    EECode_Closed = 0x004,
    EECode_Busy = 0x005,
    EECode_OSError = 0x006,
    EECode_IOError = 0x007,
    EECode_TimeOut = 0x008,
    EECode_TooMany = 0x009,
    EECode_OutOfCapability = 0x00a,
    EECode_TimeTick = 0x00b,

    EECode_NoMemory = 0x00c,
    EECode_NoPermission = 0x00d,
    EECode_NoImplement = 0x00e,
    EECode_NoInterface = 0x00f,

    EECode_NotExist = 0x010,
    EECode_NotSupported = 0x011,

    EECode_BadState = 0x012,
    EECode_BadParam = 0x013,
    EECode_BadCommand = 0x014,
    EECode_BadFormat = 0x015,
    EECode_BadMsg = 0x016,
    EECode_BadSessionNumber = 0x017,

    EECode_TryAgain = 0x018,

    EECode_DataCorruption = 0x019,
    EECode_DataMissing = 0x01a,

    EECode_InternalMemoryBug = 0x01b,
    EECode_InternalLogicalBug = 0x01c,
    EECode_InternalParamsBug = 0x01d,

    EECode_ProtocolCorruption = 0x01e,
    EECode_AbortTimeOutAPI = 0x01f,
    EECode_AbortSessionQuitAPI = 0x020,

    EECode_UnknownError = 0x021,
    EECode_MandantoryDelete = 0x022,

    EECode_NetConnectFail = 0x023,

    EECode_NetSocketSend_Error = 0x024,
    EECode_NetSocketRecv_Error = 0x025,

    EECode_NotLogin = 0x026,
    EECode_AlreadyExist = 0x027,
} EECode;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // _BASETYPE_H_
