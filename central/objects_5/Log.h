#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef LOG_H
#define LOG_H

#include "Includes.h"

class Log
{
protected:
    // Variables del archivo Principal
    int		level;
    int		debug_level;
    FILE	*log_fd;

public:
    Log();

    void Open(char *logname);
    void SetLevel(int new_level);
    void SetDebugLevel(int new_debug_level);
    void Put(int level, char *fmt, ...);
    void PutMsg(int level, char *cab, char *message, int length);
    void Close();
};
#endif
