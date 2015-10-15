#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | BatchRta.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento que maneja la respuesta del mensaje de Batch        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

extern Config           Cfg;			/* Objeto de configuracion  */
extern Log              LogAlarm;		/* Logging de actividades   */
extern TimeOutQueue		timeout_queue;	/* Cola de timeouts         */ 
extern TrxHeap			Heap;			/* Objeto Heap              */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | BatchRta::BatchRta                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
BatchRta::BatchRta(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_BATCH_RTA;

    mensaje_iso=new Iso_Msg(msg_data, data_len);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | BatchRta::~BatchRta                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
BatchRta::~BatchRta()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | BatchRta::EnviarMsg                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int BatchRta::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "BatchRta: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | BatchRta::ProcessIt                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int BatchRta::ProcessIt()
{
    char			msg_str[2000];
    int				msg_len;
    int				iRet;
    str_heap_data	heap_data;
    str_sel_ntdata	term_data;
 
	/* Loguea la respuesta */
    LogAlarm.Put(0, "BatchRta: RESPUESTA ISO: %s\n", mensaje_iso->GetField(39));

    /* Checkear consistencia del mensaje	*/
    LogAlarm.Put(0, "BatchRta: Chequea consistencia del mensaje\n");
    if (!mensaje_iso->ChkReady())
    {
		LogAlarm.Put(0, "BatchRta: ERROR Objeto Iso Inconsistente\n");
		return NOOK;
    }

    /* Se copian los datos del mensaje */
    strcpy(term_data.nro_caj_ca, mensaje_iso->GetField(41));
    strcpy(term_data.nro_com   , mensaje_iso->GetField(42));

	/* Se obtienen datos de la terminal */
    iRet=getDataByCA(&term_data);
    if (iRet!=OK)
    {
		LogAlarm.Put( 0, "BatchRta: ERROR (%d) al obtener datos de la terminal\n", iRet);
		return NOOK;
    }

	/* Arma clave del heap */
    char idHeap[20];
    sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca,  term_data.nro_suc,
                                           term_data.nro_caj, mensaje_iso->GetField(11));
  
    /* Extrae los datos del heap */
	LogAlarm.Put(5, "BatchRta: Extrae datos del Heap\n");
    iRet=Heap.GetData(idHeap,(char *)&heap_data, sizeof(heap_data));
    if (iRet==NOOK)
	{
		LogAlarm.Put(0, "BatchRta: ERROR (%d) en HeapInfo. Id:[%s]\n", Heap.GetErrno(), idHeap);
		return NOOK;
	}

    /* Verifica y extrae de TimeoutQueue */
    LogAlarm.Put(0, "BatchRta: Saca datos de la TOQ\n");
    iRet=timeout_queue.RemTimeOut(idHeap);
    if (iRet==NOOK)
    {
		LogAlarm.Put(0, "BatchRta: ERROR al sacar datos de la TOQ (%d)\n", timeout_queue.GetErrno());
		return NOOK;
    }

	/* Aplana el mensaje */
    msg_len=mensaje_iso->GetMsgString(msg_str);
    
	/* Envia por la cola administrativa */
	LogAlarm.Put(0, "BatchRta: Envia mensaje a [%d] CAut [%s]\n", heap_data.orig_pid, term_data.nro_ca);
	EnviarMsg(heap_data.orig_pid, msg_str, msg_len);
	LogAlarm.Put(5, "BatchRta: Evento finalizado\n");

    return OK;
}
