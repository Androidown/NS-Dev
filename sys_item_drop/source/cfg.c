#include <string.h>
#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#endif


//从INI文件读取字符串类型数据
char *cfgGetIniKeyString(char *title, char *key, char *filename)
{
    FILE *fp;
    char szLine[1024];
    static char tmpstr[1024];
    int rtnval;
    int i = 0;
    int flag = 0;
    char *tmp;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("have no such file \n");
        return "";
    }
    while (!feof(fp))
    {
        rtnval = fgetc(fp);
        if (rtnval == EOF)
        {
            break;
        }
        else
        {
            szLine[i++] = rtnval;
        }
        if (rtnval == '\n')
        {
#ifndef WIN32
            i--;
#endif
            szLine[--i] = '\0';
            i = 0;
            tmp = strchr(szLine, '=');
            if ((tmp != NULL) && (flag == 1))
            {
                if (strstr(szLine, key) != NULL)
                {
                    //注释行
                    if ('#' == szLine[0])
                    {
                    }
                    else if ('\/' == szLine[0] && '\/' == szLine[1])
                    {
                    }
                    else
                    {
                        //找打key对应变量
                        strcpy(tmpstr, tmp + 1);
                        fclose(fp);
                        return tmpstr;
                    }
                }
            }
            else
            {
                strcpy(tmpstr, "[");
                strcat(tmpstr, title);
                strcat(tmpstr, "]");
                if (strncmp(tmpstr, szLine, strlen(tmpstr)) == 0)
                {
                    //找到title
                    flag = 1;
                }
            }
        }
    }
    fclose(fp);
    return "";
}

//从INI文件读取整类型数据
int cfgGetIniKeyInt(char *title, char *key, char *filename)
{
    return atoi(GetIniKeyString(title, key, filename));
}


// int main(int argc, char *argv[])
// {
//     int iCatAge;
//     char szCatName[32];
//     iCatAge = GetIniKeyInt("CAT", "age", g_szConfigPath);
//     strcpy(szCatName, GetIniKeyString("CAT", "name", g_szConfigPath));
//     return 0;
// }
