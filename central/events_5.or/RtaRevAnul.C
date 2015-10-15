#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | RtaRevAnul.C                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Respuesta al mensaje de reverso de anulación enviada por   |
|                 | el centro autorizador                                      | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"
#include "Log.h"
#include "Table.h"
extern "C"
{
	#include "DBTrxOff.h"
}

extern Log				LogAlarm;       /* Objeto para Logging de alarmas. */
extern Config			Cfg;            /* Objeto de configuracion.        */
extern TrxTable			DBTrx;          /* Estructura de datos para Trxs   */
extern TimeOutQueue		timeout_queue;	/* Cola de vinculo con X.25.       */
extern TrxHeap			Heap;           /* Heap para Reg. info intraTrx.   */
extern Pos				P;              /* Objeto Pos.                     */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaRevAnul::RtaRevAnul                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RtaRevAnul::RtaRevAnul(int msgPid, char *msgData, int dataLen)
{
	event_type=EVENT_REVTRXANUL_RTA;
	IsoMsg=new Iso_Msg(msgData, dataLen);   

   memset(szMsjeISO, 0, sizeof(szMsjeISO));
   memcpy(szMsjeISO, msgData, dataLen);
   iLMsjeISO = dataLen;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaRevAnul:: ~RtaRevAnul                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RtaRevAnul::~RtaRevAnul()
{
	delete IsoMsg;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaRevAnul::EnviarMsg                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RtaRevAnul::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "RtaRevAnul: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaRevAnul::ProcessIt                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RtaRevAnul::ProcessIt()
{
	int              len;
	int              ret;
	str_sel_cadata   term_data;
	trxAnulHeap	     heap_data;
	char             msgrta[300];	
	char			 idHeap[20];
	char			 aux_str1[100];
	int				 iCont;
	Queue 			 EQueue;	
	trxRevHeap		 heap_reve;

	/* Obtiene los datos de la caja en funcion de la caja del centro autorizador */
	strcpy(term_data.nro_caj_ca, IsoMsg->GetField(41)); 
	ret=ObtCaCaj(&term_data);
	if (ret!=OK)
	{
		/* Loguea el mensaje en caso de error */
		LogAlarm.Put(0, "RtaRevAnul: ERROR (%d) al obtener datos de la caja [%s]\n", ret, term_data.nro_caj_ca);
		return NOOK;
	}

	/* Obtiene los datos del heap */
	sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca, term_data.nro_suc, term_data.nro_caj, IsoMsg->GetField(11));
	LogAlarm.Put(0, "RtaRevAnul: Extrayendo datos del heap [%s]\n", idHeap);
	ret=Heap.GetData(idHeap,(char *)&heap_reve, sizeof(heap_reve));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "RtaRevAnul: Transaccion no existe en Heap [%s]\n", idHeap);
		return NOOK;
	}
	else
	{
		LogAlarm.Put(0, "RtaRevAnul: Reenvio a RevTrxRta\n");
		Cfg.GetItem("VisaDaemon", "QueueID", aux_str1);
		if ( EQueue.Open(atoi(aux_str1)) == OK )
		{
			Heap.PutData(idHeap,(char *)&heap_reve, sizeof(heap_reve));
			EQueue.SendMsg(EVENT_SNDREVERSO_RTA, szMsjeISO, iLMsjeISO);
		}		
	}
	LogAlarm.Put(0, "RtaRevAnul: Evento finalizado\n");	
	return OK;
}
