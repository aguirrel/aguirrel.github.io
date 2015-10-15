#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TORevTrx.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Timeout al mensaje de reverso de venta enviado por el      |
|                 | TimeOutDaemon                                              | 
+-----------------+------------------------------------------------------------+
*/
#include "CACStat.h"
#include "Events.h"
#include "Log.h"
#include "Table.h"
extern "C"
{
   #include "DBTrxOff.h"
}

extern CACStatus	CACS;			/* Status Canal X.25            */
extern Config       Cfg;			/* Objeto de configuracion      */
extern Log          LogAlarm;		/* Logging de actividades       */
extern TrxHeap		Heap;			/* Objeto Heap                  */ 
extern TrxTable		DBTrx;			/* Para tabla de transacciones  */ 
extern Pos			P;				/* Objeto Pos					*/
extern Queue        XQueue;			/* Cola de X.25                 */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TORevAnul::TORevAnul                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TORevTrx::TORevTrx(int msg_pid, char *msg_data, int data_len)
{
	event_type=EVENT_TIMEOUT_REVERSO;

	memcpy(&data, msg_data, sizeof(str_timeout_event));
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ToRevTrx:: ~ToRevTrx                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TORevTrx::~TORevTrx()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TORevTrx::EnviarMsg                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TORevTrx::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "TORevTrx: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TORevTrx::ProcessIt                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TORevTrx::ProcessIt()
{
	int					msg_len;
	int					ret;
	int					cont;
	trxRevHeap			heap_data;
	Queue				AQueue;
	TrxData				trx_data;
	char				aux_str1[100];	
	char				auxCa[3];

	LogAlarm.Put( 5, "TORevTrx: TO esperando respuesta al mensaje [%s]\n", data.key);

	/* Baja el estado del centro si no hay canales disponibles */
	sprintf(auxCa,"%2.2s",data.key);
	ret = XQueue.StatusCA(atoi(auxCa));
	if (ret<=0)
	{
		LogAlarm.Put(0, "TORevTrx: Ret:[%d]. Baja estado CAut:[%s]\n", ret, auxCa);
		ret=CACS.Down(atoi(auxCa));
	    if (ret==NOOK)
		{
			LogAlarm.Put(0, "TORevTrx: ERROR:[%d] al cambiar estado CAut:[%s]\n", CACS.GetErrno(), auxCa);
		}
	}
	else
	{
		LogAlarm.Put(0, "TORevTrx: Canales disponibles:[%d]. No baja estado CAut:[%s]\n", ret, auxCa);
	}

	/* Obtiene los datos del heap */
	LogAlarm.Put(5, "TORevTrx: Extraccion de los datos del Heap.\n");
	ret=Heap.GetData(data.key,(char *)&heap_data, sizeof(heap_data));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "TORevTrx: ERROR (%d) al recuperar de Heap. Clave:[%s]\n", Heap.GetErrno(), data.key);
		EnviarMsg(data.orig_pid, "TOUT", 4);
		return NOOK;
	}

	/* Completa datos de la transaccion */
	sprintf(aux_str1, "%8.8s", data.key + 10);
	trx_data.NroTrc(aux_str1);
	sprintf(aux_str1, "%02d", E_ENVI);
	trx_data.CodEst(aux_str1);
	trx_data.CodTar(heap_data.cod_tar);
	trx_data.NroCaj(heap_data.nroTerm);
	trx_data.NroCajCa(heap_data.NroCajCa);
	sprintf(aux_str1, "%02d", E_TOUT);
	trx_data.CodEst(aux_str1);
	trx_data.Rowid(heap_data.Rowid);
	trx_data.NroSuc(heap_data.nroSuc);

	/* Actualiza base de datos */
	LogAlarm.Put(0, "TORevTrx: Actualiza BD. Rowid:[%s]\n", heap_data.Rowid);
	/*ret=DBTrx.UpdateRtaCanRet(trx_data.GetUpdDataCanRet());*/
	ret = update_rta_can_ret(trx_data.GetUpdDataCanRet());
	switch (ret)
	{
	case NOOK:
		LogAlarm.Put(0, "TORevTrx: ERROR en Update (%d). Trc:[%s] Tar:[%s] Caj:[%s] Est:[%s]\n", DBTrx.GetErrno(), 
								trx_data.NroTrc(), trx_data.CodTar(), trx_data.NroCaj(), trx_data.CodEst());    
		break;
	case SUPE_REINT:
		LogAlarm.Put(0, "TORevTrx: Superado los reintentos. CA:[%s] Suc:[%s] Caj:[%s]\n", 
						auxCa, trx_data.NroSuc(), trx_data.NroCaj());
		P.SetPosFree(auxCa, trx_data.NroSuc(), trx_data.NroCaj());
		break;
	case OK:
		break;
	default:
		LogAlarm.Put(0, "TORevTrx: Cantidad de reintentos [%d] CA:[%s] Suc:[%s] Caj:[%s]\n", 
						ret, auxCa, trx_data.NroSuc(), trx_data.NroCaj());
	}

	/* Enviar respuesta de timeout por cola administrativa */
	LogAlarm.Put(0, "TORevTrx: Envia mensaje por cola administrativa\n");
	EnviarMsg(heap_data.orig_pid, "TOUT", 4);
	LogAlarm.Put(0, "TORevTrx: Evento finalizado\n");

	return OK;
}
