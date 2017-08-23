#ifndef MAPARRAYTOSTRUCTARRAY_H_INCLUDED
#define MAPARRAYTOSTRUCTARRAY_H_INCLUDED
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include "BaseDataStructs.h"
using namespace std;

// 1. TSecurity -- db.security
void TSecurityMapArrayToStructArray(vector<map<string, s8 *> >& vResult, vector<TSecurity> &vSecurity);


#endif // MAPARRAYTOSTRUCTARRAY_H_INCLUDED
