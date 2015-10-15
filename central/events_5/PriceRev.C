#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | PriceRev.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Mensaje de TPV de anulacion de autorizacion                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "CACStat.h"
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
extern Queue        SQueue;			/* Cola de Sockets              */
extern Pos			P;              /* Objeto POS                   */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceRev::PriceRev                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
PriceRev::PriceRev(int msg_pid, char *msg_data, int data_len)
{
	str_price_msg	price_msg_str;
	event_type=EVENT_PRICEREV;
	orig_pid=msg_pid;
	memcpy((char*)&price_msg_str, msg_data, data_len);
	mensaje_price=new Price_Msg(price_msg_str);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceRev::~PriceRev                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
PriceRev::~PriceRev()
{
	delete mensaje_price;
	delete mensaje_price_rta;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PriceRev::ProcessIt                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PriceRev::ProcessIt()
{
	char			msg_str[2000];
	int				msg_len;
	int				ret;
	str_sel_cadata	term_data;
	TrxData			trx_data;
	char			aux_str1[100];
	str_upd_rev		StrDataRev;
	
	/* Crea el mensaje Price de respuesta */
	mensaje_price_rta=new Price_Msg(TYPE_PRICE_ANUL_VENTA_RTA);

	/* Obtiene el CA hacia el cual se dirige */
	strcpy(term_data.cod_tar, mensaje_price->GetCodTar());
	ret=getDataAnul(&term_data);
	if (ret!=OK)
	{ 
		LogAlarm.Put(0, "PriceRev: ERROR al obtener datos de la tarjeta [%s]\n", term_data.cod_tar);
		return NOOK;
	} 

	/* Modifica el mensaje Price de acuerdo a tipo de autorizacion */
	mensaje_price->ConvMsgPrice(atoi(term_data.tip_aut));

	/* Completa el mensaje de respuesta con los	campos del mensaje original */
	mensaje_price_rta->PutField( 1, mensaje_price->GetField( 1));
	mensaje_price_rta->PutField( 3, mensaje_price->GetField(13));
	mensaje_price_rta->PutField( 4, mensaje_price->GetField( 9));
	mensaje_price_rta->PutField( 8, mensaje_price->GetField( 4));
	mensaje_price_rta->PutField( 9, mensaje_price->GetField( 5));
	mensaje_price_rta->PutField(13, mensaje_price->GetField(21));
	mensaje_price_rta->PutField( 6, "00");
	mensaje_price_rta->PutField( 7, "000000");
	mensaje_price_rta->PutField(10, "               ");
	mensaje_price_rta->PutField(11, "                         " );
	mensaje_price_rta->PutField(12, "                              "
									"                              ");

	/* Completa estructura para actualizar la base de datos */
	sprintf(aux_str1, "%03s", mensaje_price->GetField(11));
	trx_data.NroSuc(aux_str1);
	trx_data.NroTrx(mensaje_price->GetField(9));
	trx_data.NroCaj(mensaje_price->GetField(13));
	trx_data.NroTar(mensaje_price->GetField(4));
	sprintf(aux_str1,"%05d",atol(mensaje_price->GetField(12)));
	trx_data.NroCajSf(aux_str1);
	trx_data.Monto(mensaje_price->GetField(6));
	LogAlarm.Put(0,"PriceRev: Emp:[%s] Suc:[%s] Ter:[%s] Trx:[%s] Monto:[%s]\n", 
						trx_data.NroSuc(), trx_data.NroCajSf(), trx_data.NroCaj(), trx_data.NroTrx(), trx_data.Monto()); 
	
	/* Actualiza la base de datos (codigo de estado) */
	StrDataRev=*(trx_data.GetDataRev());
	ret=DBTrx.UpdateReverso(&StrDataRev);
	if (ret==NOOK)
	{ 
		LogAlarm.Put(0, "PriceRev: ERROR  No existe transaccion a anular\n");
	} 
	else
	{
		/* Si la transaccion estaba aprobada, se bloquea la caja hasta lanzar el reverso */	
		if (ret==BLOCK)
		{
			/* Bloquea la caja */
			ret=P.SetPosBusy(term_data.nro_ca, trx_data.NroSuc(), trx_data.NroCaj());
			if (ret!=OK) 
			{
				LogAlarm.Put(0, "PriceRev: ERROR al setear caja como ocupada\n");
			}
		}
	}

	/* Aplana el mensaje */
	msg_len=mensaje_price_rta->GetMsgString(msg_str);

	/* Envia la respuesta al TPV */
	LogAlarm.Put(0, "PriceRev: Envia a TPV. CAut [%s]\n", term_data.nro_ca);
	ret=SQueue.SendMsg(orig_pid, msg_str, msg_len);
	if (ret!=OK)
	{
		LogAlarm.Put(0, "PriceRev: ERROR (%d) al enviar mensaje\n", SQueue.GetErrno());
		return NOOK;
	}
	else
	{
		LogAlarm.Put(2, "PriceRev: Respuesta enviada OK\n");
		return OK;
	}
}
