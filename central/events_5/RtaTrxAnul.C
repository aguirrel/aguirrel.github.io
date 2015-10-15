#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | RtaTrxAnul.C                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Respuesta al mensaje de anulación enviado por el centro    |
|                 | autorizador                                                | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"
#include "Log.h"
#include "Table.h"
extern "C"
{
    #include "DBTrxOff.h"
}

extern Log         LogAlarm;          /* Objeto para Logging de alarmas. */
extern Config      Cfg;               /* Objeto de configuracion.        */
extern Queue       SQueue;            /* Cola de Envia a Trx.            */
extern TimeOutQueue timeout_queue;    /* Cola de vinculo con X.25.       */
extern TrxHeap     Heap;              /* Heap para Reg. info intraTrx.   */
extern Pos         P;                 /* Objeto Pos.                     */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaTrxAnul::RtaTrxAnul                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RtaTrxAnul::RtaTrxAnul(int msgPid, char *msgData, int dataLen)
{
    event_type=EVENT_SNDTRXANUL_RTA;

    IsoMsg=new Iso_Msg(msgData, dataLen);   

   memset(szMsjeISO, 0, sizeof(szMsjeISO));
   memcpy(szMsjeISO, msgData, dataLen);
   iLMsjeISO = dataLen;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaTrxAnul:: ~RtaTrxAnul                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RtaTrxAnul::~RtaTrxAnul()
{
    delete IsoMsg;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaTrxAnul::EnviarMsg                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RtaTrxAnul::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "RtaTrxAnul: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaTrxAnul::ProcessIt                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RtaTrxAnul::ProcessIt()
{
    int             len;
    int             ret;
	int				iCont, iPos;
    str_sel_cadata  term_data;
    trxAnulHeap	    heap_data;
    char            msgrta[300];	
    char			idHeap[20];
	char			aux_str1[100];
	char			szPinpad[64];
	Queue 			EQueue;	
	str_heap_data	heap_onli;
	    
	/* Obtiene los datos de la caja en funcion de la caja del centro autorizador */
	strcpy(term_data.nro_caj_ca, IsoMsg->GetField(41)); 
    ret=ObtCaCaj(&term_data);
	if (ret!=OK)
	{
		LogAlarm.Put(0, "RtaTrxAnul: ERROR en consistencia. Caja:[%s] RESPUESTA ISO: %s\n", 
			term_data.nro_caj_ca, IsoMsg->GetField(39));
		return NOOK;
	}

	/* Obtiene los datos del heap (online enviada por evento PriceOn) */
	sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", 
				term_data.nro_ca, term_data.nro_suc, term_data.nro_caj, IsoMsg->GetField(11));
	ret=Heap.GetData(idHeap,(char *)&heap_onli, sizeof(heap_onli));  	
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "RtaTrxAnul: Transaccion [%s] No Existe en Heap\n", idHeap);
		return NOOK;
	}
	else
	{
		LogAlarm.Put(0, "RtaTrxAnul: Reenvio a PriceOnRta\n");
		Cfg.GetItem("VisaDaemon", "QueueID", aux_str1);
		if ( EQueue.Open(atoi(aux_str1)) == OK )
		{
			ret=Heap.PutData(idHeap,(char *)&heap_onli, sizeof(heap_onli));  	
			EQueue.SendMsg(EVENT_PRICEONLINE_RTA, szMsjeISO, iLMsjeISO);
		}		
	}
	LogAlarm.Put(2, "RtaTrxAnul: Evento finalizado.\n");	
    return OK;
}
