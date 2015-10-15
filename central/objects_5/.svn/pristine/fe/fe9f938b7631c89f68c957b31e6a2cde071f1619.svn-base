#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Queues.C                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Fuente para el manejo de colas de mensajes                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Queues.h"
#include "Log.h"
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::Queue                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Inicializa la cola de mensajes                             |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Queue::Queue()
{
   QueueHandle=NOOK;
   ErrorRC=ENOENT;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::Open                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Abre una cola de mensajes. Intenta durante un tiempo       |
|                 | especificado o por siempre si el timeout es igual a cero   | 
+-----------------+------------------------------------------------------------+
*/
int Queue::Open(long QueueID, long tmt)
{
	time_t tmDesde;        
	time_t tmHasta;                 

	time(&tmDesde);
	do   
	{ 
		QueueHandle=msgget(QueueID, 0);
		ErrorRC = QueueHandle==NOOK ? errno : OK;
		time(&tmHasta);
	} while ((tmHasta - tmDesde < tmt || tmt==0) && ErrorRC == ENOENT);
	return ErrorRC == OK ? OK : NOOK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::Create                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea la cola de mensajes                                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::Create(long QueueID, int QueuePerms)
{
	QueueHandle=msgget(QueueID, IPC_CREAT | QueuePerms);
	if (QueueHandle==NOOK)
	{
		ErrorRC=errno;
		return NOOK;
	}
	ErrorRC=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::SendMsg                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje por la cola de mensajes                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::SendMsg(long type, char *data, int datalen)
{
	int	cont=0;

	if (datalen>QUEUE_MAX_DATA_SIZE)
	{
		ErrorRC=0;
		return NOOK;
	}
	memcpy (&msg.data, data, datalen);
	if (type==0)
	{
		ErrorRC=0;
		return NOOK;
	}
	msg.type=type;
	msg.pid=getpid();
   
	do
	{
		len=msgsnd(QueueHandle, (struct msgbuf *)&msg, sizeof(msg.pid)+datalen,0);
		cont++;
	}
	while ((len==NOOK) && ((errno==EIDRM) || (errno==EINVAL)) && (cont<QUEUE_MAX_RETRY));

	if (len==NOOK)
	{
		ErrorRC=errno;
		return NOOK;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::GetMsg                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Recibe un mensaje desde la cola de mensajes                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::GetMsg(long type, char *data, int maxlen)
{
	int	cont=0;

	if (maxlen>QUEUE_MAX_DATA_SIZE)
	{
		ErrorRC=EINVAL;
		return NOOK;
	}
	
	do
	{
		len=msgrcv(QueueHandle, (struct msgbuf *) &msg, sizeof(msg.pid)+maxlen, type, 0);
		cont++;
	}
	while ((len==NOOK) && ((errno==EIDRM) || (errno==EINVAL)) && (cont<QUEUE_MAX_RETRY));

	if (len==NOOK)
	{
		ErrorRC=errno;
		return NOOK;
	}

	memcpy (data, &msg.data, len-sizeof(msg.pid));
	return len-sizeof(msg.pid);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::SetFlag                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea flags en la cola de mensajes                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::SetFlag(long new_f)
{
	Flags=Flags|new_f;
	return OK;       
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::ClearFlag                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Limpia flags de la cola de mensajes                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::ClearFlag(long new_f)
{
   Flags=Flags ^ new_f;
   return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::GetMsgType                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el tipo de mensaje de la cola                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
long Queue::GetMsgType()
{
   return msg.type;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::GetOrigPid                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el pid del mensaje de la cola                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::GetOrigPid()
{
	return msg.pid;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::GetErrno                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el error del mensaje de la cola                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::GetErrno()
{
	return ErrorRC;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::Destroy                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruye la cola de mensajes                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::Destroy()
{
	ErrorRC=msgctl(QueueHandle, IPC_RMID, NULL);
	if (ErrorRC==NOOK)
	{
		ErrorRC=errno;
		return NOOK;
	}
	ErrorRC=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::Flush                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Vacia la cola de mensajes                                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Queue::Flush(long type)
{
	int	cont=0;
	int  rc;
	struct msqid_ds infoQueue;

	/* Obtiene la cantidad de mensajes existentes en la cola. */
	rc=msgctl(QueueHandle,IPC_STAT,&infoQueue); 
	if (rc!=0)
	{
		return NOOK;
	}

	cont=infoQueue.msg_qnum;

	/* Extrae los mensajes existentes de la cola. */
	for (cont=0;cont < infoQueue.msg_qnum;  cont++)
	{
		len=msgrcv(QueueHandle, (struct msgbuf *) &msg, QUEUE_MAX_DATA_SIZE, type, IPC_NOWAIT);
		if (len=-1)
			break;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Queue::StatusCA                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica el estado de los canales para el centro           |
|                 | autorizador enviado. Retorna los canales activos o error<0 |
+-----------------+------------------------------------------------------------+
*/
int Queue::StatusCA(int iCA)
{
    char szSepTok[] = "#";
	char *ptrToken;
	char szConsul[128], szRespue[512];
	int  iRetorno, iAutoriz, iConfigu, iCreados, iActivos, iBloquea;
	

	/* Envia consulta de canales */
	iRetorno = -1;
	sprintf(szConsul, "C");
	if (SendMsg(99, szConsul, strlen(szConsul))==0)
	{
		/* Espera respuesta */
		iRetorno = -2;
		memset(szRespue, 0, sizeof(szRespue));
		if (GetMsg(98, szRespue, sizeof(szRespue))>0)
		{
			/* Procesa respuesta */
			iRetorno = -3;
			szRespue[sizeof(szRespue)-1] = 0;
			ptrToken = strtok(szRespue, szSepTok );
			while ( (ptrToken!=NULL) && (iRetorno<0) )
			{
				sscanf(ptrToken, "%02d-%02d-%02d-%02d-%02d", 
							&iAutoriz, &iConfigu, &iCreados, &iActivos, &iBloquea);
				if (iAutoriz==iCA)
				{
					iRetorno = iActivos;
				}
				ptrToken = strtok( NULL, szSepTok );
			}
		}
	}
	return(iRetorno);
}
    
    
