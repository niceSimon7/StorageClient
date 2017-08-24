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

// 2. TMysqlProcessList
void TMysqlProcessListMapArrayToStructArray(vector<map<string, s8 *> >& vResult, vector<TMysqlProcessList> &vMysqlProcessList);

// 3. TMysqlStatus
void TMysqlStatusMapArrayToStructArray(vector<map<string, s8 *> >& vResult, vector<TMysqlStatus> &vMysqlStatus);

#endif // MAPARRAYTOSTRUCTARRAY_H_INCLUDED
