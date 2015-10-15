#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | LinkX25.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Fuentes para el manejo de la placa X25                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
#include "Includes.h"
#include "LinkX25.h"
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::initDone                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor por defecto                                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int X25::initDone=0;
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::X25                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor para el vinculo                                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
X25::X25(char *strLinkToUse)
{
#ifdef OS_AIX	
	strcpy(linkName, strLinkToUse);
	connId                = -1;
	codUserData[0]        = 0;
	codUserData[1]        = 0;
	x25LastError          = 0;
	sysLastError          = 0;
	x25LinkDefs.flags     = X25FLG_LINK_NAME;
	x25LinkDefs.link_name = linkName;

	/* Inicializa la API X25 si no lo esta */
	if (initDone != 128)
	{
		if (x25_init(&x25LinkDefs) == -1)
		{
			x25LastError = x25_errno;
			sysLastError = errno;
			linkName[0] = '\0';			
		}
		else
		{
			x25LastError = 0;
			sysLastError = 0;
			initDone=128;
		}	
	}
#endif
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::setCodUserData                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el codeUserData para cada C.A. que lo requiera       |
|                 | El array recibido se convierte a Hexa como long double     | 
+-----------------+------------------------------------------------------------+
*/
int X25::setCodUserData(char data[2][11])
{
#ifdef OS_AIX
    codUserData[0] = strtoul(data[0], '\0', 16);
    codUserData[1] = strtoul(data[1], '\0', 16);
#endif
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::Open                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Apertura de la conversacion sobre el canal.                |
|                 | Tareas: Obtiene el X25 counter para control de recepciones | 
|                 |         Realiza la llamada                                 |
|                 |         Espera la respuesta de confirmacion                |
+-----------------+------------------------------------------------------------+
*/
int X25::Open(char *strAdrsToCall, char *null1, char *null2)
{
#ifdef OS_AIX
	int rc = OK;                                    /* Codigos de retorno   */
	struct cb_dev_info_struct *x25DevAttr;          /* Info de dispositivo  */
	static char myX25Addrs[40];                     /* Mi direccion de x.25 */
	static char call_user_data[40];

	/* Verifica que exista el nombre de conexion */
	if (linkName[0]=='\0')
    {
		x25LastError = 401;
		sysLastError = 401;
		return -1;		
    }

	/* Obtiene un contador para controlar mensajes entrantes */
	ctrId = x25_ctr_get();
	if (ctrId == -1)
	{
		x25LastError = x25_errno; 
		sysLastError = errno;
		return -2;			
	}

	/* Obtiene informacion de NUA local X25 */
	x25LinkDefs.flags     = X25FLG_LINK_NAME;
	x25LinkDefs.link_name = linkName;
	x25DevAttr = x25_device_query(&x25LinkDefs);
	if (x25DevAttr == NULL)
	{
		x25LastError = x25_errno;
		sysLastError = errno;
		return -3;
	}
	strcpy(myX25Addrs, x25DevAttr->nua);
	if (x25DevAttr->nua != NULL)
		free(x25DevAttr->nua);
	if (x25DevAttr != NULL)
		free(x25DevAttr);

	/* Carga parametros de conexion */
	strcpy(CalledAddress, strAdrsToCall);
	x25CallDefs.flags        = X25FLG_LINK_NAME;      
	x25CallDefs.link_name    = linkName;
	x25CallDefs.flags       |= X25FLG_CALLING_ADDR;  
	x25CallDefs.calling_addr = myX25Addrs;
	x25CallDefs.flags       |= X25FLG_CALLED_ADDR;   
	x25CallDefs.called_addr  = CalledAddress;
	if (codUserData[0]!=0 || codUserData[1]!=0)
	{
		x25CallDefs.flags	|= X25FLG_USER_DATA;
		x25CallDefs.user_data_len=8;
		x25CallDefs.user_data = (unsigned char *) codUserData;
	}

	/* Hace la llamada X25 */
	connId = x25_call(&x25CallDefs, ctrId);
	if (connId == -1)
	{
		x25LastError = x25_errno;
		sysLastError = errno;
		return -4;
	}
	
	/* Espera la respuesta */
	x25CtrArray[0].flags     = X25FLG_CTR_ID;
	x25CtrArray[0].ctr_id    = ctrId;
	x25CtrArray[0].flags    |= X25FLG_CTR_VALUE;
	x25CtrArray[0].ctr_value = 0;
	if (x25_ctr_wait(1, x25CtrArray) == -1)
	{
		x25LastError = x25_errno;
		sysLastError = errno;
		return -5;
	}
	
	/* Recibe la respuesta de establecimiento SVC */
	if (x25_receive(&connId, &x25RcvMsg) == -1) 
	{
		x25LastError = x25_errno;
		sysLastError = errno;
		return -6;
	}

	/* Procesa la respuesta */
	switch(x25RcvMsg.msg_type)
	{
	case X25_CALL_CONNECTED:    // Confirmacion de conexion.    
		x25LastError =   0;
		sysLastError =   0;
		rc = OK;
		break;

	case X25_CLEAR_INDICATION:  // Clear Conexion 
		x25LastError = 402;
		sysLastError = 402;
		rc = -7;
		break;

	default:					// Error desconocido
		x25LastError = 403;		
		sysLastError = 403;
		rc = -8;
		break;
	}	
	
	if (x25RcvMsg.msg_point.cb_data->data != NULL)
		free(x25RcvMsg.msg_point.cb_data->data);
	if (x25RcvMsg.msg_point.cb_data != NULL)
		free(x25RcvMsg.msg_point.cb_data);
	
	/* Retorna el resultado */
	return rc;
#else
	return OK;
#endif
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::Write                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Escribe un mensaje sobre el X25.                           |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int X25::Write (char *data, int len)
{
#ifdef OS_AIX
	int iTot, iCont;

	/* Verifica si la conexion es invalida */
	if (connId == -1)
	{
		x25LastError = 404;
		sysLastError = 404;
		return -9;
	}

	/* Envia la peticion en bloques de 128 bytes */
	iTot = (len/128);
	for (iCont = 0 ; iCont < iTot ; iCont++)
	{
		if ( ((len%128) == 0) && ((iTot-iCont) == 1) )
		{
			x25Data.flags    = X25FLG_DATA;
		}
		else
		{
			x25Data.flags    = X25FLG_DATA | X25FLG_M_BIT;
		}
		x25Data.data_len = 128;
		x25Data.data     = (unsigned char*)(data+(128*iCont));
		
		if (x25_send(connId,&x25Data) == -1)
		{
			x25LastError = x25_errno;
			sysLastError = errno;
			return -10;
		}
	} 

	/* Envia el ultimo paquete */
	if ( (len%128) != 0)
	{
		x25Data.flags    = X25FLG_DATA;
		x25Data.data_len = len%128;
		x25Data.data     = (unsigned char*)(data+(128*iCont));
		
		if (x25_send(connId,&x25Data) == -1)
		{
			x25LastError = x25_errno;
			sysLastError = errno;
			return -11;
		}
	}

	/* Retorna el resultado */
	x25LastError = 0;
	sysLastError = 0;
	return OK;
#else	
	return OK;
#endif
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::MsgPnd                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Devuelve OK si hay mensajes nuevos                         |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int X25::MsgPnd()
{
#ifdef OS_AIX
    int		ret;
    ret = x25_ctr_test(x25CtrArray[0].ctr_id);
    return  (ret>=0 ? ret : NOOK);
#else
	return OK;
#endif
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::Read                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Lee un mensaje del X25                                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int X25::Read(char *data, int * msgLen, int *msgType)
{
#ifdef OS_AIX
	int        rc=OK;
	int		  iLongMsje;

	*msgLen = 0;
	
	/* Verifica si la conexion es invalida */
	if (connId == -1)
    {
		x25LastError = 405;
		sysLastError = 405;
		return -12;
    }

	/* Obtiene cuantos mensajes estan esperando ser recibidos */
	rc = x25_ctr_test(x25CtrArray[0].ctr_id);
	if (rc == -1)
	{
		x25LastError = x25_errno;
		sysLastError = errno;
		return -13;
	}

	/* Si no hay mensajes esperando */
	if (rc==0)
	{
		/* Hace un wait sobre el SVC ya que no hay mensajes esperando */
		rc = x25_ctr_wait(1,x25CtrArray);
		if (rc == -1)
		{
			x25LastError = x25_errno;
			sysLastError = errno;
			return -14;
		}
	}

	/* Recibe un mensaje */
	rc = x25_receive(&connId, &x25RcvMsg);
	if (rc == -1)
	{
		x25LastError = x25_errno;
		sysLastError = errno;
		return -15;
	}

	/* Procesa el mensaje */
	*msgType = x25RcvMsg.msg_type;
	switch(x25RcvMsg.msg_type)
	{
		case X25_DATA:
			rc = OK;
			x25LastError = 0;
			sysLastError = 0;
			if ( (x25RcvMsg.msg_point.cb_data->flags) & X25FLG_D_BIT )
			{
				if ( x25_ack(connId) == -1 )
				{
					x25LastError = x25_errno;
					sysLastError = errno;
				}
				rc = OK4;
			}
			*msgLen = x25RcvMsg.msg_point.cb_data->data_len;
			iLongMsje = (*msgLen>MAX_TAM_ISO) ? MAX_TAM_ISO : *msgLen;
			memcpy(data, x25RcvMsg.msg_point.cb_data->data, iLongMsje);
			break;

		case X25_CLEAR_INDICATION:
			x25LastError = 406;
			sysLastError = 406;
			rc = -16;
			break;

		case X25_DATA_ACK:
			x25LastError = 0;
			sysLastError = 0;
			rc = OK1;
			break;

		case X25_RESET_INDICATION:
			x25LastError = 407;
			sysLastError = 407;
			if (x25_reset_confirm(connId) == -1)
			{
				x25LastError = x25_errno;
				sysLastError = errno;
			}	
			rc = OK3;
			break;

		default:
			x25LastError = 0;
			sysLastError = 0;
			rc = OK2;
	}

	/* Libera el mensaje */
	if (x25RcvMsg.msg_point.cb_data->data != NULL)
		free(x25RcvMsg.msg_point.cb_data->data);
	if (x25RcvMsg.msg_point.cb_data != NULL)
		free(x25RcvMsg.msg_point.cb_data);

	return rc;
#else
	return OK;
#endif
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | X25::Close                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra la X25                                              |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int X25::Close()
{
#ifdef OS_AIX
	int rc   = 0; 
	int iDat = 1;
	
	/* Limpia los mensajes pendientes */
	do
	{
		iDat = 0;
		if (x25_ctr_test(x25CtrArray[0].ctr_id) > 0)
			/*if (x25_ctr_wait(1,x25CtrArray) != -1)*/
				if (x25_receive(&connId, &x25RcvMsg) != -1)
				{
					iDat = 1;
					if (x25RcvMsg.msg_point.cb_data->data != NULL)
						free(x25RcvMsg.msg_point.cb_data->data);
					if (x25RcvMsg.msg_point.cb_data != NULL)
						free(x25RcvMsg.msg_point.cb_data);					
				}
	} while (iDat);

	/* Limpia la llamada */
	x25ClearDefs.flags = X25FLG_CAUSE;
	x25ClearDefs.flags |= X25FLG_DIAGNOSTIC;
	x25ClearDefs.cause = 0;            
	x25ClearDefs.diagnostic = 0;       
	if (x25_call_clear(connId, &x25ClearDefs, (struct cb_msg_struct *)NULL) == -1)
	{
		x25LastError = x25_errno;
		sysLastError = errno;
		rc = -17;
	}
	
	/* Borra el contador */
	if (x25_ctr_remove(x25CtrArray[0].ctr_id) == -1)
	{
		if (rc == 0)
		{
			x25LastError = x25_errno;
			sysLastError = errno;
			rc = -18;
		}
	}
	
	connId                = -1;
	x25CtrArray[0].ctr_id = -1;

	if (rc == 0)
	{
		x25LastError = 0;
		sysLastError = 0;
	}

	return rc;
#else
	return OK;
#endif
}

#ifdef X25_CLASS_TEST
void main()
{
}
#endif
