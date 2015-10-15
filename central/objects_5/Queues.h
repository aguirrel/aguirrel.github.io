#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef QUEUES_H
#define QUEUES_H

#include "Includes.h"

#ifndef OK
#define	OK				0
#define NOOK				-1
#endif

#define QUEUE_MAX_RETRY			3
#define QUEUE_MAX_DATA_SIZE		3000

class Queue
{
protected:
   int		QueueHandle;
   int		ErrorRC;
   int		len;
   long		Flags;

   struct
   {
      long	type;
      int	pid;
      char	data[QUEUE_MAX_DATA_SIZE];
   } msg;

public:
   Queue();

   int Open(long QueueID, long tmt=20);
   int Create(long QueueID, int QueuePerms);
   int SendMsg(long type, char *data, int len);
   int GetMsg(long type, char *data, int maxlen);
   int SetFlag(long new_f);
   int ClearFlag(long new_f);
   int Flush(long type);
   long GetMsgType();
   int GetOrigPid();
   int GetErrno();
   int Close() { return OK; }
   int Destroy();
   int StatusCA(int iCA);
};

#endif
