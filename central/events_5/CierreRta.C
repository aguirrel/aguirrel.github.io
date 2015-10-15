#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | CierreRta.C                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento de respuesta al mensaje de cierre de lote           |
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
| FUNCION         | CierreRta::CierreRta                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
CierreRta::CierreRta(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_CIERRE_RTA;
    mensaje_iso=new Iso_Msg(msg_data, data_len);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CierreRta::~CierreRta                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
CierreRta::~CierreRta()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CierreRta::EnviarMsg                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CierreRta::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "CierreRta: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CierreRta::ProcessIt                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CierreRta::ProcessIt()
{
    char			aux_str[100];
    char			msg_str[2000];
    int				msg_len;
    int				iRet;
    str_heap_data	heap_data;
    str_sel_ntdata	term_data;
	Queue			AQueue;
    char			idHeap[20];
    
	/* Chequea consistencia del mensaje */
    LogAlarm.Put(95, "CierreRta: Checkeo de consistencia del msg\n");
    if (!mensaje_iso->ChkReady())
    {
		LogAlarm.Put( 0, "CierreRta: ERROR Chequeo consistencia del mensaje\n");
		return NOOK;
    }

	/* Obtiene datos del mensaje */
    strcpy(term_data.nro_caj_ca, mensaje_iso->GetField(41));
    strcpy(term_data.nro_com   , mensaje_iso->GetField(42));

	/* Loguea la respuesta */
    LogAlarm.Put(0, "CierreRta: RESPUESTA ISO: %s\n", mensaje_iso->GetField(39)); 
    LogAlarm.Put(0, "CierreRta: Terminal: [%s] Nro Com: [%s]\n", term_data.nro_caj_ca, term_data.nro_com);

	/* Obtiene datos de la terminal */
    iRet=getDataByCAET(&term_data);
    if (iRet!=OK)
    {
		LogAlarm.Put( 0, "CierreRta: ERROR (%d) al obtener datos de la terminal\n", iRet);
		return NOOK;
    }

	/* Extrae los datos del heap */
    LogAlarm.Put(95, "CierreRta: Extraccion de los datos del Heap\n");
    sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca,  term_data.nro_suc,
                                           term_data.nro_caj, mensaje_iso->GetField(11));
    iRet=Heap.GetData(idHeap,(char *)&heap_data, sizeof(heap_data));
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "CierreRta: ERROR (%d) en HeapInfo. Id:[%s]\n", Heap.GetErrno(), idHeap);
		return NOOK;
	}

    /* Verifica y extrae los datos de la TimeoutQueue */
    LogAlarm.Put( 5, "CierreRta: Saca datos de la TO Queue.\n");
    strcpy(aux_str, mensaje_iso->GetField(11));
    iRet=timeout_queue.RemTimeOut(idHeap);
    if (iRet==NOOK)
    {
		LogAlarm.Put( 0, "CierreRta: ERROR (%d) sacando de TimeOut\n", timeout_queue.GetErrno());
		return NOOK;
    }

    /* Aplana el mensaje */
    msg_len=mensaje_iso->GetMsgString(msg_str);

    /* Envia mensaje por cola administrativa */
	LogAlarm.Put(0, "CierreRta: Envia mensaje  msg a [%d] CAut [%s]\n", heap_data.orig_pid, term_data.nro_ca);
	EnviarMsg(heap_data.orig_pid, msg_str, msg_len);
	LogAlarm.Put(5, "CierreRta: Evento finalizado\n");

    return OK;
}
