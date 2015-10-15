#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef CONFIG_H
#define CONFIG_H

#include "Includes.h"

#define CFG_FOUND               0

#define ERR_CFG                 10
#define CFG_ERR_STP             11
#define CFG_NOTFND              12



class Config
{
protected:
    char cfgName[65];
    FILE * cfgFile;
    
public:
    // Constructores.
    Config();
    Config(char * strConfigName);
    ~Config();
   
    // Mensajes.  
    int SetCfgName(char * strConfigName);
    int GetItem(char * topic, char * item, char * value);
    int SetItem(char * topic, char * item, char * value);

};

#endif
