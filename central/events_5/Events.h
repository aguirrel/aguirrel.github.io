#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef EVENTS_H
#define EVENTS_H

#ifndef BLOCK
  #define BLOCK		15
  #define NOBLOCK	16
#endif

#include "8583.h"
#include "CACStat.h"
#include "Config.h"
#include "Daemons.h"
#include "Log.h"
#include "PidTable.h"
#include "Queues.h"
#include "Table.h"
#include "TimeOutQueue.h"
#include "TraceNmbr.h"
#include "TrxHeap.h"
#include "Pos.h"

int ObtYear();

class Event
{
protected:
   int		event_type;
   long		orig_pid;
   
public:
   Event();
   virtual ~Event();
   virtual int ProcessIt();
   virtual int GetType();
   inline char *GetName() { return "Null"; }
};

// Eventos desarrollados en forma externa.

#include "Batch.h"
#include "BatchRta.h"
#include "Cierre.h"
#include "CierreRta.h"
#include "EchoTest.h"
#include "EchoTestRta.h"
#include "PriceOffLine.h"
#include "PriceOn.h"
#include "PriceOnRta.h"
#include "RevTrx.h"
#include "RevTrxRta.h"
#include "ShutDown.h"
#include "TimeOut.h"
#include "TOCierre.h"
#include "TOCierreB.h"
#include "TOBatch.h"
#include "TOEchoTest.h"
#include "TORevTrx.h"
#include "PriceRev.h"
#include "RtaTrxAnul.h"
#include "RtaRevAnul.h"
#endif
