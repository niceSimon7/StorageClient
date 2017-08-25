#include "String.h"
#include <string>
using namespace std;

string& StringTrim(string& str)
{
    int dwBeginPos=0;
    int dwEndPos = str.size() -1;
    while (str[dwBeginPos]==' '||str[dwBeginPos]==' ')//开头处为空格或者Tab，则跳过
    {
        dwBeginPos++;
    }

    while(str[dwEndPos]==' '||str[dwEndPos]==' ')//结尾处为空格或者Tab，则跳过
    {
        dwEndPos--;
    }

    if(dwBeginPos==0 && dwEndPos == str.size() -1)  //正常无需trim的字符串
    {
        return str;
    }

    str = str.substr(dwBeginPos, dwEndPos - dwBeginPos + 1);
    return str;
}
