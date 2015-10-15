#ifdef OS_Linux
#pragma pack(1)
#endif

#define E_REIN	57
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TmtTrxOffLine.C                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Timeout al mensaje de envio de venta off-line enviado por  |
|                 | el TimeOutDaemon                                           | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"
#include "Log.h"
#include "Table.h"
#include "TmtTrxOff.h"
#include "TrxOff.h"
extern "C"
{
	#include "DBTrxOff.h"
}

extern Config      Cfg;               /* Objeto de configuracion.        */
extern CACStatus   CACS;              /* Vinculo con centro autorizador  */
extern Log         LogAlarm;          /* Objeto para Logging de alarmas  */
extern TrxTable    DBTrx;             /* Estructura de datos ara Trxs    */
extern Queue       SQueue;            /* Cola de Envia a Trx             */
extern TrxHeap     Heap;              /* Heap para Reg. info intraTrx    */ 
extern Queue       XQueue;			  /* Cola de X.25					 */
extern Pos		   P;				  /* Objeto Pos                      */


/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TmtTrxOffLine::TmtTrxOffLine                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TmtTrxOffLine::TmtTrxOffLine(int msgPid, char *msgData, int dataLen)
{
    event_type=EVENT_TMTTRXOFFLINE;
    
    if (dataLen<=sizeof(tmtInfo))
	{
		memcpy((void *)&tmtInfo, msgData, dataLen);
	}
    tmtPid = msgPid;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TmtTrxOffLine::~TmtTrxOffLine                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TmtTrxOffLine::~TmtTrxOffLine()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TmtTrxOffLine::EnviarMsg                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TmtTrxOffLine::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "TmtTrxOff: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TORevAnul::ProcessIt                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TmtTrxOffLine::ProcessIt()
{
    int         len;
    int         ret;
    trxOffHeap	infoHeap;          
    DBTrxPk     trxPrimaryKey;     
    DBInfoTmt   info2Upd;          
    TrxData     trx_data;          
    char        aux_str1[100]; 
    int         tope_rein;         
    char        CanRet[5];
    char		auxCA[3];

    LogAlarm.Put(0, "TmtTrxOff: TimeOut en mensaje (%s)...\n", tmtInfo.key);

    /* Configura el numero de centro autorizador */
	sprintf(auxCA, "%2.2s", tmtInfo.key);
    trx_data.NroCA(auxCA);

	/* Obtiene los datos del heap */
    ret=Heap.GetData(tmtInfo.key, (char *)&infoHeap, sizeof(infoHeap));
    if (ret==NOOK)
    {
        LogAlarm.Put(0, "TmtTrxOff: ERROR (%d) al recuperar del heap. Clave: [%s]\n", Heap.GetErrno(), tmtInfo.key );
		EnviarMsg(tmtInfo.orig_pid, "TOUT", 4);
        return NOOK;
    }

//    LogAlarm.Put(0, "TmtTrxOff: Emp:[%s] Ter:[%s] Trx:[%s] Fec:[%s]\n", 
//						infoHeap.nroSuc, infoHeap.nroTerm, infoHeap.nroTrans, infoHeap.fechaTrx);

    /* Configura datos de la transaccion */
	sprintf(aux_str1, "%8.8s", tmtInfo.key+10);
    trx_data.NroTrc(aux_str1);
    trx_data.NroCaj(infoHeap.nroTerm);
    trx_data.NroSuc(infoHeap.nroSuc);
    trx_data.CodTar(infoHeap.codTar); 
    trx_data.NroCajCa(infoHeap.NroCajCa);
    LogAlarm.Put(0, "TmtTrxOff: Ca:[%s] Trc:[%s] Caj:[%s]\n", trx_data.NroCA(), trx_data.NroTrc(), trx_data.NroCajCa()); 

    /* Obtiene los datos de la transacciones de la BD */ 
    ret=get_trx_data(trx_data.GetInsVenData());
    if (ret != OK)
    {
		LogAlarm.Put(0, "TmtTrxOff: ERROR (%d) al obtener datos de la transaccion\n", ret); 
		EnviarMsg(infoHeap.origPid, "TOUT", 4);
		return NOOK;
    }
	trx_data.PutTrxData();

	/* Configura el codigo de estado */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		LogAlarm.Put(0, "TmtTrxOff: Protocolo Amex no genera reverso offline\n"); 
		/* Libera la caja */
		ret = P.SetPosFree(trx_data.NroCA(), trx_data.NroSuc(), trx_data.NroCaj() );
		if (ret != OK)
		{
			LogAlarm.Put(0, "TmtTrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
									trx_data.NroCA(), trx_data.NroSuc(), trx_data.NroCaj() );
		}
		break;
	default:
		sprintf(aux_str1, "%02d", E_TOUT);
		trx_data.CodEst(aux_str1);

		/* Actualiza transaccion a reverso o inserta uno (CRED_TRX) */
		ret=DBTrx.UpdateRevOff(trx_data.GetVenOffData()); 
		if (ret == NOOK)
		{
			LogAlarm.Put(0, "TmtTrxOff: ERROR (%d) al actualizar reverso en BD\n", DBTrx.GetErrno());
			EnviarMsg(infoHeap.origPid, "TOUT", 4);
			return NOOK;
		} 
		break;
	}
 
    /* Obtiene cantidad maxima de reintentos */
//	ret=Cfg.GetItem("CAVisaDaemon", "CantRein", aux_str1);
//    if (ret!=0)
//    {
//       strcpy(aux_str1, "5");
//    }
//    tope_rein=atoi(aux_str1);
//    strncpy(CanRet,trx_data.CanRet(),4);

    /* Pone el codigo de estado considerando si supera los reintentos */
//	if (atoi(CanRet) >= tope_rein)
//    { 
//       sprintf(info2Upd.cod_est, "%02d", E_REIN/*57*/);
//       LogAlarm.Put(0, "TmtTrxOff: Supero la cantidad de reintentos (%d)\n", tope_rein); 
//    }
//    else 
//    {
       sprintf(info2Upd.cod_est, "%02d", E_PEND); /* 00 */
//    }

	/* Carga campos clave de la transaccion */
    strcpy (trxPrimaryKey.rowid   , infoHeap.Rowid);
    strncpy(trxPrimaryKey.nroSuc  , infoHeap.nroSuc  , sizeof(trxPrimaryKey.nroSuc ));
    strncpy(trxPrimaryKey.nroTerm , infoHeap.nroTerm , sizeof( trxPrimaryKey.nroTerm ));
    strncpy(trxPrimaryKey.nroTrans, infoHeap.nroTrans, sizeof( trxPrimaryKey.nroTrans));
    strncpy(trxPrimaryKey.fechaTrx, infoHeap.fechaTrx, sizeof( trxPrimaryKey.fechaTrx));
    sprintf(info2Upd.nro_trc, "%04d", "-1");
    
	/* Loguea datos de la terminal */
	LogAlarm.Put( 0, "TmtTrxOff: Emp:[%s] Ter:[%s] Trx:[%04s] Fec:[%s] Rowid:[%s]\n", 
						infoHeap.nroSuc, infoHeap.nroTerm, infoHeap.nroTrans, infoHeap.fechaTrx, infoHeap.Rowid);

    /* Actualiza transaccion (CRED_TRX) */
	ret = DBUpdTmtOff(trxPrimaryKey,info2Upd);
	if (ret!=OK)
	{
		LogAlarm.Put(0, "TmtTrxOff: ERROR (%d) al actualizar base de datos\n",	ret);
		EnviarMsg(infoHeap.origPid, "TOUT", 4);
		return NOOK;
	}

	/* Baja el estado del centro si no hay canales disponibles */
	ret = XQueue.StatusCA(atoi(auxCA));
	if (ret<=0)
	{
		LogAlarm.Put(0, "TmtTrxOff: Ret:[%d]. Baja estado CAut:[%s]\n", ret, auxCA);
		ret=CACS.Down(atoi(auxCA));
	    if (ret==NOOK)
		{
			LogAlarm.Put(0, "TmtTrxOff: ERROR:[%d] al cambiar estado CAut:[%s]\n", CACS.GetErrno(), auxCA);
		}
	}
	else
	{
		LogAlarm.Put(0, "TmtTrxOff: Canales disponibles:[%d]. No baja estado CAut:[%s]\n", ret, auxCA);
	}

	/* Envia respuesta por cola administrativa */
    LogAlarm.Put(0, "TmtTrxOff: Envia TOUT al SendRev\n");
	EnviarMsg( infoHeap.origPid, "TOUT", 4 );

    LogAlarm.Put(0, "TmtTrxOff: Evento finalizado\n" );

	return OK;
}



