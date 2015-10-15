#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include "Includes.h"

#ifndef OK
    #define OK			0
#endif
#ifndef NOOK
    #define NOOK		-1
#endif

static struct sembuf	op_wait=	{ 0u, 0, 0 };
static struct sembuf	op_set[]=	{ 0u, 0, 0, 0, 1, SEM_UNDO };
static struct sembuf	op_reset[]= { 0u, -1, (IPC_NOWAIT | SEM_UNDO) };

class Semaphore
{
protected:
   int		SemHandle;
   int		ErrNo;
   int		ret;
   long		SemID;

public:
   Semaphore();

   int Create(long id, int flags);
   int Open(long id);
   int Set();
   /*int Wait();*/
   int Reset();
   int Close();
   int Destroy();
   
   int GetErrno();
};

#endif
