#ifndef CHAR_H_INCLUDED
#define CHAR_H_INCLUDED
#include <vector>
#include <string>
#include <map>

using namespace std;

void StringToLower(char * str);

void TableStringToMapArray(char *pszTableString, vector<map<string,char*> >& vResult, int dwRowCount);

void trim(char *str);

char *UTF8ToGBK(const char *szUTF8);

char *GBKToUTF8(const char *szGBK);

char *ReplacePattern(const char *szOriginalSQL, const char *szPattern, const char *szValue);

int TableText_AddField(char **ppszOut, int *pnMaxLen, int *pnUsedLen, const char *pszField);

int TableText_FinishLine(char **ppszOut, int *pnMaxLen, int *pnUsedLen);

char *my_strtok_r(char *head, char del, char **ppctx);



#endif // CHAR_H_INCLUDED
