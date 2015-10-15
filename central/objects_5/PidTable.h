#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef PIDTABLE_H
#define PIDTABLE_H

#include "Semaphores.h"

typedef struct
{
   int	used;
   int	pid;
} str_pid_list;

class PidTable
{
protected:
   str_pid_list	*PidList;
   int		ret;
   int		ErrNo;
   int		MaxEntries;
   int		ShMemHandle;
   Semaphore	Sem;

public:
   void PrintTable();

   PidTable();

   int Create(long key, int perms, int max_entries);
   int Open(long key, int max_entries);
   int PutPid(int pid);
   int RemPid(int pid);
   int KillAll();
   int Close();
   int Destroy();

   int GetErrno();
};

#endif
