#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+------------------------------------------------------------------------------+
|          IECISA - PLATAFORMAS SECTORIALES - DESARROLLO DISTRIBUCION          |
|              Copyright (C) 1999 - INFORMATICA EL CORTE INGLES                |
+-----------------+------------------------------------------------------------+
| FUENTE          | Daemon.c                                                   |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso que atiende las peticiones de credito de las       |
|                 | sucursales                                                 |
+-----------------+------------------------------------------------------------+
*/
#include "Includes.h"
#include "CACStat.h"
#include "Config.h"
#include "Daemons.h"
#include "EventQueue.h"
#include "Log.h"
#include "PidTable.h"
#include "Queues.h"
#include "Table.h"
#include "TimeOutQueue.h"
#include "TraceNmbr.h"
#include "TrxHeap.h"
#include "Pos.h"
#include <malloc.h>

#define NUM_VERSION		"v1.54 16-Mar-2015 (3 min)"

extern "C" 
{
	#include "DBServices.h"
}

DataBase		DB;					/* Objeto de base de datos		*/
BatchTable		DBBatch;			/* Objeto DBBatch				*/
CACStatus		CACS;				/* CAChannel Status Flag		*/	
Config			Cfg(CONFIG_FILE);	/* Objeto de configuracion		*/
Event 			*nEvent;			/* Objeto de Eventos			*/
EventQueue		EventQ;				/* Cola de Eventos				*/
Log				LogAlarm;			/* Logging de actividades		*/
PidTable		PTable;				/* Tabla de PIDs				*/
Queue			XQueue;				/* Cola de X.25					*/
Queue			SQueue;				/* Cola de Sockets				*/
Queue			AQueue;				/* Cola Administrativa			*/
TimeOutQueue	timeout_queue; 		/* Cola de timeouts				*/
TraceNmbr		TraceNumber;		/* Objeto Trace Number			*/
TrxHeap			Heap;				/* Objeto Heap					*/
TrxTable		DBTrx;				/* Objeto DBTrx					*/
Pos             P;					/* Objeto de Pos				*/
Log				LogErrTerminal;		/* Logging errores terminales	*/


#ifdef DAEMON
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion principal de la aplicacion                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int main(int argc, char *argv[])
{
	int				ret, aux_int;
	char			aux_str1[100];	
	char			aux_str2[100];	
    long			TimeOutOpen = 20;      
    int				tipoEvento;     
	str_posdata     posdata;

	/* Recupera el timeout para apertura de colas */
	if (Cfg.GetItem("General", "OpenTimeout", aux_str1) == 0)
	{
		TimeOutOpen=atol(aux_str1); 
	}
	
	/* Recupera el nombre del archivo de log */
    if (Cfg.GetItem("VisaDaemon", "LogName", aux_str1) != 0)
	{
		printf("Error leyendo configuracion (LogName)\n");
	} 
	LogAlarm.Open(aux_str1);

    /* Loguea hora de comienzo */
    LogAlarm.Put( 0,"Daemon: Comienzo [%s] - %s", NUM_VERSION, currentTimeLog() ); 

    /* Recupera y setea el nivel de log */
	if (Cfg.GetItem("VisaDaemon", "LogLevel", aux_str1) != 0)
	{
		LogAlarm.Put( 0,"Daemon::main: Error leyendo configuracion (LogLevel)\n");
	} 
    LogAlarm.SetLevel(atoi(aux_str1));
    
    /* Recupera y abre el archivo de log para errores de terminales */
    if (Cfg.GetItem("VisaDaemon","LogErrTerminal", aux_str1) != 0)
    {
       sprintf(aux_str1,"ErrTerminal.log");
    }
    LogErrTerminal.Open(aux_str1);

    /* Chequea conexion a la base de datos */
    if (!DB.ready())
	{
		LogAlarm.Put( 0, "Daemon::Inicio: Error (%d) al conectarse a la BD.!!\n", DB.GetErrno());
	}

    /* Obtiene maxima cantidad de entradas para la tabla de pids */
	if (Cfg.GetItem("PIDTable", "MaxEntries", aux_str1) != 0)
	{
		LogAlarm.Put( 0, "Daemon::Inicio: Error leyendo configuracion (MaxEntries)!!\n");
	} 
    aux_int=atoi(aux_str1);

    /* Obtiene los permisos e id de la tabla de PIDs */
	ret = Cfg.GetItem("PIDTable", "ShMemID", aux_str1);
    ret |= Cfg.GetItem("PIDTable", "ShMemPerms", aux_str2);
    if (ret!=0)
	{
		LogAlarm.Put( 0, "Daemon::Inicio: Error (%d) leyendo configuracion (ShMemPerms)!!\n", ret);
    } 
    
	/* Crea cola de PIDs */
	LogAlarm.Put(5, "Daemon::main(): Creando tabla de PIDs...\n");
    ret=PTable.Create(atol(aux_str1), atoi(aux_str2), aux_int);
    if (ret==NOOK)
    {
		LogAlarm.Put(0,"Daemon::Inicio: Error (%02d) al crear tabla de Pids!!\n", PTable.GetErrno());
	}

    /* Obtiene los permisos e id de la estructura CACS */
	LogAlarm.Put(5, "Daemon::main(): Creando CACS flag...\n");
    ret = Cfg.GetItem("CACStatVisa", "ShMemID", aux_str1);
    ret |= Cfg.GetItem("CACStatVisa", "ShMemPerms", aux_str2);
	if (ret!=0)
	{
		LogAlarm.Put( 0, "Daemon::Inicio: Error (%d) leyendo configuracion (CACStatVisa)!!\n", ret);
	} 

	/* Crea estructura de PIDs */
	ret = CACS.Create(atol(aux_str1), atoi(aux_str2));
	if (ret==NOOK)
	{
		LogAlarm.Put(0, "Error (%d)!!\n", CACS.GetErrno());
		exit(1);
	}

    /* Obtiene los permisos e id de la cola de eventos */
	ret = Cfg.GetItem("VisaDaemon", "QueueID", aux_str1);
    ret |= Cfg.GetItem("VisaDaemon", "QueuePerms", aux_str2);
    if (ret!=0)
    {
		LogAlarm.Put( 1,"Daemon::Inicio: Error (%d) leyendo configuracion (QueuePerms)!!\n", ret);
	} 

    /* Abre la cola de eventos */
	LogAlarm.Put(5, "Daemon::main(): Creando cola de eventos...\n");
    ret = EventQ.Open(atol(aux_str1), atoi(aux_str2));
    if (ret==NOOK)
	{
		LogAlarm.Put( 0,"Daemon::Inicio: Error (%d) abriendo cola de eventos!!\n", EventQ.GetErrno());
		exit(1);
	}

	/* Obtiene id y max entradas de la cola de timeout */
	ret = Cfg.GetItem("TimeOutDaemon", "ShMemID", aux_str1);
	ret |= Cfg.GetItem("TimeOutDaemon", "MaxEntries", aux_str2);
	if (ret==NOOK)
	{
		LogAlarm.Put( 0, "Daemon::Inicio: Error (%d) leyendo configuracion!!\n", ret);
	}

	/* Abre cola de timeouts */
	LogAlarm.Put(5, "Daemon::main(): Abriendo cola de timeouts...\n");
	ret=timeout_queue.Open(atol(aux_str1), atoi(aux_str2),TimeOutOpen);
	if (ret==NOOK)
	{
		LogAlarm.Put( 0, "Daemon:: main(): Error (%d) abriendo cola de timeouts!!\n", timeout_queue.GetErrno());
		exit(1);
	}

	/* Abre cola de X25 */
	Cfg.GetItem("CAVisaDaemon", "QueueID", aux_str1);
	LogAlarm.Put(5, "Daemon::main(): Abriendo cola de X.25...\n");
	ret=XQueue.Open(atol(aux_str1), TimeOutOpen);
	if (ret==NOOK)
	{
		LogAlarm.Put( 0, "Daemon::main(): Error (%d) abriendo cola de X.25)!!\n", 
		XQueue.GetErrno());
		exit(1);
	}

	/* Crea cola de sockets */
	Cfg.GetItem("TCPDaemon", "QueueID", aux_str1);
	Cfg.GetItem("TCPDaemon", "QueuePerms", aux_str2);
	LogAlarm.Put( 5, "Daemon::main(): Creando cola de sockets...\n");
	ret = SQueue.Create(atoi(aux_str1), atoi(aux_str2));
	if (ret==NOOK)
	{
		LogAlarm.Put( 0, "Daemon::main(): Error (%d) al crear cola de sockets\n", SQueue.GetErrno());
		exit(1);
	}

    /* Actualiza estados en viaje */
	ret = DBRollSendTrx();

    /* Recupera un reverso */
	ret = fetch_posdata(&posdata);

	/* Se setean las cajas como ocupadas */
    while (ret==OK)
    {
		LogAlarm.Put( 0, "Daemon::Reverso pendiente. CA:[%s] Emp:[%s] Caj:[%s]\n",
									posdata.nro_ca, posdata.nro_suc, posdata.nro_caj);
		ret = P.SetPosBusy(posdata.nro_ca, posdata.nro_suc, posdata.nro_caj);
		if (ret!=OK)
		{
			LogAlarm.Put( 0, "Daemon::main(): Error al setear caja\n");
		}
		ret = fetch_posdata(&posdata);
    }

    /* CICLO PRINCIPAL DE LA APLICACION */
	do
    {
		/* Obtiene un evento */
		LogAlarm.Put(5, "Daemon::Esperando evento...\n");
		/*waitpid(-1, NULL, WNOHANG);*/
		nEvent=EventQ.GetNext();
		if (nEvent==NULL)
		{
			ret=EventQ.GetErrno();
			continue;
		}

		/* Procesa el evento */
		ret=nEvent->ProcessIt();
		
		/* Borra el evento */
		tipoEvento=nEvent->GetType();
		delete nEvent;    

    } while (tipoEvento!=EVENT_SHUTDOWN);
	/* FIN DEL CICLO */

    /* Cierra cola de eventos */
	LogAlarm.Put( 5, "Cerrando cola de eventos...\n");
    EventQ.Close();

	/* Cierra cola de timeouts */
    LogAlarm.Put( 5, "Cerrando cola de timeouts...\n");
    timeout_queue.Close();
  
	/* Termina todos los subprocesos */
    LogAlarm.Put( 5, "Terminando todos los subprocesos...\n");
    ret=PTable.KillAll();
    if (ret==NOOK)
    {
		LogAlarm.Put( 1, "Error (%d)!!\n", PTable.GetErrno());
    }
	
	/* Espera finalizacion de todos los subprocesos */
    sleep(2);

    /* Destruye tabla de pids */
	LogAlarm.Put( 5, "Destruyendo tabla de PIDs...\n");
    ret=PTable.Destroy();
    if (ret==NOOK)
    {
		LogAlarm.Put( 1, "Error (%d)!!\n", PTable.GetErrno());
    }

	/* Destruye estructura de CACS */
    LogAlarm.Put( 5, "Destruyendo CACS flag...\n");
    ret=CACS.Destroy();
    if (ret==NOOK)
    {
		LogAlarm.Put( 1, "Error (%d)!!\n", CACS.GetErrno());
    }

	/* Cierra cola de mensajes */
	LogAlarm.Put( 5, "Destruyendo cola de sockets\n");
	ret=SQueue.Destroy();
	if (ret==NOOK)
	{
		LogAlarm.Put( 5, "Error (%d) al cerrar cola de mensajes\n", SQueue.GetErrno());
	}

    /* Cierra los archivos de log */
	LogAlarm.Close();
    LogErrTerminal.Close();

	return OK;
}
#endif 

