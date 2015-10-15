#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "Includes.h"
#include "Daemons.h"
#include "Queues.h"
#include "Events.h"
#include "TrxOff.h"
#include "RtaTrxOff.h"
#include "TmtTrxOff.h"

class EventQueue
{
protected:
   int		ErrNo;
   int		ret;
   Queue	EQueue;
   Event	*ptrEvent;
   char		msg_str[3000];

public:
   EventQueue();
   int Open(long key, int perms);
   Event *GetNext();
   int GetErrno();
   int Close();
};

#endif
