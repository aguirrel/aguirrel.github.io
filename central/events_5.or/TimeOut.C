#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TimeOut.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento de timeout al mensaje de peticion online            |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

extern "C"
{
      #include "DBTrxOff.h"
}

extern CACStatus	CACS;			/* Status Canal X.25            */
extern Config       Cfg;			/* Objeto de configuracion      */
extern Log          LogAlarm;		/* Logging de actividades       */
extern TrxHeap		Heap;			/* Objeto Heap                  */
extern TrxTable		DBTrx;			/* Para tabla de transacciones  */
extern Queue		SQueue;			/* Cola de Envia a Trx          */
extern Queue        XQueue;			/* Cola de X.25                 */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOut::TimeOut                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TimeOut::TimeOut(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_TIMEOUT;
	
    if (data_len<=sizeof(data))
		memcpy((void *)&data, msg_data, data_len);
	
    return;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOut::ProcessIt                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOut::ProcessIt()
{
    str_heap_data	heap_data;
    TrxData			trx_data;
    char			msg_str[200];
    int				ret, len;
    long			limite;
    Price_Msg		*priceRta;
    char			auxCa[3];	
	char			aux_str1[100];	
	char			szRtaCA[4];
	str_get_total	opers_data;
	str_get_limit	limit_data;
	char			szRowId[64];
	char			szCpoTerVer[25+1];
	char			szProtocolo[2];	

    LogAlarm.Put(0, "TimeOut: TO esperando respuesta al mensaje. Key: [%s]...\n", data.key);
	
	/* Baja el estado del centro si no hay canales disponibles */
	sprintf(auxCa,"%2.2s",data.key);
    ret = XQueue.StatusCA(atoi(auxCa));
	if (ret<=0)
	{
		LogAlarm.Put(0, "TimeOut: Ret:[%d]. Baja estado CAut:[%s]\n", ret, auxCa);
		ret=CACS.Down(atoi(auxCa));
	    if (ret==NOOK)
		{
			LogAlarm.Put(0, "TimeOut: ERROR:[%d] al cambiar estado CAut:[%s]\n", CACS.GetErrno(), auxCa);
		}
	}
	else
	{
		LogAlarm.Put(0, "TimeOut: Canales disponibles:[%d]. No baja estado CAut:[%s]\n", ret, auxCa);
	}

    trx_data.NroCA(auxCa);
	
	/* Extrae los datos del heap */
    ret=Heap.GetData(data.key, (char *)&heap_data, sizeof(heap_data));
	if (ret==NOOK)
	{
        LogAlarm.Put(0, "TimeOut: ERROR (%d) al extraer de heap. Clave:[%s]\n",	Heap.GetErrno(), data.key);
		return NOOK;
	}
	LogAlarm.Put(0, "TimeOut: Dato extraido del heap. Clave:[%s] anul_onl[%c]\n", data.key, heap_data.anul_onl[0]);
	
    /* Coloca el numero de trace */
	sprintf(aux_str1, "%8.8s", data.key + 10);
    trx_data.NroTrc(aux_str1);
    
	/* Numero de referencia */
	trx_data.NroRef("");

	/* Coloca el codigo de estado */
	sprintf(aux_str1, "%02d", E_TOUT); /* 06 */
	trx_data.CodEst(aux_str1);
    
	/* Completa los campos de la transaccion */
	trx_data.NroCaj(heap_data.nro_term);
    trx_data.CodTar(heap_data.cod_tar);
    trx_data.NroCajCa(heap_data.nro_caj_ca);

    /*Empr03*/
    memset(aux_str1 , 0 , sizeof(aux_str1));
    memcpy(aux_str1 , data.key + 2 , 3);
    trx_data.NroSuc(aux_str1);
	
    LogAlarm.Put(0, "TimeOut: Actualiza BD. Ca:[%s] Emp:[%s] CajCa:[%s] Trc:[%s]\n",
						trx_data.NroCA(), trx_data.NroSuc(), trx_data.NroCajCa(), trx_data.NroTrc());
	
    /* Actualiza la transaccion */
	ret=DBTrx.UpdateRtaTo(trx_data.GetUpdDataTO());
    if (ret==NOOK)
    {
		LogAlarm.Put(0, "TimeOut: ERROR (%d) al actualizar transaccion\n", DBTrx.GetErrno());
		return NOOK;
    }
	
	/* Obtiene datos de la transaccion */
    ret=get_trx_data(trx_data.GetInsVenData());
    if (ret!=OK)
    {
		LogAlarm.Put(0, "TimeOut: ERROR (%d) al obtener datos de la transaccion de la base\n", ret);
        return NOOK;
    }
    trx_data.PutTrxData();   
	
	/* Loguea datos de la transaccion */
	LogAlarm.Put(0, "TimeOut: Emp:[%s] Suc:[%s] Ter:[%s] Trx:[%s]\n", 
					trx_data.NroSuc(), trx_data.NroCajSf(), trx_data.NroCaj(), trx_data.NroTrx());

	/* Verifica si la transaccion ya fue actualizada mediante reversos */
    ret=is_updated(trx_data.GetInsVenData());
    if (ret > 0 )
    {
        LogAlarm.Put(0, "TimeOut: Transaccion ya resuelta por TPV\n");
        return NOOK;
    }
    else
    {
        LogAlarm.Put(5, "TimeOut: Transaccion a resolverse localmente\n");
    }
	
	if (atoi(trx_data.CodTrx())!=T_PAGO)
	{
		if (heap_data.anul_onl[0] == '1')
		{
			sprintf(aux_str1, "%02d", E_ANUL);
			LogAlarm.Put(0, "TimeOut: Anulacion offline no permitida\n");
			ret = BDUpdateEstado(heap_data.rowid, aux_str1);
			LogAlarm.Put(0, "TimeOut: Rowid devolucion [%s] Ret [%d]\n", heap_data.rowid, ret);
		}
		else
		{
			/* Carga datos para consultar los totales de la tarjeta */
			sprintf(opers_data.nro_tar, "%-*s", sizeof(opers_data.nro_tar)-1, trx_data.NroTar());
			sprintf(opers_data.cod_est, "%0*d", sizeof(opers_data.cod_est)-1, E_PEND); /* 00 */
			strcpy(opers_data.cod_tar,trx_data.CodTar());
			strcpy(opers_data.plan_sf, trx_data.PlanSF()); 
			strcpy(opers_data.cod_trx, trx_data.CodTrx());

			/* Carga datos para obtener los limites de respaldo del tipo de tarjeta y plan */
			strcpy(limit_data.cod_tar, trx_data.CodTar()); 
			strcpy(limit_data.plan_sf, trx_data.PlanSF());

			/* Obtiene los limites de respaldo del tipo de tarjeta */
			ret=getLimit(&limit_data);
			if (ret!=OK)
			{
				LogAlarm.Put(0, "TimeOut: ERROR (%d) al obtener el limite de la tarjeta\n", ret);
				return NOOK;
			}
			LogAlarm.Put(0, "TimeOut: Limites de respaldo. General:%.2f - Cuotas:%.2f - Plan %s:%.2f\n", 
						(double)limit_data.limite/100.0 , (double)limit_data.limcuot/100.0 ,
						limit_data.plan_sf, (double)limit_data.limplan/100.0 );

			/* Obtiene el total comprado con la tarjeta */
			ret=getTotalGeneral(&opers_data);
			if (ret!=OK)
			{
				LogAlarm.Put(0, "TimeOut: ERROR (%d) al obtener el total de la tarjeta\n", ret);
				return NOOK;
			}
			LogAlarm.Put(0, "TimeOut: Total de la tarjeta: %.2f\n", (double)opers_data.monto/100);
		}

		/* Cambia el modo de la operacion a OFF-LINE */
		sprintf(aux_str1, "%02d", T_OFLI); 
		trx_data.ModEnv(aux_str1);
		
		/* Cambia el modo de ingreso */
		switch (DBTipoProtocoloISO(trx_data.NroCA()))
		{
		case PROT_ISO_AMEX:
			if (strcmp(trx_data.ModIng(), "072")==0)
			{
				trx_data.ModIng("012");
			}
			if (strcmp(trx_data.ModIng(), "062")==0)
			{
				trx_data.ModIng("022");
			}
			break;
		default:
			break;
		}

		/* Pone el numero de trace (NULL) */
		trx_data.NroTrc("-1");
		
		if (heap_data.anul_onl[0] == '1')
		{
			limite=NOOK;
			sprintf(aux_str1, "%02d", E_RECH);
			trx_data.CodEst(aux_str1);
		}
		else
		{
			/* Obtiene el monto de la transaccion en curso */
			long monto;
			monto=atol(trx_data.Monto());
			LogAlarm.Put(0, "TimeOut: Esta operacion: %.2f\n", (double)monto/100);
			LogAlarm.Put(0, "TimeOut: Total general de la tarjeta: %.2f\n", (double)opers_data.monto/100.0);
    
			/* Si las compras realizadas anteriormente mas la actual supera el limite */
			if ((opers_data.monto+monto)>limit_data.limite)
			{
				/* Se prepara el estado para registrar la operacion como rechazada */
				LogAlarm.Put(0, "TimeOut: Supera el limite\n");
				limite=NOOK;
				sprintf(aux_str1, "%02d", E_RECH);
				trx_data.CodEst(aux_str1);
			}
			else
			{
				/* Si el plan es en cuotas */
				if (strncmp(trx_data.PlanSF(), "00", 2) != 0)
				{
					/* Obtiene el total comprado en cuotas */
					ret= getTotalCuotas(&opers_data);
					if (ret!=OK)
					{
						LogAlarm.Put(0, "TimeOut: ERROR (%d) obtener el total de la tarjeta en cuotas\n",ret);
						return NOOK;
					}
					LogAlarm.Put(0, "TimeOut: Total de la tarjeta en cuotas: %.2f\n", (double)opers_data.monto/100.0);
					
					/* Si las compras realizadas anteriormente en cuotas mas la actual supera el limite */
					if ((opers_data.monto+monto)>limit_data.limcuot)
					{
						limite=NOOK;
					}
					else
					{
						/* Obtiene el total comprado en cuotas para ese plan */
						ret= getTotalPlan(&opers_data);
						if (ret!=OK)
						{
							LogAlarm.Put(0, "TimeOut: ERROR (%d) al obtener el total de tarjeta para el plan\n",ret);
							return NOOK;
						}
						LogAlarm.Put(0, "TimeOut: Total de la tarjeta para el plan: %.2f\n", (double)opers_data.monto/100.0);
						
						
						/* Si las compras realizadas anteriormente para el plan mas la actual supera el limite */
						if ((opers_data.monto+monto)>limit_data.limplan)
						{
							limite=NOOK;
						}
						else
						{
							limite=OK;
						}           
					}
				}
				else
				{
					limite=OK;
				}
			
				/* Si la tarjeta no tiene limite de respaldo para operrar offline */
				if (limite==NOOK)
				{	
					/* Se prepara el estado para registrar la operacion como rechazada */
					LogAlarm.Put(0, "TimeOut: Supera el limite...\n");
					limite=NOOK;
					sprintf(aux_str1, "%02d", E_RECH);
					trx_data.CodEst(aux_str1);
				}
				else
				{
					/* Si la tarjeta tiene respaldo para operar offline */
					
					LogAlarm.Put(0, "TimeOut: No supera el limite\n");
				
					/* El limite es OK */
					limite=OK;

					/* Prepara el codigo de estado */
					sprintf(aux_str1, "%02d", E_PEND); /* 00 */
					trx_data.CodEst(aux_str1);
				
					/* Recupera el codigo de autorizacion parametrizable y lo registra */
					sprintf(aux_str1, getCodAutLocal(trx_data.CodTar()));	
					trx_data.CodAut(aux_str1);
				
					/* Registra el modo de autorizacion */ 
					sprintf(aux_str1,"%02d",M_CRED);
					trx_data.ModAut(aux_str1);
					
					/* Registra la fecha de la transaccion */
					/*sprintf(aux_str1, "%04d%4.4s", ObtYear(), trx_data.FecLoc());
					trx_data.FecTrx(aux_str1);*/

					/* Almacena la transaccion en la base */
					ret=DBTrx.Insert(trx_data.GetInsVenData(), szRowId);
					if (ret==NOOK)
					{
						LogAlarm.Put(0, "TimeOut: ERROR (%d) al insertar transaccion aprobada\n", DBTrx.GetErrno());
						return NOOK;
					}
				}
			}
		}
	} /* end del if (atoi(trx_data.CodTrx())!=T_PAGO) */
	else	/* Pago de resumen */
	{
		/* No se permite pago de resumen offline en cuotas */
		if (strncmp(trx_data.PlanSF(), "00", 2) != 0)
		{
			LogAlarm.Put(0, "TimeOut: Pago de resumen offline en cuotas no permitido\n");
			sprintf(aux_str1, "%02d", E_RECH);
			trx_data.CodEst(aux_str1);
			limite=NOOK;
		}
		else
		{
			/* Cambia el modo de la operacion a OFF-LINE */
			sprintf(aux_str1, "%02d", T_OFLI); 
			trx_data.ModEnv(aux_str1);

			/* Cambia el modo de ingreso */
			switch (DBTipoProtocoloISO(trx_data.NroCA()))
			{
			case PROT_ISO_AMEX:
				if (strcmp(trx_data.ModIng(), "072")==0)
				{
					trx_data.ModIng("012");
				}
				if (strcmp(trx_data.ModIng(), "062")==0)
				{
					trx_data.ModIng("022");
				}
				break;
			default:
				break;
			}			

			/* Pone el numero de trace (NULL) */
			trx_data.NroTrc("-1");
			
			/* Prepara el codigo de estado */
			sprintf(aux_str1, "%02d", E_PEND); /* 00 */
			trx_data.CodEst(aux_str1);
		
			/* Recupera el codigo de autorizacion parametrizable y lo registra */
			sprintf(aux_str1, getCodAutLocal(trx_data.CodTar()));	
			trx_data.CodAut(aux_str1);
		
			/* Registra el modo de autorizacion */ 
			sprintf(aux_str1,"%02d",M_CRED);
			trx_data.ModAut(aux_str1);
			
			/* Registra la fecha de la transaccion */
			/*sprintf(aux_str1, "%04d%4.4s", ObtYear(), trx_data.FecLoc());
			trx_data.FecTrx(aux_str1);*/

			/* Almacena la transaccion en la base */
			ret=DBTrx.Insert(trx_data.GetInsVenData(), szRowId);
			if (ret==NOOK)
			{
				LogAlarm.Put(0, "TimeOut: ERROR (%d) al insertar transaccion aprobada\n", DBTrx.GetErrno());
				return NOOK;
			}                                                  

			/* Acepta siempre el pago offline */
			limite=OK;
		}
	}

    LogAlarm.Put(0, "TimeOut: Autoriza localmente\n");
	
	/* Crea mensaje price */
    Price_Msg *msg_rta=new Price_Msg(TYPE_PRICE_VENTA_RTA);
    msg_rta->PutHeapData(heap_data);
	
    /* Prepara codigo de accion a enviar al TPV */
    if (limite==OK)
    {
		msg_rta->PutField( 6, "00"); /* 00 = Operacion permitida */
		msg_rta->PutField( 9, getCodAutLocal(trx_data.CodTar()));
		LogAlarm.Put( 0, "TimeOut: RESPUESTA LOCAL: 00\n");
		sprintf(szRtaCA, "000");
    }
    else
    {
		if (atoi(trx_data.CodTrx())!=T_PAGO)
		{
			msg_rta->PutField( 6, "38"); /* 38 = Supera el limite */
			msg_rta->PutField( 9, "000000");
			LogAlarm.Put( 0, "TimeOut: RESPUESTA LOCAL: 38\n");
			sprintf(szRtaCA, "103");
		}
		else
		{
			msg_rta->PutField( 6, "11"); /* 11 = Pago no permitido p/resumen */
			msg_rta->PutField( 9, "000000");
			LogAlarm.Put( 0, "TimeOut: RESPUESTA LOCAL: 11\n");
			sprintf(szRtaCA, "103");
		}
    }
    
	/* Pone el numero de tarjeta en la respuesta */
	msg_rta->PutField( 7, trx_data.NroTar());
   	
	/* Inserta el numero de lote/cupon en los datos adicionales del mensaje de respuesta price */
    char szCampo13[LON_CAMPO_13 + 1];
    memset(szCampo13, 0, sizeof(szCampo13));
    sprintf(szCampo13, "%03d%s%s%s%s ", atoi(trx_data.NroLot()), FLAG_TIMEOUT, PINPAD_NULO, szRtaCA, heap_data.plan_sf);
    msg_rta->PutField(13, szCampo13);
	
	/* Agrega el numero de cuenta */
	memset(aux_str1, ' ', sizeof(aux_str1));
	aux_str1[LON_CAMPO_34] = 0;
	msg_rta->PutField(17, aux_str1);
	
	/* Agrega el mensaje de visor */
	memset(aux_str1, ' ', sizeof(aux_str1));
	aux_str1[LON_CAMPO_63] = 0;
	msg_rta->PutField(18, aux_str1);
		
	/* Pago de resumen */
	if (atoi(trx_data.CodTrx())==T_PAGO)
	{	
		/* Tipo de operacion mensaje price */
		sprintf(aux_str1, "%02d", T_PAGO);
		msg_rta->PutField(8, aux_str1);
	}

	/* Completa campos faltantes */
	msg_rta->PutField(11, trx_data.NroCom());
	msg_rta->PutField(15, trx_data.CodTar());
	msg_rta->PutField(16, "000000000000");

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
	sprintf(szCpoTerVer, FTO_VER, trx_data.NroCajCa(), szProtocolo, trx_data.PlanISO(), 
								  heap_data.anul_onl[0], VERSION_NRO, trx_data.NroTicOri(), trx_data.FecOri() );
	msg_rta->PutField(12, szCpoTerVer);

	/* Aplana el mensaje */
	len=msg_rta->GetMsgString(msg_str);
	
    /* Borra mensaje de respuesta */
	delete msg_rta;

    /* Envia respuesta al TCPDaemon */
	ret=SQueue.SendMsg(heap_data.orig_pid, msg_str, len);
    if (ret!=OK)
    {
		LogAlarm.Put(0, "TimeOut: ERROR (%d) al enviar respuesta\n", SQueue.GetErrno());
		return NOOK;
    }
	else
	{
		LogAlarm.Put(2, "TimeOut: Respuesta enviada OK\n");
	    return OK;
	}
}
