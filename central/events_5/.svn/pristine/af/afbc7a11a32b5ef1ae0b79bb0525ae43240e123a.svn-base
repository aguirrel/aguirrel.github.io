#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Batch.C                                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento que envía un mensaje de cierre de lote batch al     |
|                 | centro autorizador                                         | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

extern "C"
{
	#include "DBTrxOff.h"
}

extern Config           Cfg;			/* Objeto de configuracion      */
extern Log              LogAlarm;		/* Logging de actividades       */
extern TimeOutQueue		timeout_queue;	/* Cola de timeouts             */
extern TraceNmbr		TraceNumber;	/* Objeto para el nro de trace  */
extern TrxHeap			Heap;			/* Objeto Heap                  */      
extern Queue            XQueue;			/* Cola de X.25                 */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Batch::Batch                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Batch::Batch(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_BATCH;
    orig_pid=msg_pid;
    mensaje_iso=new Iso_Msg(msg_data, data_len);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Batch::~Batch                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Batch::~Batch()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Batch::ProcessIt                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Batch::ProcessIt()
{
    char			aux_str[100];
    char			msg_str[2000];
    int				msg_len;
    int				ret;
    str_heap_data	heap_data;
    str_sel_ntdata	term_data;
    char			idHeap[20];

    /* Obtiene los datos del mensaje */
    LogAlarm.Put(5, "Batch: Obteniendo datos de la terminal.\n");
    strcpy(term_data.nro_caj_ca, mensaje_iso->GetField(41));
    strcpy(term_data.nro_com   , mensaje_iso->GetField(42));

    /* Obtiene datos de la terminal */
	ret=getDataByCA(&term_data);
	if (ret!=OK)
	{
		LogAlarm.Put( 0, "Batch: ERROR (%d) al obtener datos de la terminal\n", ret);
		return NOOK;
	}

    /* Se indica al objeto TraceNmbr con que centro autorizador se trabaja */
    TraceNumber.SetNroSuc(term_data.nro_suc);
    TraceNumber.SetNroCaj(term_data.nro_caj);
    TraceNumber.SetNroCA (term_data.nro_ca );
    TraceNumber.SetCodConCie(term_data.cod_con_cie);

	/* Setea centro autorizador */
	mensaje_iso->SetNroCA8583(term_data.nro_ca);
	
	/* Obtiene tarjeta y la carga para agregar 'F' final */
	switch (DBTipoProtocoloISO(term_data.nro_ca))
	{
	case PROT_ISO_AMEX:
		sprintf(aux_str, "%s", mensaje_iso->GetField(2));
		mensaje_iso->PutField(2, aux_str);
		break;
	default:
		break;
	}
	
    /* Completar el mensaje con los datos generados	*/
    strcpy(aux_str, TraceNumber);
    TraceNumber.Inc();
    mensaje_iso->PutField(11, aux_str); 

    /* Chequea consistencia del mensaje	*/
    LogAlarm.Put(5, "Batch: Checkeo de consistencia del msg.\n");
    if (!mensaje_iso->ChkReady())
	{
		LogAlarm.Put(0,"Batch: ERROR mensaje ISO incompleto\n");
		return NOOK;
	}

    /* Registra transaccion en TrxHeap	*/
    LogAlarm.Put(5, "Batch: Obteniendo datos para el Heap.\n");
    heap_data.orig_pid=orig_pid;
    LogAlarm.Put(5, "Batch: Poniendo datos en el Heap.\n");
    sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s", term_data.nro_ca, term_data.nro_suc,
                                           term_data.nro_caj, mensaje_iso->GetField(11));
    ret=Heap.PutData(idHeap,(char *)&heap_data, sizeof(heap_data));
    if (ret==NOOK)
	{
		LogAlarm.Put(5, "Batch: ERROR (%d) al agregar en el Heap\n", Heap.GetErrno());
		return NOOK;
	}

    /* Registra en la TimeoutQueue	*/
    LogAlarm.Put(5, "Batch: Agregando a la TO Queue.\n");
    Cfg.GetItem("TimeOutDaemon", "MsgTimeOut320", aux_str);
    ret=timeout_queue.SetTimeOut(idHeap, atoi(aux_str), GetType(), "");
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "Batch: ERROR (%d) al agregar en TimeOutQueue\n", timeout_queue.GetErrno());
		return NOOK;
	}

	/* Aplana el mensaje */
    msg_len=mensaje_iso->GetMsgString(msg_str);

    /* Envia el mensaje por X25 */
    LogAlarm.Put(0, "Batch: Envia a X.25. CAut [%s]\n", term_data.nro_ca);
	ret=XQueue.SendMsg(atoi(term_data.nro_ca)+1, msg_str, msg_len);
    if (ret==NOOK)
	{
		LogAlarm.Put(0, "Batch: ERROR (%d) al enviar a Cola X.25\n", XQueue.GetErrno());
		return NOOK;
	}
	LogAlarm.Put(5, "Batch: Evento finalizado\n");

    return OK;
}
