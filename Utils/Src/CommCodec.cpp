#include <ctype.h>
#include <stdio.h>
#include "CommCodec.h"
#include "StrTokr.h"
#include <iconv.h>
#include <string.h>


void StringToLower(char * str)
{
    for(int i=0;i<strlen(str);i++)
	{
        str[i]=tolower(str[i]);
	}

}

string&  StringReplaceAll(string&   str,const   string&   old_value,const   string&   new_value)
{
	if(0 == strcmp(old_value.c_str(),new_value.c_str()))//防止死循环
	{
		return str;
	}

    while(true)   {
        string::size_type   pos(0);
        if(   (pos=str.find(old_value))!=string::npos   )
            str.replace(pos,old_value.length(),new_value);
        else   break;
    }
    return   str;
}


//表格数据转成对象数组.
//pszTableString格式为: 行与行之间用\n分隔，字段之间用\t分隔。其中第一行为字段名称
void TableStringToMapArray(char *pszTableString, vector<map<string,char*> >& vResult, int dwRowCount)
{
	int i,nLen,nCount,nMaxLen,nListCount;
	char* szFieldNames[10000];
	char *pszStart;
	char *pszLine,*pszLineCtx;
	char *pszField,*pszFieldCtx;
	map<string,char*> mapRow;
	vResult.clear();
	vResult.reserve(dwRowCount);

    if(pszTableString == NULL)
    {
        return;
    }
	//第一行，列名
	pszLine = my_strtok_r(pszTableString,'\n',&pszLineCtx);
	if (!pszLine)
	{
		return;
	}

	nListCount = 0;
	pszField = my_strtok_r(pszLine,'\t',&pszFieldCtx);
	while(pszField)
	{
		trim(pszField);
		if (nListCount >= 10000)
		{
//		    LogMessage("ERROR|%s|%d|too much data from TableString",__FUNCTION__,__LINE__);
			break;
		}
		StringToLower(pszField);
		szFieldNames[nListCount]=pszField;
		nListCount++;
		pszField = my_strtok_r(NULL,'\t',&pszFieldCtx);
	}
	pszStart = pszLineCtx;

	//后续各行，数据
	nCount = 0;
	pszLine = my_strtok_r(pszStart,'\n',&pszLineCtx);
	while(pszLine)     ///寻找\n
	{
		mapRow.clear();

		i = 0;
		pszField = my_strtok_r(pszLine,'\t',&pszFieldCtx);
		while(pszField)   //寻找\t
		{
			trim(pszField);
			mapRow[szFieldNames[i]]=pszField;
			i++;
			if (i>=nListCount)
			{
				break;
			}
			pszField = my_strtok_r(NULL,'\t',&pszFieldCtx);
		}

		vResult.push_back(mapRow);
		nCount++;
		pszLine = my_strtok_r(NULL,'\n',&pszLineCtx);
	}

	return;
}

//去掉前面和后面的空格
void trim(char *str)
{
	int head,tail,len;
	head = 0;
	while (str[head] && str[head]==' ') {
		head++;
	}
	len = strlen(str);
	memmove(str, str+head, len-head+1);

	tail = strlen(str)-1;
	if (tail<0) {
		return;
	}
	while (tail>=0 && str[tail]==' ') {
		str[tail] = '\000';
		tail--;
	}
}


/* x2c() and unescape_url() stolen from NCSA code */
char HexToChar(const char *what)
{
  register char digit;

  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return(digit);
}

void EscapeUrl(const char *raw_url, char *out, int max_size)
{
	unsigned char uc;
	int x, y;
	x=y=0;
	while(raw_url[x] && y<=max_size-4) {
		uc = ((unsigned char*)raw_url)[x];
		if ((uc>='0'&&uc<='9') || (uc>='a'&&uc<='z') || (uc>='A'&&uc<='Z') ||(uc=='_')) {
			out[y] = raw_url[x];
			y++;
		}else{
			sprintf(out+y, "%%%02X", (unsigned)(unsigned char)raw_url[x]);
			y+=3;
		}
		x++;
	}
	out[y] = '\0';
}

void UnescapeUrl(const char *encoded_url, char *out, int max_size)
{
	int x,y;
	char ch;
	x=y=0;
	while(encoded_url[x] && y<max_size-1){
		ch = encoded_url[x];
		if(ch == '%') {
			if(!encoded_url[x+1] || !encoded_url[x+2]){
				break;
			}
			out[y] = HexToChar(&encoded_url[x+1]);
			x+=3;
		}else{
			if(ch=='+'){
				ch = ' ';
			}
			out[y] = ch;
			x++;
		}
		y++;
	}
	out[y] = '\0';
}

time_t GetTimeFromString(char *timestr)
 {
	char *pszField,* pszLineCtx,* pszLineCtx2;
	time_t tick;
	struct tm ct;

	memset(&ct,0,sizeof(struct tm));

	pszField = my_strtok_r(timestr,' ',&pszLineCtx);

	if (!pszField)//日期
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}

	pszField = my_strtok_r(pszField,'-',&pszLineCtx2);

	if (!pszField)//yyyy
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}
	ct.tm_year = atoi(pszField) - 1900;

	pszField = my_strtok_r(NULL,'-',&pszLineCtx2);
	if (!pszField)//MM
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}
	ct.tm_mon = atoi(pszField) - 1;

	pszField = my_strtok_r(NULL,'-',&pszLineCtx2);
	if (!pszField)//DD
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}
	ct.tm_mday = atoi(pszField);


	pszField = my_strtok_r(NULL,' ',&pszLineCtx);
	if (!pszField)//时间
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}
	pszField = my_strtok_r(pszField,':',&pszLineCtx);

	if (!pszField)//小时
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}
	ct.tm_hour = atoi(pszField);

	pszField = my_strtok_r(NULL,':',&pszLineCtx);
	if (!pszField)//分
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}
	ct.tm_min = atoi(pszField);

	pszField = my_strtok_r(NULL,':',&pszLineCtx);
	if (!pszField)//秒
	{
//		LogMessage("ERROR|%s|%d|time format exception %s",__FUNCTION__,__LINE__,timestr);
		return 0;
	}
	ct.tm_sec = atoi(pszField);

	tick = mktime(&ct);

	return tick;
 }


void UnescapeUrl(char *encoded_url)
{
	int x,y;
	char ch;
	x=y=0;
	while((ch=encoded_url[x]) != '\0'){
		if(ch == '%') {
			if(!encoded_url[x+1] || !encoded_url[x+2]){
				break;
			}
			encoded_url[y] = HexToChar(&encoded_url[x+1]);
			x+=3;
		}else{
			if(ch=='+'){
				ch = ' ';
			}
			encoded_url[y] = ch;
			x++;
		}
		y++;
	}
	encoded_url[y] = '\0';
}

void GetFormItem(const char *form_str, const char *field_name, char *value, int value_size)
{
	int i,len,j;

	value[0] = 0;
	i=0;
	len = (int)strlen(field_name);
	do{
		if(strncmp(form_str+i, field_name, len)==0){
			if(form_str[i+len] == '='){
				//found. parse the value str.
				j = 0;
				i += (len+1);
				while(form_str[i] && form_str[i]!='&'){
					value[j] = form_str[i];
					i++;
					j++;
					if(j>=value_size-1){
						break;
					}
				}
				value[j] = 0;
				//urlencoding转义
				UnescapeUrl(value);
				return;
			}else{
				i += len;
			}
		}
		//查找下一个'&'
		while(form_str[i] && form_str[i]!='&'){
			i++;
		}
		if(form_str[i]=='&'){
			i++;
		}
	}while(form_str[i]);
}

void GetFormItemRaw(const char *form_str, const char spector_char/* '&','$' etc*/, const char *field_name, char *value, int value_size)
{
	int i,len,j;

	value[0] = 0;
	i=0;
	len = (int)strlen(field_name);
	do{
		if(strncmp(form_str+i, field_name, len)==0){
			if(form_str[i+len] == '='){
				//found. parse the value str.
				j = 0;
				i += (len+1);
				while(form_str[i] && form_str[i]!=spector_char){
					value[j] = form_str[i];
					i++;
					j++;
					if(j>=value_size-1){
						break;
					}
				}
				value[j] = 0;
				return;
			}else{
				i += len;
			}
		}
		//查找下一个'&'
		while(form_str[i] && form_str[i]!=spector_char){
			i++;
		}
		if(form_str[i]==spector_char){
			i++;
		}
	}while(form_str[i]);
}

//注意：返回的指针需要调用者释放
char *GetFormItem(const char *form_str, const char *field_name)
{
	int i,len,j,nMaxSize;
	char *pszValue,*pszTemp;

	nMaxSize = 4096;
	pszValue = new char[nMaxSize];
	if (!pszValue)
	{
//		LogMessage("allocate memory for pszValue failed in GetFormItem() 2");
		return NULL;
	}

	pszValue[0] = 0;
	i=0;
	len = (int)strlen(field_name);
	do{
		if(strncmp(form_str+i, field_name, len)==0){
			if(form_str[i+len] == '='){
				//found. parse the value str.
				j = 0;
				i += (len+1);
				while(form_str[i] && form_str[i]!='&'){
					pszValue[j] = form_str[i];
					i++;
					j++;
					if(j>=nMaxSize-1){
						//重新分配内存
						pszTemp = new char[nMaxSize*2];
						if (!pszTemp)
						{
//							LogMessage("allocate memory for pszTemp failed in GetFormItem() 2");
							delete pszValue;
							return NULL;
						}
						memcpy(pszTemp,pszValue,j);
						delete pszValue;
						pszValue = pszTemp;
						nMaxSize = nMaxSize*2;
					}
				}
				pszValue[j] = 0;
				//urlencoding转义
				UnescapeUrl(pszValue);
				return pszValue;
			}else{
				i += len;
			}
		}
		//查找下一个'&'
		while(form_str[i] && form_str[i]!='&'){
			i++;
		}
		if(form_str[i]=='&'){
			i++;
		}
	}while(form_str[i]);
	return pszValue;
}

//注意：返回的指针需要调用者释放
char * GetFormItemRaw(const char *form_str, const char spector_char/* '&','$' etc*/, const char *field_name)
{
	int i,len,j,nMaxSize;
	char *pszValue,*pszTemp;

	nMaxSize = 4096;
	pszValue = new char[nMaxSize];
	if (!pszValue)
	{
//		LogMessage("allocate memory for pszValue failed in GetFormItemRaw() 2");
		return NULL;
	}

	pszValue[0] = 0;
	i=0;
	len = (int)strlen(field_name);
	do{
		if(strncmp(form_str+i, field_name, len)==0){
			if(form_str[i+len] == '='){
				//found. parse the value str.
				j = 0;
				i += (len+1);
				while(form_str[i] && form_str[i]!=spector_char){
					pszValue[j] = form_str[i];
					i++;
					j++;
					if(j>=nMaxSize-1){
						//重新分配内存
						pszTemp = new char[nMaxSize*2];
						if (!pszTemp)
						{
//							LogMessage("allocate memory for pszTemp failed in GetFormItemRaw() 2");
							delete pszValue;
							return NULL;
						}
						memcpy(pszTemp,pszValue,j);
						delete pszValue;
						pszValue = pszTemp;
						nMaxSize = nMaxSize*2;
					}
				}
				pszValue[j] = 0;
				//不转义
				return pszValue;
			}else{
				i += len;
			}
		}
		//查找下一个'&'
		while(form_str[i] && form_str[i]!=spector_char){
			i++;
		}
		if(form_str[i]==spector_char){
			i++;
		}
	}while(form_str[i]);
	return pszValue;
}

void GetHeaderItem(const char *header_str, const char *item_name, char *out_buf, int buf_size)
{
	const char *str;
	int len;
	int key_len;

	out_buf[0] = 0;
	str = strstr(header_str, item_name);
	if(str){
		key_len = (int)strlen(item_name);
		str += (key_len+1); //skip item name and ':'
		while((*str)==' ') str++;  //skip ' '
		len = 0;
		while(str[len] && !(str[len]=='\r' && str[len+1]=='\n')){
			len++;
		}
		if(len>buf_size-1){
			len = buf_size-1;
		}
		strncpy(out_buf, str, len);
		out_buf[len] = 0;
	}
}

//GBK编码：第一字节：0x81-0xFE,  第二字节：0x40-0xFE
void RegularGBKString(char* str)
{
	bool bHalf;
	unsigned char ch;
	unsigned char *ptr;

	ptr = (unsigned char*)str;
	bHalf = false;
	while ((ch=*ptr)!=0) {
		if (bHalf)
		{
			//正在等待收另一半汉字
			if (ch>=0x40 && ch<=0xFE)
			{
				//收到了另一半汉字，正确
			}
			else
			{
				//收到了另一半，但不是汉字。将前一半汉字置成?
				*(ptr-1) = '?';
			}
			bHalf = false;
		}
		else
		{
			if (ch==0x80 && ch==0xFF) {
				//第一字节既不是ASCII也不是GBK，替换成'?'
				*ptr = '?';
			}
			else if (ch>=0x81 && ch<=0xFE)
			{
				bHalf = true;
			}
		}
		++ptr;
	}

	//假设输入为GBK字符串，删除半个汉字
	ptr = (unsigned char*)str;
}

//ASCII字符必须是0x01-0x7F之间的字符。否则以?代替
void RegularAsciiString(char *str)
{
	unsigned char ch;
	unsigned char *ptr;
	ptr = (unsigned char*)str;
	while((ch=*ptr)!=0)
	{
		if (ch>0x7F)
		{
			*ptr = '?';
		}
		++ptr;
	}
}

void RegularSQLString(char* str)
{
    int i;
    char c;
    i=0;
    while((c=str[i])!='\0') {
		if (c==';' || c == '\'' || c == '\"') {
			str[i] = ' ';
		}
        i++;
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

//字段之间用\t分隔
//如果字段内容含有\t或\n，将它替换成空格。
//注： 更完美的替换应该是用 "\t" 替换'\t'，但这样会导致字段长度发生变化。为简化处理直接替换成空格。
//     因此对于包含\t或\n的文本字段就会有问题，使用时要特别注意。
int TableText_AddField(char **ppszOut, int *pnMaxLen, int *pnUsedLen, const char *pszField)
{
	int i,nLen,nNeedLen,nNewMaxLen;
	char *pszResult,*pszTail;
	char ch;

	if (NULL == pszField)
	{
//		LogMessage("pszField == NULL in TableText_AddField()");
		return -1;
	}

	if (NULL == *ppszOut)
	{
		//未分配过内存。
		nNewMaxLen = 1024;
		*ppszOut = new char[nNewMaxLen];
		*pnMaxLen = nNewMaxLen;
		*pnUsedLen = 0;
	}

	if (NULL == *ppszOut)
	{
		//仍然为NULL，说明上一步分配内存失败了。
//		LogMessage("*ppszOut == NULL in TableText_AddField()");
		return -2;
	}

	nLen = strlen(pszField);
	nNeedLen = ((*pnUsedLen)+nLen) + 2;
	if (nNeedLen >= (*pnMaxLen))
	{
		//内容超长，重新分配内存
		nNewMaxLen = (*pnMaxLen)*2 + 40;
		if (nNewMaxLen < nNeedLen)
		{
			//乘以2仍小于要求的长度。则取需要的长度
			nNewMaxLen = nNeedLen + 40;
		}
		pszResult = new char[nNewMaxLen];
		if (!pszResult)
		{
//			LogMessage("allocate memory for pszResult failed in TableText_AddField()");
			return -3;
		}
		memcpy(pszResult, *ppszOut, (*pnUsedLen) + 1);
		delete *ppszOut;
		*ppszOut = pszResult;
		*pnMaxLen = nNewMaxLen;
	}
	pszTail = *ppszOut + (*pnUsedLen);
	for (i=0;i<nLen;i++)
	{
		ch = pszField[i];
		if ('\t' == ch  ||  '\n'==ch)
		{
//			LogMessage("replace \\t or \\n to space in TableText_AddField(), pszField=%s", pszField);
			ch = ' ';
		}
		pszTail[i] = ch;
	}
	pszTail += nLen;
	strcpy(pszTail,"\t");
	*pnUsedLen = (*pnUsedLen) + nLen + 1;
	return 0;
}

//行之间用\n分隔
//每一行至少含有一个字段。而最末尾始终放置一个'\t'。如果空间不足在ora_Text_AddField()就会重新分配。
//因此，这里要做的就是将最后一个'\t'替换成'\n'
int TableText_FinishLine(char **ppszOut, int *pnMaxLen, int *pnUsedLen)
{
	int nUsedLen;

	if (NULL == *ppszOut)
	{
//		LogMessage("*ppszOut == NULL TableText_FinishLine()");
		return -1;
	}
	nUsedLen = *pnUsedLen;
	if (nUsedLen <= 0)
	{
//		LogMessage("used length == 0 in TableText_FinishLine()");
		return -2;
	}
	if(nUsedLen >= (*pnMaxLen))
	{
		//一定是出错了！
//		LogMessage("used length > max length in TableText_FinishLine()");
		return -3;
	}
	(*ppszOut)[nUsedLen-1] = '\n';
	(*ppszOut)[nUsedLen] = 0;

	return 0;
}




