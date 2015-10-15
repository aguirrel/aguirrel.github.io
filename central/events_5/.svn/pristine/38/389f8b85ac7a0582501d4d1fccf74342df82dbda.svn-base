#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | PriceOnLineRta.C                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento de respuesta de compra al mensaje online            |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

extern Config		Cfg;			/* Objeto de configuracion      */
extern Log			LogAlarm;		/* Logging de actividades       */ 
extern TimeOutQueue	timeout_queue;	/* Cola de timeouts             */
extern TrxHeap		Heap;			/* Objeto Heap                  */  
extern TrxTable		DBTrx;			/* Para tabla de transacciones  */
extern Queue		SQueue;			/* Cola de Sockets              */
extern Pos			P;				/* Objeto Pos                   */

extern "C"
{
      #include "DBTrxOff.h"
}

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceOnLineRta::PriceOnLineRta                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
PriceOnLineRta::PriceOnLineRta(int msg_pid, char *msg_data, int data_len)
{
	int		iType;
	char	szCodiProc[7];

	event_type=EVENT_PRICEONLINE_RTA;
	mensaje_iso=new Iso_Msg(msg_data, data_len);   

    memset(szCodiProc, 0, sizeof(szCodiProc));
	memcpy(szCodiProc, mensaje_iso->GetField(3), 6);
	LogAlarm.Put(0, "PriceOnRta: Codigo de proceso:%s\n", szCodiProc);
	
	if (   ( atol(szCodiProc) == OPCODE_CASHB_POS_ANU_CA_P)
		|| ( atol(szCodiProc) == OPCODE_CASHB_POS_ANU_CC_P)
		|| ( atol(szCodiProc) == OPCODE_CASHB_POS_ANU_CA_D)
		|| ( atol(szCodiProc) == OPCODE_CASHB_POS_ANU_CC_D)  
		|| ( atol(szCodiProc) == OPCODE_CASHB_VISA_ANULA_P)  )
	{
		iType = TYPE_VISA_VENTA_RTA;
		mensaje_iso->SetMsgType(iType);
	}
	else
	{
		if (szCodiProc[1]=='2')
		{
			szCodiProc[1] = 0;
			switch (atol(szCodiProc))
			{
			case OPCODE_VISA_VENTA: 
				iType=TYPE_VISA_VENTA_RTA;
				break;
			case OPCODE_VISA_DEVOL: 
				iType=TYPE_VISA_DEVOL_RTA;
				break; 
			case OPCODE_VISA_ANUL_VENTA: 
				iType=TYPE_VISA_ANUL_VENTA_RTA;
				break;
			case OPCODE_VISA_ANUL_DEVOL:
				iType=TYPE_VISA_ANUL_DEVOL_RTA;
				break;
			}
			LogAlarm.Put(0, "PriceOnRta: Tipo de mensaje:%d\n", iType);
			mensaje_iso->SetMsgType(iType);
		}
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceOnLineRta::~PriceOnLineRta                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
PriceOnLineRta::~PriceOnLineRta()
{
	delete mensaje_price;
	delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceOnLineRta::ProcessIt                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PriceOnLineRta::ProcessIt()
{
	int				ret;
	int				len;
	char			msg_str[2000];
	char			aux_str[1024];
	str_heap_data	heap_data;
	str_sel_ntdata	term_data;
	char			aux_monto[13];
	TrxData			trx_data;
	char			idHeap[20];
	char			szRtaCA[4];
	int				iCodProc;
	int				iRechazoTCI;
	char			est_dev[3];
	char			szCpoTerVer[25+1];
	char			szProtocolo[2];	
	char			szFechaCompra[16];
	char			szVisor[64];
		
	/* Loguea la respuesta del centro */
	memset(aux_str, 0, sizeof(aux_str));
	memcpy(aux_str, mensaje_iso->GetField(39), 2);
	sprintf(szRtaCA, "%03s", aux_str);
	LogAlarm.Put(0, "PriceOnRta: RESPUESTA ISO: %s\n", aux_str);

	/* Obtiene datos de la terminal */
	strcpy(term_data.nro_caj_ca, mensaje_iso->GetField(41));
	strcpy(term_data.nro_com   , mensaje_iso->GetField(42));
	LogAlarm.Put(0, "PriceOnRta: CajCa:[%s] Com:[%s]\n", term_data.nro_caj_ca, term_data.nro_com);
	ret=getDataByCA(&term_data);
	if(ret!=OK)
	{
		LogAlarm.Put(0, "PriceOnRta: ERROR (%d) al obtener datos de la terminal\n", ret);
		return NOOK;
	}

	/* Convierte el mensaje iso */
	mensaje_price=new Price_Msg;
	mensaje_iso->SetNroCA8583(term_data.nro_ca);
	mensaje_price->SetNroCAPrice(term_data.nro_ca);
	*mensaje_price=*mensaje_iso;

	/* Modificacion PINPAD */
	/* Agrega la Working key si corresponde */
	char szPinPad[64];
	memset(szPinPad, 0, sizeof(szPinPad));
	if (mensaje_iso->ChkField(52))
	{
		LogAlarm.Put(0, "PriceOnRta: Tarjeta con PinPad\n");
		memcpy(szPinPad, mensaje_iso->GetField(52), PINPAD_LEN);
	}
	else
	{
		memcpy(szPinPad, PINPAD_NULO, PINPAD_LEN);
	}
	/* Fin Modificacion PINPAD */

	/* Agrega el numero de cuenta */
	memset(aux_str, ' ', sizeof(aux_str));
	if (mensaje_iso->ChkField(34))
	{
		memcpy(aux_str, mensaje_iso->GetField(34), sizeof(aux_str));
	} 
	aux_str[LON_CAMPO_34+2] = 0;
	mensaje_price->PutField(17, aux_str+2);
		
	/* Arma la clave del heap */
	sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", 
				term_data.nro_ca, term_data.nro_suc, term_data.nro_caj, mensaje_iso->GetField(11));

	/* Extrae los datos del heap */
	ret=Heap.GetData(idHeap,(char *)&heap_data, sizeof(heap_data));
	if(ret==NOOK)
	{
		LogAlarm.Put(0, "PriceOnRta: ERROR (%d) al extraer de Heap. Clave:[%s]\n", Heap.GetErrno(), idHeap);
		return NOOK;
	}

	/* Extrae los datos de la TOQ */
	ret=timeout_queue.RemTimeOut(idHeap);
	if(ret==NOOK)
	{
		LogAlarm.Put(0,"PriceOnRta: ERROR [%d] al extraer de TOQ. Continua procesando\n", timeout_queue.GetErrno());
	}

	/* Agrega el mensaje de visor */
	memset(szVisor, 0, sizeof(szVisor));
	if (mensaje_iso->ChkField(63))
	{		
		memset(aux_str, 0, sizeof(aux_str));
		memcpy(aux_str, mensaje_iso->GetField(63), sizeof(aux_str)-1);
		len= 3;	while (aux_str[len] == ' ') {len++;}
		memcpy(szVisor, &aux_str[len], sizeof(szVisor)-1);
		if ( ( szVisor[0] == '.' ) || ( szVisor[0] == ',' ) )
		{
			szVisor[0] = '.';
			strtok(szVisor, "|^");
		}
	}
	if (strlen(szVisor)==0)
	{
		getDetallePlan(heap_data.cod_tar, heap_data.plan_sf, szVisor);
	}
	len = strlen(szVisor);
	if ( len < LON_CAMPO_63 )
		memset(&szVisor[len], ' ', LON_CAMPO_63-len);		
	szVisor[LON_CAMPO_63] = 0;
	mensaje_price->PutField(18, szVisor);
	LogAlarm.Put(0, "PriceOnRta: MsjeVisor [%s]\n", mensaje_price->GetField(18));

	/* Inserta el numero de lote/cupon en los datos adicionales del mensaje de respuesta price */
	char szCampo13[LON_CAMPO_13 + 1];
	sprintf(szCampo13, "%03d%s", atoi(term_data.nro_lot)%1000, FLAG_ONLINE);
	strncat(szCampo13, szPinPad, PINPAD_LEN);
	strncat(szCampo13, szRtaCA, RTA_CA_LEN);
	strncat(szCampo13, heap_data.plan_sf, PLAN_SF_LEN);
	strcat(szCampo13, " ");
	mensaje_price->PutField(13, szCampo13);

	/* Completa mensaje price con los datos del heap */
	mensaje_price->PutHeapData(heap_data);

	/* Coloca el codigo de tarjeta */
	memset(aux_str, 0, sizeof(aux_str));
	memcpy(aux_str, heap_data.cod_tar, 2);
	mensaje_price->PutField(15,aux_str);

	/* Actualiza el monto de la transaccion, de acuerdo a si es Unipago o no */
	sprintf(aux_monto,"%012d",0);
	mensaje_price->PutField(16,aux_monto);

	/* Obtiene los datos de la transaccion */
	trx_data.CodTar(term_data.cod_tar);
	mensaje_iso->GetDBData(trx_data);
	mensaje_price->GetDBData(trx_data);
	
	/* Completa datos de la transaccion */
	trx_data.NroSuc(term_data.nro_suc);
	trx_data.NroCaj(term_data.nro_caj);
	trx_data.NroCajCa(term_data.nro_caj_ca);
	trx_data.FecRta(currentTimestamp());
	trx_data.NroCA(term_data.nro_ca);

	/* Loguea datos de la transaccion */
	LogAlarm.Put(0, "PriceOnRta: Emp:[%s] Suc:[%s] Ter:[%s] Trx:[%s] CAut:[%s]\n", 
						term_data.nro_suc, term_data.nro_caj_sf, term_data.nro_caj, 
						mensaje_price->GetTransaccion(), term_data.nro_ca);

	/* Actualiza el codigo de estado */
	if(mensaje_iso->Aprob()==OK)
	{
		sprintf(aux_str, "%02d", E_APRO);
		sprintf(est_dev, "%02d", E_DEAN);
		iRechazoTCI = 0;
	}
	else
	{
		sprintf(aux_str, "%02d", E_RECH);
		sprintf(est_dev, "%02d", E_RECH);
		iRechazoTCI = 1;
	}
	trx_data.CodEst(aux_str);

	/* Actualiza devolucion si es una respuesta de anulacion */
	if (heap_data.anul_onl[0] == '1')
	{
		LogAlarm.Put(0, "PriceOnRta: Respuesta de anulacion. RowId:[%s] Estado:[%s]\n", heap_data.rowid, est_dev);
		if (BDUpdateEstado(heap_data.rowid, est_dev))
		{
			LogAlarm.Put(0, "PriceOnRta: ERROR (%d) al actualizar BD\n", DBTrx.GetErrno());
			return NOOK;
		}
	}

	/* Actualiza transaccion en base de datos */
	memset(szFechaCompra, 0, sizeof(szFechaCompra));
	ret=DBTrx.UpdateRtaVen(trx_data.GetUpdData(), heap_data.anul_onl[0], szFechaCompra);
	if(ret==NOOK)
	{
		LogAlarm.Put(0, "PriceOnRta: ERROR (%d) al actualizar BD\n", DBTrx.GetErrno());
		return NOOK;
	}
	
	/* Si es anulacion, actualiza fecha de compra original para homologacion Amex */
	if ( (heap_data.anul_onl[0] == '1') && (strlen(szFechaCompra)>0) )
	{
		LogAlarm.Put(0, "PriceOnRta: Fecha original de compra:[%s]\n", szFechaCompra);
		mensaje_price->PutField(16, szFechaCompra);
	}

	/* Libera la caja si pudo actualizar la transaccion correctamente        */
	/* Si la actualizacion fallo pudo significar que entro primero un evento */
	/* PriceRev de la caja y el evento PriceOnRta llego tardio con lo que no */ 
	/* se debe liberar                                                       */
	ret = P.SetPosFree(term_data.nro_ca, term_data.nro_suc, term_data.nro_caj);
	if(ret != OK)
	{
		LogAlarm.Put(0, "PriceOnRta: ERROR en SetPosFree. CA:[%s] Emp:[%s] Caj:[%s]\n", 
						term_data.nro_ca, term_data.nro_suc, term_data.nro_caj);
	}

	/* Pago de resumen */
	iCodProc = atoi(mensaje_iso->GetField(3));
	switch (iCodProc)
	{
	case OPCODE_VISA_PAGO_RESUMEN:
		sprintf(aux_str, "%02d", T_PAGO);
		mensaje_price->PutField(8,aux_str);
		if (iRechazoTCI==1)
		{
			sprintf(aux_str, "11");
			mensaje_price->PutField(6, aux_str);
		}
		break;
	case OPCODE_VISA_DEVO_RESUMEN:
		sprintf(aux_str, "%02d", T_DEVP);
		mensaje_price->PutField(8,aux_str);
		break;
	}

	/* Numero de comercio */
	mensaje_price->PutField(11, heap_data.nro_com);

        /* EMV */
        if(strlen(mensaje_iso->GetField(55)))
        {
            LogAlarm.Put(0, "* EMV * CRIPTOGRAMA EN RESPUESTA\n");
            mensaje_price->PutField(19, &((mensaje_iso->GetField(55))[3]));
        }

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
	sprintf(szCpoTerVer, FTO_VER, term_data.nro_caj_ca, szProtocolo, heap_data.plan_iso, 
								  heap_data.anul_onl[0], VERSION_NRO, heap_data.trans_ori, heap_data.fecha_ori );
	mensaje_price->PutField(12, szCpoTerVer);

	/* Aplana el mensaje de respuesta */
	len=mensaje_price->GetMsgString(msg_str);
	
	/* Envia la respuesta a la caja */
	ret=SQueue.SendMsg(heap_data.orig_pid, msg_str, len);
	if(ret!=OK)
	{
		LogAlarm.Put(0, "PriceOnRta: ERROR (%d) al enviar respuesta\n", SQueue.GetErrno());
		return NOOK;
	}
	else
	{
		LogAlarm.Put(0, "PriceOnRta: Respuesta enviada OK. Cod.TPV:[%s]\n", mensaje_price->GetField(6));
		return OK;
	}
}
