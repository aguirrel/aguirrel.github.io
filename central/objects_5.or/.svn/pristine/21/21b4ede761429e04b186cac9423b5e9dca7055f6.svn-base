#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | EventQueue.C                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Manejo de la cola de eventos                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "EventQueue.h"
#include "Log.h"

extern Log	LogAlarm;
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EventQueue::EventQueue                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
EventQueue::EventQueue()
{
	ptrEvent=NULL;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EventQueue::Open                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Abre la cola de eventos                                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int EventQueue::Open(long key, int perms)
{
	ret=EQueue.Create(key, perms);
	if (ret==NOOK)
	{
		ErrNo=EQueue.GetErrno();
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EventQueue::GetNext                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el siguiente evento                                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/      
Event *EventQueue::GetNext()
{
	ret=EQueue.GetMsg(-300, msg_str, sizeof(msg_str));
	if (ret==NOOK)
	{
		ErrNo=EQueue.GetErrno();
		return NULL;
	}
	
	LogAlarm.Put(0, "EventQueue: Evento recibido [%03ld] %s", 
					 EQueue.GetMsgType(), currentTimeLog() );
	switch (EQueue.GetMsgType())
	{
		case EVENT_PRICEANUL:
			ptrEvent=new PriceRev(EQueue.GetOrigPid(), msg_str, ret);
		//	ptrEvent=new PriceAnul(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_PRICEONLINE:
			ptrEvent=new PriceOnLine(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_PRICEOFFLINE:
			ptrEvent=new PriceOffLine(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_PRICEONLINE_RTA:
			ptrEvent=new PriceOnLineRta(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_CIERRE:
			ptrEvent=new Cierre(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_CIERRE_RTA:
			ptrEvent=new CierreRta(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_BATCH:
			ptrEvent=new Batch(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_BATCH_RTA:
			ptrEvent=new BatchRta(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_TIMEOUT:
			ptrEvent=new TimeOut(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_TIMEOUT_CIERRE:
			ptrEvent=new TOCierre(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_TIMEOUT_CIERRE_B:
			ptrEvent=new TOCierreB(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_TIMEOUT_BATCH:
			ptrEvent=new TOBatch(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_TIMEOUT_ECHOTEST:
			ptrEvent=new TOEchoTest(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_TIMEOUT_REVERSO:
			ptrEvent=new TORevTrx(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_ECHOTEST:
			ptrEvent=new EchoTest(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_ECHOTEST_RTA:
			ptrEvent=new EchoTestRta(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_SHUTDOWN:
			ptrEvent=new ShutDown(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_SNDTRXOFFLINE:
			ptrEvent=new SndTrxOffLine(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_RTATRXOFFLINE:
			ptrEvent=new RtaTrxOffLine(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_TMTTRXOFFLINE:
			ptrEvent=new TmtTrxOffLine(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_SNDREVERSO:
			ptrEvent=new RevTrx(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_SNDREVERSO_RTA:
			ptrEvent=new RevTrxRta(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_PRICEREV:
			ptrEvent=new PriceRev(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_REVTRXANUL_RTA:
			ptrEvent=new RtaRevAnul(EQueue.GetOrigPid(), msg_str, ret);
			break;
		case EVENT_SNDTRXANUL_RTA:
			ptrEvent=new RtaTrxAnul(EQueue.GetOrigPid(), msg_str, ret);
			break;
		default:
			ptrEvent=new Event();
			break;
	} /* end del switch */
	return ptrEvent;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | GetErrno                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el error producido                                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int EventQueue::GetErrno()
{
	return ErrNo;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EventQueue::Close                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra la cola de eventos                                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int EventQueue::Close()
{
	ret=EQueue.Destroy();
	if (ret==NOOK)
	{
		ErrNo=EQueue.GetErrno();
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}


