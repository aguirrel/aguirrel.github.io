#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TimeOutQueue.C                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto que maneja la cola de time out                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Log.h"
#include "TimeOutQueue.h"

extern Log		LogAlarm;

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::TimeOutQueue                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TimeOutQueue::TimeOutQueue()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::~TimeOutQueue                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TimeOutQueue::~TimeOutQueue()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::Open                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Apertura de la cola de time out                            |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::Open(long SemKey, int entries,long tmtSecs )
{
	time_t tmDesde;            // segundos para timeOut.
	time_t tmHasta;                 

	/* Obtiene un identificador de memoria compartida */
	time(&tmDesde);
	do   
	{ 
		ShMemHandle=shmget(SemKey, sizeof(str_timeout)*max_entries, 0);
		ErrNo=(ShMemHandle==NOOK ? errno : OK);
		time(&tmHasta);
	} while ((tmHasta - tmDesde < tmtSecs || tmtSecs==0) && ErrNo == ENOENT);

	/* Configura el maximo de entradas posibles en la cola */
	max_entries=entries;
	
	/* Continua si obtuvo el identificador */
	if (ShMemHandle==-1)
	{
		ErrNo=errno;
		return NOOK;
	}

	/* Obtiene memoria compartida */
	timeout_data=(str_timeout *)shmat(ShMemHandle, 0, 0);
	if (timeout_data==NULL)
	{
		ErrNo=errno;
		return NOOK;
	}

	/* Abre el semaforo */
	ret=Sem.Open(SemKey);
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	/* Retorna el resultado */
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::SetTimeOut                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Agrega un elemento a la cola                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::SetTimeOut(char *id, long exp_time, int event_type, char *cod)
{
   int		cont;

   /* Obtiene el semaforo */
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}
	
	/* Verifica si la clave se encuentra en la cola */
	for (cont=0; cont<max_entries; cont++)
	{
		if ((!strcmp(timeout_data[cont].id,id)) && (timeout_data[cont].exp_tmstmp!=0))
		{
			Sem.Reset();
			ErrNo=EEXIST;
			return NOOK;
		}
	}

	/* Al no estar en cola, busca una posicion libre para insertarla */
	for (cont=0; cont<max_entries; cont++)
	{
		if ((timeout_data[cont].id[0]=='\0') && (timeout_data[cont].exp_tmstmp==0))
			break;
	}
	
	/* Verifica que no haya llegado al final de la cola */
	if (timeout_data[cont].exp_tmstmp!=0)
	{
		Sem.Reset();
		ErrNo=EEXIST;
		return NOOK;
	}

	/* Agrega el elemento en la cola */
	LogAlarm.Put(19, "TimeOutQueue::SetTimeOut(): id: [%s] posicion: %d\n",	id, cont);
	strcpy(timeout_data[cont].id,id);
	strcpy(timeout_data[cont].cod_ser,cod);
	timeout_data[cont].exp_tmstmp=time(NULL)+exp_time;
	timeout_data[cont].event_type=event_type;
	timeout_data[cont].expired=0;

	/* Libera el semaforo */
	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	/* Retorna el resultado */
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::RemTimeOut                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Saca un elemento a la cola                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::RemTimeOut(char * id)
{
	int cont;

	/* Obtiene el semaforo */
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		Sem.Reset();
		return NOOK;
	}
	
	/* Busca el elemento hasta encontrarlo */
	for (cont=0; cont<max_entries; cont++)
	{
		if ((!strcmp(timeout_data[cont].id,id)) && (timeout_data[cont].exp_tmstmp!=0))
		{
			break;
		}
	}
	
	/* Si no lo encuentra finaliza */
	if (strcmp(timeout_data[cont].id,id))
	{
		Sem.Reset();
		ErrNo=ENOENT;
		return NOOK;
	}
	
	/* Si lo encuentra blanquea el dato y finaliza */
	if (timeout_data[cont].expired!=0)
	{
		memset(&timeout_data[cont], 0, sizeof(str_timeout));
		Sem.Reset();
		ErrNo=ETIME;
		return NOOK;
	}
	LogAlarm.Put(19, "TimeOutQueue::RemTimeOut(): id: [%s] posicion: %d\n",	id, cont);

	/* Blanquea el dato */
	memset(&timeout_data[cont], 0, sizeof(str_timeout));

	/* Libera el semaforo */
	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	/* Retorna el resultado */
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::FindTimeOut                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Busca un elemento en la cola                               |
|                 | Si lo encuentra devuelve 1. Caso contrario devuelve 0      | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::FindTimeOut(char * id)
{
   int cont;

   /* Obtiene el semaforo */ 
   ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		Sem.Reset();
		return NOOK;
	}

   /* Busca el elemento en la cola */
	for (cont=0; cont<max_entries; cont++)
	{
		if ((!strcmp(timeout_data[cont].id,id)) && 
		(timeout_data[cont].exp_tmstmp!=0))
		{
			break;  	/* Sale si lo encuentra */
		}
	}

	/* Libera el semaforo */
	Sem.Reset();

	/* Retorna el resultado */
	if (!strcmp(timeout_data[cont].id,id))
	{
		return 1;
	}
	return 0;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::Close                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra la cola de time out                                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::Close()
{
	shmdt((char *)timeout_data);
	Sem.Close();
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::GetErrno                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Devuelve el ultimo error ocurrido                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::GetErrno()
{
	return ErrNo;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::Create                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea la cola de time out                                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::Create(long SemKey, int entries, int  perms)  
{
	int    cont;  

	/* Obtiene la cantidad maximas de entradas de la cola */
	max_entries=entries;

	/* Obtiene un identificador de memoria compartida */
	ShMemHandle=shmget(SemKey, sizeof(str_timeout)*max_entries, perms | IPC_CREAT/* | IPC_EXCL*/);
	if (ShMemHandle==-1)
	{
		LogAlarm.Put(0, "TimeOutQueue: Error al crear area de shm (%d)!!\n", errno);
		return NOOK;
	}

	/* Obtiene memoria compartida */
	timeout_data=(str_timeout *)shmat(ShMemHandle, 0, 0);
	if (timeout_data==(str_timeout *)-1)
	{
		LogAlarm.Put(0, "TimeOutQueue: Error al asignar area de shm (%d)!!\n", errno);
		return NOOK;
	}

	/* Crea el semaforo */
	ret=Sem.Create(SemKey, perms);
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "TimeOutQueue: Error al crear el semaforo. \n");
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	/* Obtiene el semaforo */
	ret=Sem.Set();
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "TimeOutQueue: Error en el Set del semaforo. \n");
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	/* Blanquea la tabla de time out */
	LogAlarm.Put(0, "TimeOutQueue: Reseteando tabla de timeout...\n");
	for (cont=0; cont<max_entries; cont++)
	{
		memset(&timeout_data[cont], 0, sizeof(str_timeout));
	}

	/* Libera el semaforo */
	ret=Sem.Reset();
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "TimeOutQueue: Error Reset del semaforo . \n");
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	/* Retorna el resultado */
	LogAlarm.Put(0, "TimeOutQueue: Termino la creacion de la cola timeout \n");
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::Destroy                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruye la cola de time out                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::Destroy()
{
	shmdt((char *) timeout_data);
	shmctl(ShMemHandle, IPC_RMID, NULL);
	Sem.Destroy();
  
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::FindTO                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     |                                                            |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::FindTO()
{
	int cont;

	/* Busca en las entradas de la cola */
	for (cont=0; cont<max_entries; cont++)
	{
		/* Obtiene el semaforo */
		Sem.Set();
		if (ret==NOOK)
		{
			LogAlarm.Put(0, "TimeOutQueue: Error en el Set del semaforo. \n");
			ErrNo=Sem.GetErrno();
			return NOOK;
		}

		/* Obtiene la primera posicion con timeout vencido */
		if ((time(NULL)>=timeout_data[cont].exp_tmstmp) && (timeout_data[cont].exp_tmstmp!=0))
		{
			Sem.Reset();
			return   cont;
		}

		/* Libera el semaforo */
		Sem.Reset();
	}
	return -1;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::GetTimeOut                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el elemento del dato                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
str_timeout* TimeOutQueue::GetTimeOut(int cont)
{
	return &timeout_data[cont];
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::SetOne                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el tiempo para que al vencer borre el elemento       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::SetOne(int cont, int tiempo)
{
	timeout_data[cont].expired=1;
	timeout_data[cont].exp_tmstmp=time(NULL) + tiempo;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeOutQueue::SetSecond                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Blanquea el elemento del dato                              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TimeOutQueue::SetSecond(int cont)
{
	memset(&timeout_data[cont], 0, sizeof(str_timeout));
	return OK;
}
