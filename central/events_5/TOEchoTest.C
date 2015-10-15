#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TOEchoTest.C                                               |  
|-----------------+------------------------------------------------------------+
| DESCRIPCION     | Timeout al mensaje de echotest enviado por el              |
|                 | TimeOutDaemon                                              | 
+-----------------+------------------------------------------------------------+
*/
#include "CACStat.h"
#include "Events.h"
#include "Log.h"
#include "Table.h"

extern Config       Cfg;			/* Objeto de configuracion  */
extern Log          LogAlarm;		/* Logging de actividades   */
extern TrxHeap		Heap;			/* Objeto Heap              */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOEchoTest::TOEchoTest                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOEchoTest::TOEchoTest(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_TIMEOUT_ECHOTEST;

    memcpy(&data, msg_data, sizeof(str_timeout_event));
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOEchoTest::~TOEchoTest                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TOEchoTest::~TOEchoTest()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOEchoTest::EnviarMsg                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola administrativa                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOEchoTest::EnviarMsg(int orig_pid, char *msg, int len)
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
		LogAlarm.Put(0, "TOEchoTest: ERROR al enviar mensaje por cola administrativa\n");
	}
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TOEchoTest::ProcessIt                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TOEchoTest::ProcessIt()
{
    int				ret;
    str_heap_data	heap_data;
    
    LogAlarm.Put( 5, "TOEchoTest: TO esperando respuesta al mensaje [%d]\n", data.key);

	/* Extrae los datos del heap */
	LogAlarm.Put(95, "TOEchoTest: Extrae los datos del Heap. Clave:[%s]\n", data.key);
	ret=Heap.GetData(data.key,(char *)&heap_data, sizeof(heap_data));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "TOEchoTest: ERROR (%d) al extraer del heap\n", Heap.GetErrno());
		EnviarMsg(data.orig_pid, "NACK", 4);
		return NOOK;
	}

    /* Enviar mensaje por cola administrativa */
    LogAlarm.Put(0, "TOEchoTest: Envia mensaje por cola administrativa. CAut [%s]\n", data.key );
    EnviarMsg(heap_data.orig_pid, "NACK", 4);
	LogAlarm.Put(5, "TOEchoTest: Evento finalizado\n");

    return OK;
}
