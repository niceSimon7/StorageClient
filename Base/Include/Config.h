#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <libxml/tree.h>
#include <libxml/parser.h>
#include "BaseType.h"

struct S_VectorConfig
{
    char szName[80];
    char szSQL[8192];
};

struct S_DiskConfig
{
    int nType;
    char szBaseDir[128];
    int nReserveSpace;
};

bool Cfg_Load(const char *config_xml_file=NULL);
void Cfg_Unload();
void Cfg_Save ();
void Cfg_SectionItem(const char *section_name, const char *item_name, char *item_value, int max_size);
void Cfg_SectionItem(xmlNodePtr root_node,const char *section_name, const char *item_name, char *item_value, int max_size);
void Cfg_DecryptSectionItem(const char *section_name, const char *item_name, char *item_value, int max_size);
void Cfg_EncryptSectionItem(const char *section_name, const char *item_name,char *item_value, int max_size);
void Cfg_ServerAddress(const char *server_name, char *ip, int max_size, unsigned short *port);
void Cfg_MysqlServer(const char *mysql_name,
                     char *host, int host_max_size,
                     unsigned short *port,
                     char *user, int user_max_size,
                     char *password, int password_max_size);

void Cfg_GetVectorList(struct S_VectorConfig *pVectors, int nMaxCount, int *pnReturnCount);

void Cfg_GetDisks(struct S_DiskConfig *pSpaces,int nMaxCount,int *pnReturnCount);

bool Cfg2_Load(const char *config_xml_file);
void Cfg2_Unload();
bool Cfg2_SectionItem(const char *section_name, u32 dwSectionIdx, const char *item_name, char *item_value, int max_size);//dwSectionIdx is for those sections with the same name

#endif

