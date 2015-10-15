#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Cierre.C                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento que envía un mensaje de cierre de lote al centro    |
|                 | autorizador                                                | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

extern BatchTable	DBBatch;		/* Para tabla de cierres        */
extern Config       Cfg;			/* Objeto de configuracion      */
extern Log          LogAlarm;		/* Logging de actividades       */
extern TimeOutQueue	timeout_queue;	/* Cola de timeouts             */
extern TraceNmbr	TraceNumber;	/* Objeto para el nro de trace  */
extern TrxHeap		Heap;			/* Objeto Heap                  */
extern Queue        SQueue;			/* Cola de Sockets              */
extern Queue        XQueue;			/* Cola de X.25                 */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Cierre::Cierre                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Cierre::Cierre(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_CIERRE;
    orig_pid=msg_pid;
    mensaje_iso=new Iso_Msg(msg_data, data_len);   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Cierre::~Cierre                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Cierre::~Cierre()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Cierre::ProcessIt                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Cierre::ProcessIt()
{
    char			aux_str[100];
    char			msg_str[2000];
    int				msg_len;
    int				ret;
    str_heap_data	heap_data;
    BatchData		batch_data;
    str_sel_ntdata	term_data;
	char			aux_str1[100];
    char			idHeap[20];

	/* Obtiene los datos del mensaje */
    strcpy(term_data.nro_caj_ca, mensaje_iso->GetField(41));
    strcpy(term_data.nro_com   , mensaje_iso->GetField(42));
    
	/* Completa la estructura con los datos del mensaje */
	batch_data.NroCajCa(term_data.nro_caj_ca);

	/* Obtiene datos de la terminal */
	ret=getDataByCACie(&term_data);
	if (ret!=OK)
	{
		LogAlarm.Put( 0, "Cierre: ERROR (%d) al obtener datos de la terminal\n", ret);
		return NOOK;
	}

	/* Indica al objeto TraceNmbr el centro autorizador */
    TraceNumber.SetNroSuc(term_data.nro_suc);
    TraceNumber.SetNroCaj(term_data.nro_caj);
    TraceNumber.SetNroCA (term_data.nro_ca );
    TraceNumber.SetCodConCie(term_data.cod_con_cie);

    /* Completar el mensaje con los datos generados */
    mensaje_iso->PutField(7, currentTimestamp());
    strcpy(aux_str, TraceNumber);
    TraceNumber.Inc();
    mensaje_iso->PutField(11, aux_str); 
    mensaje_iso->PutField(15, currentTimestamp());

    /* Chequea consistencia del mensaje	*/
	LogAlarm.Put(95, "Cierre: Chequea consistencia del mensaje\n");
	if (!mensaje_iso->ChkReady())
	{
		LogAlarm.Put( 0, "Cierre: ERROR de consistencia en el mensaje\n");
		return NOOK;
	}

    /* Registrar transaccion en TrxHeap	*/
    LogAlarm.Put(95, "Cierre: Obtiene datos para el Heap\n");
    heap_data.orig_pid=orig_pid;

	/* Pone datos en el heap */
    LogAlarm.Put(95, "Cierre: Poniendo datos en el Heap.\n");
    sprintf(idHeap,"%2.2s%3.3s%5.5s%8.8s",term_data.nro_ca,term_data.nro_suc,term_data.nro_caj,mensaje_iso->GetField(11));
    ret=Heap.PutData(idHeap,(char *)&heap_data, sizeof(heap_data));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "Cierre: ERROR (%d) al poner los datos en el heap!!\n", Heap.GetErrno());
		return NOOK;
	}

    /* Registra en la TimeoutQueue con un timeout diferente a los otros eventos */
    LogAlarm.Put( 5, "Cierre: Agrega a la cola de timeout\n");
    Cfg.GetItem("TimeOutDaemon", "MsgTimeOut500", aux_str1);
    ret=timeout_queue.SetTimeOut(idHeap, atoi(aux_str1), GetType(), ""/*CodSer*/);
	if (ret==NOOK)
	{
		LogAlarm.Put( 0, "Cierre: ERROR (%d) al agregar en cola de timeout!!\n", timeout_queue.GetErrno());
		return NOOK;
	}

    /* Si se graba registro de cierre */
	memset(aux_str1, 0, sizeof(aux_str1));
	Cfg.GetItem("AdminDaemon", "InsertCierre", aux_str1);
	if (aux_str1[0] != 'N')
	{
		/* Completa la estructura */
		batch_data.NroCA(term_data.nro_ca);
		mensaje_iso->GetDBData(batch_data);
		batch_data.CanRet("0000");
		sprintf(aux_str, "%02d", E_ENVI);
		batch_data.CodEst(aux_str);
		batch_data.NroSuc(term_data.nro_suc);
		batch_data.NroCaj(term_data.nro_caj);
		batch_data.NroCom(term_data.nro_com);

		/* Registra en la base de datos */
		LogAlarm.Put( 5, "Cierre: Registra en base de datos\n");
		ret=DBBatch.Insert(batch_data.GetInsData());
		if (ret!=OK)
		{
			LogAlarm.Put( 1, "Cierre: ERROR (%d) al registrar en base de datos\n", DBBatch.GetErrno());
			return NOOK;
		}
	}

	/* Aplana el mensaje */
    msg_len=mensaje_iso->GetMsgString(msg_str);

    /* Envia el mensaje al centro autorizador */
	LogAlarm.Put(0, "Cierre: Envia a X.25. CAut [%s]\n", term_data.nro_ca);
    ret=XQueue.SendMsg(atol(term_data.nro_ca)+1, msg_str, msg_len);
	if (ret!=OK)
	{
		LogAlarm.Put(0, "Cierre: ERROR (%d) mandando mensaje a X.25!!\n", SQueue.GetErrno());
		return NOOK;
	}
    return OK;
}
