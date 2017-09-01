#ifndef CHAR_H_INCLUDED
#define CHAR_H_INCLUDED
#include "BaseType.h"
#include "Time.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

char* StringToLower(IN char * str);

void trim(char *str);

char *UTF8ToGBK(const char *szUTF8);

char *GBKToUTF8(const char *szGBK);

//char *ReplacePattern(const char *szOriginalSQL, const char *szPattern, const char *szValue);

//s8* DatabaseReplaceSupportedPatterns(const s8 *szOriginalSQL);


#endif // CHAR_H_INCLUDED
