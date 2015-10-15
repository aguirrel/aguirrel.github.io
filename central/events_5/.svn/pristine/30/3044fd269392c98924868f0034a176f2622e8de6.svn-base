#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | EchoTestRta.C                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Evento que envía un mensaje de echo test al centro         |
|                 | autorizador                                                | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

extern Config           Cfg;			/* Objeto de configuracion  */
extern Log              LogAlarm;		/* Logging de actividades   */
extern TimeOutQueue		timeout_queue;	/* Cola de timeouts         */
extern TrxHeap			Heap;			/* Objeto Heap              */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTestRta::EchoTestRta                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
EchoTestRta::EchoTestRta(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_ECHOTEST_RTA;

    mensaje_iso=new Iso_Msg(msg_data, data_len);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTestRta::~EchoTestRta                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
EchoTestRta::~EchoTestRta()
{
    delete mensaje_iso;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTestRta::EnviarMsg                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int EchoTestRta::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "EchoTestRta: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EchoTestRta::ProcessIt                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int EchoTestRta::ProcessIt()
{
	str_sel_ntdata	term_data;
	int			ret;
	Queue		AQueue;
	char		aux_str1[100];		
	char		idHeap[20];

	/* Checkear consistencia del mensaje	*/
	LogAlarm.Put(0, "EchoTestRta: Checkea consistencia del mensaje\n");

	if (!mensaje_iso->ChkReady())
	{
		LogAlarm.Put( 0, "EchoTestRta: ERROR en consistencia del mensaje\n");
		return NOOK;
	}

	/* Obtiene terminal y comercio del mensaje iso */
	strncpy(term_data.nro_caj_ca, mensaje_iso->GetField(41), sizeof(term_data.nro_caj_ca));
	strncpy(term_data.nro_com, mensaje_iso->GetField(42), 	sizeof(term_data.nro_com));

	/* Busca datos de la terminal */
	LogAlarm.Put( 5, "EchoTestRta: Terminal. Caj:[%s] Com:[%s]\n", mensaje_iso->GetField(41), mensaje_iso->GetField(42));
	ret=getDataByCAET(&term_data);
	if (ret!=OK)
	{
		LogAlarm.Put( 0, "EchoTestRta: ERROR (%d) al obtener datos de la terminal\n", ret);
		return NOOK;
	}                    

	/* Arma clave para el heap */
	sprintf(idHeap,"%2.2s", term_data.nro_ca);

	/* Recupera datos del heap */
	ret=Heap.GetData(idHeap,(char *)&orig_pid, sizeof(orig_pid));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "EchoTestRta: ERROR (%d) al recuperar del heap [%s]\n", Heap.GetErrno(),idHeap);
		return NOOK;
	}

	/* Remueve de la TOQ */
	LogAlarm.Put(5, "EchoTestRta: Remueve datos de la TOQ\n");
	ret=timeout_queue.RemTimeOut(idHeap);
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "EchoTestRta: ERROR (%d) al remover el Timeout\n", timeout_queue.GetErrno());
	}

	/* Envia la respuesta por la cola administrativa */
	LogAlarm.Put(0, "EchoTestRta: Envia respuesta por la cola administrativa. CAut [%s]\n", term_data.nro_ca);

	/* Envia respuesta por la cola administrativa */
	ret=EnviarMsg(orig_pid, "ACK", 3);
	if (ret!=OK)
	{
		LogAlarm.Put(0, "EchoTestRta: ERROR (%d) al enviar por cola administrativa\n", AQueue.GetErrno());
		return NOOK;
	}

	
	return OK;
}
