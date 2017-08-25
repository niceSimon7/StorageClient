#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <stdio.h>
#include "Config.h"
#include "Base64.h"

xmlDocPtr g_cfg_document=NULL;
xmlNodePtr g_root_node=NULL;
char g_cfg_fname[255];

xmlDocPtr g_cfg_document2=NULL;
xmlNodePtr g_root_node2=NULL;

bool Cfg_Load(const char *xml_file)
{
    char *buf;
    long flen;
    size_t ret=0;
    snprintf(g_cfg_fname,sizeof(g_cfg_fname),"%s",xml_file);
    FILE *fp = fopen(xml_file,"rb");
    if(!fp)
    {
//		LogMessage("Cann not open config file!");
        return false;
    }
    fseek(fp,0,SEEK_END);
    flen= ftell(fp);
    buf = new char[flen+1];
    fseek(fp,0,SEEK_SET);
    ret = fread(buf,1,flen,fp);
    buf[flen]=0;
    fclose(fp);

    g_cfg_document = xmlParseMemory(buf, flen);
    delete buf;
    if(!g_cfg_document)
    {
//		LogMessage("parse config file failed.");
        return false;
    }
    g_root_node = xmlDocGetRootElement(g_cfg_document);
    if(!g_root_node)
    {
//		LogMessage("can not find the root node of config file.");
        return false;
    }
    return true;
}

void Cfg_Unload()
{
    if(g_cfg_document)
    {
        xmlFreeDoc(g_cfg_document);
    }
    g_cfg_document = NULL;
    g_root_node = NULL;
    strcpy(g_cfg_fname,"");
}

void Cfg_SectionItem(xmlNodePtr root_node,const char *section_name, const char *item_name, char *item_value, int max_size)
{
    xmlNodePtr node,node2;
    xmlChar *xml;
    strcpy(item_value,"");

    if(NULL == root_node)
    {
        return;
    }

    for(node=root_node->children; root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,section_name) == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,item_name)==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strncpy(item_value,(const char*)xml,max_size);
                        item_value[max_size-1] = 0;
                        xmlFree(xml);
                    }
                }
            }
            break;
        }
    }
}


void Cfg_SectionItem(const char *section_name, const char *item_name, char *item_value, int max_size)
{
    xmlNodePtr node,node2;
    xmlChar *xml;
    strcpy(item_value,"");

    for(node=g_root_node->children; g_root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,section_name) == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,item_name)==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strncpy(item_value,(const char*)xml,max_size);
                        item_value[max_size-1] = 0;
                        xmlFree(xml);
                    }
                }
            }
            break;
        }
    }
}

void Cfg_DecryptSectionItem(const char *section_name, const char *item_name, char *item_value, int max_size)
{
    xmlNodePtr node,node2;
    xmlChar *xml;
    strcpy(item_value,"");
    char szEnBuffer[29];
    szEnBuffer[0]='\0';
    int nLen;

    for(node=g_root_node->children; g_root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,section_name) == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,item_name)==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        snprintf(szEnBuffer,sizeof(szEnBuffer),"%s",(const char*)xml);
                        Base64_Decode(szEnBuffer,strlen(szEnBuffer),(unsigned char*)item_value,max_size,&nLen);
                        item_value[nLen-1] = 0;
                        xmlFree(xml);
                    }
                }
            }
            break;
        }
    }

    for(int i=0; i<strlen(item_value); i++)
    {
        if('*'==item_value[i])
        {
            item_value[i]=0;
        }
    }
}


void Cfg_EncryptSectionItem(const char *section_name, const char *item_name,char *item_value, int max_size)
{
    xmlNodePtr node,node2;
    xmlChar *xml;
    char szEnBuffer[29];
    char szBuffer[20];//最长取19位
    int nLen;

    snprintf(szBuffer,sizeof(szBuffer),"%s",item_value);
    while(strlen(szBuffer)<19)
    {
        snprintf(szBuffer+strlen(szBuffer),sizeof(szBuffer)-strlen(szBuffer),"*****");
    }

    Base64_Encode((const unsigned char*)szBuffer,strlen(szBuffer),szEnBuffer,sizeof(szEnBuffer),&nLen);
    szEnBuffer[28]='\0';

    for(node=g_root_node->children; g_root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,section_name) == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,item_name)==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {

                        xmlSetProp(node2, BAD_CAST "value", (const xmlChar*)szEnBuffer);
                        xmlFree(xml);
                    }
                }
            }
            break;
        }
    }

}

void Cfg_Save ()
{
    xmlSaveFormatFile (g_cfg_fname, g_cfg_document, 1);

}

void Cfg_ServerAddress(const char *base_node_name, char *ip, int max_size, unsigned short *port)
{
    char ip_buf[80];
    char port_buf[40];
    xmlNodePtr node,node2;
    xmlChar *xml;
    strcpy(ip_buf,"");
    strcpy(port_buf,"");

    for(node=g_root_node->children; g_root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,base_node_name) == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,"ip")==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strcpy(ip_buf,(const char*)xml);
                        xmlFree(xml);
                    }
                }
                if(strcmp((const char*)node2->name,"port")==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strcpy(port_buf,(const char*)xml);
                        xmlFree(xml);
                    }
                }
            }
            break;
        }
    }

    strncpy(ip,ip_buf,max_size);
    *port = (unsigned short)atoi(port_buf);
}

void Cfg_MysqlServer(const char *mysql_name,
                     char *host, int host_max_size,
                     unsigned short *port,
                     char *user, int user_max_size,
                     char *password, int password_max_size)
{
    char port_buf[80];
    char host_buf[80];
    char user_buf[40];
    char password_buf[40];
    xmlNodePtr node,node2;
    xmlChar *xml;
    unsigned short save_port;

    save_port = 3306;
    strcpy(host_buf,"");
    strcpy(user_buf,"");
    strcpy(password_buf,"");

    for(node=g_root_node->children; g_root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,mysql_name) == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,"host")==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strncpy(host_buf,(const char*)xml,sizeof(host_buf));
                        host_buf[sizeof(host_buf)-1] = 0;
                        xmlFree(xml);
                    }
                }
                if(strcmp((const char*)node2->name,"port")==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strncpy(port_buf,(const char*)xml,sizeof(port_buf));
                        port_buf[sizeof(port_buf)-1] = 0;
                        save_port = atoi(port_buf);
                        xmlFree(xml);
                    }
                }
                if(strcmp((const char*)node2->name,"user")==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strncpy(user_buf,(const char*)xml,sizeof(user_buf));
                        user_buf[sizeof(user_buf)-1] = 0;
                        xmlFree(xml);
                    }
                }
                if(strcmp((const char*)node2->name,"password")==0)
                {
                    xml = xmlGetProp(node2, BAD_CAST "value");
                    if(xml)
                    {
                        strncpy(password_buf,(const char*)xml,sizeof(password_buf));
                        password_buf[sizeof(password_buf)-1] = 0;
                        xmlFree(xml);
                    }
                }
            }
            break;
        }
    }

    strncpy(host,host_buf,host_max_size);
    *port = save_port;
    strncpy(user,user_buf,user_max_size);
    strncpy(password,password_buf,password_max_size);
}

void Cfg_GetVectorList(struct S_VectorConfig *pVectors, int nMaxCount, int *pnReturnCount)
{
    char szNameBuf[80];
    char szSQLBuf[8192];
    xmlNodePtr node,node2;
    xmlChar *xml;
    int nCount;

    *pnReturnCount = 0;
    nCount = 0;
    for(node=g_root_node->children; g_root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,"string_vectors") == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,"vector")==0)
                {
                    strcpy(szNameBuf,"");
                    strcpy(szSQLBuf,"");
                    xml = xmlGetProp(node2, BAD_CAST "name");
                    if(xml)
                    {
                        strncpy(szNameBuf,(const char*)xml,sizeof(szNameBuf)-1);
                        szNameBuf[sizeof(szNameBuf)-1] = 0;
                        xmlFree(xml);
                    }
                    xml = xmlGetProp(node2, BAD_CAST "sql");
                    if(xml)
                    {
                        strncpy(szSQLBuf,(const char*)xml,sizeof(szSQLBuf)-1);
                        szSQLBuf[sizeof(szSQLBuf)-1] = 0;
                        xmlFree(xml);
                    }
                    if (0==strlen(szNameBuf))
                    {
//						LogMessage("vector name is empty");
                        continue;
                    }
                    if (nCount>=nMaxCount)
                    {
//						LogMessage("more vectors, ignore some.");
                        continue;
                    }

                    strncpy(pVectors[nCount].szName, szNameBuf, sizeof(pVectors[nCount].szName)-1);
                    pVectors[nCount].szName[sizeof(pVectors[nCount].szName)-1] = 0;

                    strncpy(pVectors[nCount].szSQL, szSQLBuf, sizeof(pVectors[nCount].szSQL)-1);
                    pVectors[nCount].szSQL[sizeof(pVectors[nCount].szSQL)-1] = 0;
                    nCount++;
                }
            }
        }
    }

    *pnReturnCount = nCount;
}

void Cfg_GetDisks(S_DiskConfig *pDisks, int nMaxCount, int *pnReturnCount)
{
    char szType[16];
    char szBaseDir[128];
    int nReserve;
    xmlNodePtr node,node2;
    xmlChar *xml;
    int nCount;
    char cUnit;

    *pnReturnCount = 0;
    nCount = 0;
    for(node=g_root_node->children; g_root_node && node; node=node->next)
    {
        if(strcmp((const char*)node->name,"disk_spaces") == 0)
        {
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,"disk")==0)
                {
                    strcpy(szType,"");
                    strcpy(szBaseDir,"");
                    nReserve = 0;
                    xml = xmlGetProp(node2, BAD_CAST "type");
                    if(xml)
                    {
                        strncpy(szType,(const char*)xml,sizeof(szType)-1);
                        szType[sizeof(szType)-1] = 0;
                        xmlFree(xml);
                    }
                    xml = xmlGetProp(node2, BAD_CAST "base_dir");
                    if(xml)
                    {
                        strncpy(szBaseDir,(const char*)xml,sizeof(szBaseDir)-1);
                        szBaseDir[sizeof(szBaseDir)-1] = 0;
                        xmlFree(xml);
                    }
                    xml = xmlGetProp(node2, BAD_CAST "reserve_space");
                    if(xml)
                    {
                        nReserve = atoi((const char*)xml);
                        //单位可能是 M或G，如果是G则乘以1000
                        //cUnit = (const char*)xml[strlen(xml)-1];
                        cUnit = *((const char*)xml+strlen((const char *)xml)-1);
                        if ('m'==cUnit || 'M'==cUnit)
                        {
                            //单位M
                        }
                        else if ('g'==cUnit || 'G'==cUnit)
                        {
                            //单位G
                            nReserve *= 1024;
                        }
                        else
                        {
                            //单位不是M也不是G，可能出问题了。
//							LogMessage("bad space unit: %c", cUnit);
                        }
                        xmlFree(xml);
                    }
                    if (nCount>=nMaxCount)
                    {
//						LogMessage("more disks, ignore some.");
                        continue;
                    }

                    strncpy(pDisks[nCount].szBaseDir, szBaseDir, sizeof(pDisks[nCount].szBaseDir)-1);
                    pDisks[nCount].szBaseDir[sizeof(pDisks[nCount].szBaseDir)-1] = 0;
                    pDisks[nCount].nType = atoi(szType);

                    pDisks[nCount].nReserveSpace = nReserve;
                    nCount++;
                }
            }
        }
    }

    *pnReturnCount = nCount;
}

#if 0
bool Cfg2_Load(const char *config_xml_file)
{
    xmlDocPtr doc;           //定义解析文档指针
    xmlNodePtr curNode, curNode1;      //定义结点指针(你需要它为了在各个结点间移动)
    xmlChar *szKey;          //临时字符串变量

    if (!config_xml_file)
    {
        LogMessage("Cfg_Load2, config_xml_file is NULL");
        return false;
    }

    doc = xmlReadFile(config_xml_file,"GB2312",XML_PARSE_RECOVER); //解析文件
    //检查解析文档是否成功，如果不成功，libxml将指一个注册的错误并停止。
    //一个常见错误是不适当的编码。XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存。
    //如果文档是这样，libxml将自动地为你转换到UTF-8。更多关于XML编码信息包含在XML标准中.
    if (NULL == doc)
    {
        LogMessage("Document %s not parsed successfully", config_xml_file);
        return false;
    }

    curNode = xmlDocGetRootElement(doc); //确定文档根元素

    /*检查确认当前文档中包含内容*/
    if (NULL == curNode)
    {
        fprintf(stderr,"empty document\n");
        xmlFreeDoc(doc);
        return false;
    }

    /*在这个例子中，我们需要确认文档是正确的类型。“root”是在这个示例中使用文档的根类型。*/

    if (xmlStrcmp(curNode->name, BAD_CAST "data"))
    {
        fprintf(stderr,"document of the wrong type, root node != data\n");
        xmlFreeDoc(doc);
        return false;
    }

    curNode = curNode->xmlChildrenNode;

    xmlNodePtr propNodePtr = curNode;

    u32 dwIdx = 0 ;

    while(curNode != NULL)
    {
        //取出节点中的内容
        if ((!xmlStrcmp(curNode->name, (const xmlChar *)"contract")))
        {
            curNode1 = curNode->xmlChildrenNode;
            while(curNode1 != NULL)
            {
                if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"conid")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("conid: %s\n", szKey);
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"priceUnit")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("priceUnit: %.2f\n", atof((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"exchange")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("exchange: %s\n", szKey);
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"multiplier")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("multiplier: %d\n", atoi((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"marginrate")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("marginrate: %.2f\n", atof((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"onlyCloseYesterday")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("onlyCloseYesterday: %d\n", atoi((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"openSpeculateMax")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("openSpeculateMax: %d\n", atoi((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"openHedgeMax")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("openHedgeMax: %d\n", atoi((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"cancelSpeculateMax")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("cancelSpeculateMax: %d\n", atoi((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"cancelHedgeMax")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("cancelHedgeMax: %d\n", atoi((const s8*)szKey));
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"marketBeginTime")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("marketBeginTime: %s\n", szKey);
                    xmlFree(szKey);
                }
                else if ((!xmlStrcmp(curNode1->name, (const xmlChar *)"restTime")))
                {
                    szKey = xmlNodeGetContent(curNode1);
                    printf("restTime: %s\n", szKey);
                    xmlFree(szKey);
                }
                curNode1 = curNode1->next;
            }
        }

        curNode = curNode->next;
    }

    xmlFreeDoc(doc);
    return true;
}
#endif

//2017.01.26 sq add for parse Ctp.xml format ONLY
bool Cfg2_Load(const char *config_xml_file)
{
    xmlChar *szKey;          //临时字符串变量

    if (!config_xml_file)
    {
//       LogMessage("Cfg_Load2, config_xml_file is NULL");
        return false;
    }

    g_cfg_document2 = xmlReadFile(config_xml_file,"GB2312",XML_PARSE_RECOVER); //解析文件
    //检查解析文档是否成功，如果不成功，libxml将指一个注册的错误并停止。
    //一个常见错误是不适当的编码。XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存。
    //如果文档是这样，libxml将自动地为你转换到UTF-8。更多关于XML编码信息包含在XML标准中.
    if (NULL == g_cfg_document2)
    {
//       LogMessage("Document %s not parsed successfully", config_xml_file);
        return false;
    }

    g_root_node2 = xmlDocGetRootElement(g_cfg_document2); //确定文档根元素

    /*检查确认当前文档中包含内容*/
    if (NULL == g_root_node2)
    {
//       LogMessage("empty document.");
        xmlFreeDoc(g_cfg_document2);
        return false;
    }

    return true;
}

void Cfg2_Unload()
{
    if(g_cfg_document2)
    {
        xmlFreeDoc(g_cfg_document2);
    }
    g_cfg_document2 = NULL;
    g_root_node2 = NULL;

}

bool Cfg2_SectionItem(const char *section_name, u32 dwSectionIdx, const char *item_name, char *item_value, int max_size)//dwSectionIdx is for those sections with the same name
{
    xmlNodePtr node,node2;
    xmlChar *szKey;
    strcpy(item_value,"");
    u32 dwCurSectionIdx = 0;

    for(node=g_root_node2->children; g_root_node2 && node; node=node->next)
    {
        if(strcmp((const char*)node->name,section_name) == 0)
        {
            if (dwCurSectionIdx < dwSectionIdx)
            {
                dwCurSectionIdx++;
                continue;
            }
            for(node2=node->children; node2; node2=node2->next)
            {
                if(strcmp((const char*)node2->name,item_name)==0)
                {
                    szKey = xmlNodeGetContent(node2);
                    if(szKey)
                    {
                        strncpy(item_value,(const char*)szKey,max_size);
                        item_value[max_size-1] = 0;
                        xmlFree(szKey);
                        return true;
                    }
                }
            }
            return false;
        }
    }
    return false;
}

