#ifdef OS_Linux
#pragma pack(1)
#endif

#define NUM_VERSION "v1.52 1-Oct-2014 (3 min)"

/*
+------------------------------------------------------------------------------+
|                   Copyright (C) 2003 - COTO CICSA                            |
+-----------------+------------------------------------------------------------+
| F. Fuente       | CierreLoteCASuc.C                                          |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| Descripcion     | Proceso que realiza el cierre de lote de las operaciones   |
|                 | de tarjetas por SUCURSAL y CENTRO AUTORIZADOR              |
+-----------------+------------------------------------------------------------+
*/

/****************************************/
/* Formato del Log Visual: [ww,x,y,z]	*/
/* ww : Nivel en el arbol				*/
/* x  : Indicador de Nodo(0) u Hoja(1)  */
/* y  : Indicador de OK(0) o Error(1)   */
/* z  : Nivel de Log 0..3				*/
/****************************************/

/* Includes */
#include "8583.h"
#include "CACStat.h"
#include "Config.h"
#include "Daemons.h"
#include "Includes.h"
#include "Log.h"
#include "PidTable.h"
#include "Queues.h"
#include "Table.h"
extern "C" 
{
	#include "DBServices.h"
	#include "DBTrxOff.h"
}

/* Definiciones */
#define ERROR_COLA_MENSAJE	-2	/* Error irrecuperable: Cola de mensajes o recuperar totales */
#define ERROR_TIMEOUT_VINC	-3	/* Error irrecuperable: Timeout en vinculo                   */
#define ERROR_BASE_DE_DATO	-4	/* Error irrecuperable: Operaciones sobre base de datos      */
#define ERROR_CONFIG_TERMI	-5	/* Advertencia: Alguna terminal sin configurar               */
#define ERROR_MARCA_CIERRE	-6	/* Advertencia: Alguna terminal con error al marcar cierre   */
#define ERROR_BATCH_CIERRE	-7	/* Advertencia: Alguna terminal con error en batch upload    */
#define ERROR_CIERRE_TERMI	-8	/* Advertencia: Alguna terminal con cierre marcado invalido  */
#define ERROR_RESPUESTA_BB	-9  /* Advertencia: Alguna terminal con respuesta BB             */
#define ERROR_DIF_CONCILIA	-10 /* Advertencia: Alguna terminal con diferencia conciliacion  */
#define ERROR_SIN_TRANSACC	-11 /* Sale por OK: Alguna terminal sin transacciones            */
#define ERROR_CIERRE_RECHA	-12 /* Advertencia: Alguna terminal con cierre rechazado         */
#define ERROR_PISA_TARJETA	-13 /* Advertencia: Alguna terminal sin pisar datos de tarjeta   */

#define NOOK1				-9
#ifndef NOOK
#define NOOK				-1
#endif
#ifndef OK
#define OK					0
#endif

#define NORMAL				0
#define BATCH				1

/* Objetos globales */
BatchData		batch_data;
BatchTable		DBBatch;
CACStatus		CACS;
Config			Cfg(CONFIG_FILE);
Log				LogVisual;
PidTable		PTable;
Queue			AQueue;
Queue			EQueue;
str_fet_term	term_data;
TrxData			trx_data;
TrxTable		DBTrx;

/* Variables externas */
extern int SqlErrno;

/* Variables globales */
char useBloqueoCaja;
char useBatchUpload;
char useCierreNormal;
char szArchivoTemporal[256];
char szArchivoFinal[256];

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ObtenerFechaCierre                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene la fecha actual de cierre                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *ObtenerFechaCierre()
{
	static char cTimestamp[40];
	struct tm *newtime;
	time_t ltime;
	
	time(&ltime);
	newtime=localtime(&ltime);
	sprintf(cTimestamp,"%04d%02d%02d",newtime->tm_year+1900,
		newtime->tm_mon+1,newtime->tm_mday);
	return cTimestamp;
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Strip                                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Quita los blancos al numero de tarjeta                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *Strip(char *str)
{
	int	iCont;
	
	while (str[0]==' ')
	{
		for (iCont=0; iCont<strlen(str); iCont++)
		{
			str[iCont]=str[iCont+1];
		}
	}
	for (iCont=0; iCont<strlen(str); iCont++)
	{
		if (str[iCont]==' ')
		{
			str[iCont]=0;
		}
	}
	return str;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | *pcEnmascarar                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Enmascara los campos sensibles de tarjetas                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *pcEnmascarar(int iCpo, char *szCpo)
{
	int iTam;
	static char szRes[256];

	memcpy(szRes, szCpo, sizeof(szRes));
	if ( (iCpo==2) || (iCpo==14) || (iCpo==35) || (iCpo==45) || (iCpo==55) )
	{
		iTam = strlen(szCpo);
		if ( (iTam>=1) && (iTam<=255) )
		{
			memset(szRes, '*', iTam);
		}
	}
	return (szRes);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ProcesaBatchUpl                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Lanza el batch de la terminal con diferencias              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int ProcesaBatchUpl(str_fet_term term_data)
{
	char			szAuxStr1[100];
	char			szAuxStr2[100];
	char			szMsgStr[2000];
	int				iMsgLen;
	int				iCont;
	int				retProc;
	Iso_Msg			*msg_iso;
	Iso_Msg			*rta_iso;
	TrxData			trx_data;
	str_sel_cadata	term_data2;
	int				iCantProces = 0;
	int				iCantCierre = 0;
	int				iNroCuotas;
		
	/* Arma la estructura para consulta */
	sprintf(szAuxStr1, "%02d", E_APRO);
	trx_data.CodEst(szAuxStr1);
	trx_data.NroCA(term_data.nro_ca);
	trx_data.NroCaj(term_data.nro_caj);
	trx_data.NroLot(term_data.nro_lot);
	trx_data.NroCajCa(term_data.nro_caj_ca);

	/* Obtiene la cantidad de transacciones a enviar en batch upload */
	iCantCierre = DBTrx.TrxCountCierre(trx_data.GetIsLastData());
	if (iCantCierre <= 0)
	{
		LogVisual.Put(0, "[04,1,1,0] Error BD (obteniendo cantidad). Cod=%d\n", DBTrx.GetErrno());
		return ERROR_BASE_DE_DATO;
	}

	/* Recupera una transaccion */
	iCantProces++;
	retProc=DBTrx.Fetch(trx_data.GetFetBUData());
	LogVisual.Put(3,"[04,1,0,3] Retorno de DBTrx.Fetch:%d. GetErrno:%d\n", retProc, DBTrx.GetErrno() );
	if ((retProc==NOOK) && (DBTrx.GetErrno()!=1403))
	{
		LogVisual.Put(0, "[04,1,1,0] Error BD (obteniendo transacción). Cod=%d\n",DBTrx.GetErrno());
		return ERROR_BASE_DE_DATO;
	}

	/* Verifica que haya transacciones para enviar */
	if ((retProc==NOOK) && (DBTrx.GetErrno()==1403))
	{
		LogVisual.Put(0, "[04,1,0,0] No hay transacciones para Batch Upload\n");
		return ERROR_SIN_TRANSACC;
	}
	
	LogVisual.Put(0, "[04,0,0,0] Detalle de Transacciones a enviar [Cantidad %d]\n", iCantCierre);
	
	/* Repite mientras hay transacciones para enviar */
	while (retProc==OK) 
	{
		trx_data.PutFetBUData();

		/* Loguea datos de la transaccion a enviar */
		LogVisual.Put(0, "[05,0,0,0] Transaccion:%s - Monto:%s - Msje #%d/%d\n",
						 trx_data.NroTic(), trx_data.Monto(), iCantProces, iCantCierre );
		LogVisual.Put(1, "[06,1,0,1] NroCuotas:%s - NroTrace:%s - CodAutor:%s\n",
						 trx_data.NroCuo(), trx_data.NroTrc(), trx_data.CodAut());
		LogVisual.Put(1, "[06,1,0,1] ModEnvio:%s - PlanSF:%s - PlanISO:%s - CodMone:%s\n",
				         trx_data.ModEnv(), trx_data.PlanSF(), trx_data.PlanISO(), trx_data.ModIng());
		LogVisual.Put(1, "[06,1,0,1] NroTrace:%s - FecLocal:%s - CodTrx:%s\n",
						 trx_data.NroTrc(), trx_data.FecLoc(), trx_data.CodTrx());
		LogVisual.Put(1, "[06,1,0,1] ModIngr:%s - CodEstado:%s - NroRef: %s\n", 
						 trx_data.ModIng(), trx_data.CodEst(), trx_data.NroRef());

		/* Pone el codigo de mensaje de acuerdo a si es cierre normal o batch */
		msg_iso=new Iso_Msg(TYPE_VISA_BATCH);
		
		/* Recupera los datos de la terminal */
		strcpy(term_data2.cod_tar, trx_data.CodTar());
		strcpy(term_data2.nro_suc, term_data.nro_suc);
		strcpy(term_data2.nro_caj, term_data.nro_caj);
		strcpy(term_data2.plan_sf, trx_data.PlanSF());
		retProc=getDataByNT2(&term_data2); /*-*/
		LogVisual.Put(3,"[06,1,0,3] Retorno de getDataByNT2:%d\n", retProc); /*-*/
		if (retProc!=OK)
		{
			while ( DBTrx.Fetch(trx_data.GetFetBUData()) == OK ) { /* Reposiciona cursor */ }
			LogVisual.Put(0,"[06,1,1,0] Error BD (datos terminal). Cod=%d\n",retProc);
			delete msg_iso;
			return ERROR_BASE_DE_DATO;
		} 
		
		/* Completa campos del mensaje ISO */
		strcpy(szAuxStr1, trx_data.NroTar());
		Strip(szAuxStr1);
		sprintf(szAuxStr2, "%02d%s", strlen(szAuxStr1), szAuxStr1);
		msg_iso->PutField(2, szAuxStr2);

		/* Si no es ultima operacion del batch antepone 1 al codigo de mensaje */
		strcpy(szAuxStr1, trx_data.CodPro());
		if (iCantCierre != iCantProces)
		{
			szAuxStr1[strlen(szAuxStr1)-1]='1';
		}
		msg_iso->PutField( 3, szAuxStr1);
		
		msg_iso->PutField( 4, trx_data.Monto());
		if ( atol(trx_data.Retiro()) > 0 )
		{
			switch(term_data2.prot_cash[0])
			{
			case CASHBACK_VISA:
				sprintf(szAuxStr1, "%012ld", atol(trx_data.Monto()) + atol(trx_data.Retiro()) );
				msg_iso->PutField( 4, szAuxStr1);
				break;
			}
		}
		
		switch (DBTipoProtocoloISO(trx_data.NroCA()))
		{
		case PROT_ISO_AMEX:
			msg_iso->PutField( 7, trx_data.FecEnv());
			break;
		default:
			msg_iso->PutField( 7, currentTimestamp());
			break;
		}
		msg_iso->PutField(12, trx_data.FecLoc()+4);
		msg_iso->PutField(13, trx_data.FecLoc());
		msg_iso->PutField(14, trx_data.FecVen());
		switch (DBTipoProtocoloISO(trx_data.NroCA()))
		{
		case PROT_ISO_AMEX:
			break;
		default:
			msg_iso->PutField(17, trx_data.FecLoc());
			break;
		}
		msg_iso->PutField(22, trx_data.ModIng());
		msg_iso->PutField(24, term_data.cod_red);
		msg_iso->PutField(25, "00");
		msg_iso->PutField(37, trx_data.NroRef());
		
		sprintf(szAuxStr1, "%s", trx_data.CodAut());
		switch (DBTipoProtocoloISO(trx_data.NroCA()))
		{
		case PROT_ISO_AMEX:
			if ( (atoi(trx_data.CodTrx())==T_DEVO) || (atoi(trx_data.CodTrx())==T_DEVP) )
			{
				if (atoi(trx_data.ModEnv())==T_OFLI)		
				{
					sprintf(szAuxStr1, "      "); // Devolucion offline Amex
				}
			}
			break;
		default:
			break;
		}
		msg_iso->PutField(38, szAuxStr1);

		msg_iso->PutField(39, trx_data.CodRta());
		msg_iso->PutField(41, trx_data.NroCajCa());
		msg_iso->PutField(42, trx_data.NroCom());
		
		/* #37322 Cuotas articulos nacionales */
		iNroCuotas = ( term_data.plan_esp > 0 ) ? term_data.plan_esp : atoi(trx_data.NroCuo()) ;

		/* Completa campos del mensaje ISO */
		sprintf(szAuxStr1, "003%1.1s%02d",trx_data.PlanISO(), iNroCuotas);
		if ( (atoi(trx_data.CodTrx())==T_DEVO) || (atoi(trx_data.CodTrx())==T_DEVP) )
		{
			if(useCampCuot(trx_data.NroCA())==1)
			{
				sprintf(szAuxStr1, "013%1s%02d%04d%6s",	
					trx_data.PlanISO(),	
					iNroCuotas, 
					atoi(trx_data.NroTicOri()), 
					trx_data.FecOri());
			}			
		}
		/* Compras AMEX de contado no se envia campo 48 */
		switch (DBTipoProtocoloISO(trx_data.NroCA()))
		{
		case PROT_ISO_AMEX: 
			if (strcmp(szAuxStr1, "003001")==0) 
			{
				memset(szAuxStr1, 0, sizeof(szAuxStr1));
			}
			break;
		default:
			break;
		}
		/* Agrega campo */
		if ( strlen(szAuxStr1) > 0 )
		{
			msg_iso->PutField(48,szAuxStr1);
		}
		msg_iso->PutField(49, trx_data.CodMon());
		
		/* Agrega monto cashback */
		if ( atol(trx_data.Retiro()) > 0 )
		{
			switch (term_data2.prot_cash[0])
			{
			case CASHBACK_VISA:
			case CASHBACK_POSN:
				sprintf(szAuxStr1, "012%012ld", atol(trx_data.Retiro()));
				msg_iso->PutField(54, szAuxStr1);
				break;
			}
		}

		sprintf(szAuxStr1, "004%04d", atoi(trx_data.NroTic()));
		msg_iso->PutField(62, szAuxStr1);
		
		/* Finaliza el llenado de los campos del mensaje ISO */
		if (   (atoi(trx_data.ModEnv())==T_ONLI)		// Operacion ONLINE
			|| (atoi(trx_data.ModEnv())==T_ANUL) )		// Anulacion ONLINE
		{
			sprintf(szAuxStr1, "010%04d%6s", TYPE_VISA_VENTA/10, trx_data.NroTrc());
		}
		else
		{
			sprintf(szAuxStr1, "010%04d%6s", TYPE_VISA_VENTA_LOCAL/10, trx_data.NroTrc());
		}
		msg_iso->PutField(63, szAuxStr1);

		/* Loguea el mensaje a enviar */
		LogVisual.Put(2, "[06,0,0,2] Mensaje enviado (Batch)\n");
		LogVisual.Put(2, "[07,0,0,2] Formato ascii\n");
		for (iCont=1; iCont<64; iCont++)
		{
			if (msg_iso->ChkField(iCont)==TRUE)
			{
				LogVisual.Put(2, "[08,1,0,2] Campo [%02d]: [%s]\n", iCont, pcEnmascarar(iCont, msg_iso->GetField(iCont)) );							
			}
		}	
		
		/* Aplana el mensaje a enviar al centro */
		iMsgLen=msg_iso->GetMsgString(szMsgStr);
	
		/* Loguea el mensaje a enviar */
		/*LogVisual.Put(2, "[07,0,0,2] Formato hexadecimal\n");*/
		/*LogVisual.PutMsg(2, "[08,1,0,2]", szMsgStr, iMsgLen);*/

		/* Envia el mensaje */
		LogVisual.Put(3, "[06,1,0,3] Entrada a cola de mensajes (Envío Batch)\n");
		retProc=EQueue.SendMsg(EVENT_BATCH, szMsgStr, iMsgLen);
		LogVisual.Put(3, "[06,1,0,3] Salida de cola de mensajes (Envío Batch). Cod:%d\n", retProc);

		/* Verifica si hay error en el envio a cola de mensajes */
		if (retProc==NOOK)
		{
			while ( DBTrx.Fetch(trx_data.GetFetBUData()) == OK ) { /* Reposiciona cursor */ }
			LogVisual.Put(0, "[06,1,1,0] Error enviando mensaje. Cod=%d\n", EQueue.GetErrno());
			delete msg_iso;
			return ERROR_COLA_MENSAJE;
		}
		
		/* Recibe el mensaje del centro autorizador */
		LogVisual.Put(3, "[06,1,0,3] Entrada a cola de mensajes (Recepción Batch)\n");
		retProc=AQueue.GetMsg(getpid(), szMsgStr, sizeof(szMsgStr));
		LogVisual.Put(3, "[06,1,0,3] Salida de cola de mensajes (Recepción Batch). Cod=%d\n", retProc);
		
		/* Verifica si hay error en la recepcion del mensaje */
		if (retProc==NOOK)
		{		
			while ( DBTrx.Fetch(trx_data.GetFetBUData()) == OK ) { /* Reposiciona cursor */ }
			LogVisual.Put(0,"[06,1,1,0] Error esperando respuesta. Cod=%d\n", AQueue.GetErrno());
			delete msg_iso;
			return ERROR_COLA_MENSAJE;
		}
		
		LogVisual.Put(2, "[06,0,0,2] Mensaje recibido (Batch)\n");
		/*LogVisual.Put(2, "[07,0,0,2] Formato hexadecimal\n");*/
		/*LogVisual.PutMsg(2, "[08,1,0,2]", szMsgStr, iMsgLen);*/
			
		/* Crea el mensaje */
		rta_iso=new Iso_Msg(szMsgStr, iMsgLen);

		/* Loguea los campos del mensaje recibido */
		LogVisual.Put(2, "[07,0,0,2] Formato ascii\n");
		for (iCont=1; iCont<64; iCont++)
		{
			if (rta_iso->ChkField(iCont)==TRUE)
			{
				LogVisual.Put(2, "[08,1,0,2] Campo [%02d]: [%s]\n", iCont, pcEnmascarar(iCont, rta_iso->GetField(iCont)) );
			}
		}

		/* Verifica codigo de error timeout FF */
		if (strcmp(rta_iso->GetField(38), "FF")==0)
		{
			LogVisual.Put(0, "[06,1,1,0] Error: Timeout en Batch.\n");
			LogVisual.Put(0, "[06,1,1,0] Se cancela Batch Upload TOUT (Protocolo Amex)\n");
			delete rta_iso;
			delete msg_iso;
			while ( DBTrx.Fetch(trx_data.GetFetBUData()) == OK ) { /* Reposiciona cursor */ }
			return ERROR_TIMEOUT_VINC;
		}

		/* Setea el centro autorizador */
		rta_iso->SetNroCA8583(term_data.nro_ca);

		/* Loguea resultado de la transaccion */
		if (rta_iso->Aprob()==OK) 
		{
			LogVisual.Put(0, "[06,1,0,0] Transacción Aprobada\n");
		}
		else 
		{
			LogVisual.Put(0, "[06,1,1,0] Transacción Rechazada. Respuesta CA=%s\n",rta_iso->GetField(39));
			LogVisual.Put(0, "[06,1,1,0] Se cancela Batch Upload RECH (Protocolo Amex)\n");
			delete rta_iso;
			delete msg_iso;
			while ( DBTrx.Fetch(trx_data.GetFetBUData()) == OK ) { /* Reposiciona cursor */ }
			return ERROR_BATCH_CIERRE;				
		}  			
		
		/* Borra el mensaje */
		delete rta_iso;

		/* Borra el mensaje de consulta */
		delete msg_iso;

		/* Recupera la siguiente transaccion */ 
		iCantProces++;
		retProc=DBTrx.Fetch(trx_data.GetFetBUData());
		LogVisual.Put(3,"[06,1,0,3] Retorno de DBTrx.Fecth:%d. GetErrno:%d\n", retProc, DBTrx.GetErrno());
		if ((retProc==NOOK) && (DBTrx.GetErrno()!=1403))
		{
			LogVisual.Put(0, "[06,1,1,0] Error BD (obteniendo siguiente transacción). "
										 "Cod=%d\n",DBTrx.GetErrno());
			return ERROR_BASE_DE_DATO;
		}
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ProcesaCierre                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Lanza el cierre de totales de la terminal seleccionada     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int ProcesaCierre(str_fet_term term_data, int tipo_cierre)
{
	char			szAuxStr1[100];
	char			szMsgStr[2000];
	int				iMsgLen;
	int				iCont;
	int				retProc;
	int				retBDat = OK;
	char			szAuxFecha[9];
	int				iNroLote;
	Iso_Msg			*msg_iso;
	str_act_cierre	act_data;
	char			szCampoRta38[16];
	char			szCampoRta39[16];
	char			szMontoVen[16];
	char			szMontoAnu[16];
	
	/* Arma la estructura para consulta */
	batch_data.NroSuc(term_data.nro_suc);
	batch_data.NroCaj(term_data.nro_caj);
	batch_data.NroCA(term_data.nro_ca);
	batch_data.NroLot(term_data.nro_lot);
	batch_data.NroCom(term_data.nro_com);
	batch_data.NroCajCa(term_data.nro_caj_ca);
	sprintf(szAuxStr1, "%02d", E_APRO);
	batch_data.CodEst(szAuxStr1);

	/* Recupera los totales de la terminal */
	retProc=DBTrx.Select(batch_data.GetSelData());
	LogVisual.Put(3,"[04,1,0,3] Retorno de DBTrx.Select:%d, GetErrno:%d\n", retProc, DBTrx.GetErrno());
	if (retProc)
	{
		LogVisual.Put(0, "[04,1,0,0] Error al recuperar totales\n");
		return ERROR_BASE_DE_DATO;
	}
	batch_data.PutSelData();

	/* Procesa si los totales son distintos de cero */
	if ((atol(batch_data.CanVen())==0) && 
		(atol(batch_data.CanDev())==0) && 
		(atol(batch_data.CanAnu())==0) &&
		(atol(batch_data.CanPag())==0) &&
		(atol(batch_data.CanDep())==0) &&
		(atol(batch_data.CanAnp())==0)    )
	{
		strcpy(szAuxFecha,ObtenerFechaCierre());
		batch_data.FecCie(szAuxFecha); 
		retProc = DBBatch.UpdateDev(batch_data.GetUpdData());
		LogVisual.Put(3,"[04,1,0,3] Retorno de DBBatch.UpdateDev:%d. GetErrno:%d\n", retProc, DBBatch.GetErrno() );	
		if (retProc==OK)
		{
			/* Pisa el dato de tarjeta */
			retProc=DBBatch.UpdatePostCierre(batch_data.GetUpdData());
			LogVisual.Put(3,"[04,1,0,3] Retorno de DBBatch.UpdatePostCierre:%d. GetErrno:%d\n", retProc, DBBatch.GetErrno() );	
			if (retProc==NOOK)
			{
				LogVisual.Put(3,"[04,1,0,0] Error pisando datos de tarjetas\n");
				return ERROR_PISA_TARJETA;
			} 
		}

		LogVisual.Put(0, "[04,1,0,0] No hay transacciones\n");
		return ERROR_SIN_TRANSACC;
	}

	/* Bloquea la caja si es AMEX */
	if ( (tipo_cierre == NORMAL) && (useBloqueoCaja == 'S') )
	{
		switch (DBTipoProtocoloISO(term_data.nro_ca))
		{
		case PROT_ISO_AMEX:
			retProc=update_bloqueo_term(&term_data, 1);
			LogVisual.Put(3,"[04,1,0,3] Retorno de update_bloqueo_term:%d\n", retProc);
			break;
		}
	}

	/* Pone el codigo de mensaje de acuerdo a si es cierre normal o batch */
	if (tipo_cierre == NORMAL)
	{
		msg_iso=new Iso_Msg(TYPE_VISA_CIERRE);
	}
	else
	{
		msg_iso=new Iso_Msg(TYPE_VISA_CIERRE_B);
	}

	/* Loguea los totales */
	LogVisual.Put(0, "[04,0,0,0] Envio de Totales\n");
	LogVisual.Put(0, "[05,1,0,0] Ventas:       Cantidad:%s - Monto:%s\n",
					 batch_data.CanVen(), batch_data.MonVen() );
	LogVisual.Put(0, "[05,1,0,0] Devoluciones: Cantidad:%s - Monto:%s\n",
					 batch_data.CanDev(), batch_data.MonDev() );
	LogVisual.Put(0, "[05,1,0,0] Anulaciones : Cantidad:%s - Monto:%s\n",
					 batch_data.CanAnu(), batch_data.MonAnu() );
	if ( (term_data.prot_cash[0] == CASHBACK_VISA) || (term_data.prot_cash[0] == CASHBACK_POSN) )
	{
		if ( ( atol(batch_data.MonRet()) != 0 ) || ( atol(batch_data.MonAnr()) != 0 ) )
		{
			LogVisual.Put(0, "[05,1,0,0] Retiros     :               - Monto:%s\n", batch_data.MonRet() );		
			LogVisual.Put(0, "[05,1,0,0] Anul.Retiros:               - Monto:%s\n", batch_data.MonAnr() );
		}
	}
	
	/* Completa campos del mensaje ISO */
	msg_iso->PutField(24, term_data.cod_red    );
	msg_iso->PutField(41, term_data.nro_caj_ca );
	msg_iso->PutField(42, term_data.nro_com_cie);

	/* Pago de resumen */
	if (term_data.usa_pagos[0] != 'S')
	{
		/*LogVisual.Put(0, "[05,1,0,0] Retiros:                    - Monto:%s\n", batch_data.MonRet() );*/
		sprintf(szMontoVen, "%s", batch_data.MonVen());
		sprintf(szMontoAnu, "%s", batch_data.MonAnu());
		switch (term_data.prot_cash[0])
		{
		case CASHBACK_VISA:
				sprintf(szMontoVen, "%012ld", atol(batch_data.MonVen()) + atol(batch_data.MonRet()));
				sprintf(szMontoAnu, "%012ld", atol(batch_data.MonAnu()) + atol(batch_data.MonAnr()));
				break;
		}

		sprintf(szAuxStr1, "051%3s%4s%12s%4s%12s%4s%12s", 
			batch_data.NroLot(), 
			batch_data.CanVen(), szMontoVen, 
			batch_data.CanDev(), batch_data.MonDev(), 
			batch_data.CanAnu(), szMontoAnu);
	}
	else
	{
		LogVisual.Put(0, "[05,1,0,0] Pagos:        Cantidad:%s - Monto:%s\n",
						 batch_data.CanPag(), batch_data.MonPag() );
		LogVisual.Put(0, "[05,1,0,0] Devol Pagos:  Cantidad:%s - Monto:%s\n",
						 batch_data.CanDep(), batch_data.MonDep() );
		LogVisual.Put(0, "[05,1,0,0] Anul Pagos:   Cantidad:%s - Monto:%s\n",
						 batch_data.CanAnp(), batch_data.MonAnp() );

		sprintf(szAuxStr1, "099%3s%4s%12s%4s%12s%4s%12s%4s%12s%4s%12s%4s%12s", 
			batch_data.NroLot(), batch_data.CanVen(), batch_data.MonVen(), 
			batch_data.CanDev(), batch_data.MonDev(), batch_data.CanAnu(), 
			batch_data.MonAnu(), batch_data.CanPag(), batch_data.MonPag(), 
			batch_data.CanDep(), batch_data.MonDep(), batch_data.CanAnp(), 
			batch_data.MonAnp() );
	}		

	msg_iso->PutField(63, szAuxStr1);

	/* Loguea los campos del mensaje de peticion */
	LogVisual.Put(2, "[05,0,0,2] Mensaje enviado (Cierre)\n");
	LogVisual.Put(2, "[06,0,0,2] Formato ascii\n");
	for (iCont=1; iCont<64; iCont++)
	{
		if (msg_iso->ChkField(iCont)==TRUE)
		{
			LogVisual.Put(2, "[07,1,0,2] Campo [%02d]: [%s]\n", iCont, msg_iso->GetField(iCont));							
		}
	}
	
	/* Aplana el mensaje a enviar al centro */
	iMsgLen=msg_iso->GetMsgString(szMsgStr);
	
	/* Loguea el mensaje a enviar */
	/*LogVisual.Put(2, "[06,0,0,2] Formato hexadecimal\n");*/
	/*LogVisual.PutMsg(2, "[07,1,0,2]", szMsgStr, iMsgLen);*/
	
	/* Envia el mensaje */
	LogVisual.Put(3, "[05,1,0,3] Entrada a cola de mensajes (Envío Cierre)\n");
	retProc=EQueue.SendMsg(EVENT_CIERRE, szMsgStr, iMsgLen);
	LogVisual.Put(3, "[05,1,0,3] Salida de cola de mensajes (Envío Cierre). Cod=%d\n", retProc);

	/* Borra el mensaje */
	delete msg_iso;
	
	/* Verifica si hay error en el envio a cola de mensajes */
	if (retProc == NOOK)
	{
		LogVisual.Put(0,"[05,1,1,0] Error mandando mensaje. Cod=%d\n", EQueue.GetErrno());
		return ERROR_COLA_MENSAJE;
	}	

	/* Recibe el mensaje del centro autorizador */
	LogVisual.Put(3, "[05,1,0,3] Entrada a cola de mensajes (Recepción Cierre)\n");
	retProc=AQueue.GetMsg(getpid(), szMsgStr, sizeof(szMsgStr));
	LogVisual.Put(3, "[05,1,0,3] Salida de cola de mensajes (Recepción Cierre). Cod=%d\n", retProc);
		
	/* Verifica si hay error en la recepcion del mensaje */
	if (retProc == NOOK)
	{
		LogVisual.Put(0,"[05,1,1,0] Error esperando respuesta. Cod=%d\n", AQueue.GetErrno());
		return ERROR_COLA_MENSAJE;
	} 
	
	LogVisual.Put(2, "[05,0,0,2] Mensaje recibido (Cierre)\n");
	/*LogVisual.Put(2, "[06,0,0,2] Formato hexadecimal\n");*/
	/*LogVisual.PutMsg(2, "[07,1,0,2]", szMsgStr, iMsgLen);*/
			
	/* Crea el mensaje */
	msg_iso = new Iso_Msg(szMsgStr, iMsgLen);

	/* Loguea los campos del mensaje recibido */
	LogVisual.Put(2, "[06,0,0,2] Formato ascii\n");	
	for (iCont=1; iCont<64; iCont++)
	{
		if (msg_iso->ChkField(iCont)==TRUE)
		{
			LogVisual.Put(2, "[07,1,0,2] Campo [%02d]: [%s]\n", iCont, msg_iso->GetField(iCont));							
		}
	}
	
	/* Loguea la respuesta del centro */
	LogVisual.Put(0,"[05,1,0,0] Respuesta CA: %s\n", msg_iso->GetField(39));
	
	/* Actualiza la estructura */
	msg_iso->GetDBData(batch_data);	
	batch_data.NroCA (term_data.nro_ca );
	batch_data.NroSuc(term_data.nro_suc);
	batch_data.NroCaj(term_data.nro_caj);
	batch_data.NroLot(term_data.nro_lot);
	batch_data.NroCom(term_data.nro_com);
	batch_data.NroCajCa(term_data.nro_caj_ca);
	batch_data.FecRta(msg_iso->GetField(7));
	strcpy(act_data.nro_ca,batch_data.NroCA()); 
	strcpy(act_data.cod_rta, msg_iso->GetField(39));

	sprintf(szCampoRta38, msg_iso->GetField(38));
	sprintf(szCampoRta39, msg_iso->GetField(39));
	delete msg_iso;

	/* Verifica codigo de error timeout FF */
	if (strcmp(szCampoRta38, "FF")==0)
	{
		/* Actualiza base de datos */
		LogVisual.Put(0, "[05,1,1,0] Error: Timeout en cierre\n");		
		if (strcmp(batch_data.AgrCie(), "N") != 0)
		{
			sprintf(szAuxStr1, "%02d", E_TOUT);
			batch_data.CodEst(szAuxStr1);
			batch_data.FecCie("");
			retProc=DBBatch.Update(batch_data.GetUpdData());
			LogVisual.Put(3,"[05,1,0,3] Retorno de DBBatch.Update:%d\n", retProc);
			if (retProc == NOOK)
			{
				LogVisual.Put(0,"[05,1,1,0] Error BD (Rta 'FF'). Cod=%d\n",DBBatch.GetErrno());
			} 
		}
		return ERROR_TIMEOUT_VINC;
	}
	
	/* Verifica codigo de error cancelar BB */
	if (strcmp(szCampoRta39, "BB")==0)
	{
		retProc = update_bloqueo_term(&term_data, 0);
		LogVisual.Put(3, "[05,1,0,3] Retorno de update_bloqueo_term:%d\n", retProc);
		LogVisual.Put(0, "[05,1,1,0] Error: Respuesta BB (Cancelar cierre)\n");
		return ERROR_RESPUESTA_BB;
	}
	
	/* Verifica respuesta del centro */
	if (DBRtaAct(act_data) == A_APRO)
	{	
		/* Actualiza transacciones a aprobadas */
		LogVisual.Put(0, "[05,1,0,0] Cierre Aprobado\n");
		sprintf(szAuxStr1, "%02d", E_APRO);
		batch_data.CodEst(szAuxStr1);
		strcpy(szAuxFecha,ObtenerFechaCierre());
		batch_data.FecCie(szAuxFecha); 
		retProc=DBBatch.UpdateTrx(batch_data.GetUpdData());
		LogVisual.Put(3,"[05,1,0,3] Retorno de DBBatch.UpdateTrx:%d. SqlErrno:%d\n", retProc, SqlErrno);	
		if (retProc==NOOK)
		{
			LogVisual.Put(0,"[05,1,1,0] Error BD (Rta CA='OK'). Cod=%d\n", DBBatch.GetErrno());
			retBDat = ERROR_BASE_DE_DATO;
		} 
		
		/* Obtiene el numero de lote */
		iNroLote = atoi(term_data.nro_lot)+1;
		if (iNroLote>=1000)
		{
			iNroLote = 1;
		}
		sprintf(szAuxStr1, "%03d", iNroLote);
		strcpy(term_data.nro_lot, szAuxStr1);

		/* Actualiza la terminal */
		retProc=update_term(&term_data);
		LogVisual.Put(3,"[05,1,0,3] Retorno de update_term:%d\n", retProc);
		if (retProc==NOOK)
		{
			LogVisual.Put(0,"[05,1,1,0] Error BD (actualizando terminal). Cod=%d\n", DBBatch.GetErrno());
			retBDat = ERROR_BASE_DE_DATO;
		}
		else
		{
			/* Pisa el dato de tarjeta */
			retProc=DBBatch.UpdatePostCierre(batch_data.GetUpdData());
			LogVisual.Put(3,"[05,1,0,3] Retorno de DBBatch.UpdatePostCierre:%d. GetErrno:%d\n", retProc, DBBatch.GetErrno() );	
			if (retProc==NOOK)
			{
				LogVisual.Put(3,"[05,1,0,0] Error pisando datos de tarjetas\n");
				retBDat = ERROR_PISA_TARJETA;
			} 
		}
		return retBDat;
	}
	else
	{
		/* Actualiza transacciones a rechazadas CRED_CIERRE */
		LogVisual.Put(0, "[05,1,1,0] Diferencia en conciliación de cierre\n");					
		if (strcmp(batch_data.AgrCie(), "N") != 0)
		{
			sprintf(szAuxStr1, "%02d", E_RECH);
			batch_data.CodEst(szAuxStr1);
			batch_data.FecCie("");
			retProc=DBBatch.Update(batch_data.GetUpdData());
			LogVisual.Put(3,"[05,1,0,3] Retorno de DBBatch.Update:%d\n", retProc);
			if (retProc==NOOK)
			{
				LogVisual.Put(0,"[05,1,1,0] Error BD (Rta CA='NOK'). Cod=%d\n",DBBatch.GetErrno());				
				return ERROR_BASE_DE_DATO;
			}
		}
		if (strcmp(szCampoRta39, "95")==0)
		{
			LogVisual.Put(0, "[05,1,1,0] Diferencia en conciliación de cierre\n");
			return ERROR_DIF_CONCILIA;
		}
		else
		{
			LogVisual.Put(0, "[05,1,1,0] Rechazo en cierre. Respuesta %s\n", szCampoRta39);
			return ERROR_CIERRE_RECHA;
		}
	}		
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | vShutDown                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra las colas abiertas y finaliza la aplicacion         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void vShutDown(int iSignal)
{
	int		iRet= 0;
	int		iReintentos= 0;
	char	szComando[256];

	/* Loguea fin de proceso */
	LogVisual.Put(0,"[01,0,0,0] Fin de Proceso\n");
	
	/* Cierra los CACS */
	CACS.Normal(atoi(term_data.nro_ca));
	iRet=CACS.Close();
	if (iRet == NOOK)
	{
		LogVisual.Put(0,"[02,1,1,0] Error cerrando CACS flags\n. Cod=%d\n", CACS.GetErrno());
	} 
	else
	{
		LogVisual.Put(0, "[02,1,0,0] CACS flags cerrados\n");		
	}

	/* Cierra la tabla de PIDs */
	iRet=PTable.RemPid(getpid());
	if (iRet == NOOK)
	{
		LogVisual.Put(0, "[02,1,1,0] Error removiendo PID de la tabla. Cod=%d\n", PTable.GetErrno());
	} 
	else
	{
		LogVisual.Put(0, "[02,1,0,0] PID removido de la tabla\n");
	}

	/* Cierra la cola administrativa */
	iRet=AQueue.Close();
	if (iRet == NOOK)
	{
		LogVisual.Put(0, "[02,1,1,0] Error cerrando cola administrativa. Cod=%d\n", AQueue.GetErrno());
	}
	else
	{
		LogVisual.Put(0, "[02,1,0,0] Cola administrativa cerrada\n");
	}

	/* Loguea fin de proceso */
	if (iSignal==0)
	{
		LogVisual.Put(0, "[02,1,0,0] Fin de proceso. Retorno:%d. %s", iSignal, currentTimeLog() );
	}
	else
	{
		LogVisual.Put(0, "[02,1,1,0] Fin de proceso. Retorno:%d. %s", iSignal, currentTimeLog() );
	}

	/* Concatena el archivo de log temporal al archivo final */
	iRet= 0;
	sprintf(szComando, "cat %s >> %s", szArchivoTemporal, szArchivoFinal);
	LogVisual.Put(3, "[02,1,0,3] Llamada al sistema para ejecutar comando:\n");
	LogVisual.Put(3, "[02,1,0,3] %s\n", szComando);
	do
	{
		if (iRet != 0) 
		{
			LogVisual.Put(3, "[02,1,1,3] Error al copiar archivo de Log\n"); 
			iReintentos++;
			sleep(5);
		}
		iRet= system(szComando);
	} while ((iRet!=0) && (iReintentos<5));
	
	/* Borra archivo temporal */
	if (iRet == 0)
	{
		sprintf(szComando, "rm -f %s\n", szArchivoTemporal);
		system(szComando);
	}
	
	/* Cierra archivo de log */
	LogVisual.Close();

	/* Retorna el resultado */
	exit(iSignal);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | vShutDownTerminal                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Llama a funcion de finalizacion de aplicacion              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void vShutDownTerminal(int iSignal=9)
{
	vShutDown(9);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | vInit                                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Inicia las variables de configuracion                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void vInit(char *nro_ca, char *nro_caj_sf, char *nro_suc)
{
	FILE	*fArch;
	int		iRet;
	char	szFecha[20]; 
	char	szComando[256];
	char	szAuxStr1[100];
	char	szAuxStr2[100];
	
	/* Toma las señales por si se aborta */ 
	signal(SIGINT, vShutDownTerminal);
	signal(SIGTERM, vShutDownTerminal);
	
	/*  Toma fecha de archivo fecha.dat (contiene la fecha del dia anterior) */
	if ((fArch=fopen("../log/fecha.dat", "r"))==NULL)
	{
		printf("\nError en apertura de archivo (fecha.dat)\n");
		exit(1);
	}
	fscanf(fArch, "%s", szFecha);
	fclose(fArch);
	
	/* Arma el nombre del archivo de log */
	if (Cfg.GetItem("AdminDaemon", "LogCierreVisualCASuc", szAuxStr1)!=CFG_FOUND)
	{
		sprintf(szAuxStr1, "LogCierre");
	}
	sprintf(szArchivoFinal, "%s%s.%s.%s", szAuxStr1, nro_ca, "SUC", szFecha); 
	sprintf(szArchivoTemporal, "LogCierreCASuc%s%s%s.%s.tmp", nro_ca, nro_suc, nro_caj_sf, szFecha);
	
	/* Borra el archivo de log si existe */
	sprintf(szComando, "rm -f %s\n", szArchivoTemporal);
	system(szComando);

	/* Abre archivo de log */
	LogVisual.Open(szArchivoTemporal);
	
	/* Configura el nivel de log */
	if (Cfg.GetItem("AdminDaemon", "LogLevelVisualCASuc", szAuxStr1)!=CFG_FOUND)
	{
		sprintf(szAuxStr1, "3");
	}
	LogVisual.SetLevel(atoi(szAuxStr1));

	/* Configura el nivel de log por pantalla */
	if (Cfg.GetItem("AdminDaemon", "LogLevelScreenVisualCASuc", szAuxStr1)!=CFG_FOUND)
	{
		sprintf(szAuxStr1, "3");
	}
	LogVisual.SetDebugLevel(atoi(szAuxStr1));

	/* Loguea el inicio del proceso */
	memset(szAuxStr1, 0, sizeof(szAuxStr1));
	strncpy(szAuxStr1, &szFecha[0],2);
	strcat(szAuxStr1, "/");
	strncat(szAuxStr1, &szFecha[2], 2);
	LogVisual.Put(0, "[00,0,0,0] Empresa:%03s - Sucursal:%05s - Centro:%02s - Fecha:%s\n", nro_suc, nro_caj_sf, nro_ca, szAuxStr1);
	LogVisual.Put(0, "[01,0,0,0] Inicio de Proceso\n");
	LogVisual.Put(0, "[02,1,0,0] CierreLoteCASuc. %s\n", NUM_VERSION);

	/* Crea cola administrativa */
	Cfg.GetItem("AdminDaemon", "QueueID", szAuxStr1);
	Cfg.GetItem("AdminDaemon", "QueuePerms", szAuxStr2);
	iRet = AQueue.Create(atol(szAuxStr1), atoi(szAuxStr2));
	if (iRet == NOOK)
	{
		LogVisual.Put(0, "[02,1,1,0] Error creando cola administrativa. Cod=%d\n", AQueue.GetErrno());
		vShutDown(1);
	}
	else
	{
		LogVisual.Put(0, "[02,1,0,0] Cola administrativa creada\n");
	}

	/* Abre cola de eventos */
	Cfg.GetItem("VisaDaemon", "QueueID", szAuxStr1);
	iRet = EQueue.Open(atoi(szAuxStr1));
	if (iRet != OK )
	{
		LogVisual.Put(0, "[02,1,1,0] Error abriendo cola de eventos. Cod=%d\n", EQueue.GetErrno());
		vShutDown(1);
	}
	else
	{
		LogVisual.Put(0, "[02,1,0,0] Cola de eventos abierta\n");
	}

	/* Abre flags de estados */
	Cfg.GetItem("CACStatVisa", "ShMemID", szAuxStr1);
	while (CACS.Open(atoi(szAuxStr1))==NOOK)
	{
		if (CACS.GetErrno()!=ENOENT)
		{
			LogVisual.Put(0, "[02,1,1,0] Error abriendo CACS flags. Cod=%d\n", CACS.GetErrno());
			vShutDown(1);
		}
		else
		{
			LogVisual.Put(0, "[02,1,0,0] CACS flags abiertos\n");
		}
	}

	/* Abre tabla de PIDs */
	Cfg.GetItem("PIDTable", "ShMemID", szAuxStr1);
	Cfg.GetItem("PIDTable", "MaxEntries", szAuxStr2);
	while (PTable.Open(atoi(szAuxStr1), atoi(szAuxStr2))==NOOK)
	{
		if (PTable.GetErrno()!=ENOENT)
		{
			LogVisual.Put(0, "[02,1,1,0] Error abriendo tabla de PIDs. Cod=%d\n",PTable.GetErrno());
			vShutDown(1);
		} 
		else
		{
			LogVisual.Put(0, "[02,1,0,0] Tabla de PIDs abierta\n");
		}	
	}

	/* Agrega PID a la tabla de PIDs */
	iRet = PTable.PutPid(getpid());
	if (iRet == NOOK)
	{
		LogVisual.Put(0, "[02,1,1,0] Error agregando PID a la tabla de PIDs. Cod=%d\n", PTable.GetErrno());
		vShutDown(1);
	}
	else
	{
		LogVisual.Put(0, "[02,1,0,0] PID agregado a la tabla de PIDs\n");
	}

	/* Recupera flag para saber si borrar o no los datos de las tarjetas */
	if (Cfg.GetItem("AdminDaemon", "RemoveDataCard", szAuxStr2)==CFG_FOUND)
	{
		memset(szAuxStr1, 0, sizeof(szAuxStr1));
		szAuxStr1[0] = szAuxStr2[0];
		batch_data.DelDat(szAuxStr1);
	}
	else
	{
		batch_data.DelDat("N");
	}

	/* Recupera flag para saber si agregar registro de cierre en CRED_CIERRE */
	if (Cfg.GetItem("AdminDaemon", "InsertCierre", szAuxStr2)==CFG_FOUND)
	{
		memset(szAuxStr1, 0, sizeof(szAuxStr1));
		szAuxStr1[0] = szAuxStr2[0];
		batch_data.AgrCie(szAuxStr1);
	}
	else
	{
		batch_data.AgrCie("S");
	}

	/* Recupera si el centro efectua batch upload */
	sprintf(szAuxStr1, "UseBatchUpload%02s", nro_ca);
	if (Cfg.GetItem("CAVisaDaemon", szAuxStr1, szAuxStr2)==CFG_FOUND)
	{
		useBatchUpload = szAuxStr2[0];
	}
	else
	{
		useBatchUpload = 'S';
	}

	/* Recupera si el centro efectua cierre normal */
	sprintf(szAuxStr1, "UseCierreNormal%02s", nro_ca);
	if (Cfg.GetItem("CAVisaDaemon", szAuxStr1, szAuxStr2)==CFG_FOUND)
	{
		useCierreNormal = szAuxStr2[0];
	}
	else
	{
		useCierreNormal = 'S';
	}

	/* Recupera si el centro bloquea las cajas */
	sprintf(szAuxStr1, "UseBloqueoCaja%02s", nro_ca);
	if (Cfg.GetItem("CAVisaDaemon", szAuxStr1, szAuxStr2)==CFG_FOUND)
	{
		useBloqueoCaja = szAuxStr2[0];
	}
	else
	{
		useBloqueoCaja = 'N';
	}


	/* Loguea finalizacion correcta de inicializacion */
	LogVisual.Put(0, "[02,1,0,0] Inicio finalizado correctamente\n");
	LogVisual.Put(0, "[02,1,0,0] Efectuar cierre batch upload: %c\n", useBatchUpload);
	LogVisual.Put(0, "[02,1,0,0] Efectuar cierre normal:       %c\n", useCierreNormal);
	LogVisual.Put(0, "[02,1,0,0] Efectuar bloqueo de cajas:    %c\n", useBloqueoCaja);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion principal de la aplicacion                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int main(int argc, char *argv[])
{
	int		retTerminal;
	int		retBatch;
	int		retCierre;
	int		retBad;
	char	szAuxStr2[100];
	int		iErrorTerminal=0;
	char	szCajCA[16];
		
	/* Verifica correccion en parametros */
	if (    (  argc<4 )
		 || (  argc>6 ) 
	     || ( (argc==4) && (    (strlen(argv[1]) != (sizeof(term_data.nro_ca)     -1) ) 
		                     || (strlen(argv[2]) != (sizeof(term_data.nro_caj_sf) -1) ) 
							 || (strlen(argv[3]) != (sizeof(term_data.nro_suc)    -1) )  
                           )
			)
	     || ( (argc==5) && (    (strlen(argv[1]) != (sizeof(term_data.nro_ca)     -1) ) 
		                     || (strlen(argv[2]) != (sizeof(term_data.nro_caj_sf) -1) ) 
							 || (strlen(argv[3]) != (sizeof(term_data.nro_suc)    -1) )  
							 || (strlen(argv[4]) != (sizeof(term_data.nro_caj_ca) -1) )  
                           )
			)
       )
	{
		printf("\nUso:\t%s <Num.Cen> <Num.Suc> <Num.Emp> [<Num.CajCA>]\n\n", argv[0]);
		printf("\tNum.Cen - Numero de Centro Aut (%d digitos)\n"  , (sizeof(term_data.nro_ca)     -1));
		printf("\tNum.Suc - Numero de Sucursal   (%d digitos)\n"  , (sizeof(term_data.nro_caj_sf) -1));
		printf("\tNum.Emp - Numero de Empresa    (%d digitos)\n"  , (sizeof(term_data.nro_suc)    -1));
		printf("\tNum.Caj - Numero de CajaCA     (%d digitos)\n\n", (sizeof(term_data.nro_caj_ca) -1));
		exit(1);
	}
	else
	{
		memset(szCajCA, 0, sizeof(szCajCA));
		if (argc==5)
		{
			sprintf(szCajCA, argv[4]);
		}
	}
	
	/* Inicia variables de configuracion */
	vInit(argv[1],argv[2], argv[3]);
	
	/* Loguea hora de comienzo de proceso */
	LogVisual.Put(0, "[01,0,0,0] Envio de Operaciones. Pid:[%d]\n", getpid());
	LogVisual.Put(0, "[02,1,0,0] Comienzo de proceso:%s", currentTimeLog());
	
	/* Ciclo principal por terminal */
	do 
	{	/*******************************/
		/* Obtiene terminal a procesar */
		/*******************************/

		/* Configura centro y sucursal a cerrar */
		strncpy(term_data.nro_ca    , argv[1], sizeof(term_data.nro_ca)     -1);
		strncpy(term_data.nro_caj_sf, argv[2], sizeof(term_data.nro_caj_sf) -1);
		strncpy(term_data.nro_suc   , argv[3], sizeof(term_data.nro_suc)    -1);
		strncpy(term_data.nro_caj_ca, szCajCA, sizeof(term_data.nro_caj_ca) -1);

		/* Recupera la terminal */
		retTerminal=fetch_termdata_ca_suc(&term_data);
		
		/* Error al recuperar terminal */
		if (retTerminal == NOOK)
		{
			LogVisual.Put(0, "[02,1,1,0] Error BD irrecuperable obteniendo datos de terminal. Cod=%d\n", SqlErrno);
			LogVisual.Put(0, "[01,1,1,0] CANCELADO PROCESO DE CIERRE POR ERRORES DE BASE DE DATOS\n");
			iErrorTerminal = ERROR_BASE_DE_DATO;
			break;
		}
		
		/* Error en terminal (mal configurada en CRED_CAS o CRED_COM_CIE) */
		if (retTerminal == NOOK1)
		{
			LogVisual.Put(0, "[02,0,0,0] Terminal:%s - CajaCA:%s - Lote:%s - CodConCie:%s\n",
					term_data.nro_caj, term_data.nro_caj_ca, 
					term_data.nro_lot, term_data.cod_con_cie);
			LogVisual.Put(0, "[03,1,1,0] Error. Verificar configuración en crédito\n");
			iErrorTerminal = ERROR_CONFIG_TERMI;
			continue;
		}

		/* No hay terminales */
		if (retTerminal == 1403)
		{
			continue;
		}

		/* Loguea datos de la terminal a cerrar */
		LogVisual.Put(0, "[02,0,0,0] Terminal:%s - CajaCA:%s - Lote:%s - CodConCie:%s\n",
						term_data.nro_caj, term_data.nro_caj_ca, term_data.nro_lot, term_data.cod_con_cie);
		LogVisual.Put(1, "[03,1,0,1] NroSuc:%s - NroCom:%s\n", term_data.nro_suc, term_data.nro_com);
		LogVisual.Put(1, "[03,1,0,1] CodRed:%s - NroComCie:%s\n", term_data.cod_red, term_data.nro_com_cie);

		/********************************/
		/* Procesa los cierres normales */
		/********************************/

		if (useCierreNormal == 'N')
		{
			LogVisual.Put(0, "[03,0,0,0] Cierre Normal no configurado\n");
			continue;
		}
		else
		{	
			/* Procesa el cierre normal de la terminal */
			LogVisual.Put(0, "[03,0,0,0] Cierre Normal (A)\n");
			CACS.Cierre(atoi(term_data.nro_ca), atoi(term_data.nro_suc), atoi(term_data.nro_caj));
			retCierre = ProcesaCierre(term_data, NORMAL);
			if (retCierre != ERROR_SIN_TRANSACC)
				iErrorTerminal = retCierre;
			CACS.Normal(atoi(term_data.nro_ca));
			LogVisual.Put(3,"[04,1,0,3] Retorno de ProcesaCierre (Normal):%d\n", retCierre);	
			
			/* Error critico: base de datos */
			if (retCierre == ERROR_BASE_DE_DATO)
			{
				LogVisual.Put(0,"[01,1,1,0] CANCELADO PROCESO DE CIERRE POR PROBLEMAS EN BASE DE DATOS\n");
				break;
			}
			/* Error critico: cola de mensajes */
			if (retCierre == ERROR_COLA_MENSAJE)
			{
				LogVisual.Put(0,"[01,1,1,0] CANCELADO PROCESO DE CIERRE AL RECUPERAR TOTALES / COLA DE MENSAJES\n");
				break;
			}
			/* Error critico: timeout */
			if (retCierre == ERROR_TIMEOUT_VINC)
			{
				LogVisual.Put(0, "[04,1,1,0] Timeout. Problemas con el canal de comunicación\n");
				LogVisual.Put(0,"[01,1,1,0] CANCELADO PROCESO DE CIERRE POR TIMEOUT EN VINCULO\n");		
				break;
			}
			/* Otro error salvo diferencia conciliacion */
			if (retCierre != ERROR_DIF_CONCILIA)
			{
				continue;
			}
		} /* fin de useCierreNormal */
		
		/*****************************/
		/* Procesa los cierres batch */
		/*****************************/

		if (useBatchUpload == 'N')
		{
			LogVisual.Put(0, "[03,0,0,0] Cierre Batch no configurado\n");
			continue;
		}
		else
		{
			/*  Procesa cierre batch de la terminal */
			LogVisual.Put(0, "[03,0,0,0] Batch Upload\n");
			CACS.Cierre(atoi(term_data.nro_ca), atoi(term_data.nro_suc), atoi(term_data.nro_caj));
			retBatch = ProcesaBatchUpl(term_data);
			if (retBatch != ERROR_SIN_TRANSACC)
				iErrorTerminal = retBatch;
			CACS.Normal(atoi(term_data.nro_ca));
			LogVisual.Put(3,"[04,1,0,3] Retorno de ProcesaBatchUpload:%d\n", retBatch);

			/* Sale si son errores criticos */
			if ( (retBatch==ERROR_TIMEOUT_VINC) || (retBatch==ERROR_COLA_MENSAJE) )
			{
				break;
			}

			/* BatchUpl OK */
			if (retBatch==OK)
			{
				/* Procesa el cierre normal de la terminal */
				LogVisual.Put(0, "[03,0,0,0] Cierre Normal (B)\n");
				CACS.Cierre(atoi(term_data.nro_ca), atoi(term_data.nro_suc), atoi(term_data.nro_caj));
				retCierre=ProcesaCierre(term_data, BATCH);
				if (retCierre != ERROR_SIN_TRANSACC)
					iErrorTerminal = retCierre;
				CACS.Normal(atoi(term_data.nro_ca));
				LogVisual.Put(3,"[04,1,0,3] Retorno de ProcesaCierre (Batch):%d\n", retCierre);	

				/* Error critico: base de datos */
				if (retCierre == ERROR_BASE_DE_DATO)
				{
					LogVisual.Put(0,"[01,1,1,0] CANCELADO PROCESO DE CIERRE POR PROBLEMAS EN BASE DE DATOS\n");
					break;
				}		
				/* Error critico: cola de mensajes */
				if (retCierre == ERROR_COLA_MENSAJE)
				{
					LogVisual.Put(0,"[01,1,1,0] CANCELADO PROCESO DE CIERRE AL RECUPERAR TOTALES / COLA DE MENSAJES\n");
					break;
				}
				/* Error critico: timeout */
				if (retCierre == ERROR_TIMEOUT_VINC)
				{
					LogVisual.Put(0, "[04,1,1,0] Timeout. Problemas con el canal de comunicación\n");
					LogVisual.Put(0,"[01,1,1,0] CANCELADO PROCESO DE CIERRE POR TIMEOUT EN VINCULO\n");		
					break;
				}
				
				/* Si falla el cierre, lo marca invalido */
				if (retCierre==ERROR_CIERRE_RECHA)
				{
					/* Marca error en el cierre si hubo problemas */
					LogVisual.Put(0, "[03,0,1,0] Marca cierre invalido\n");
					retBad=DBMarkBadCierre(&term_data);
					LogVisual.Put(3,"[04,1,0,3] Retorno de DBMarkBadCierre:%d\n", retBad);
					if (retBad != OK) 
					{
						iErrorTerminal = ERROR_MARCA_CIERRE;
						LogVisual.Put(0, "[04,1,1,0] Error BD (Marcando cierre inválido) Cod=%d\n", retBad);				
					}
					else
					{	
						iErrorTerminal = ERROR_CIERRE_TERMI;
						LogVisual.Put(0, "[04,1,0,0] Cierre marcado como inválido\n");						
					}
					LogVisual.Put(0, "[04,1,1,0] Cierre finalizado con errores\n");
				}
			} /* retBatch == OK */
		} /* fin de useBatchUpload */
	} while (retTerminal != 1403);
	
	/* Finaliza */
	vShutDown(abs(iErrorTerminal));

	return OK;
}
