#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | EchoTest.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento que envía un mensaje de echo test al centro         |
|                 | autorizador                                                | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

extern Config           Cfg;			/* Objeto de configuracion        */
extern Log              LogAlarm;		/* Logging de actividades         */ 
extern TimeOutQueue		timeout_queue;	/* Cola de timeouts               */
extern TraceNmbr		TraceNumber;	/* Objeto para el nro de trace    */
extern TrxHeap			Heap;			/* Objeto Heap                    */
extern Queue            XQueue;			/* Cola de X.25                   */
extern Pos				P;				/* Objeto Pos                     */
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTest::EchoTest                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
EchoTest::EchoTest(int msg_pid, char *msg_data, int data_len)
{
	/* Carga el evento en el objeto */
	event_type=EVENT_ECHOTEST;
	
	/* Crea el mensaje de echo test */
	mensaje_iso=new Iso_Msg(TYPE_VISA_TEST);

	/* Copia los datos del mensaje al objeto */
	memcpy((char*)&ping_data, msg_data, sizeof(ping_data));

	/* Carga el pid en el objeto */
	orig_pid=msg_pid;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTest::~EchoTest                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
EchoTest::~EchoTest()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTest::EnviarMsg                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int EchoTest::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "EchoTest: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTest::ProcessIt                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int EchoTest::ProcessIt()
{
	char	msg_str[2000];
	int		len;
	int		ret;
	int		HeapOcup = 0;
	int		TOQueueOcup = 0;
	char	aux_str1[100];	
	char	idHeap[20];
	str_sel_cadata	term_data;
	int     iCantidad;

	/* Arma los datos de la terminal */
	memset(&term_data, 0, sizeof(term_data));
	strncpy(term_data.nro_suc, ping_data.nro_suc, sizeof(term_data.nro_suc));
	strncpy(term_data.nro_caj, ping_data.nro_caj, sizeof(term_data.nro_caj));
	strncpy(term_data.nro_ca , ping_data.nro_ca , sizeof(term_data.nro_ca ));
	LogAlarm.Put(0, "EchoTest: Suc:[%s] Caj:[%s] CA:[%s]\n", term_data.nro_suc, term_data.nro_caj, term_data.nro_ca);

	/* Obtiene datos de la terminal */
	ret=getDataByNTET(&term_data);
	if (ret!=OK)
	{
		LogAlarm.Put( 1, "EchoTest: ERROR (%d) al obtener datos de la terminal\n", ret);
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	/* Se carga en el objeto TraceNmbr el centro autorizador con el que se trabaja */
	TraceNumber.SetNroSuc(term_data.nro_suc);
	TraceNumber.SetNroCaj(term_data.nro_caj);
	TraceNumber.SetNroCA (term_data.nro_ca );
	TraceNumber.SetCodConCie("0");

	/* Arma mensaje iso */
	mensaje_iso->PutField(7, currentTimestamp());
	strcpy(aux_str1, TraceNumber);
	TraceNumber.Inc();
	mensaje_iso->PutField(11, aux_str1); 
	mensaje_iso->PutField(24, term_data.cod_red);
	mensaje_iso->PutField(41, term_data.nro_caj_ca);
	mensaje_iso->PutField(42, term_data.nro_com_et);

	/* Arma clave del heap */
	sprintf(idHeap,"%02d",atoi(term_data.nro_ca));

	/* Verifica si se encuentra en el heap */
	LogAlarm.Put(0, "EchoTest: Busca clave en el heap [%s]\n", idHeap);
	ret=Heap.FindData(idHeap, (char *)&orig_pid,sizeof(orig_pid));
	if (ret!=1)
	{
		LogAlarm.Put(0, "EchoTest: Heap ocupado. Clave:[%s]\n", idHeap);
		HeapOcup = 1;
	}
	LogAlarm.Put(0, "EchoTest: Valor de HeapOcup %d\n", HeapOcup);

	/* Verifica si se encuentra en la TOQ */
	ret=timeout_queue.FindTimeOut(idHeap);
	if (ret==1)
	{
		LogAlarm.Put(0, "EchoTest: Se encontro en TOQueue. Clave:[%s]\n", idHeap);
		TOQueueOcup = 1;
	}
	LogAlarm.Put(0, "EchoTest: Valor de TOQueueOcup %d\n", TOQueueOcup);

	/* Si el ping esta activo */
	if ((HeapOcup == 1) && (TOQueueOcup == 1))
	{
		/* Envia respuesta por la cola administrativa */
		LogAlarm.Put(0, "EchoTest: Ping Activo. Esta ocupado\n");
		LogAlarm.Put(0, "EchoTest: Envia respuesta por la cola administrativa\n");
		ret= EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}
	else 
	{
		/* Si el head esta ocupado */
		if (HeapOcup == 1)
		{
			LogAlarm.Put( 0, "EchoTest: Se encuentra en Heap pero no en TOQ\n");
			ret=Heap.GetData(idHeap, (char *)&orig_pid,sizeof(orig_pid));
			if (ret==NOOK)
			{
				LogAlarm.Put( 0, "EchoTest: ERROR (%d) al extraer de heap\n", Heap.GetErrno());
				EnviarMsg(orig_pid, "ERROR", 5);			
				return NOOK;
			}
		}

		/* Si la TOQ esta ocupada */ 
		if (TOQueueOcup == 1) 
		{
			LogAlarm.Put( 0, "EchoTest: Se encuentra en TOQ pero no en Heap\n");
			ret=timeout_queue.RemTimeOut(idHeap);
			if (ret==NOOK)
			{
				LogAlarm.Put(0, "EchoTest: ERROR (%d) al extraer de TOQ\n", timeout_queue.GetErrno());
				EnviarMsg(orig_pid, "ERROR", 5);
				return NOOK;
			}
		}
	}

	/* Registra en el heap */
	LogAlarm.Put(0, "EchoTest: Registra mensaje en el heap.\n");
	ret=Heap.PutData(idHeap, (char *)&orig_pid,sizeof(orig_pid));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "EchoTest: ERROR (%d) al registrar en el heap\n", Heap.GetErrno());
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	/* Registra en la TimeoutQueue	*/
	LogAlarm.Put(0, "EchoTest: Registra mensaje en la TOQ\n");
	Cfg.GetItem("TimeOutDaemon", "MsgTimeOut1", aux_str1);
	ret=timeout_queue.SetTimeOut(idHeap, atoi(aux_str1), GetType(), "");
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "EchoTest: ERROR (%d) al agregar a la TOQ\n", timeout_queue.GetErrno());

		/* Retira del heap si hubo error */
		ret=Heap.GetData(idHeap, (char *)&orig_pid,sizeof(orig_pid));
		if (ret==NOOK)
		{
			LogAlarm.Put( 0, "EchoTest: ERROR (%d) al obtener del heap\n", Heap.GetErrno());
		}
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}

	/* Aplana el mensaje */
	len=mensaje_iso->GetMsgString(msg_str);

	/* Envia por la cola de X.25 */
	LogAlarm.Put(0, "EchoTest: Envia a X.25. CAut [%s]\n", term_data.nro_ca);
	ret=XQueue.SendMsg(atoi(term_data.nro_ca)+1, msg_str, len);
	if (ret!=OK)
	{
		LogAlarm.Put( 0, "EchoTest: ERROR (%d) al enviar por X.25!!\n", XQueue.GetErrno());
		EnviarMsg(orig_pid, "ERROR", 5);
		return NOOK;
	}
	return OK;
}
