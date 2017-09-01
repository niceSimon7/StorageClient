#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string>
using namespace std;

//C++ Style
string& StringTrim(string& str);

string& DatabaseReplaceSupportedPatterns(string& szOriginalSQL);

string& StringReplace(string& str, const string& old_value, const string& new_value);


#endif // STRING_H_INCLUDED
