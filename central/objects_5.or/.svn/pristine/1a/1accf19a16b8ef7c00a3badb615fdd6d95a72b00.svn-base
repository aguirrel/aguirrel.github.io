#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | SendRev.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso que envia al centro autorizador las transacciones  |
|                 | pendientes de envio                                        | 
+-----------------+------------------------------------------------------------+
*/
#include "CACStat.h"
#include "Config.h"
#include "Queues.h"
#include "Log.h"
#include "Daemons.h"
#include "Price.h"
#include "Sockets.h"
#include "PidTable.h"
#include "Table.h"

extern "C"
{
    #include "DBServices.h"
}

DataBase	DB;					/* Para login en Base de datos   */
CACStatus	CACS1;				/* CA Channel Status flag        */
CACStatus	CACS2;				/* CA Channel Status flag        */
Log			LogAlarm;			/* Logging de actividades        */
Queue		AQueue1;			/* Cola Administrativa           */
Queue		AQueue2;			/* Cola Administrativa           */
Queue		EQueue1;			/* Cola de eventos               */ 
Queue		EQueue2;			/* Cola de eventos               */
Config		Cfg1("credito.ini");/* Objeto de Configuracion       */
Config		Cfg2("credito2.ini");/* Objeto de Configuracion      */
PidTable	PTable1;			/* Tabla de PIDs                 */
PidTable	PTable2;			/* Tabla de PIDs                 */
int			NroCA;				/* Numero de Centro Autorizador  */

#define		NUM_VERSION			"1.37 28-Nov-2012"

#define		DAEMON_1			1
#define		DAEMON_2			2

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | piObtenerInstanciaDaemon                                   |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Determina a que instancia de Daemon debe enviarse el       |
|                 | mensaje                                                    |
+-----------------+------------------------------------------------------------+
*/
int piObtenerInstanciaDaemon(char *szNroSuc)
{
	int iInstancia = DAEMON_1;
	char szSuc[16];
	char szRta[16];

	sprintf(szSuc, "Suc%s", szNroSuc);
	if ( Cfg2.GetItem("HomologacionAmex", szSuc, szRta) == CFG_FOUND )
	{
		if (szRta[0]=='S')
		{
			iInstancia = DAEMON_2;
		}
	}
	return (iInstancia);
}		
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvShutDown                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Finaliza la aplicacion                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void pvShutDown(int sig_id=0)
{
    int		iRet;

    LogAlarm.Put(0, "SendRev: Finaliza aplicacion. CA:%02d\n", NroCA);  
	
	/* */
	iRet= CACS1.NotPing(NroCA);
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al actualizar canal\n", CACS1.GetErrno());
    }

	/* */
	iRet= CACS2.NotPing(NroCA);
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al actualizar canal\n", CACS2.GetErrno());
    }

    /* Cierra cola administrativa */
	iRet= AQueue1.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al cerrar cola administrativa\n", AQueue1.GetErrno());
    }

    /* Cierra cola administrativa */
	iRet= AQueue2.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al cerrar cola administrativa\n", AQueue2.GetErrno());
    }

    /* Borra PID de la tabla */
	iRet= PTable1.RemPid(getpid());
    if (iRet==NOOK)
    {
		LogAlarm.Put(0, "SendRev: Error (%d) al borrar PID\n", PTable1.GetErrno());
    }

	iRet= PTable2.RemPid(getpid());
    if (iRet==NOOK)
    {
		LogAlarm.Put(0, "SendRev: Error (%d) al borrar PID\n", PTable2.GetErrno());
    }

    /* Cierra cola de eventos */
    iRet= EQueue1.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al cerrar cola de eventos\n", EQueue1.GetErrno());
    }

    /* Cierra cola de eventos */
    iRet= EQueue2.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al cerrar cola de eventos\n", EQueue2.GetErrno());
    }

    /* Cierra objeto del canal */
    iRet= CACS1.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al cerrar CACS1 flag\n", CACS1.GetErrno());
    }
        
    /* Cierra objeto del canal */
    iRet= CACS2.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendRev: Error (%d) al cerrar CACS2 flag\n", CACS2.GetErrno());
    }
    
	LogAlarm.Put(0, "SendRev: Proceso finalizado\n\n");

	/* Cierra el archivo de log */
	LogAlarm.Close();

	/* Retorna el resultado */
    exit(abs(sig_id));
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvInit                                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Inicia la aplicacion                                       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void pvInit()
{
	int 	iRet;
	char    QID[100];       
	char    MID[100];       
	char    QPerms[100];
	char	szCacs[100];
	char	szLogSend[100];	
	char	MaxEntries[20]; 

	/* Abre y configura el archivo de log */
	Cfg1.GetItem("AdminDaemon", "LogSend", szLogSend);
	LogAlarm.Open(szLogSend);
	LogAlarm.SetLevel(10);
	LogAlarm.SetDebugLevel(10);

    LogAlarm.Put(0, "SendRev: Inicia aplicacion [%s]. CA:%02d. %s", NUM_VERSION, NroCA, currentTimeLog());  

	/* Setea las alarmas */
	signal(SIGTERM, pvShutDown);
	signal(SIGINT,  pvShutDown);
	signal(SIGALRM, pvShutDown);

	/* Crea la cola administrativa para recibir las respuestas */
	Cfg1.GetItem("AdminDaemon", "QueueID",    QID);
	Cfg1.GetItem("AdminDaemon", "QueuePerms", QPerms);
	iRet= AQueue1.Create(atol(QID), atoi(QPerms));
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendRev: Error (%d) al crear cola administrativa.\n", AQueue1.GetErrno());
		pvShutDown();
	}      

	/* Crea la cola administrativa para recibir las respuestas */
	Cfg2.GetItem("AdminDaemon", "QueueID",    QID);
	Cfg2.GetItem("AdminDaemon", "QueuePerms", QPerms);
	iRet= AQueue2.Create(atol(QID), atoi(QPerms));
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendRev: Error (%d) al crear cola administrativa.\n", AQueue2.GetErrno());
		pvShutDown();
	}      

	/* Abre cola de eventos */
	Cfg1.GetItem("VisaDaemon", "QueueID", QID);
	while (EQueue1.Open(atol(QID))==NOOK)
	{
		if (EQueue1.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendRev: Error (%d) al abrir cola de eventos\n", EQueue1.GetErrno());
			pvShutDown();
		}
	}

	/* Abre cola de eventos */
	Cfg2.GetItem("VisaDaemon", "QueueID", QID);
	while (EQueue2.Open(atol(QID))==NOOK)
	{
		if (EQueue2.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendRev: Error (%d) al abrir cola de eventos\n", EQueue2.GetErrno());
			pvShutDown();
		}
	}

	/* Abre tabla de PIDs */
	Cfg1.GetItem("PIDTable", "ShMemID", MID);
	Cfg1.GetItem("PIDTable", "MaxEntries", MaxEntries);
	while (PTable1.Open(atol(MID), atoi(MaxEntries))==NOOK)
	{
		if (PTable1.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendRev: Error (%d) al abrir tabla de PID\n", PTable1.GetErrno());
			pvShutDown();
		}
	}

	/* Abre tabla de PIDs */
	Cfg2.GetItem("PIDTable", "ShMemID", MID);
	Cfg2.GetItem("PIDTable", "MaxEntries", MaxEntries);
	while (PTable2.Open(atol(MID), atoi(MaxEntries))==NOOK)
	{
		if (PTable2.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendRev: Error (%d) al abrir tabla de PID\n", PTable2.GetErrno());
			pvShutDown();
		}
	}

	/* Agrega PID a la tabla */
	iRet= PTable1.PutPid(getpid());
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendRev: Error (%d) al agregar PID a la tabla\n", PTable1.GetErrno());
		pvShutDown();
	}

	/* Agrega PID a la tabla */
	iRet= PTable2.PutPid(getpid());
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendRev: Error (%d) al agregar PID a la tabla\n", PTable2.GetErrno());
		pvShutDown();
	}

	/* Abre objeto CACS */
	Cfg1.GetItem("CACStatVisa", "ShMemID", szCacs);
	while (CACS1.Open(atol(szCacs))==NOOK)
	{
		if (CACS1.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendRev: Error (%d) al abrir CACS1\n", CACS1.GetErrno());
			pvShutDown();
		}
	}

	/* Abre objeto CACS */
	Cfg2.GetItem("CACStatVisa", "ShMemID", szCacs);
	while (CACS2.Open(atol(szCacs))==NOOK)
	{
		if (CACS2.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendRev: Error (%d) al abrir CACS2\n", CACS2.GetErrno());
			pvShutDown();
		}
	}

	/* Actualiza CACS flags */
	iRet= CACS1.Ping(NroCA);
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendRev: Error (%d) al actualizar CACS1\n", CACS1.GetErrno());
	}

	/* Actualiza CACS flags */
	iRet= CACS2.Ping(NroCA);
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendRev: Error (%d) al actualizar CACS2\n", CACS2.GetErrno());
	}

	/* Declara cursor de movimientos */
	declare_cod_tar_cursor();
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | piCanalesLevantados                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Devuelve la cantidad de canales levantados del centro      |
|                 | autorizador                                                |
+-----------------+------------------------------------------------------------+
*/
int piCanalesLevantados(char *nro_ca, int iInstancia)
{
	int	 iRet;
	char szComando[256];

	if ( iInstancia == DAEMON_1 )
	{
		sprintf(szComando, "%s %s 1>/dev/null 2>/dev/null", "CfgX25", nro_ca);
	}
	else
	{
		sprintf(szComando, "%s %s 1>/dev/null 2>/dev/null", "CfgX25_2", nro_ca);
	}
	iRet = system(szComando);
	iRet = (iRet>64) ? 0 : iRet;
	LogAlarm.Put(0, "SendRev: Verificacion canales CA:[%s]. Rta:[%02d]\n", nro_ca, iRet);
	return (iRet);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | SendTrx                                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia las transacciones pendientes al Daemon, para que las |
|                 | mande al centro autorizador                                | 
+-----------------+------------------------------------------------------------+
*/
int SendTrx(char *nro_ca, char *cod_tar, char *cod_est, int event_type)
{
	char    RstEventData[175];
	char    RtaData[50];      
	char	CajaCa[9];
	char	NroTrx[9];
	char	NroSuc[6];
	int     iRet;  
	int		iGetErrno;
	int		iInstancia;

	/* Abre cursor de transacciones pendientes de envio (offline o reversos) */
	iRet=DBOpenCurTPVOffline(cod_tar,cod_est);
		
	/* Verifica el resultado de la apertura */
	if (iRet != OK)
	{
		LogAlarm.Put(0, "SendRev: Error al declarar cursor (%d)\n", iRet);
		return iRet;
	}

	/* Obtiene primera caja para enviar */
	iRet = DBGetNextTPVOffline(CajaCa, NroTrx, NroSuc);
	if (iRet==OK)
	{
		iInstancia = piObtenerInstanciaDaemon(NroSuc);
	}

	/* Si no hay registros retorna bien */
	if (abs(iRet)==1403)
	{
		LogAlarm.Put(0, "SendRev: No hay transacciones para enviar\n");
		return OK;
	}

	/* Repite mientras haya cajas para enviar */
	while (iRet == OK)
	{
		/* Arma mensaje de evento */
		sprintf(RstEventData, "2000%02s%08s%04ld", cod_tar, CajaCa, atol(NroTrx));
		LogAlarm.Put(0, "SendRev: Evento a Suc %s, Daemon %d, Mensaje:[%s]\n", NroSuc, iInstancia, RstEventData );

		/* Envia mensaje al Daemon para que procese */
		if ( iInstancia == DAEMON_1 )
		{
			iRet = EQueue1.SendMsg(event_type, RstEventData, strlen(RstEventData));
			iGetErrno = EQueue1.GetErrno();
		}
		else
		{
			iRet = EQueue2.SendMsg(event_type, RstEventData, strlen(RstEventData));
			iGetErrno = EQueue2.GetErrno();
		}
		if (iRet==NOOK)
		{
			LogAlarm.Put(0, "SendRev: Error (%d) al enviar mensaje\n", iGetErrno);
		}
		else
		{
			/* Espera la respuesta del daemon */
			LogAlarm.Put(0, "SendRev: Espera respuesta del daemon\n");
			memset(RtaData, 0, sizeof(RtaData));
			alarm(60);
			if ( iInstancia == DAEMON_1 )
			{
				iRet = AQueue1.GetMsg(getpid(), RtaData, sizeof(RtaData));
				iGetErrno = AQueue1.GetErrno();
			}
			else
			{
				iRet = AQueue2.GetMsg(getpid(), RtaData, sizeof(RtaData));
				iGetErrno = AQueue2.GetErrno();
			}
			alarm(0);
			
			/* Si no hay respuesta */
			if (iRet==NOOK)
			{
				LogAlarm.Put(0, "SendRev: Error (%d) al recibir respuesta por cola de mensajes\n", iGetErrno);
				iRet = E_TOUT;
			}

			/* Si respuesta es correcta */
			if (strncmp(RtaData, "ACK", 3)==0)
			{
				/* Levanta el canal */
				LogAlarm.Put(0, "SendRev: Respuesta ACK. CA:[%02d]\n", NroCA);
				if ( iInstancia == DAEMON_1 )
				{
					iRet = CACS1.Up(NroCA);
					iGetErrno = CACS1.GetErrno();
				}
				else
				{
					iRet = CACS2.Up(NroCA);
					iGetErrno = CACS2.GetErrno();
				}
				if (iRet==NOOK)
				{
					LogAlarm.Put(0, "SendRev: Error (%d) al actualizar estado de canal\n", iGetErrno);
				}
			}

			/* Si la caja esta bloqueada */
			if (strncmp(RtaData,"BLOCK", 5)==0)
			{
				/* Levanta el canal */
				LogAlarm.Put(0, "SendRev: Caja Bloqueada: [%s]\n", CajaCa);
				if ( iInstancia == DAEMON_1 )
				{
					iRet=CACS1.Up(NroCA);
					iGetErrno = CACS1.GetErrno();
				}
				else
				{
					iRet=CACS2.Up(NroCA);
					iGetErrno = CACS2.GetErrno();
				}
				if (iRet==NOOK)
				{
					LogAlarm.Put(0, "SendRev: Error (%d) al actalizar estado de canal\n", iGetErrno);
				}
			}
			
			/* Si hay TimeOut */
			if (strncmp(RtaData, "TOUT", 4)==0)
			{
				/* Si hay canales levantados continua iterando */
				LogAlarm.Put(0, "SendRev: Time Out al esperar respuesta\n");
				if ( piCanalesLevantados(nro_ca, iInstancia) >0 )
				{
					LogAlarm.Put(0, "SendRev: Se continua iterando\n");
					iRet = OK;
				}
				else
				{
					LogAlarm.Put(0, "SendRev: Sin canales disponibles\n");
					iRet = E_TOUT;
				}
			}

			/* Si hay error en el mensaje */
			if (strncmp(RtaData, "ERROR", 5)==0)
			{
				LogAlarm.Put(0, "SendRev: Error al procesar caja [%s]\n", &RtaData[6]);
			}
		}
	
		/* Obtiene la siguiente caja */
		if (iRet != E_TOUT)
		{
			iRet = DBGetNextTPVOffline(CajaCa, NroTrx, NroSuc);
			if (iRet==OK)
			{
				iInstancia = piObtenerInstanciaDaemon(NroSuc);
			}
		}

	} /* end del while */

	/* Cierra el cursor */
	DBCloseCurTPVOffline();
	
	return ( iRet==E_TOUT ? E_TOUT : OK );
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion principal de la aplicacion                         |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int main(int argc, char *argv[])
{
	int			iRet = 0;
	char		nro_ca[3];
	char		cod_tar[3];
	char		cod_est[3];

	/* Verifica correccion en parametros */
	if ((argc!=2)||(strlen(argv[1])>2))
	{
		printf("\nuso:\tSendRev <Numero CA>\n\tNumero CA - Numero de Centro Autorizador\n\n");
		exit(1);
	}

	/* Obtiene el numero de centro autorizador */
	sprintf(nro_ca, argv[1]) ;
	NroCA=atoi(nro_ca);

	/* Inicia variables de configuracion */
	pvInit();

	/****************************/
	/* Envia todos los reversos */
	/****************************/

	LogAlarm.Put(0, "SendRev: Inicia envio de reversos. CA:[%02d]\n", NroCA);
	do
	{
		/* Obtiene siguiente codigo de tarjeta para ese centro autorizador */
		iRet = getNextCodTar(cod_tar, nro_ca);  
		if (iRet == OK)
		{
			/* Procesa el codigo de tarjeta */
			LogAlarm.Put(0, "SendRev: Procesa codigo de tarjeta: %02s\n", cod_tar);
			sprintf(cod_est, "%02d", E_TOUT);
			iRet = SendTrx(nro_ca, cod_tar, cod_est, EVENT_SNDREVERSO);
			if (iRet != OK)
			{
				if (iRet == E_TOUT)
				{
					LogAlarm.Put(0, "SendRev: Error de vinculo con centro autorizador CA:[%s]\n", nro_ca);
				}
				else
				{
					LogAlarm.Put(0, "SendRev: Error al procesar CA:[%s] Tar:[%s]\n\n", nro_ca, cod_tar);
				}
			}
		} 
	} while (iRet == OK);
	if (iRet==1403) iRet = 0;
	LogAlarm.Put(0, "SendRev: Finaliza envio de reversos. Retorno %d\n", iRet );

	/* Finaliza el proceso si hay error */
	if (iRet!=0)
	{
		pvShutDown();
	}

	/******************************/
	/* Envia todos los pendientes */
	/******************************/
	
	LogAlarm.Put(0, "SendRev: Inicia envio de pendientes. CA:[%02d]\n", NroCA);
	do
	{
		/* Obtiene siguiente codigo de tarjeta para ese centro autorizador */
		iRet = getNextCodTar(cod_tar, nro_ca);  
		if (iRet == OK)
		{
			/* Procesa el codigo de tarjeta */
			LogAlarm.Put(0, "SendRev: Procesa codigo de tarjeta: %02s\n", cod_tar);
			sprintf(cod_est, "%02d", E_PEND);
			iRet = SendTrx(nro_ca, cod_tar, cod_est, EVENT_SNDTRXOFFLINE);
			if (iRet != OK)
			{
				if (iRet == E_TOUT)
				{
					LogAlarm.Put(0, "SendRev: Error de vinculo con centro autorizador CA:[%s]\n", nro_ca);
				}
				else
				{
					LogAlarm.Put(0, "SendRev: Error al procesar CA:[%s] Tar:[%s]\n\n", nro_ca, cod_tar);
				}
			}
		} 
	} while (iRet == OK);
	if (iRet==1403) iRet = 0;
	LogAlarm.Put(0, "SendRev: Finaliza envio de pendientes. Retorno %d\n", iRet );

	/* Finaliza el proceso */
	pvShutDown();
	return OK;
}
