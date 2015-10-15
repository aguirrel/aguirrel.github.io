#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+------------------------------------------------------------------------------+
|          IECISA - PLATAFORMAS SECTORIALES - DESARROLLO DISTRIBUCION          |
|              Copyright (C) 1999 - INFORMATICA EL CORTE INGLES                |
+-----------------+------------------------------------------------------------+
| FUENTE          | TimeOutDaemon.C                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso que chequea el timeout de las transacciones que    |
|                 | estan en espera                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Config.h"
#include "Daemons.h"
#include "Log.h"
#include "PidTable.h"
#include "Queues.h"
#include "Semaphores.h"
#include "TimeOutQueue.h"

#define NUM_VERSION		"v1.46 02-Jul-2013 (3 min)"

Config			Cfg(CONFIG_FILE);	/* Objeto de configuracion  */
Log				LogAlarm;			/* Logging de actividades   */
PidTable		PTable;				/* Tabla de PIDs            */
Queue			EQueue;				/* Cola de eventos          */
Semaphore		Sem;				/* Objeto Semaphore         */
TimeOutQueue    timeout_queue;		/* Cola de timeout          */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | shut_down                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Finaliza la aplicacion                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void shut_down(int sig_id=0)
{
    timeout_queue.Destroy();
    exit(0);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | init                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Inicia las estructuras y memoria compartida                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int init()
{
    int	ret;
    int	cont;
	int	max_entries;
	char aux_str1[100];
	char aux_str2[100];

    /* Configura las señales de terminacion */
	signal(SIGINT, shut_down);
    signal(SIGTERM, shut_down);

    /* Obtiene el nombre del archivo de log */
	Cfg.GetItem("TimeOutDaemon", "LogName", aux_str1);
    LogAlarm.Open(aux_str1);

    /* Setea el nivel de log */
	Cfg.GetItem("TimeOutDaemon", "LogLevel", aux_str1);
    LogAlarm.SetLevel(atoi(aux_str1));

	/* Loguea hora de comienzo */
    LogAlarm.Put( 0,"TimeOutDaemon::init(): Comienzo [%s] - %s", NUM_VERSION, currentTimeLog()); 

    /* Obtiene la cantidad maxima de entradas en la cola */
	Cfg.GetItem("TimeOutDaemon", "MaxEntries", aux_str1);
    max_entries=atoi(aux_str1);

    /* Obtiene la cantidad maxima de entradas en la tabla de PIDs */
	Cfg.GetItem("PIDTable", "ShMemID", aux_str1);
    Cfg.GetItem("PIDTable", "MaxEntries", aux_str2);

    /* Abre tabla de PIDs */
	LogAlarm.Put(0, "TimeOutDaemon::init(): Abre tabla de PIDs [%s] [%s]\n", aux_str1, aux_str2);
    while (PTable.Open(atoi(aux_str1), atoi(aux_str2))==NOOK)
    {
		if (PTable.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "TimeOutDaemon::init(): Error (%d)!!\n", PTable.GetErrno());
			exit(1);
		}
    }
    
	/* Pone el pid en la tabla */
	LogAlarm.Put(0, "TimeOutDaemon::init(): Adding PID to PID table...\n");
    ret=PTable.PutPid(getpid());
    if (ret==NOOK)
    {
		LogAlarm.Put(0, "TimeOutDaemon::init(): Error (%d)!!\n", PTable.GetErrno());
		exit(1);
    }

    /* Obtiene los identificadores y permisos de la cola de timeout */
	Cfg.GetItem("TimeOutDaemon", "ShMemID", aux_str1);
    Cfg.GetItem("TimeOutDaemon", "ShMemPerms", aux_str2);

    /* Crea cola de timeout */
    LogAlarm.Put(0, "TimeOutDaemon::init(): Creando area de memoria compartida para la Timeout_Queue\n");  
    ret=timeout_queue.Create(atoi(aux_str1), max_entries, atoi(aux_str2));
    if (ret==NOOK)
    {
		LogAlarm.Put(0, "TimeOutDaemon::init(): Error (%d) al crear area para la Timeout_Queue \n", timeout_queue.GetErrno());
    }

    /* Obtiene identificador de la cola de eventos */
	Cfg.GetItem("VisaDaemon", "QueueID", aux_str1);
    
	/* Abre cola de eventos */
	LogAlarm.Put(0, "TimeOutDaemon::init(): Abriendo cola de eventos...\n");
    ret= EQueue.Open(atoi(aux_str1));
    if (ret==NOOK)
	{
		LogAlarm.Put(1, "TimeOutDaemon::init(): Error (%d) al abrir cola de eventos!!\n", EQueue.GetErrno());
		return NOOK;
	}

    return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | process_timeout                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Procesa el timeout.                                        |
|                 | Parametro: cont: Posicion de la cola con el timeout        | 
+-----------------+------------------------------------------------------------+
*/
void process_timeout(int cont)
{
	int		ret;
	int		event_type;
	str_timeout     	*timeout_data;
	str_timeout_event	timeout_event_data;
	char	aux_str1[100];	
	
	/* Obtiene los datos del elemento con time out vencido */
	timeout_data=timeout_queue.GetTimeOut(cont);
	LogAlarm.Put(0, "process_timeout(): Se encontro un TO (id:%s)\n", timeout_data->id);
	
	/* Si expired==0 */
	if (timeout_data->expired==0)
	{
		/* Setea el primer timeout */
		Cfg.GetItem("TimeOutDaemon", "MsgTimeOut2", aux_str1);
		LogAlarm.Put(0, "process_timeout(): Primer timeout\n");
		timeout_queue.SetOne(cont, atoi(aux_str1)); 
		
		/* Generar evento TimeOut */
		LogAlarm.Put(0, "process_timeout(): Generando evento TimeOut...\n");
		
		/* Completa la estructura del timeout */
		strcpy(timeout_event_data.key,timeout_data->id);
		strcpy(timeout_event_data.cod_ser,timeout_data->cod_ser);
		
		/* Obtiene el tipo de evento */
		switch (timeout_data->event_type)
		{
		case EVENT_REVTRXANUL:
			LogAlarm.Put(5,"process_timeout():  generando TimeOut de Reverso Anulacion \n");
			event_type=EVENT_TIMEOUT_REV_ANUL;
			break;
		case EVENT_SNDTRXANUL:
			LogAlarm.Put(5,"process_timeout():  Generando Timeout de Anulacion \n");
			event_type=EVENT_TIMEOUT_ANULACION;
			break;
		case EVENT_CIERRE:
			LogAlarm.Put(5, "process_timeout(): Generando TimeOut de cierre...\n");
			event_type=EVENT_TIMEOUT_CIERRE;
			break;
		case EVENT_CIERRE_B:
			LogAlarm.Put(5, "process_timeout(): Generando Timeout de cierre Batch\n");
			event_type=EVENT_TIMEOUT_CIERRE_B;
			break;
		case EVENT_BATCH:
			LogAlarm.Put(5, "process_timeout(): Generando Timeout en trx de batch upload\n");
			event_type=EVENT_TIMEOUT_BATCH;
			break;
		case EVENT_SNDTRXOFFLINE:
			LogAlarm.Put(5, "process_timeout(): Generando Timeout de Transaccion offline\n");
			event_type=EVENT_TMTTRXOFFLINE;
			break;
		case EVENT_SNDREVERSO:
			LogAlarm.Put(5, "process_timeout(): Generando Timeout de reverso\n");
			event_type=EVENT_TIMEOUT_REVERSO;
			break;
		case EVENT_ECHOTEST:
			LogAlarm.Put(5, "process_timeout(): Generando Timeout de echo test\n");
			
			event_type=EVENT_TIMEOUT_ECHOTEST;
			break;
		default:
			LogAlarm.Put(5, "process_timeout(): Generando Timeout generico\n");
			event_type=EVENT_TIMEOUT;
		}
		
		/* Envia el mensaje al Daemon */
		ret=EQueue.SendMsg(event_type, (char *)&timeout_event_data, sizeof(str_timeout_event));
		if (ret==NOOK)
		{
			LogAlarm.Put(5, "process_timeout: Error (%d) en Envio de Evento Timeout!!\n", EQueue.GetErrno());
		}
	}
	else  /* expired en 1 (ya se habia cumplido el 1er. timeout) */
	{
		/* Borra el timeout de la cola */
		LogAlarm.Put(0, "process_timeout: Segundo timeout. Borrando de la tabla...\n");
		timeout_queue.SetSecond(cont);
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | search_timeout                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Busca un timeout en la cola                                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void search_timeout()
{
    int	   cont;
    int	   ret;
	char   aux_str1[100];		
    
    /* Loguea la hora */
	LogAlarm.Put(0, "search_TO(): Revisando TO...%s",  currentTimeLog());
    
    /* Busca un time out */
    cont=timeout_queue.FindTO();
    if (cont > -1)
    {
        /* Procesa un time out */ 
		process_timeout(cont);
    }
	else
	{
		/* Espera para siguiente ejecucion */
		LogAlarm.Put(0, "search_TO(): Esperando...\n");
		Cfg.GetItem("TimeOutDaemon", "SleepTime", aux_str1);
		sleep(atoi(aux_str1));
		LogAlarm.Put(0, "search_timeout(): Listo.\n");
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Principal de la aplicacion                                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int main(int argc, char *argv[])
{
	/* Inicia las estructuras */
    if (init()==NOOK)
	{
		exit(1);
	}

    /* Repite el ciclo */
	while(1)
    {
		search_timeout();
    }

	/* Finaliza la aplicacion */
    shut_down();
	return OK;
}
