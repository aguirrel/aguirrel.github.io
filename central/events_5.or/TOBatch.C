#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TOBatch.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento de timeout al mensaje de batch                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "CACStat.h"
#include "Events.h"
#include "Log.h"
#include "Table.h"

extern CACStatus	CACS;			/* Status Canal X.25        */
extern Config       Cfg;			/* Objeto de configuracion  */
extern Log          LogAlarm;		/* Logging de actividades   */  
extern TrxHeap		Heap;			/* Objeto Heap              */
extern Queue        XQueue;			/* Cola de X.25             */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOBatch::TOBatch                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOBatch::TOBatch(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_TIMEOUT_BATCH;

    memcpy(&data, msg_data, sizeof(str_timeout_event));
    mensaje_iso=new Iso_Msg(TYPE_VISA_BATCH_RTA);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOBatch::~TOBatch                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOBatch::~TOBatch()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOBatch::EnviarMsg                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOBatch::EnviarMsg(int orig_pid, char *msg, int len)
{
	Queue AQueue;
	int iRet;
	char msgString[500];
	char auxString1[100];

	/* Abre cola de mensajes administrativos */
	Cfg.GetItem("AdminDaemon", "QueueID", auxString1);
	iRet = AQueue.Open(atol(auxString1));
	if (iRet==OK)
	{
		/* Envia mensaje */
		memset(msgString, 0, sizeof(msgString));
		memcpy(msgString, msg, len);
		iRet = AQueue.SendMsg(orig_pid, msgString, len);
	}
	if (iRet)
	{
		LogAlarm.Put(0, "TOBatch: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOBatch::ProcessIt                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOBatch::ProcessIt()
{
    char			msg_str[2000];
    int				msg_len;
    int				iRet;
    str_heap_data	heap_data;
	char			auxCa[3];
    
    LogAlarm.Put(0, "TOBatch: TimeOut al esperar respuesta al mensaje [%s]\n", data.key);

    /* Baja el estado del centro si no hay canales disponibles */
	sprintf(auxCa,"%2.2s",data.key);
	iRet = XQueue.StatusCA(atoi(auxCa));
	if (iRet<=0)
	{
		LogAlarm.Put(0, "TOBatch: Ret:[%d]. Baja estado CAut:[%s]\n", iRet, auxCa);
		iRet=CACS.Down(atoi(auxCa));
	    if (iRet==NOOK)
		{
			LogAlarm.Put(0, "TOBatch: ERROR:[%d] al cambiar estado CAut:[%s]\n", CACS.GetErrno(), auxCa);
			return NOOK;
		}
	}
	else
	{
		LogAlarm.Put(0, "TOBatch: Canales disponibles:[%d]. No baja estado CAut:[%s]\n", iRet, auxCa);
	}
    
	/* Extra los datos del heap */
    LogAlarm.Put(5, "TOBatch: Extrae datos del Heap\n");
    iRet=Heap.GetData(data.key,(char *)&heap_data, sizeof(heap_data));
    if (iRet==NOOK)
    {
		LogAlarm.Put( 0, "TOBatch: ERROR (%d) al sacar datos del Heap\n", Heap.GetErrno());
		return NOOK;
    }

    /* Pone codigo de respuesta FF indicando que fue Timeout */
    mensaje_iso->PutField(38, "FF");

    /* Aplana el mensaje */
    msg_len=mensaje_iso->GetMsgString(msg_str);

	/* Envia el mensaje por cola administrativa */
    LogAlarm.Put(0, "TOBatch: Enviando msg a (%d).\n", heap_data.orig_pid);
	EnviarMsg(heap_data.orig_pid, msg_str, msg_len);
	LogAlarm.Put(0, "TOBatch: Evento finalizado\n");

    return OK;
}
