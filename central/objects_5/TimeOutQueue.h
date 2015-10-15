#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef TIMEOUTQUEUE_H
#define TIMEOUTQUEUE_H

#include "Includes.h"
#include "Daemons.h"
#include "Semaphores.h"

#ifndef OK
  #define OK				0
  #define NOOK				-1
#endif

typedef struct
{
   char		id[50];
   long		exp_tmstmp;
   int		event_type;
   char		expired;
   char     cod_ser[4];
} str_timeout;

typedef struct
{
   long		orig_pid;
   char     key[50];
   char     cod_ser[4];
} str_timeout_event;

class TimeOutQueue
{
protected:
   str_timeout	*timeout_data;
   int          ShMemHandle;
   int          ErrNo;
   int          ret;
   int			max_entries;
   Semaphore	Sem;

public:
   TimeOutQueue();
   ~TimeOutQueue();
   
   int Open(long SemKey, int entries,long tmtSecs=20);
   int SetTimeOut(char * id, long exp_time, int event_type, char *cod);
   int RemTimeOut(char * id);
   int FindTimeOut(char * id);
   int Close();
   int Create(long SemKey, int entries, int combin);
   int Destroy(); 
   int FindTO();
   str_timeout *GetTimeOut(int cont);
   int SetOne(int cont, int tiempo);
   int SetSecond(int cont);
  
   int GetErrno();
};

#endif
