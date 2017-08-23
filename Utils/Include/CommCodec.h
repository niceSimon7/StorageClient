#if !defined(_COMM_CODEC_H_)
#define _COMM_CODEC_H_
#include <string>
#include <vector>
#include <map>


using namespace std;

void GetHeaderItem(const char *header_str, const char *item_name, char *out_buf, int buf_size);
void EscapeUrl(const char *raw_url, char *out, int max_size);
void UnescapeUrl(char *encoded_url);
void UnescapeUrl(const char *encoded_url, char *out, int max_size);

//对于内容不超过4096的可以调用两个函数
void GetFormItem(const char *form_str, const char *field_name, char *value, int value_size);
void GetFormItemRaw(const char *form_str, const char spector_char/* '&','$' etc*/, const char *field_name, char *value, int value_size);

//对于内容可能超长的调用这个函数，但是返回的指针需要调用者释放
char *GetFormItem(const char *form_str, const char *field_name);
char *GetFormItemRaw(const char *form_str, const char spector_char/* '&','$' etc*/, const char *field_name);

//字符集转换相关
void RegularAsciiString(char *str);
void RegularGBKString(char* str);
char *UTF8ToGBK(const char *szUTF8);
char *GBKToUTF8(const char *szGBK);

void RegularSQLString(char* str);
char *ReplacePattern(const char *szOriginalSQL, const char *szPattern, const char *szValue);

//Oracle结果 字符串转换成数组
void TableStringToMapArray(char *pszTableString, vector<map<string,char*> >& vResult, int dwRowCount = 1);

string&  StringReplaceAll(string&   str,const   string&   old_value,const   string&   new_value) ;


void trim(char *str);
void StringToLower(char * str);



//字段之间用\t分隔
//如果字段内含有\t或\n，将它替换成空格。
//注： 更完美的替换应该是用 "\t" 替换'\t'，但这样会导致字段长度发生变化。为简化处理直接替换成空格。
//     因此对于包含\t或\n的文本字段就会有问题，使用时要特别注意。
int TableText_AddField(char **ppszOut, int *pnMaxLen, int *pnUsedLen, const char *pszField);

//行之间用\n分隔
//每一行至少含有一个字段。而最末尾始终放置一个'\t'。如果空间不足在ora_Text_AddField()就会重新分配。
//因此，这里要做的就是将最后一个'\t'替换成'\n'
int TableText_FinishLine(char **ppszOut, int *pnMaxLen, int *pnUsedLen);

time_t GetTimeFromString(char *timestr);




#endif
