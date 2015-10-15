#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | RtaTrxOff.C                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Respuesta al mensaje de transacciones offline  de venta    |
|                 | enviado por el centro autorizador                          | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"
#include "Log.h"
#include "Table.h"
#include "RtaTrxOff.h"
#include "TrxOff.h"

extern "C"
{
	#include "DBTrxOff.h"
}

extern Log				LogAlarm;       /* Objeto para Logging de alarmas  */
extern Config			Cfg;            /* Objeto de configuracion.        */
extern TimeOutQueue		timeout_queue;	/* Cola de vinculo con X.25        */
extern TrxHeap			Heap;           /* Heap para Reg. info intraTrx    */
extern Pos				P;              /* Objeto Pos                      */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaTrxOffLine::RtaTrxOffLine                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RtaTrxOffLine::RtaTrxOffLine(int msgPid, char *msgData, int dataLen)
{
    event_type=EVENT_RTATRXOFFLINE;
    IsoMsg=new Iso_Msg(msgData, dataLen);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaTrxOffLine:: ~RtaTrxOffLine                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RtaTrxOffLine::~RtaTrxOffLine()
{
    delete IsoMsg;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RtaTrxOffLine::EnviarMsg                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RtaTrxOffLine::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "RtaTrxOff: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrxRta::ProcessIt                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RtaTrxOffLine::ProcessIt()
{
	int              len;
	int              ret;
	int				 iCont;
	trxOffHeap       infoHeap;          // Informacion del heap.
	DBTrxPk          trxPrimaryKey;     // Informacion del heap.
	DBTrxUpdInfo     info2Upd;          // Informacion a actualizar.
	str_sel_cadata   term_data;         // Datos de la Terminal.
	int              upd_rta=1;
	char			 idHeap[20];

	/* Loguea la respuesta del centro */
	LogAlarm.Put(0, "RtaTrxOff: RESPUESTA ISO: %s\n", IsoMsg->GetField(39));

	/* Loguea el mensaje */
	for (iCont=0; iCont<64; iCont++)
	{
		if (IsoMsg->ChkField(iCont))
		{
			LogAlarm.Put(5, "RtaTrxOff: Campo %02d: Valor:[%s]\n", iCont, IsoMsg->GetField(iCont) );
		}
	}

	/* Obtiene numero de caja y comercio del centro autorizador */
	strncpy(term_data.nro_caj_ca, IsoMsg->GetField(41), sizeof(term_data.nro_caj_ca));
	strncpy(term_data.nro_com   , IsoMsg->GetField(42), sizeof(term_data.nro_com));
	LogAlarm.Put(0, "RtaTrxOff: Datos de la transaccion: Caj:[%s] Com:[%s]\n", term_data.nro_caj_ca, term_data.nro_com);

	/* Obtiene datos de la terminal */
	ret=getDataByCA(&term_data);
	if (ret!=OK)
	{
		LogAlarm.Put(0, "RtaTrxOff: ERROR (%d) al obtener datos de la terminal\n", ret );
		return NOOK;
	}

	/* Setea el centro autorizador */
	IsoMsg->SetNroCA8583(term_data.nro_ca);

	/* Arma clave para el heap */
	sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca, term_data.nro_suc,	term_data.nro_caj, IsoMsg->GetField(11));

	/* Obtiene datos del heap */
	ret=Heap.GetData(idHeap,(char *)&infoHeap, sizeof(infoHeap));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "RtaTrxOff: ERROR %d al extraer de heap. Clave:[%s]\n", Heap.GetErrno(), idHeap);
		return NOOK;
	}
	LogAlarm.Put(0, "RtaTrxOff: Emp:[%s] Ter:[%s] Trx:[%s] Lot:[%s] Fec:[%s] Rowid:[%s]\n",
							term_data.nro_suc, infoHeap.nroTerm , infoHeap.nroTrans, term_data.nro_lot, infoHeap.fechaTrx,
							infoHeap.Rowid);

	/* Arma datos de actualizacion */
	strncpy(info2Upd.fec_rta, currentTimestamp(),   sizeof(info2Upd.fec_rta));
	strncpy(info2Upd.fec_env, currentTimestamp(),   sizeof(info2Upd.fec_env));
	strncpy(info2Upd.nro_ref, IsoMsg->GetField(37), sizeof(info2Upd.nro_ref));
	strncpy(info2Upd.cod_aut, IsoMsg->GetField(38), sizeof(info2Upd.cod_aut));
	strncpy(info2Upd.cod_rta, IsoMsg->GetField(39), sizeof(info2Upd.cod_rta));
	strncpy(info2Upd.nro_trc, IsoMsg->GetField(11), sizeof(info2Upd.nro_trc));
	strncpy(info2Upd.nro_lot, term_data.nro_lot,    sizeof(info2Upd.nro_lot));

	/* Recupera el codigo de estado */
	if (IsoMsg->Aprob()==OK)
	{
		sprintf(info2Upd.cod_est, "%02d", E_APRO);
		upd_rta=1;
	}
	else 
	{
		if ((!strcmp(IsoMsg->GetField(39), "94"))||
			(!strcmp(IsoMsg->GetField(39), "96")))
		{ 
			sprintf(info2Upd.cod_est, "%02d", E_PEND);
			upd_rta=0;
		}
		else 
		{
			sprintf(info2Upd.cod_est, "%02d", E_RECH);
			upd_rta=1;
		}
	}
	LogAlarm.Put(0, "RtaTrxOff: Estado de actualizacion:[%s]. Upd_rta:[%d]\n", info2Upd.cod_est, upd_rta);

	/* Completa estructura con la clave */
	strcpy(trxPrimaryKey.nro_ca,term_data.nro_ca);
	strcpy(trxPrimaryKey.nro_caj_ca,term_data.nro_caj_ca);
	strncpy(trxPrimaryKey.nroSuc  ,infoHeap.nroSuc , sizeof(trxPrimaryKey.nroSuc ));
	strncpy(trxPrimaryKey.nroTerm ,infoHeap.nroTerm, sizeof( trxPrimaryKey.nroTerm ));
	strncpy(trxPrimaryKey.nroTrans,infoHeap.nroTrans, sizeof( trxPrimaryKey.nroTrans));
	strncpy(trxPrimaryKey.fechaTrx,infoHeap.fechaTrx, sizeof( trxPrimaryKey.fechaTrx));
	sprintf(trxPrimaryKey.modIngre, "%0*d", sizeof(trxPrimaryKey.modIngre),	T_OFLI);
	LogAlarm.Put(0, "RtaTrxOff: Ter:[%s] Trx:[%s] Fec:[%s] CA:[%s] CajaCA:[%s]\n",	
							trxPrimaryKey.nroTerm, trxPrimaryKey.nroTrans, 
							trxPrimaryKey.fechaTrx, trxPrimaryKey.nro_ca, trxPrimaryKey.nro_caj_ca);	

	/* Si se actualiza transaccion */
	if (upd_rta)
	{
		LogAlarm.Put(0, "RtaTrxOff: Voy a Upd en BD con Rta: [%s].\n", info2Upd.cod_rta);
		ret=DBUpdOffTrx(trxPrimaryKey, info2Upd);
		if (ret!=OK)
		{
			/* Vuelve a poner los datos en el heap para el evento de time out */
			LogAlarm.Put(0, "RtaTrxOff: ERROR (%d) al actualizar BD\n",	ret);
			ret=Heap.PutData(idHeap,(char *)&infoHeap, sizeof(infoHeap));

			return NOOK;
		}
	}
	else /* No se actualiza transaccion */
	{ 
		LogAlarm.Put(0, "RtaTrxOff: Voy Upd con Rta: [%s] para Rein\n",	info2Upd.cod_rta);
		ret=DBUpdOffRein(trxPrimaryKey, info2Upd);
		if (ret!=OK)
		{
			/* Vuelve a poner los datos en el heap para el evento de time out */
			LogAlarm.Put(0, "RtaTrxOff: ERROR (%d) al actualizar BD\n",	ret);
			ret=Heap.PutData(idHeap,(char *)&infoHeap, sizeof(infoHeap));

			return NOOK;
		}
	} 

	/* Saca datos de la TOQ	*/
	LogAlarm.Put(0, "RtaTrxOff: Extrae datos de la TOQ\n");
	ret=timeout_queue.RemTimeOut(idHeap);
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "RtaTrxOff: ERROR %d al sacar datos de TOQ. Clave:[%s]\n", timeout_queue.GetErrno(), idHeap);
	}

	/* Libera la caja */
	ret = P.SetPosFree(term_data.nro_ca, term_data.nro_suc, term_data.nro_caj);
	if (ret != OK)
	{
		LogAlarm.Put(0, "RtaTrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
									term_data.nro_ca, term_data.nro_suc, term_data.nro_caj);
	}

	/* Envia mensaje por la cola administrativa */
	LogAlarm.Put(0, "RtaTrxOff: Envia mensaje por cola administrativa, CAut [%s]\n", term_data.nro_ca);
	EnviarMsg(infoHeap.origPid, "ACK", 3);
	LogAlarm.Put(0, "RtaTrxOff: Evento finalizado\n");
	
	return OK;
}
