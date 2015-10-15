#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TOCierreB.C                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento timeout a mensaje de totales de cierre de lote      |
|                 | batch                                                      | 
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
| FUNCION         | TOCierreB::TOCierreB                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOCierreB::TOCierreB(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_TIMEOUT_CIERRE_B;

    memcpy(&data, msg_data, sizeof(str_timeout_event));
    mensaje_iso=new Iso_Msg(TYPE_VISA_CIERRE_B_RTA);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOCierreB::~TOCierreB                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOCierreB::~TOCierreB()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOCierreB::EnviarMsg                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOCierreB::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "TOCierreB: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOCierreB::ProcessIt                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOCierreB::ProcessIt()
{
    char			msg_str[2000];
    int				msg_len;
    int				iRet;
    str_heap_data	heap_data;
    char			auxCa[3];
    
    /* Cambia el estado del canal X.25 */
	LogAlarm.Put(0, "TOCierreB: TimeOut. Cambiando el estado del canal. Mensaje (%d)\n", data.key);
    
	/* Baja el estado del centro si no hay canales disponibles */
	sprintf(auxCa,"%2.2s",data.key);
	iRet = XQueue.StatusCA(atoi(auxCa));
	if (iRet<=0)
	{
		LogAlarm.Put(0, "TOCierreB: Ret:[%d]. Baja estado CAut:[%s]\n", iRet, auxCa);
		iRet=CACS.Down(atoi(auxCa));
	    if (iRet==NOOK)
		{
			LogAlarm.Put(0, "TOCierreB: ERROR:[%d] al cambiar estado CAut:[%s]\n", CACS.GetErrno(), auxCa);
			return NOOK;
		}
	}
	else
	{
		LogAlarm.Put(0, "TOCierreB: Canales disponibles:[%d]. No baja estado CAut:[%s]\n", iRet, auxCa);
	}

    /* Verifica si el proceso de Ping esta corriendo */
    if (CACS.IsNotPing(atoi(auxCa)))
    {
        LogAlarm.Put(5, "TOCierreB: Lanzando el proceso de ping\n");
        CACS.IsPing(atoi(auxCa));
    }

    /* Extrae los datos del heap */
    LogAlarm.Put(5, "TOCierreB: Extrae los datos del Heap.\n");
    iRet=Heap.GetData(data.key,(char *)&heap_data, sizeof(heap_data));
    if (iRet==NOOK)
	{
		LogAlarm.Put(0, "TOCierreB: ERROR (%d) en HeapInfo!!\n", Heap.GetErrno());
		return NOOK;
	}

    /* Pone el codigo de respuesta FF indicando que fue Timeout */
    mensaje_iso->PutField(38, "FF");

	/* Aplana el mensaje */
    msg_len=mensaje_iso->GetMsgString(msg_str);

    /* Envia el mensaje por cola administrativa */
	LogAlarm.Put(5, "TOCierreB: Enviando mensaje a (%d).\n", heap_data.orig_pid);
	EnviarMsg(heap_data.orig_pid, msg_str, msg_len);
	LogAlarm.Put(0, "TOCierreB: Evento finalizado\n");

    return OK;
}


