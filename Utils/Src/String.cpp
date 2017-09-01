#include "String.h"
#include <string>
using namespace std;

//// C Style
//string& StringTrim(string& str)
//{
//    int dwBeginPos=0;
//    int dwEndPos = str.size() -1;
//    while (str[dwBeginPos]==' '||str[dwBeginPos]==' ')//开头处为空格或者Tab，则跳过
//    {
//        dwBeginPos++;
//    }
//
//    while(str[dwEndPos]==' '||str[dwEndPos]==' ')//结尾处为空格或者Tab，则跳过
//    {
//        dwEndPos--;
//    }
//
//    if(dwBeginPos==0 && dwEndPos == str.size() -1)  //正常无需trim的字符串
//    {
//        return str;
//    }
//
//    str = str.substr(dwBeginPos, dwEndPos - dwBeginPos + 1);
//    return str;
//}

//C++ Style
string& StringTrim(string& str){
    if(str.empty()){
        return str;
    }

    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ")+1);
    return str;
}

string& DatabaseReplaceSupportedPatterns(string& szOriginalSQL){
    string sPattern("{$NOW$}");

    //If do not have {$NOW$}, do nothing(Mostly)
    string::size_type pos = szOriginalSQL.find(sPattern);
    if(pos == string::npos){
        return szOriginalSQL;
    }

    //Get Time string
    time_t tNow;
    time(&tNow);

    struct tm ct;
    localtime_r(&tNow, &ct);

    char szNow[80];
    snprintf(szNow+1, sizeof(szNow)-1, "%04d-%02d-%02d %02d:%02d:%02d",
             (int)(ct.tm_year +1900), (int)(ct.tm_mon +1), (int)(ct.tm_mday),
             (int)(ct.tm_hour), (int)(ct.tm_min), (int)(ct.tm_sec)
            );
    string sTime(szNow);

    //Relpace all patterns
    return StringReplace(szOriginalSQL, sPattern, sTime);
}

string& StringReplace(string& str, const string& old_value, const string& new_value){
    while(true){
        string::size_type pos = str.find(old_value);
        if(pos != string::npos){
            str.replace(pos,old_value.length(), new_value);
        }else{
            return str;
        }
    }
}
