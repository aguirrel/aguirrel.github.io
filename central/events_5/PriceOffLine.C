#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | PriceOffLine.C                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia los mensajes de transacciones de autorizacion local  |
|                 | provenientes del TPV al centro autorizador                 | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h" 
#include "Log.h"
#include "Table.h"
extern "C"
{
      #include "DBTrxOff.h"
}

extern Config       Cfg;			/* Objeto de configuracion      */
extern Log          LogAlarm;		/* Logging de actividades       */
extern TrxTable		DBTrx;			/* Para tabla de transacciones  */
extern Queue		SQueue;			/* Cola de Sockets              */
extern Log			LogErrTerminal;	/* Logging errores terminales   */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceOffLine::PriceOffLine                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
PriceOffLine::PriceOffLine(int msg_pid, char *msg_data, int data_len)
{
	str_price_msg   price_msg_str;

	event_type=EVENT_PRICEOFFLINE;

	orig_pid=msg_pid;

	memcpy((char *)&price_msg_str, msg_data, data_len);

	mensaje_price=new Price_Msg(price_msg_str);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceOffLine::~PriceOffLine                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
PriceOffLine::~PriceOffLine()
{
    delete mensaje_price;
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceOffLine::ProcessIt                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PriceOffLine::ProcessIt()
{
    str_sel_cadata   term_data;
    TrxData          trx_data;
    Price_Msg        * priceRta;
    char             msg_str[2000];
    char             aux_str[20];
    int              cont;
    int              len;
    int              ret;
	int				 iCodTrx;
	str_upd_rev		 StrDataRev;
	char			 szRowId[64];
	char			 szCpoTerVer[25+1];
	char			 szProtocolo[2];
	char			 szRta[3];
	
    /* Invierte la fecha de vencimiento siempre */
    mensaje_price->InvertirFecha();

    /* Registra datos de la terminal provenientes del mensaje */
    strcpy(term_data.plan_sf, mensaje_price->ObtPlanSf());
    strcpy(term_data.cod_tar, mensaje_price->GetCodTar());
    strcpy(term_data.nro_suc, mensaje_price->GetEmpresa());
    strcpy(term_data.nro_caj, mensaje_price->GetTerminal());

    /* Loguea datos de la terminal */
	LogAlarm.Put(0, "PriceOffLine: Emp:[%s] Suc:[%s] Ter:[%s] Trx:[%s] Tar:[%s] Plan:[%s]\n",
											mensaje_price->GetEmpresa()     ,
											mensaje_price->GetSucursal()    ,
											mensaje_price->GetTerminal()    ,
											mensaje_price->GetTransaccion() ,
						                    mensaje_price->GetCodTar()      ,
											mensaje_price->ObtPlanSf());

    /* Busca la terminal en BD */
	ret=getDataByNT2(&term_data);
	if ( (ret!=OK) || (term_data.caj_bloq==1) )
    {
		/* Convierte el mensaje a formato ISO */
		mensaje_iso=new Iso_Msg();
		*mensaje_iso=*mensaje_price;

		/* Loguea el error de terminal */
		LogErrTerminal.Put(0, "Fecha Hora %s", currentTimeLog() ); 
		LogErrTerminal.Put(0, "Caja: %s - Sucursal: %s - Tarjeta: %s - Plan: %s - Evento: PriceOffLine\n\n", 
									mensaje_price->GetTerminal(),
									mensaje_price->GetField(15),
									mensaje_price->GetCodTar(),
									mensaje_price->ObtPlanSf());
		
		if (term_data.caj_bloq==1)
		{
			LogAlarm.Put (0, "PriceOn: ERROR Caja bloqueada por cierre anterior fallido\n");
			sprintf(szRta, "08");
		}
		else
		{
			LogAlarm.Put(0, "PriceOffLine: ERROR (%d) al obtener datos de la terminal\n", ret/*DBTerm.GetErrno()*/);
			sprintf(szRta, "00");
		}

		/* Crea mensaje de respuesta */
		priceRta=new Price_Msg(TYPE_PRICE_VENTA_LOCAL_RTA);
		
		/* Completa los campos del mensaje */
		priceRta->PutField(6, szRta);
		priceRta->PutField(7, "      ");    /* 12 digitos a 6 (!) */
		strcpy(aux_str, mensaje_iso->GetField(2));
		while (strlen(aux_str)<21)
		{
			strcat(aux_str, " ");
		}
		priceRta->PutField( 8, aux_str+2);
		priceRta->PutField( 1, mensaje_price->GetField(1));
		priceRta->PutField( 3, mensaje_price->GetField(16));
		priceRta->PutField( 4, mensaje_price->GetField(11));
		priceRta->PutField( 9, mensaje_price->GetField(5));
		priceRta->PutField(10, mensaje_iso->GetField(42));
		priceRta->PutField(13, mensaje_price->GetField(23));

		/* Aplana el mensaje */
		len=priceRta->GetMsgString(msg_str);

		/* Borra el mensaje */
		delete priceRta;
		
		/* Envia la respuesta de rechazo */
		ret=SQueue.SendMsg(orig_pid, msg_str, len);
		if (ret!=OK)
		{
			LogAlarm.Put(0, "PriceOffLine: ERROR (%d) al enviar respuesta de error en terminal\n", SQueue.GetErrno());
			return NOOK;
		}
		else
		{
			LogAlarm.Put(0, "PriceOffLine: RESPUESTA LOCAL: %s\n", szRta );
			return OK;
		}
	}

	/* Setea el centro autorizador */
    mensaje_price->SetNroCAPrice(term_data.nro_ca);

    /* Convierte el mensaje a formato ISO */
    mensaje_iso=new Iso_Msg();
	mensaje_iso->SetNroCA8583(term_data.nro_ca);
    *mensaje_iso=*mensaje_price;

    /* Completa campos en el mensaje ISO */
	mensaje_iso->PutField(24, term_data.cod_red);
    mensaje_iso->PutField(41, term_data.nro_caj_ca);
	sprintf(aux_str,"%-15s",term_data.nro_com);
    mensaje_iso->PutField(42, aux_str);
   
	/* Completa estructura para almacenar en la base */
	trx_data.NroLot(term_data.nro_lot);
    trx_data.NroCajCa(term_data.nro_caj_ca);
    trx_data.NroCom(term_data.nro_com); 
    trx_data.NroCA(term_data.nro_ca);
    trx_data.NroCuo(mensaje_price->ObtCuotas());
    trx_data.PlanISO(term_data.plan_iso);
    trx_data.PlanSF(mensaje_price->ObtPlanSf());
    trx_data.CodMon(term_data.cod_mon);

	/* Completa estructura para almacenar en la la base */ 
    mensaje_price->GetDBData(trx_data);
    mensaje_iso->GetDBData(trx_data);
    trx_data.NroTrc("-1");
	trx_data.NroRef("");

    /* Soporte de tarjetas Visa con fecha de vencimiento en blanco */
	char fec_ven[5];
    char tip_tar[3]; 
    strcpy(tip_tar, mensaje_price->GetField(17));
    strcpy(fec_ven, mensaje_price->GetField(8));

    /* Si la tarjeta es Visa y la fecha de vencimiento esta en blanco se almacena en estado 39 */
	if ((fec_ven[0]=='\0') && (!strcmp(tip_tar,"02")))
    {
		LogAlarm.Put(0, "PriceOffLine: Transaccion pasa a estado 39. Ven:[%s] Tar:[%s]\n", fec_ven, tip_tar);
		sprintf(aux_str, "39");
    }
    else
    {
		sprintf(aux_str,"00");
    }

	/* Completa el codigo de estado */ 
    trx_data.CodEst(aux_str);

    /* Completa el codigo de autorizacion */
	char CodAutCero[COD_AUT_LEN+1]; 
	char CodAutOrig[COD_AUT_LEN+1];
    memset(CodAutCero, '0', sizeof(CodAutCero));
    strcpy(CodAutOrig, mensaje_price->GetField(18));
    if (strncmp(CodAutOrig, CodAutCero, COD_AUT_LEN)==0)
    {			
		strcpy(aux_str, getCodAutLocal(tip_tar));
    }
    else
    {
		strcpy(aux_str, mensaje_price->GetField(18));
    }
	/* Si no es Amex formatea con ceros a izquierda */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		break;
	default:
		sprintf(CodAutOrig, "%06ld", atol(aux_str));
		sprintf(aux_str, CodAutOrig);
		break;
	}
	trx_data.CodAut(aux_str);
	LogAlarm.Put(0, "PriceOffLine: Autorizacion [%s]\n", aux_str);

    /* Carga la cantidad de reintentos */
	trx_data.CanRet("0000");

	/* Carga el modo de autorizacion */
    trx_data.ModAut(mensaje_price->GetField(19));

    /* Carga la fecha de la transaccion */
    sprintf(aux_str,"%04s%4.4s", trx_data.AnoTrx(), trx_data.FecLoc());
    trx_data.FecTrx(aux_str);

	/* Pago de resumen */
	iCodTrx = atoi(mensaje_price->GetField(5));
	switch (iCodTrx)
	{
	case T_PAGO:
		sprintf(aux_str, "%02d", T_PAGO);
		trx_data.CodTrx(aux_str);
		sprintf(aux_str, "%06d", OPCODE_VISA_PAGO_RESUMEN);
		trx_data.CodPro(aux_str);
		break;
	case T_DEVP:
		sprintf(aux_str, "%02d", T_DEVP);
		trx_data.CodTrx(aux_str);
		sprintf(aux_str, "%06d", OPCODE_VISA_DEVO_RESUMEN);
		trx_data.CodPro(aux_str);
		break;
	}

	/* Retiro cashback */
	trx_data.Retiro("0");

	/* Devolucion offline */
	if (iCodTrx == T_DEVO)
	{
		memset(&StrDataRev, 0, sizeof(StrDataRev));
		trx_data.NroCajOri(mensaje_price->GetField(32));
		trx_data.NroTicOri(mensaje_price->GetField(33));
		strcpy(StrDataRev.nro_tar   , trx_data.NroTar()    );
		strcpy(StrDataRev.nro_suc   , trx_data.NroSuc()    );
		strcpy(StrDataRev.nro_caj_sf, trx_data.NroCajSf()  );
		strcpy(StrDataRev.nro_caj   , trx_data.NroCajOri() );
		strcpy(StrDataRev.nro_trx   , trx_data.NroTicOri() );
		strcpy(StrDataRev.monto     , trx_data.Monto()     );
		strcpy(StrDataRev.retiro    , trx_data.Retiro()     );
		DBTrx.SelTrxOrig(&StrDataRev);
		trx_data.FecOri(StrDataRev.fec_ori);
		LogAlarm.Put(5, "PriceOffLine: T_DEV %s %s %s %s %s %s %s %s\n",
							StrDataRev.nro_tar   , StrDataRev.nro_suc,   
							StrDataRev.nro_caj_sf, StrDataRev.nro_caj,   
							StrDataRev.nro_trx   , StrDataRev.monto  ,  
							StrDataRev.retiro    , StrDataRev.fec_ori );
	}

	/* Inserta la transaccion en la base */
    ret=DBTrx.Insert(trx_data.GetInsVenData(), szRowId);
	if (ret==NOOK)
	{
		/* Si ya se encuentra en la base */
		if (DBTrx.GetErrno()==1 || DBTrx.GetErrno()==-1)
		{
			/* Se actualiza la transaccion si ya se encuentra */
			LogAlarm.Put(0, "PriceOffLine: ERROR. Transaccion duplicada. Se actualiza estado\n");
			ret=DBTrx.UpdOffline(trx_data.GetDataRev());
			if (ret==OK)
			{
				LogAlarm.Put(0, "PriceOffLine: Cambia estado de no enviar a offline\n");
			}
		}
		else
		{
			LogAlarm.Put(0, "PriceOffLine: ERROR (%d) al insertar transaccion\n", DBTrx.GetErrno());
			return NOOK;
		}
	}
	else
	{
		LogAlarm.Put(5, "PriceOffLine: Se inserta transaccion en la base\n");
	}		

	/* Crea el mensaje de respuesta offline */
    priceRta=new Price_Msg(TYPE_PRICE_VENTA_LOCAL_RTA);
    priceRta->PutField(6, "00"); 

    /* Completa los campos del mensaje de respuesta */
	sprintf(aux_str, "%-03s   ", trx_data.NroLot());
	priceRta->PutField(7, aux_str);

    strcpy(aux_str, mensaje_iso->GetField(2));
    while (strlen(aux_str)<21)
	{
		strcat(aux_str, " ");
	}
    priceRta->PutField( 8, aux_str+2);
    priceRta->PutField( 1, mensaje_price->GetField(1)); 
    priceRta->PutField( 3, mensaje_price->GetField(16));
    priceRta->PutField( 4, mensaje_price->GetField(11));
    priceRta->PutField( 9, mensaje_price->GetField(5));
    priceRta->PutField(10, mensaje_iso->GetField(42));
    priceRta->PutField(13, mensaje_price->GetField(23));

	/* Arma el campo de protocolo a devolver */
	switch (DBTipoProtocoloISO(trx_data.NroCA()))
	{
	case PROT_ISO_AMEX:
		sprintf(szProtocolo, "A");
		break;
	default:
		sprintf(szProtocolo, "B");
		break;
	}

	/* Devuelve nro de terminal y version */
	sprintf(szCpoTerVer, FTO_VER, mensaje_iso->GetField(41), szProtocolo, trx_data.PlanISO(), 
								  NO_ANUL, VERSION_NRO, trx_data.NroTicOri(), trx_data.FecOri() );
	priceRta->PutField(11, szCpoTerVer);

    /* Aplana el mensaje */
	len=priceRta->GetMsgString(msg_str);

	/* Borra el mensaje price */
    delete priceRta;

    /* Envia el mensaje */
	LogAlarm.Put(0, "PriceOffLine: Envia a TPV. CAut [%s]\n", term_data.nro_ca);
	ret=SQueue.SendMsg(orig_pid, msg_str, len);
    if (ret!=OK)
    {
        LogAlarm.Put(0, "PriceOffLine: ERROR (%d) al enviar respuesta\n", SQueue.GetErrno());
        return NOOK;
    }
	else
	{
		LogAlarm.Put(0, "PriceOffLine: RESPUESTA LOCAL: 00\n" );
	    return OK;
	}
}
