#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TOCierre.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento timeout a mensaje de cierre de lote                 |
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
| FUNCION         | TOCierre::TOCierre                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOCierre::TOCierre(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_TIMEOUT_CIERRE;

    memcpy(&data, msg_data, sizeof(str_timeout_event));
    mensaje_iso=new Iso_Msg(TYPE_VISA_CIERRE_RTA);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOCierre::~TOCierre                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOCierre::~TOCierre()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOCierre::EnviarMsg                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOCierre::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "TOCierre: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOCierre::ProcessIt                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOCierre::ProcessIt()
{
    char			msg_str[2000];
    int				msg_len;
    int				iRet;
    str_heap_data	heap_data;
    char			auxCa[3];

    /* Baja el estado del centro si no hay canales disponibles */
	sprintf(auxCa,"%2.2s",data.key);
	iRet = XQueue.StatusCA(atoi(auxCa));
	if (iRet<=0)
	{
		LogAlarm.Put(0, "TOCierre: Ret:[%d]. Baja estado CAut:[%s]\n", iRet, auxCa);
		iRet=CACS.Down(atoi(auxCa));
	    if (iRet==NOOK)
		{
			LogAlarm.Put(0, "TOCierre: ERROR:[%d] al cambiar estado CAut:[%s]\n", CACS.GetErrno(), auxCa);
			return NOOK;
		}
	}
	else
	{
		LogAlarm.Put(0, "TOCierre: Canales disponibles:[%d]. No baja estado CAut:[%s]\n", iRet, auxCa);
	}
    
	/* Verifica si el proceso de Ping esta corriendo */
    if (CACS.IsNotPing(atoi(auxCa)))
    {
        LogAlarm.Put(5, "TOCierre: Lanzando el proceso de ping\n");
        CACS.IsPing(atoi(auxCa));
    }

    /* Extrae los datos del heap */
	LogAlarm.Put(5, "TOCierre: Extrae los datos del Heap.\n");
    iRet=Heap.GetData(data.key,(char *)&heap_data, sizeof(heap_data));
    if (iRet==NOOK)
	{
		LogAlarm.Put(0, "TOCierre: ERROR (%d) en HeapInfo\n", Heap.GetErrno());
		return NOOK;
	}

    /* Pone el codigo de respuesta FF indicando que fue Timeout */
    mensaje_iso->PutField(38, "FF");

    /* Pone el campo trace al mensajes iso */
    mensaje_iso->PutField(11, &data.key[12]);

    /* Aplana el mensaje */
    msg_len=mensaje_iso->GetMsgString(msg_str);
    
	/* Envia el mensaje por la cola administrativa */
	LogAlarm.Put(0, "TOCierre: Envia mensaje a (%d)\n", heap_data.orig_pid);
	EnviarMsg(heap_data.orig_pid, msg_str, msg_len);
	LogAlarm.Put(0, "TOCierre: Evento finalizado\n");

    return OK;
}
