#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | RevTrxRta.C                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Respuesta al mensaje de reverso de venta enviado por el    |
|                 | centro autorizador                                         | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"
#include "Log.h"
#include "Table.h"

extern Config			Cfg;               /* Objeto de configuracion.        */
extern Log              LogAlarm;          /* Objeto para Logging de alarmas  */
extern TimeOutQueue		timeout_queue;	   /* Cola de timeout                 */
extern TrxHeap			Heap;              /* Heap para registrar info trx    */
extern Pos				P;                 /* Objeto Pos.                     */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrxRta::RevTrxRta                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RevTrxRta::RevTrxRta(int msgPid, char *msg_data, int data_len)
{
	event_type=EVENT_SNDREVERSO_RTA;
	
	orig_pid=msgPid;

	IsoMsg=new Iso_Msg(msg_data, data_len);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrxRta:: ~RevTrxRta                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RevTrxRta::~RevTrxRta()
{
	delete IsoMsg;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrxRta::EnviarMsg                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RevTrxRta::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "RevTrxRta: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrxRta::ProcessIt                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RevTrxRta::ProcessIt()
{
	int				len;
	int				ret;
	int				iCont;
	trxRevHeap		heap_data;	
	Queue			AQueue;		
	str_sel_ntdata	term_data;	
	str_upd_rta_rev	trx_data;	
	char			aux_str1[100];    
	char			aux_str2[100];    
	char			idHeap[20];

	/* Loguea la respuesta del centro */
	LogAlarm.Put(0, "RevTrxRta: RESPUESTA ISO: %s\n", IsoMsg->GetField(39));

	/* Loguea el mensaje */
	for (iCont=0; iCont<64; iCont++)
	{
		if (IsoMsg->ChkField(iCont))
		{
			LogAlarm.Put(5, "RevTrxRta: Campo %02d: Valor:[%s]\n", iCont, IsoMsg->GetField(iCont) );
		}
	}
	
	/* Obtiene numero de caja y comercio del centro autorizador */
	strncpy(term_data.nro_caj_ca, IsoMsg->GetField(41), sizeof(term_data.nro_caj_ca));
	strncpy(term_data.nro_com, IsoMsg->GetField(42), sizeof(term_data.nro_com));
	LogAlarm.Put(0, "RevTrxRta: Reverso de transaccion. CajaCA:[%s] Comercio:[%s]\n", term_data.nro_caj_ca, term_data.nro_com);

	/* Obtiene datos de la terminal */
	ret=getDataByCA(&term_data);
	if (ret!=OK)
	{
		LogAlarm.Put( 0, "RevTrxRta: ERROR (%d) al obtener datos de la terminal [%s]\n", ret, term_data.nro_caj_ca);
		return NOOK;
	}

	/* Setea el centro autorizador */
	IsoMsg->SetNroCA8583(term_data.nro_ca);

	/* Arma clave para el heap */
	sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca, term_data.nro_suc,	term_data.nro_caj, IsoMsg->GetField(11));

	/* Obtiene datos del heap */
	ret=Heap.GetData(idHeap,(char *)&heap_data, sizeof(heap_data));
	if (ret==NOOK)
	{
		LogAlarm.Put( 1, "RevTrxRta: ERROR %d al extraer de heap. Clave: [%s]\n", Heap.GetErrno(), idHeap);
		return NOOK;
	}
	LogAlarm.Put(0, "RevTrxRta: Emp:[%s] Ter:[%s] Trx:[%s] Fec:[%s] Rowid:[%s]\n", 
						heap_data.nroSuc, heap_data.nroTerm, heap_data.nroTrans, heap_data.fechaTrx, heap_data.Rowid);

	/* Saca datos de la TOQ */
	ret=timeout_queue.RemTimeOut(idHeap);
	if (ret==NOOK)
	{
		LogAlarm.Put( 0, "RevTrxRta: ERROR %d al sacar datos de TOQ. Clave:[%s]\n", timeout_queue.GetErrno(), idHeap);
	}

	/* OBtiene el codigo de estado de la transaccion */
	if (IsoMsg->Aprob()==OK)
	{
		sprintf(aux_str1, "%02d", E_ANUL); /* 07 */
	}
	else
	{
		sprintf(aux_str1, "%02d", E_REAN); /* 08 */
	}
	sprintf(aux_str2, "%02d", E_PEND); /* 00 */
	strcpy(trx_data.cod_est_on , aux_str1            );
	strcpy(trx_data.cod_est_off, aux_str2            );

	/* Arma los datos de la transaccion */
	strcpy(trx_data.cod_tar    , term_data.cod_tar   );
	strcpy(trx_data.nro_suc    , term_data.nro_suc   );
	strcpy(trx_data.nro_caj    , term_data.nro_caj   );
	strcpy(trx_data.nro_trc    , IsoMsg->GetField(11));
	strcpy(trx_data.nro_caj_ca , term_data.nro_caj_ca);
	strcpy(trx_data.nro_ca     , term_data.nro_ca   );

	LogAlarm.Put( 5, "RevTrxRta: On:[%s] Off:[%s] Tar:[%s] Trace:[%s] CajaCa:[%s] CA:[%s]\n",
	trx_data.cod_est_on, trx_data.cod_est_off, trx_data.cod_tar, trx_data.nro_trc,trx_data.nro_caj_ca, trx_data.nro_ca);

	/* Actualiza BD */
	LogAlarm.Put( 15, "RevTrxRta: Actualiza BD\n");
	ret=update_rta_rev(&trx_data);
	if (ret!=OK)
	{
		LogAlarm.Put( 1, "RevTrxRta: ERROR (%d) al actualizar base de datos\n", ret);
		return NOOK;
	}

	/* Libera la caja */
	ret = P.SetPosFree(term_data.nro_ca, term_data.nro_suc, term_data.nro_caj);
	if (ret != OK)
	{
		LogAlarm.Put(5, "RevTrxRta: SetPosFree libre. CA:[%s] Emp:[%s] Caj:[%s]\n", 
						term_data.nro_ca, term_data.nro_suc, term_data.nro_caj);
	}

	/* Envia mensaje por la cola administrativa */
	LogAlarm.Put(0, "RevTrxRta: Envia mensaje por cola administrativa. CAut [%s]\n", term_data.nro_ca);
	EnviarMsg(heap_data.orig_pid, "ACK", 3);
	LogAlarm.Put(5, "RevTrxRta: Evento finalizado\n");

	return OK;
}
