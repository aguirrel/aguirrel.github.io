#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | RtaRevAnul.C                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia los mensajes de reversos de transacciones al centro  |
|                 | autorizador                                                | 
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

extern Config		    Cfg;			/* Objeto de configuracion      */
extern Log				LogAlarm;		/* Logging de actividades       */ 
extern TimeOutQueue		timeout_queue;	/* Cola de timeouts             */ 
extern TrxHeap			Heap;			/* Objeto Heap                  */
extern TrxTable			DBTrx;			/* Para tabla de transacciones  */
extern Queue			XQueue;			/* Cola de X.25                 */
extern Pos				P;				/* Objeto PoS                   */
extern Log				LogErrTerminal;	/* Logging errores terminales   */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrx::RevTrx                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RevTrx::RevTrx(int msg_pid, char *msg_data, int data_len)
{
	char szAux[5];

	/* Carga el evento en el objeto */
	event_type=EVENT_SNDREVERSO;

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

	/* Crea el mensaje para enviar al CA */
	IsoMsg=new Iso_Msg(TYPE_VISA_REV_VENTA);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrx:: ~RevTrx                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
RevTrx::~RevTrx()
{
	delete IsoMsg;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrx::EnviarMsg                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RevTrx::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "RevTrx: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RevTrx::ProcessIt                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int RevTrx::ProcessIt()
{
	int					len;
	int					ret;
	int					iCont;
	char				msg_str[2000];
	int					cont;
	trxRevHeap			heap_data;          
	str_sel_ntdata		term_data;
	TrxData				trx_data;
	char                aux_str1[100];
	char				aux_str2[100];
	str_tarjeta         data_tar;
	char				idHeap[20];
	int					iIngresoBanda = 0;
	int					iEnvioOffline = 0;
	int					iRevAnulacion = 0;
	int					iErrorBD;
	int					iNroCuotas;
		
	/* Carga el codigo de tarjeta */
	trx_data.CodTar(CodTarjeta);

	/* Carga el codigo de estado */
	sprintf(aux_str1, "%02d", E_TOUT);
	trx_data.CodEst(aux_str1);
	
	/* Carga el numero de caja del centro autorizador */
	trx_data.NroCajCa(NroCajCa);

	/* Carga el numero de transaccion */
	trx_data.NroTrx(NroTrx);

	/* Recupera un reverso a enviar */
    LogAlarm.Put(0, "RevTrx: Lee reverso [%s] [%s] [%s] [%s]\n", 
						trx_data.NroCajCa(), trx_data.CodTar(), trx_data.CodEst(), trx_data.NroTrx());
	ret=DBGetNextTrx(trx_data.GetFetBUData(), &iErrorBD);
	if (ret!=OK)
	{  
		/* Asume que no existen transacciones pendientes */
		LogAlarm.Put( 0, "RevTrx: ERROR. Retorno de recuperacion de reverso:%d Errno:%d\n", ret, iErrorBD);
		EnviarMsg(orig_pid, "ERROR", 5);
		return OK;  
	}
	trx_data.PutFetBUData();

	/* Loguea datos de la transaccion */
	LogAlarm.Put(0, "RevTrx: Com:[%s] Caj:[%s] Cuo:[%s] Iso:[%s] "
					"PlanSF:[%s] Mon:[%s] Tar:[%s]\n",
					trx_data.NroCom() , trx_data.NroCajCa(), trx_data.NroCuo(),
					trx_data.PlanISO(), trx_data.PlanSF()  , trx_data.CodMon(), CodTarjeta);

	/* Obtiene el centro autorizador en funcion del codigo de tarjeta */
	strcpy(data_tar.cod_tar, trx_data.CodTar());
	ret = ObtCA(&data_tar);
	if (ret!=OK)
	{
		LogAlarm.Put(0,"RevTrx: ERROR al obtener CA\n");
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}       

	/* Obtiene datos de la terminal */
	strcpy(term_data.nro_caj, trx_data.NroCaj());
	strcpy(term_data.cod_tar, trx_data.CodTar());
	strcpy(term_data.nro_suc, trx_data.NroSuc());
	strcpy(term_data.nro_caj, trx_data.NroCaj());
	strcpy(term_data.plan_sf, trx_data.PlanSF());

	/* Busca los restantes datos de la terminal en BD */ 
	ret=getDataByNT2(&term_data);
	if ( (ret!=OK) || (term_data.caj_bloq==1) )
	{
		/* Loguea errores de terminal */
		LogErrTerminal.Put(0, "Fecha Hora %s", currentTimeLog() );
		LogErrTerminal.Put(0, "Caja: %s - Sucursal: %s - Tarjeta: %s - Plan: %s - Evento: RevTrx\n\n", 
								trx_data.NroCaj(), trx_data.NroSuc(), trx_data.CodTar(), trx_data.PlanSF());
		LogAlarm.Put( 0, "RevTrx: ERROR (%d) de terminal (%s) bloqueada (%d)\n", ret, term_data.nro_caj, term_data.caj_bloq); 

		/* Envia respuesta al SendRev */
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}
	
	/* Setea centro autorizador */
	IsoMsg->SetNroCA8583(trx_data.NroCA());

	/* Obtiene la forma de ingreso */
	if (   !strcmp(trx_data.ModIng(), "022") 
		|| !strcmp(trx_data.ModIng(), "062") )
	{
		iIngresoBanda = 1;
	}

	/* Obtiene el modo de envio */
	if ( !strcmp(trx_data.ModEnv(), "01") ) 
	{
		iEnvioOffline = 1;
	}

	/* Obtiene si es reverso de anulacion */
	if ( !strcmp(trx_data.CodTrx(), "05") ) 
	{		
		iRevAnulacion = 1;
	}

	LogAlarm.Put( 0, "RevTrx: iIngresoBanda:%d - iEnvioOffline:%d - iRevAnulacion:%d\n", iIngresoBanda, iEnvioOffline, iRevAnulacion);

	/* Numero de tarjeta (ingreso manual o banda con offline) */
	strcpy(aux_str1, trx_data.NroTar());
	for (cont=0; cont<strlen(aux_str1); cont++)
	{
		if ((aux_str1[cont]<'0') || (aux_str1[cont]>'9'))
		{
			aux_str1[cont]='\0';
		}
	}
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		sprintf(aux_str2, "%02d%s", strlen(aux_str1), aux_str1);
		IsoMsg->PutField(2, aux_str2);
		break;
	default:
		if ( (iIngresoBanda==0) || (iEnvioOffline==1) )
		{
			sprintf(aux_str2, "%02d%s", strlen(aux_str1), aux_str1);
			IsoMsg->PutField(2, aux_str2);
		}
		break;
	}

	/* Codigo de procesamiento */
	IsoMsg->PutField(3, trx_data.CodPro());

	/* Monto */
	IsoMsg->PutField(4, trx_data.Monto());
	if ( atol(trx_data.Retiro()) > 0 )
	{
		switch (term_data.prot_cash[0])
		{
		case CASHBACK_VISA: 
			sprintf(aux_str1, "%012ld", atol(trx_data.Monto())+atol(trx_data.Retiro()) );
			IsoMsg->PutField(4, aux_str1);
			break;
		default:
			break;
		}
	}
	
	/* Fecha de envio */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		IsoMsg->PutField(7, trx_data.FecEnv());
		break;
	default:
		IsoMsg->PutField(7, currentTimestamp());
		break;
	}

	/* Numero de trace */
	IsoMsg->PutField(11, trx_data.NroTrc());

	/* Hora local */
	IsoMsg->PutField(12, trx_data.FecLoc()+4);

	/* Fecha local */
	IsoMsg->PutField(13, trx_data.FecLoc());

	/* Fecha de vencimiento (ingreso manual o banda con offline) */
	memset(aux_str1, 0, sizeof(aux_str1));
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		sprintf(aux_str1, "%s", trx_data.FecVen());
		break;
	default:
		if ( (iIngresoBanda==0) || (iEnvioOffline==1) )
		{
			sprintf(aux_str1, "%s", trx_data.FecVen());
		}
		break;
	}
	if (strlen(aux_str1)>0)
	{
		IsoMsg->PutField(14, trx_data.FecVen());
	}

	/* Modo de ingreso */
	IsoMsg->PutField(22, trx_data.ModIng());

	/* Codigo de red */
	IsoMsg->PutField(24, term_data.cod_red);

	/* Codigo de condicion */
	IsoMsg->PutField(25, "00");

	/* Track 2 */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		break;
	default:
		if (iIngresoBanda==1)
		{
			sprintf(aux_str1, "%02d%s", strlen(trx_data.Track2()), trx_data.Track2());
			IsoMsg->PutField(35, aux_str1);
		}
		break;
	}
	
	/* Numero de referencia */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		if (iRevAnulacion==1)
		{
			sprintf(aux_str1, "%s", trx_data.NroRef());
			if (strlen(aux_str1)>0)
			{
				IsoMsg->PutField(37, aux_str1);
			}
		}
		break;
	default:
		if (iRevAnulacion==1)
		{
			if (IsoMsg->EsAnulacionPinpad())
			{
				sprintf(aux_str1, "%s", trx_data.NroRef());
				if (strlen(aux_str1)>0)
				{
					IsoMsg->PutField(37, aux_str1);
				}	
			}
		}
		break;
	}
	
	/* Codigo de autorizacion (offline) */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		if ((iEnvioOffline==1)||(iRevAnulacion==1))
		{
			sprintf(aux_str1, "%s", trx_data.CodAut());
			if (strlen(aux_str1)>0)
			{
				if ( (atoi(trx_data.CodTrx())==T_DEVO) || (atoi(trx_data.CodTrx())==T_DEVP) )
				{
					if (iEnvioOffline == 1)
					{
						sprintf(aux_str1, "      "); // Devolucion offline Amex
					}
				}
				IsoMsg->PutField(38, aux_str1);
			}
		}
		break;
	default:
		break;
	}

	/* Numero de caja centro autorizador */
	IsoMsg->PutField(41, term_data.nro_caj_ca);

	/* Numero de comercio */
	IsoMsg->PutField(42, term_data.nro_com);

	/* Track I (banda y online ) */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		break;
	default:
		if ( (iIngresoBanda==1) && (iEnvioOffline==0) )
		{
			memset(aux_str2, 0, sizeof(aux_str2));
			memcpy(aux_str2, trx_data.Track1(), strlen(trx_data.Track1()));
			iCont = strlen(aux_str2)-1;
			while ( (iCont>=0) && (aux_str2[iCont]==' ') )
				iCont--;
			aux_str2[iCont+1] = 0;
			if (strlen(aux_str2)>0)
			{
				sprintf(aux_str1, "%02d%s", strlen(aux_str2), aux_str2);
				IsoMsg->PutField(45, aux_str1);
			}
		}
		break;
	}

	/* #37322 Cuotas articulos nacionales */
	iNroCuotas = ( term_data.plan_esp > 0 ) ? term_data.plan_esp : atoi(trx_data.NroCuo()) ;
	/* Cuotas */
	sprintf(aux_str1,"003%1.1s%2.2d",trx_data.PlanISO(), iNroCuotas);
	if( (atoi(trx_data.CodTrx())==T_DEVO) || (atoi(trx_data.CodTrx())==T_DEVP) )
	{
		if(useCampCuot(trx_data.NroCA())==1)
		{
			sprintf(aux_str1, "013%1.1s%02d%04d%6s", trx_data.PlanISO(), iNroCuotas,
													  atoi(trx_data.NroTicOri()), trx_data.FecOri());
		}
	}
	/* Compras AMEX de contado no se envia campo 48 */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
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
	IsoMsg->PutField(49, trx_data.CodMon());

	/* Monto de retiro cashback */
	if ( atol(trx_data.Retiro()) > 0 )
	{
		switch (term_data.prot_cash[0])
		{
		case CASHBACK_VISA: 
		case CASHBACK_POSN:
			sprintf(aux_str1, "012%012ld", atol(trx_data.Retiro()) );
			IsoMsg->PutField(54, aux_str1);
			break;
		default:
			break;
		}
	}
	
	/* Codigo de seguridad (online) */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		break;
	default:
		if (iEnvioOffline==0)
		{
			memset(aux_str2, 0, sizeof(aux_str2));
			memcpy(aux_str2, trx_data.CodSeg(), strlen(trx_data.CodSeg()));
			iCont = strlen(aux_str2)-1;
			while ( (iCont>=0) && (aux_str2[iCont]==' ') )
				iCont--;
			aux_str2[iCont+1] = 0;
			if (aux_str2[0]!=0)
			{
				sprintf(aux_str1, "%03d%s", strlen(aux_str2), aux_str2);
				IsoMsg->PutField(55, aux_str1);
			}
		}
		break;
	}

	/* Fecha de diferimiento */
	if (term_data.dias_dif > 0)
	{
		if( (atoi(trx_data.CodTrx())!=T_DEVO) && (atoi(trx_data.CodTrx())!=T_DEVP) )
		{
			char szFechaDif[6+1];
			if (getFechaDif(trx_data.FecTrx(), term_data.dias_dif, szFechaDif)==0)
			{
				ret = strlen(CAMPO_FECHA_DIFE) + strlen(szFechaDif);
				sprintf(aux_str1, "%03d%s%s", ret, CAMPO_FECHA_DIFE, szFechaDif);
				ret = IsoMsg->PutField(59, aux_str1);
				LogAlarm.Put(10, "RevTrx: Campo 59:[%s] Ret:[%d]\n", aux_str1, ret);
			}
		}
	}

	/* Numero de ticket */
	sprintf(aux_str1, "004%04d", atoi(trx_data.NroTic()));
	IsoMsg->PutField(62, aux_str1);

	/* Completa la estructura para el heap */
	LogAlarm.Put(5, "RevTrx: Completa la estructura para el Heap.\n");
	heap_data.orig_pid=orig_pid;
	strncpy(heap_data.cod_tar , trx_data.CodTar(), sizeof(heap_data.cod_tar));
	strncpy(heap_data.nroSuc  , trx_data.NroSuc(), sizeof(heap_data.nroSuc ));
	strncpy(heap_data.nroTerm , trx_data.NroCaj(), sizeof(heap_data.nroTerm));
	strncpy(heap_data.nroTrans, trx_data.NroTic(), sizeof(heap_data.nroTrans));
	strncpy(heap_data.fechaTrx, trx_data.FecLoc(), sizeof(heap_data.fechaTrx));
	strcpy(heap_data.NroCajCa , trx_data.NroCajCa());
	strcpy(heap_data.Rowid    , trx_data.Rowid());
	heap_data.cod_tar [sizeof(heap_data.cod_tar )-1]='\0';
	heap_data.nroSuc  [sizeof(heap_data.nroSuc  )-1]='\0';
	heap_data.nroTerm [sizeof(heap_data.nroTerm )-1]='\0';
	heap_data.nroTrans[sizeof(heap_data.nroTrans)-1]='\0';
	heap_data.fechaTrx[sizeof(heap_data.fechaTrx)-1]='\0';
	heap_data.NroCajCa[sizeof(heap_data.NroCajCa)-1]='\0';
	heap_data.Rowid   [sizeof(heap_data.Rowid   )-1]='\0';

	/* Loguea datos del heap */
	LogAlarm.Put( 0, "RevTrx: Emp:[%s] Suc:[%s] Ter:[%s] Trx:[%04d] Rowid:[%s]\n", 
						heap_data.nroSuc, term_data.nro_caj_sf, heap_data.nroTerm, 
						atoi(heap_data.nroTrans), heap_data.Rowid);
	LogAlarm.Put( 0, "RevTrx: Fecha:[%s] Trace [%s]\n", heap_data.fechaTrx, IsoMsg->GetField(11));

	/* Pone los datos en el heap */
	sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca, term_data.nro_suc, term_data.nro_caj, IsoMsg->GetField(11));
	ret=Heap.PutData(idHeap,(char *)&heap_data, sizeof(heap_data));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "RevTrx: ERROR (%d) al  insertar en Heap. Clave:[%s]\n", Heap.GetErrno(), idHeap);
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	/* Comienza a contar el Time Out */
	Cfg.GetItem("TimeOutDaemon", "MsgTimeOut1", aux_str2);
	ret=timeout_queue.SetTimeOut(idHeap, atoi(aux_str2), GetType(), "");
	if (ret==NOOK)
	{
		/* Retira del heap si hubo error */
		ret=Heap.GetData(idHeap, (char *)&heap_data,sizeof(heap_data));
		if (ret==NOOK)
		{
			LogAlarm.Put( 0, "RevTrx: ERROR (%d) al obtener del heap\n", Heap.GetErrno());
		}

		/* Envia respuesta de rechazo */
		LogAlarm.Put(0, "RevTrx: IdHeap:[%s] Tiempo:[%d] Tipo:[%d]\n", idHeap, atoi(aux_str2), GetType());
		LogAlarm.Put(0, "RevTrx: ERROR (%d) al registrar en cola de timeout\n", ret);
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	/* Actualiza estado y numero de caja */
	sprintf(aux_str1, "%02d", E_ENRE); /* 19 */
	trx_data.CodEst(aux_str1);
	trx_data.NroCajCa(term_data.nro_caj_ca);

	/* Actualiza base de datos */
	ret=DBTrx.UpdateRtaTo(trx_data.GetUpdDataTO());
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "RevTrx: ERROR (%d) al actualizar base de datos\n", DBTrx.GetErrno());
		LogAlarm.Put(0, "RevTrx: CA:[%s] Trc:[%s] Caj:[%s] Emp:[%s]\n", 
							trx_data.NroCA(), trx_data.NroTrc(), trx_data.NroCajCa(), trx_data.NroSuc());
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	/* Aplana el mensaje iso */
	len=IsoMsg->GetMsgString(msg_str);

	/* Loguea el mensaje */
	for (iCont=0; iCont<64; iCont++)
	{
		if (IsoMsg->ChkField(iCont))
		{
			LogAlarm.Put(5, "RevTrx: Campo %02d: Valor:[%s]\n", iCont, IsoMsg->GetField(iCont) );
		}
	}

	/* Envia el mensaje al X25Daemon */
	LogAlarm.Put(0, "RevTrx: Envia a X.25. CAut [%s]\n", data_tar.nro_ca);
	ret=XQueue.SendMsg(atoi(data_tar.nro_ca)+1, msg_str, len);
	if (ret!=OK)
	{
		LogAlarm.Put( 0, "RevTrx: ERROR (%d) al enviar a X.25\n", XQueue.GetErrno());
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	LogAlarm.Put( 0, "RevTrx: Evento finalizado\n");
	return OK;
}
