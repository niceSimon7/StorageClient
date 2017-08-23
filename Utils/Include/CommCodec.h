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

//�������ݲ�����4096�Ŀ��Ե�����������
void GetFormItem(const char *form_str, const char *field_name, char *value, int value_size);
void GetFormItemRaw(const char *form_str, const char spector_char/* '&','$' etc*/, const char *field_name, char *value, int value_size);

//�������ݿ��ܳ����ĵ���������������Ƿ��ص�ָ����Ҫ�������ͷ�
char *GetFormItem(const char *form_str, const char *field_name);
char *GetFormItemRaw(const char *form_str, const char spector_char/* '&','$' etc*/, const char *field_name);

//�ַ���ת�����
void RegularAsciiString(char *str);
void RegularGBKString(char* str);
char *UTF8ToGBK(const char *szUTF8);
char *GBKToUTF8(const char *szGBK);

void RegularSQLString(char* str);
char *ReplacePattern(const char *szOriginalSQL, const char *szPattern, const char *szValue);

//Oracle��� �ַ���ת��������
void TableStringToMapArray(char *pszTableString, vector<map<string,char*> >& vResult, int dwRowCount = 1);

string&  StringReplaceAll(string&   str,const   string&   old_value,const   string&   new_value) ;


void trim(char *str);
void StringToLower(char * str);



//�ֶ�֮����\t�ָ�
//����ֶ��ں���\t��\n�������滻�ɿո�
//ע�� ���������滻Ӧ������ "\t" �滻'\t'���������ᵼ���ֶγ��ȷ����仯��Ϊ�򻯴���ֱ���滻�ɿո�
//     ��˶��ڰ���\t��\n���ı��ֶξͻ������⣬ʹ��ʱҪ�ر�ע�⡣
int TableText_AddField(char **ppszOut, int *pnMaxLen, int *pnUsedLen, const char *pszField);

//��֮����\n�ָ�
//ÿһ�����ٺ���һ���ֶΡ�����ĩβʼ�շ���һ��'\t'������ռ䲻����ora_Text_AddField()�ͻ����·��䡣
//��ˣ�����Ҫ���ľ��ǽ����һ��'\t'�滻��'\n'
int TableText_FinishLine(char **ppszOut, int *pnMaxLen, int *pnUsedLen);

time_t GetTimeFromString(char *timestr);




#endif
