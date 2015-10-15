#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | SendPing.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso que genera evento de EchoTest para verificar el    |
|                 | vinculo con el centro autorizador                          | 
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

#define MAX_REINTENTOS		3
#define ESPERA_REINTENTOS	10

CACStatus	CACS;				/* CA Channel Status flag        */
Log			LogAlarm;			/* Logging de actividades        */
Queue		AQueue;				/* Cola Administrativa           */
Queue		EQueue;				/* Cola de eventos               */ 
Config		Cfg(CONFIG_FILE);	/* Objeto de Configuracion       */
PidTable	PTable;				/* Tabla de PIDs                 */
int			NroCA;				/* Numero de Centro Autorizador  */

#define		NUM_VERSION			"v1.46 02-Jul-2013 (3 min)"

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

    LogAlarm.Put(0, "SendPing: Finaliza aplicacion. CA:%02d\n", NroCA);  
	
	/* */
	iRet= CACS.NotPing(NroCA);
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendPing: Error (%d) al actualizar canal\n", CACS.GetErrno());
    }

    /* Cierra cola administrativa */
	iRet= AQueue.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendPing: Error (%d) al cerrar cola administrativa\n", AQueue.GetErrno());
    }

    /* Borra PID de la tabla */
	iRet= PTable.RemPid(getpid());
    if (iRet==NOOK)
    {
		LogAlarm.Put(0, "SendPing: Error (%d) al borrar PID\n", PTable.GetErrno());
		exit(1);
    }

    /* Cierra cola de eventos */
    iRet= EQueue.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendPing: Error (%d) al cerrar cola de eventos\n", AQueue.GetErrno());
    }

    /* Cierra objeto del canal */
    iRet= CACS.Close();
    if (iRet==NOOK)
    {
        LogAlarm.Put(0, "SendPing: Error (%d) al cerrar CACS flag\n", CACS.GetErrno());
    }
    LogAlarm.Put(0, "SendPing: Proceso finalizado\n");
    
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
	Cfg.GetItem("AdminDaemon", "LogName", szLogSend);
	LogAlarm.Open(szLogSend);
	LogAlarm.SetLevel(10);
    LogAlarm.Put(0, "SendPing: Inicia aplicacion [%s]. CA:%02d. %s", NUM_VERSION, NroCA, currentTimeLog());  
   
	/* Setea las alarmas */
	signal(SIGTERM, pvShutDown);
	signal(SIGINT,  pvShutDown);
	signal(SIGALRM, pvShutDown);

	/* Crea la cola administrativa para recibir las respuestas */
	Cfg.GetItem("AdminDaemon", "QueueID",    QID);
	Cfg.GetItem("AdminDaemon", "QueuePerms", QPerms);
	iRet= AQueue.Create(atol(QID), atoi(QPerms));
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendPing: Error (%d) al crear cola administrativa.\n", AQueue.GetErrno());
		pvShutDown();
	}      

	/* Abre cola de eventos */
	Cfg.GetItem("VisaDaemon", "QueueID", QID);
	while (EQueue.Open(atol(QID))==NOOK)
	{
		if (EQueue.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendPing: Error (%d) al abrir cola de eventos\n", EQueue.GetErrno());
			pvShutDown();
		}
	}

	/* Abre tabla de PIDs */
	Cfg.GetItem("PIDTable", "ShMemID", MID);
	Cfg.GetItem("PIDTable", "MaxEntries", MaxEntries);
	while (PTable.Open(atol(MID), atoi(MaxEntries))==NOOK)
	{
		if (PTable.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendPing: Error (%d) al abrir tabla de PID\n", PTable.GetErrno());
			pvShutDown();
		}
	}

	/* Agrega PID a la tabla */
	iRet= PTable.PutPid(getpid());
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendPing: Error (%d) al agregar PID a la tabla\n", PTable.GetErrno());
		pvShutDown();
	}

	/* Abre objeto CACS */
	Cfg.GetItem("CACStatVisa", "ShMemID", szCacs);
	while (CACS.Open(atol(szCacs))==NOOK)
	{
		if (CACS.GetErrno()!=ENOENT)
		{
			LogAlarm.Put(0, "SendPing: Error (%d) al abrir CACS\n", CACS.GetErrno());
			pvShutDown();
		}
	}

	/* Actualiza CACS flags */
	iRet= CACS.Ping(NroCA);
	if (iRet==NOOK)
	{
		LogAlarm.Put(0, "SendPing: Error (%d) al actualizar CACS\n", CACS.GetErrno());
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | SendPing                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje de echo test al centro autorizador para   |
|                 | determinar si existe vinculo                               | 
+-----------------+------------------------------------------------------------+
*/
int piSendPing(str_ping_data ping_data)
{
	int			iContReintentos=0;      
	char		EventDataRta[128]; 
	int			iRet;              
	int 		iReintentar= TRUE;

	do
	{
		/* Envia mensaje de echo test */
		LogAlarm.Put(0, "SendPing: Envia mensaje de echo test\n"); 
		printf("SendPing: Envia mensaje de echo test\n");
		iRet=EQueue.SendMsg(EVENT_ECHOTEST, (char *)&ping_data, sizeof(ping_data));
		LogAlarm.Put(0, "SendPing: Valor de retorno:[%ld]\n", iRet); 

		/* Espera la respuesta */
		LogAlarm.Put(0, "SendPing: Espera respuesta por cola administrativa\n");
		memset(EventDataRta, 0, sizeof(EventDataRta));
		alarm(120);
		iRet=AQueue.GetMsg(getpid(), EventDataRta, sizeof(EventDataRta));
		alarm(0);

		/* Si no se recibe respuesta */
		if (iRet==NOOK)
		{
			printf("SendPing: Error al recibir respuesta\n");
			LogAlarm.Put(0, "SendPing: Error (%d) al recibir respuesta\n", AQueue.GetErrno());
			iReintentar = FALSE;
		}
		else
		{
			/* Loguea la respuesta */
			LogAlarm.Put(0, "SendPing: Respuesta recibida:[%s]\n", EventDataRta);
		}

		/* Si la caja esta ocupada */
		if (strncmp(EventDataRta, "ERROR", 5)==0)
		{
			printf("SendPing: Error. Caja ocupada o con problemas\n"); 
			LogAlarm.Put(0, "SendPing: Error. Caja ocupada o con problemas\n"); 
			iReintentar = FALSE;
		}

		/* Si la respuesta es correcta */
		if (strncmp(EventDataRta, "ACK", 3)==0)
		{
			printf("SendPing: Respuesta ACK. CA:[%02d]\n", NroCA);
			LogAlarm.Put(0, "SendPing: Respuesta ACK. CA:[%02d]\n", NroCA);

			/* Levanta el canal */
			iRet=CACS.Up(NroCA);
			if (iRet==NOOK)
			{
				LogAlarm.Put(0, "SendPing(): Error (%d) al actualizar CACS\n", CACS.GetErrno());
			}
			iReintentar = FALSE;
		}

		/* Si la respuesta no es correcta */
		if (strncmp(EventDataRta, "NACK", 4)==0)
		{
			iContReintentos++;
			printf("SendPing: TIME OUT\n");
			LogAlarm.Put(0, "SendPing: Respuesta: TIME OUT\n");
			printf("SendPing: Espera %d segundos para reintentar\n", ESPERA_REINTENTOS);
			LogAlarm.Put(0, "SendPing: Espera %d segundos para reintento\n", ESPERA_REINTENTOS);
			sleep(ESPERA_REINTENTOS);
		}
	} while ((iReintentar) && (iContReintentos < MAX_REINTENTOS));
	
	/* Retorna la respuesta*/
	if (iContReintentos >= MAX_REINTENTOS)
	{
		printf("SendPing: Superado los reintentos de Ping\n");
		LogAlarm.Put(0, "SendPing: Superado los reintentos de Ping\n");
		return NOOK;
	}
	else
	{
		return OK;
	}
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
	int				iRet=0;
	char			szNroCA[3];
	char			szNroSuc[32];		
	char			szNroCaj[32];	
	str_ping_data	ping_data;

	/* Verifica correccion en parametros */
	if ((argc!=2)||(strlen(argv[1])>2))
	{
	    printf("\nuso:\tSendPing <Numero CA>\n\tNumero CA - Numero de Centro Autorizador\n\n");
		exit(1);
	}

	/* Obtiene el numero de centro autorizador */
	memset(szNroCA, 0, sizeof(szNroCA));
	strncpy(szNroCA, argv[1], sizeof(szNroCA)-1);
	NroCA=atoi(szNroCA);

	/* Inicia variables de configuracion */
	pvInit();

	/* Obtiene datos de la terminal definida para Ping */
	Cfg.GetItem("AdminDaemon", "NroSucTest", szNroSuc);
	Cfg.GetItem("AdminDaemon", "NroCajTest", szNroCaj);

	/* Copia a estructura */
	strcpy(ping_data.nro_ca  , szNroCA);
	strcpy(ping_data.nro_suc , szNroSuc);
	strcpy(ping_data.nro_caj , szNroCaj);
	LogAlarm.Put(0, "SendPing: Datos de Test: CA:[%s] Suc:[%s] Caja:[%s]\n", szNroCA, szNroSuc, szNroCaj);

	/* Envia Pings para verificar el vinculo */
	iRet= piSendPing(ping_data);
	
	/* Finaliza la aplicacion */
	pvShutDown(iRet);
	return OK;
}
