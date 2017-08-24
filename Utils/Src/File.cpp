#include "File.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void gfGetModuleFileName(int pid, char *path, int path_size)
{
    char buf[80];
    pid_t the_pid;
    the_pid = (pid_t)pid;

    if (((int)the_pid) == 0)
    {
        the_pid = getpid();
    }

    snprintf(buf, sizeof(buf), "/proc/%d/exe", (int)the_pid);

    memset(path, 0, path_size);
    ssize_t ret = readlink(buf, path, path_size - 1);
}

//获得与程序在同一个目录下的配置文件:config.xml
void gfGetConfigFile(char *config_file, int size)
{
    int i;
    char appname[1024];
    char path[1024];
    gfGetModuleFileName(0, appname, sizeof(appname));
    i = (int)strlen(appname) - 1;

    while (appname[i] != '/' && i >= 0)
    {
        i--;
    }

    if (i > 0)
    {
        appname[i] = 0;
    }

    memset(path, 0, sizeof(path));
    strncpy(path, appname, sizeof(path) - 20);
    i = strlen(path);
    path[i] = '/';
    //配置文件名：Config.xml
    strcpy(path + i + 1, "Config/Config.xml");
    strncpy(config_file, path, size - 1);
}
