#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | SndTrxOffLine.C                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia los mensajes de transacciones offline pendientes     |
|                 | al centro autorizador                                      | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"
#include "TrxOff.h"
#include "Log.h"
#include "Table.h"
extern "C"
{
      #include "DBTrxOff.h"
}

extern Config       Cfg;			/* Objeto de configuracion     */
extern Log			LogAlarm;		/* Logging de actividades      */
extern TimeOutQueue	timeout_queue;	/* Cola de timeouts            */
extern TraceNmbr	TraceNumber;	/* Nros de trace recibidos     */
extern TrxHeap		Heap;			/* Objeto Heap                 */
extern Queue        XQueue;			/* Cola de X.25                */
extern Pos			P;				/* Objeto Pos                  */
extern Log			LogErrTerminal;	/* Logging errores terminales  */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | SndTrxOffLine::EnviarMsg                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int SndTrxOffLine::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "TrxOff: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | SndTrxOffLine::SndTrxOffLine                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
SndTrxOffLine::SndTrxOffLine(int msg_pid, char *msg_data, int data_len)
{
	char szAux[5];

	/* Carga el evento en el objeto */
    event_type=EVENT_SNDTRXOFFLINE;
  
	/* Carga el pid en el objeto */
	orig_pid=msg_pid;
    
	/* Carga el codigo de tarjeta en el objeto */
	strncpy(CodTarjeta, msg_data+4, sizeof(CodTarjeta)-1);
    CodTarjeta[sizeof(CodTarjeta)-1]='\0';
    
	/* Carga el numero de caja del centro autorizador en el objeto */
	strncpy(NroCajCa, msg_data+6, sizeof(NroCajCa)-1);
    NroCajCa[sizeof(NroCajCa)-1]='\0';

	/* Carga el numero de transaccion en el objeto */
	strncpy(szAux, msg_data+14, sizeof(szAux)-1);
    szAux[sizeof(szAux)-1]='\0';
	sprintf(NroTrx, "%ld", atol(szAux));

}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | SndTrxOffLine:: ~SndTrxOffLine                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
SndTrxOffLine::~SndTrxOffLine()
{
    delete IsoMsg;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | SndTrxOffLine::ProcessIt                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int SndTrxOffLine::ProcessIt()
{
    int             len;
    int             ret;
    int				iCont;
	str_sel_cadata  term_data;
    char            msg_str[2000];
    char            aux_str[20];
    trxOffHeap      infoHeap;         
    DBInfoEnvio     InfoEnvio;        
    str_fet_bu_trx  trxOff;
    int             pos_ocup;
    str_tarjeta     data_tar;
	char			aux_str1[100];	
    DBTrxPk         trxPk;             
    char			idHeap[20];
	char			szFechaEnvio[16];
	int				iErrorBD;
	int				iNroCuotas;

	/* Carga estructura para busqueda de transaccion offline */
    strncpy(trxOff.cod_tar   , CodTarjeta, sizeof(trxOff.cod_tar));
    strncpy(trxOff.cod_est   , "00"      , sizeof(trxOff.cod_est));
    strncpy(trxOff.nro_caj_ca, NroCajCa  , sizeof(trxOff.nro_caj_ca));
    strncpy(trxOff.nro_trx   , NroTrx    , sizeof(trxOff.nro_trx));
	trxOff.nro_caj_ca[sizeof(trxOff.nro_caj_ca)-1]='\0';
    trxOff.cod_tar   [sizeof(trxOff.cod_tar)   -1]='\0';
	trxOff.cod_est   [sizeof(trxOff.cod_est)   -1]='\0';
	trxOff.nro_trx   [sizeof(trxOff.nro_trx)   -1]='\0';

    /* Busca en BD una transaccion offline */
    LogAlarm.Put(0, "TrxOff: Lee offline [%s] [%s] [%s] [%s]\n", trxOff.nro_caj_ca, trxOff.cod_tar, trxOff.cod_est, trxOff.nro_trx);
	ret=DBGetNextTrx(&trxOff, &iErrorBD);
    if (ret!=OK)
    {
		LogAlarm.Put( 0, "TrxOff: Retorno de recuperacion de offline:%d Errno:%d\n", ret, iErrorBD);
		EnviarMsg(orig_pid, "ERROR", 5);
		return OK; 
	}
    //LogAlarm.Put(0, "TrxOff: Rowid a enviar [%s]\n",trxOff.rowid);

	/* Obtiene datos del centro autorizador */
    LogAlarm.Put(5, "TrxOff: Obtiene codigo de centro\n");
	strcpy(data_tar.cod_tar, trxOff.cod_tar);
	ret = ObtCA(&data_tar);
	if (ret!=OK)
	{
		LogAlarm.Put(0, "TrxOff: ERROR al obtener CA. Tar:[%s]\n", trxOff.cod_tar);
		EnviarMsg(orig_pid, "ERROR", 5);
		return OK; 
	}       

	/* Verifica si la caja esta libre */
    strcpy(term_data.nro_caj, trxOff.nro_caj);
    pos_ocup = P.IsPosFree(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
    if (pos_ocup)
    {    
		/* Marca la caja como ocupada */
		ret = P.SetPosBusy(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
		if (ret!=OK)
		{
			LogAlarm.Put(0, "TrxOff: ERROR al setear caja como ocupada\n");
		}
    }
    else
    {
        /* Retorna envio de caja bloqueada */
		LogAlarm.Put(0, "TrxOff: ERROR. Caja bloqueada\n");
		EnviarMsg(orig_pid, "BLOCK", 5);
		return NOOK;
    }

    /* Arma el mensaje base de acuerdo al codigo de transaccion */
	switch (atoi(trxOff.cod_trx))
	{
	case T_VENT:
		IsoMsg=new Iso_Msg(TYPE_VISA_VENTA_LOCAL);
		break;
	case T_DEVO:
		IsoMsg=new Iso_Msg(TYPE_VISA_DEVOL_LOCAL);
		break;
	case T_PAGO:
		IsoMsg=new Iso_Msg(TYPE_VISA_VENTA_LOCAL);
		sprintf(aux_str1, "%06d", OPCODE_VISA_PAGO_RESUMEN);
		IsoMsg->PutField(3, aux_str1);
		break;
	case T_DEVP:
		IsoMsg=new Iso_Msg(TYPE_VISA_DEVOL_LOCAL);
		sprintf(aux_str1, "%06d", OPCODE_VISA_DEVO_RESUMEN);
		IsoMsg->PutField(3, aux_str1);
		break;
	default: /* Tipo de transaccion desconocida */
		/* Libera la caja */
		ret = P.SetPosFree(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
		if (ret != OK)
		{
			LogAlarm.Put(0, "TrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
							data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
		}
		LogAlarm.Put(0, "TrxOff: ERROR. Mensaje desconocido\n");
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	/* Setea centro autorizador */
	IsoMsg->SetNroCA8583(data_tar.nro_ca);

	/* Arma la informacion para el heap */
    infoHeap.origPid=orig_pid;
    strncpy( infoHeap.nroSuc  , trxOff.nro_suc, sizeof(infoHeap.nroSuc  ) );
    strncpy( infoHeap.nroTerm , trxOff.nro_caj, sizeof(infoHeap.nroTerm ) );
    strncpy( infoHeap.nroTrans, trxOff.nro_tic, sizeof(infoHeap.nroTrans) );
    strncpy( infoHeap.fechaTrx, trxOff.fec_loc, sizeof(infoHeap.fechaTrx) );
    strncpy( infoHeap.codTar  , trxOff.cod_tar, sizeof(infoHeap.codTar  ) );
    strncpy( infoHeap.NroCajCa, trxOff.nro_caj_ca,sizeof(infoHeap.NroCajCa));
    strcpy ( infoHeap.Rowid,    trxOff.rowid);
    infoHeap.nroSuc  [sizeof(infoHeap.nroSuc   )-1]='\0';
    infoHeap.nroTerm [sizeof(infoHeap.nroTerm  )-1]='\0';
    infoHeap.nroTrans[sizeof(infoHeap.nroTrans )-1]='\0';
    infoHeap.fechaTrx[sizeof(infoHeap.fechaTrx )-1]='\0';
    infoHeap.codTar  [sizeof(infoHeap.codTar   )-1]='\0';
    infoHeap.NroCajCa[sizeof(infoHeap.NroCajCa )-1]='\0';   

     /* Convierte la tarjeta a standard ISO (sin espsacio y formato LLL-VAR */
    int counter=0;
    for (counter=0;counter<sizeof(trxOff.nro_tar);counter++)
    {
		if (trxOff.nro_tar[counter]<'0' || trxOff.nro_tar[counter]>'9')
		{
			break;
		} 
    }
    trxOff.nro_tar[counter]='\0';

    /* Configura datos de la terminal */
	strcpy(term_data.cod_tar, trxOff.cod_tar);
    strcpy(term_data.nro_suc, infoHeap.nroSuc);
    strcpy(term_data.nro_caj, infoHeap.nroTerm);
    strcpy(term_data.plan_sf, trxOff.plan_sf);

    /* Busca terminal en base de datos */
	ret=getDataByNT2(&term_data);
    if ( (ret!=OK) || (term_data.caj_bloq==1) )
    {
		/* Loguea error de terminal */
		LogErrTerminal.Put(0, "Fecha Hora %s", currentTimeLog() );
        LogErrTerminal.Put(0, "Caja: %s - Sucursal: %s - Tarjeta: %s - Plan: %s - Evento: TrxOff\n\n", 
							term_data.nro_caj, term_data.nro_suc, term_data.cod_tar, term_data.plan_sf);
        LogAlarm.Put(0, "TrxOff: ERROR (%d) de terminal (%s) bloqueada (%d)\n", ret, term_data.nro_caj, term_data.caj_bloq);
    
        /* Libera la caja */
		ret = P.SetPosFree(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
        if (ret != OK)
        {
             LogAlarm.Put(0, "TrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
								data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
        }
        EnviarMsg(orig_pid, "ERROR", 5);
        return NOOK;
    }

    /* Numero de Tarjeta */
	sprintf(aux_str1,"%02d%s",counter,trxOff.nro_tar);
    IsoMsg->PutField(2, aux_str1);

    /* Monto */
	sprintf(aux_str1,"%012ld",atol(trxOff.monto));
    IsoMsg->PutField(4, aux_str1);

	/* Fecha de transmision */
	sprintf(szFechaEnvio, currentTimestamp());
    IsoMsg->PutField(7, szFechaEnvio);

	/* Si hora operacion mayor a transmision se rechaza */
	/* Quitar si sincronizan hora los servidores        */
	switch (DBTipoProtocoloISO(trxOff.nro_ca))
	{
	case PROT_ISO_AMEX: 
		if (strcmp(trxOff.fec_loc, szFechaEnvio)>0)
		{
			LogAlarm.Put(0, "TrxOff: Ter:[%s] Trx:[%04d]\n", infoHeap.nroTerm, atoi(infoHeap.nroTrans));
			LogAlarm.Put(0, "TrxOff: ERROR fecha/hora FHoperac[%s] FHtransm[%s]\n", trxOff.fec_loc, szFechaEnvio);
			/* Libera la caja */
			ret = P.SetPosFree(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
			if (ret != OK)
			{
				LogAlarm.Put(0, "TrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
								data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
			}
			EnviarMsg(orig_pid, "ERROR", 5);
	        return NOOK;
		}
		break;
	default:
		break;
	}

   	/* Numero de trace */
    TraceNumber.SetNroSuc(term_data.nro_suc);
    TraceNumber.SetNroCaj(term_data.nro_caj);
    TraceNumber.SetNroCA (term_data.nro_ca );
    TraceNumber.SetCodConCie(term_data.cod_con_cie);
    strcpy(aux_str, TraceNumber);
    TraceNumber.Inc();
    IsoMsg->PutField(11, aux_str); 

	/* Hora local */
	IsoMsg->PutField(12, trxOff.fec_loc+4);
    
	/* Fecha local */
	IsoMsg->PutField(13, trxOff.fec_loc);
    
	/* Fecha de expiracion */
	IsoMsg->PutField(14, trxOff.fec_ven);
    
	/* Fecha de captura */
	switch (DBTipoProtocoloISO(trxOff.nro_ca))
	{
	case PROT_ISO_AMEX:
		break;
	default:
		IsoMsg->PutField(17, trxOff.fec_loc);
		break;
	}
    
	/* Modo de ingreso */
	IsoMsg->PutField(22, trxOff.mod_ing );
		
	/* Codigo de red */
    IsoMsg->PutField(24, term_data.cod_red);
    
	/* Codigo de condicion */
	IsoMsg->PutField(25, "00" );

	/* Track 2 si ingreso fue por banda */
	switch (DBTipoProtocoloISO(trxOff.nro_ca))
	{
	case PROT_ISO_AMEX:
		break;
	default:
		if (   !strcmp(trxOff.mod_ing, "022") 
			|| !strcmp(trxOff.mod_ing, "062") ) 
		{
			sprintf(aux_str1, "%02d%s", strlen(trxOff.track2), trxOff.track2);
			IsoMsg->PutField(35, aux_str1);
		}
		break;
	}

	/* Codigo de autorizacion */
	sprintf(aux_str1, "%s", trxOff.cod_aut);
	switch (DBTipoProtocoloISO(trxOff.nro_ca))
	{
	case PROT_ISO_AMEX:
		if( (atoi(trxOff.cod_trx)==T_DEVO) || (atoi(trxOff.cod_trx)==T_DEVP) )
		{
			if (atoi(trxOff.mod_env)==T_OFLI)
			{
				sprintf(aux_str1, "      "); // Devolucion offline Amex
			}
		}
		break;
	default:
		break;
	}
	IsoMsg->PutField(38, aux_str1);

	/* Terminal centro autorizador */
    IsoMsg->PutField(41, term_data.nro_caj_ca);

	/* Comercio centro autorizador */	
    sprintf(aux_str1,"%-15s",term_data.nro_com);
    IsoMsg->PutField(42, aux_str1);

	/* #37322 Cuotas articulos nacionales */
	iNroCuotas = ( term_data.plan_esp > 0 ) ? term_data.plan_esp : atoi(trxOff.nro_cuo) ;
	/* Campo Cuotas */
	sprintf(aux_str1,"003%1.1s%2.2d",trxOff.plan_iso, iNroCuotas);
	if( (atoi(trxOff.cod_trx)==T_DEVO) || (atoi(trxOff.cod_trx)==T_DEVP) )
    {
		if(useCampCuot(trxOff.nro_ca)==1)
		{
			sprintf(aux_str1, "013%1.1s%02d%04d%6s", 
						trxOff.plan_iso, iNroCuotas, atoi(trxOff.nro_tic_ori), trxOff.fec_ori);
		}
    }
	/* Compras AMEX de contado no se envia campo 48 */
	switch (DBTipoProtocoloISO(trxOff.nro_ca))
	{
	case PROT_ISO_AMEX: 
		if (strcmp(aux_str1, "003001")==0) 
		{
			memset(aux_str1, 0, sizeof(aux_str1));
		}
		break;
	default:
		break;
	}
	/* Agrega campo */
	if ( strlen(aux_str1) > 0 )
	{
		IsoMsg->PutField(48, aux_str1);
	}

	/* Codigo de moneda */
	IsoMsg->PutField(49, trxOff.cod_mon );

	/* Fecha de diferimiento (si no es devolucion) */
	if (term_data.dias_dif > 0)
	{
		if( (atoi(trxOff.cod_trx)!=T_DEVO) && (atoi(trxOff.cod_trx)!=T_DEVP) )
		{
			char szFechaDif[6+1];
			if (getFechaDif(trxOff.fec_trx, term_data.dias_dif, szFechaDif)==0)
			{
				ret = strlen(CAMPO_FECHA_DIFE) + strlen(szFechaDif);
				sprintf(aux_str1, "%03d%s%s", ret, CAMPO_FECHA_DIFE, szFechaDif);
				ret = IsoMsg->PutField(59, aux_str1);
				LogAlarm.Put(10, "TrxOff: Campo 59:[%s] Ret:[%d]\n", aux_str1, ret);
			}
		}
	}

    /* Numero de ticket */
	sprintf(aux_str1,"004%04d", atoi(trxOff.nro_tic));
    IsoMsg->PutField(62, aux_str1 );

    LogAlarm.Put(0, "TrxOff: Emp:[%s] Suc:[%s] Ter:[%s] Trx:[%04d] Rowid:[%s]\n", 
						term_data.nro_suc, term_data.nro_caj_sf, infoHeap.nroTerm, 
						atoi(infoHeap.nroTrans), trxOff.rowid);

	/* Arma clave para el heap */
    sprintf(idHeap, "%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca, term_data.nro_suc, term_data.nro_caj, IsoMsg->GetField(11));

    /* Pone datos en el heap */
    LogAlarm.Put(5, "TrxOff: Pone datos en el Heap\n");
	ret=Heap.PutData(idHeap,(char *)&infoHeap, sizeof(infoHeap));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "TrxOff: ERROR (%d) al insertar en Heap. Clave:[%s]\n", Heap.GetErrno(), idHeap);

		/* Libera la caja */
		ret = P.SetPosFree(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
		if (ret != OK)
		{
			LogAlarm.Put(0, "TrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
								data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
		}
        EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

    /* Carga estructura para actualizar la base de datos */
	strncpy( trxPk.nroSuc  , trxOff.nro_suc, sizeof(trxPk.nroSuc  ) );
    strncpy( trxPk.nroTerm , trxOff.nro_caj, sizeof(trxPk.nroTerm ) );
    strncpy( trxPk.nroTrans, trxOff.nro_tic, sizeof(trxPk.nroTrans) );
    strncpy( trxPk.fechaTrx, trxOff.fec_loc, sizeof(trxPk.fechaTrx) );
    strncpy( trxPk.rowid   , trxOff.rowid  , sizeof(trxPk.rowid   ) );

    /* Carga estructura para actualizar la base de datos */
	strncpy(InfoEnvio.cod_est, "01", sizeof(InfoEnvio.cod_est ));
    strncpy(InfoEnvio.fec_env, szFechaEnvio, sizeof(InfoEnvio.fec_env ));
    strncpy(InfoEnvio.nro_lot, "0000", sizeof(InfoEnvio.nro_lot));
    strncpy(InfoEnvio.nro_trc, IsoMsg->GetField(11), sizeof(InfoEnvio.nro_trc));

	/* Actualiza la base de datos */
    ret=DBUpdEnvio(trxPk , InfoEnvio);
    LogAlarm.Put(0, "TrxOff: Actualiza informacion de Envio. Retorno:%d\n", ret);

    /* Registra en cola de timeout */
	strcpy(aux_str, IsoMsg->GetField(11));
    Cfg.GetItem("TimeOutDaemon", "MsgTimeOut1", aux_str1);
	LogAlarm.Put(0, "TrxOff: Timeout ID Trace [%s]\n", idHeap);
    ret=timeout_queue.SetTimeOut(idHeap, atoi(aux_str1), GetType(), "");
    if (ret==NOOK)
    {
        LogAlarm.Put(0, "TrxOff: ERROR (%d) al registrar en TOQ\n",ret);

		/* Libera la caja */
		ret = P.SetPosFree(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
		if (ret != OK)
		{
			LogAlarm.Put(0, "TrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
							data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
		}

		/* Retira del heap */
		ret=Heap.GetData(idHeap,(char *)&infoHeap, sizeof(infoHeap));
		if (ret == NOOK)
		{
			LogAlarm.Put( 0, "TrxOff: ERROR (%d) al obtener del heap\n", Heap.GetErrno());
		}
		
		/* Envia respuesta */
		EnviarMsg(orig_pid, "ERROR", 5);
		
		return NOOK;
    }

    /* Aplana el mensaje */
    len=IsoMsg->GetMsgString(msg_str);

	/* Loguea el mensaje */
	for (iCont=0; iCont<64; iCont++)
	{
		if (IsoMsg->ChkField(iCont))
		{
			LogAlarm.Put(5, "TrxOff: Campo %02d: Valor:[%s]\n", iCont, IsoMsg->GetField(iCont) );
		}
	}

    /* Envia el mensaje a X25 */
	LogAlarm.Put(0, "TrxOff: Envia a X.25. CAut [%s]\n", term_data.nro_ca);
    ret=XQueue.SendMsg(atoi(term_data.nro_ca)+1, msg_str, len);
    if (ret!=OK)
    {
        LogAlarm.Put(0, "TrxOff: ERROR en Envio X.25(%d)\n", XQueue.GetErrno());

		/* Libera la caja */
        ret = P.SetPosFree(data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
        if (ret != OK)
        {
             LogAlarm.Put(0, "TrxOff: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
								data_tar.nro_ca, trxOff.nro_suc, trxOff.nro_caj);
        }
        
		/* Envia respuesta */
		EnviarMsg(orig_pid, "ERROR", 5);

        return NOOK;
    }
    return OK;
}
