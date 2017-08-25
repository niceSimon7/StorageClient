#include "Char.h"
#include <ctype.h>
#include <stdio.h>
#include <iconv.h>
#include <string.h>

char* StringToLower(IN char * str)
{
    for(int i=0; i<strlen(str); i++)
    {
        str[i]=tolower(str[i]);
    }
    return str; //全局，非局部
}


//去掉前面和后面的空格
void trim(char *str)
{
    int head,tail,len;
    head = 0;
    while (str[head] && str[head]==' ')
    {
        head++;
    }
    len = strlen(str);
    memmove(str, str+head, len-head+1);

    tail = strlen(str)-1;
    if (tail<0)
    {
        return;
    }
    while (tail>=0 && str[tail]==' ')
    {
        str[tail] = '\000';
        tail--;
    }
}

char *UTF8ToGBK(const char *szUTF8)
{
    iconv_t hIconv;
    size_t nInLen,nGBKLen;
    int nMaxLen,nConverted;
    const char *pszInStr;
    char *pszGBKStr,*pszGBKTmp;

    if (NULL==szUTF8)
    {
        szUTF8 = "";
    }
    pszInStr = szUTF8;
    nInLen = strlen(szUTF8);
    nMaxLen = nInLen*2;
    pszGBKStr = new char[nMaxLen+16];
    if (!pszGBKStr)
    {
//		LogMessage("allocate memory for pszGBKStr failed in UTF8ToGBK()");
        return NULL;
    }
    pszGBKStr[0] = 0;
    //UTF-8转成GB2312
    hIconv = iconv_open("GBK","UTF-8");
    if (((iconv_t)-1)!=hIconv)
    {
        pszGBKTmp = pszGBKStr;
        nGBKLen = nMaxLen;
        nConverted = iconv(hIconv, (char**)&pszInStr, &nInLen, &pszGBKTmp, &nGBKLen);

        if (nConverted<0)
        {
            nGBKLen = nMaxLen;
        }
        pszGBKStr[nMaxLen-nGBKLen] = '\0';
        iconv_close(hIconv);
    }
    else
    {
        memcpy(pszGBKStr, szUTF8, nInLen+1);
    }
    return pszGBKStr;
}

//将GBK转成UTF-8字符串
char *GBKToUTF8(const char *szGBK)
{
    iconv_t hIconv;
    size_t nInLen,nUtf8Len;
    int nMaxLen,nConverted;
    const char *pszInStr;
    char *pszUtf8Str,*pszUtf8Tmp;

    if (NULL==szGBK)
    {
        szGBK = "";
    }
    pszInStr = szGBK;
    nInLen = strlen(szGBK);
    nMaxLen = nInLen*2;
    pszUtf8Str = new char[nMaxLen+16];
    if (!pszUtf8Str)
    {
//		LogMessage("allocate memory for pszUtf8Str failed in GBKToUTF8()");
        return NULL;
    }
    pszUtf8Str[0] = 0;
    //GB2312转成UTF-8
    hIconv = iconv_open("UTF-8","GBK");
    if (((iconv_t)-1)!=hIconv)
    {
        pszUtf8Tmp = pszUtf8Str;
        nUtf8Len = nMaxLen;

        nConverted = iconv(hIconv, (char**)&pszInStr, &nInLen, &pszUtf8Tmp, &nUtf8Len);

        if (nConverted<0)
        {
            nUtf8Len = nMaxLen;
        }
        pszUtf8Str[nMaxLen-nUtf8Len] = '\0';
        iconv_close(hIconv);
    }
    else
    {
        memcpy(pszUtf8Str, szGBK, nInLen+1);
    }
    return pszUtf8Str;
}

char *ReplacePattern(const char *szOriginalSQL, const char *szPattern, const char *szValue)
{
    char *pszNew,*pszNew2;
    int nMaxLen;
    int nValueLen;
    int nLen;
    int i,j,k,l,ml;
    char szThisPattern[80];
    bool bFoundEnd,bNeedReplace;

    nValueLen = strlen(szValue);
    nLen = strlen(szOriginalSQL);
    nMaxLen = nLen+128;
    pszNew = new char[nMaxLen];
    if (!pszNew)
    {
//		LogMessage("allocate memory for pszNew failed in ReplacePattern()");
        return NULL;
    }

    i = j = k = 0;
    while (i<nLen)
    {
        if (szOriginalSQL[i]=='{' && i<nLen-1 && szOriginalSQL[i+1]=='$')
        {
            //处理模板，有两种结果：需要替换、不需要替换
            //寻找模板结尾
            j = i;
            bFoundEnd = false;
            szThisPattern[0] = 0;
            while (j<nLen-1)
            {
                if (szOriginalSQL[j]=='$' && szOriginalSQL[j+1]=='}')
                {
                    //找到结尾
                    bFoundEnd = true;
                    l = (j+2) - i;
                    if (l>(int)(sizeof(szThisPattern))-1)
                    {
//						LogMessage("Too long pattern '%s'", szOriginalSQL+i);
                        l = sizeof(szThisPattern)-1;
                        delete pszNew;
                        return NULL;
                    }
                    strncpy(szThisPattern, szOriginalSQL+i, l);
                    szThisPattern[l] = 0;
                    break;
                }
                j++;
            }

            if (!bFoundEnd)
            {
                //模板不配对，错误
//				LogMessage("patern not match in ReplacePattern(): %s", szOriginalSQL+i);
                delete pszNew;
                return NULL;
            }

            if (strcmp(szPattern, szThisPattern)==0)
            {
                //需要替换
                bNeedReplace = true;
                ml = k+nValueLen;
            }
            else
            {
                bNeedReplace = false;
                ml = k+l;
            }
            if (ml > nMaxLen-1)
            {
                //重新分配内存
                nMaxLen *= 2;
                if (nMaxLen<ml)
                {
                    nMaxLen = ml + 128;
                }
                pszNew2 = new char[nMaxLen];
                if (!pszNew2)
                {
//					LogMessage("allocate memory for pszNew2 failed 1 in ReplacePattern()");
                    delete pszNew;
                    return NULL;
                }
                memcpy(pszNew2,pszNew,k);
                delete pszNew;
                pszNew = pszNew2;
            }

            if (bNeedReplace)
            {
                memcpy(pszNew+k, szValue, nValueLen);
                k += nValueLen;
            }
            else
            {
                memcpy(pszNew+k, szThisPattern, l);
                k += l;
            }
            i += l;
        }
        else
        {
            //尚未到'{$'，拷贝
            if (k+1 > nMaxLen-1)
            {
                nMaxLen *= 2;
                if (nMaxLen<ml)
                {
                    nMaxLen = ml + 128;
                }
                pszNew2 = new char[nMaxLen];
                if (!pszNew2)
                {
//					LogMessage("allocate memory for pszNew2 failed 2 in ReplacePattern()");
                    delete pszNew;
                    return NULL;
                }
                memcpy(pszNew2,pszNew,k);
                delete pszNew;
                pszNew = pszNew2;
            }
            pszNew[k] = szOriginalSQL[i];
            k++;
            i++;
        }
    }

    pszNew[k] = 0;
    return pszNew;
}
