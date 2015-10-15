#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+------------------------------------------------------------------------------+
|          IECISA - PLATAFORMAS SECTORIALES - DESARROLLO DISTRIBUCION          |
|              Copyright (C) 1999 - INFORMATICA EL CORTE INGLES                |
+-----------------+------------------------------------------------------------+
| FUENTE          | TCPDaemon.c                                                |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso que atiende las peticiones de credito de las       |
|                 | sucursales                                                 |
+-----------------+------------------------------------------------------------+
*/
#include "Daemons.h"
#include "Config.h"
#include "Log.h"
#include "PidTable.h"
#include "Queues.h"
#include "Sockets.h"
#include "QChild.h"

#define NUM_VERSION		"v1.46 02-Jul-2013 (3 min)"

#define HEADER_SIZE		(sizeof(szHeader)-1)

/* Declaracion de prototipos */
int piProcesaMensaje();

Config		Cfg(CONFIG_FILE);	/* Objeto de configuracion       */
Log         LogAlarm;			/* Log de actividades            */
PidTable	PTable;				/* Tabla de PIDs                 */
Queue		SQueue;				/* Cola de mensajes de lectura   */
Queue		EQueue;				/* Cola de mensajes de escritura */
Socket		LSocket;            /* Socket desde sucursal         */
Socket		RSocket;			/* Socket hacia sucursal         */
Socket		*inSocket;          /* Socket de lectura             */
QChild      QHijos;             /* Objeto contador de hijos      */
int         TOPEHIJOS;          /* Cantidad maxima de hijos      */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvFinHijo                                                  |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Actualiza el contador de hijos ante una interrupcion       |
|                 | externa hacia los procesos hijos                           |
+-----------------+------------------------------------------------------------+
*/
void pvFinHijo (int sig_id=0)
{
	int iRet;
	int iNroPid;

	iNroPid = getpid();
	LogAlarm.Put( 0, "TCPDaemon[%09d]: Termina este proceso hijo por alarma\n", iNroPid);
	iRet=QHijos.DecQChild();
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: ERROR al decrementar QHijos. \n", iNroPid);
	}
	QHijos.Close();
	exit(0);
	return;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | piProcesaMensaje                                           |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Procesa una peticion de credito. Lee el mensaje recibido   |
|                 | del TPV, lo envia a procesar, espera respuesta y contesta  |
+-----------------+------------------------------------------------------------+
*/
int  piProcesaMensaje()
{
	int		iRet;
	char	TipoMensajePrice[5];
	int		iCodMsg;
	int		iCont;
	int		iLongitud;
	char    szHeaderLen[5];
	char    szHeaderAux[3];
	char	szAuxStr1[100];	
	char	szAuxStr2[100];	
	char	szData[2048];
	char	szHeader[7];
	int		iNroPid;
	int		iTamResta=0;
	
	/* Setea las alarmas para hacer exit en el caso de terminarse el tiempo */
	signal(SIGINT , pvFinHijo );
	signal(SIGTERM, pvFinHijo );
	signal(SIGALRM, pvFinHijo );

	/* Obtiene el numero de pid */
	iNroPid = getpid();

	/* Loguea comienzo de ejecucion */
	LogAlarm.Put(0, "TCPDaemon[%09d]: Comienza proceso de mensaje. %s", iNroPid, currentTimeLog());

	/* Setea el timeout para matar el proceso si es necesario */
	Cfg.GetItem("TCPDaemon", "TimeToLive", szAuxStr1);
	sprintf(szAuxStr2, "30"); /* Valor minimo por defecto */
	if (atoi(szAuxStr1)>30) 
	{
		sprintf(szAuxStr2, szAuxStr1);
	}
	LogAlarm.Put(5,"TCPDaemon[%09d]: Tiempo maximo de ejecucion %d segundos\n", iNroPid, atoi(szAuxStr2));
	alarm(atoi(szAuxStr2));

	/* Agrega el PID a la tabla */
	LogAlarm.Put(5, "TCPDaemon[%09d]: Agrega proceso a tabla de PIDs\n", iNroPid);
	iRet=PTable.PutPid(iNroPid);
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) al agregar PID\n", iNroPid, PTable.GetErrno());
		return NOOK;
	}

	/* Cierra el socket del padre */
	iRet=LSocket.Close();
	
	/* Lee header del mensaje */
	LogAlarm.Put( 0, "TCPDaemon[%09d]: Conecta a sucursal %s:%d\n", 
						iNroPid, inSocket->GetRemoteAddress(), inSocket->GetRemotePort());
	memset(szHeader, 0, sizeof(szHeader));
	iRet=inSocket->Read(szHeader, HEADER_SIZE);
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) en lectura de header\n", iNroPid, inSocket->GetErrno());
		PTable.RemPid(iNroPid);
		return NOOK;
	}
	LogAlarm.Put( 5, "TCPDaemon[%09d]: Header: '%s'. Longitud: %d\n", iNroPid, szHeader, (atoi(szHeader+2))-HEADER_SIZE); 

	/* Obtiene el entorno donde responder */
	memset(szHeaderAux, 0, sizeof(szHeaderAux));
	strncpy(szHeaderAux, szHeader, 2); 

	/* Lee el mensaje de credito */
	iRet=inSocket->Read(szData, (atoi(szHeader+2))-HEADER_SIZE);
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) en lectura de mensaje\n", iNroPid, inSocket->GetErrno());
		PTable.RemPid(iNroPid);
		return NOOK;
	}
#define USA_PINPAD	1
#ifdef USA_PINPAD
	/* QUITAR CUANDO TODAS LAS TERMINALES TENGAN LA APLICACION DE PINPAD */
	else
	{
		/* Pinpad nulo para comparacion */
		char szNullPinPad[9];
		memset(szNullPinPad, 0, sizeof(szNullPinPad));

		/* Tipo de mensaje */
		sprintf(TipoMensajePrice,"%4.4s",szData+31); 
		
		/* Reverso en formato viejo */
		if ( ( iRet==184 ) &&
			 ( (atoi(TipoMensajePrice)==1420) || (atoi(TipoMensajePrice)==1421) ) 
		   )
		{
			strncpy(szData+184, "00000000" , 8);
			strncpy(szHeader+2, "0198"     , 4);
			iRet= 192;
		}
		/* Reverso en formato nuevo no completado a "00000000" como debe ser */
		if ( ( iRet==192 ) &&		
			 ( (atoi(TipoMensajePrice)==1420) || (atoi(TipoMensajePrice)==1421) ) &&
			 ( (strncmp(szData+184, "        ", 8)==0) || (strncmp(szData+184, szNullPinPad, 8)==0) )
		   )
		{
			strncpy(szData+184, "00000000" , 8);
		}
		/* Reverso sin codigo de cuenta, se agrega */
		if ( ( iRet==192 ) &&		
			 ( (atoi(TipoMensajePrice)==1420) || (atoi(TipoMensajePrice)==1421) ) 
		   )
		{
			strncpy(szData+192, "1" , 1);
			strncpy(szHeader+2, "0199" , 4);
			iRet= 193;
		}

		/* Peticion 1200 no completado a "00000000" como debe ser */
		if ( ( ( iRet==267 ) || ( iRet==268 ) ) &&		
			 ( (atoi(TipoMensajePrice)==1200) ) &&
			 ( (strncmp(szData+144, "        ", 8)==0) || (strncmp(szData+144, szNullPinPad, 8)==0) )
		   )
		{
			strncpy(szData+144, "00000000" , 8);
		}
		/* Si el mensaje viene sin el campo de código de cuenta para pinpad, se agrega */
		if (iRet==267)
		{
			strncpy(szData+267, "1", 1);
			strncpy(szHeader+2, "0274", 4);
		}
		/* Si el mensaje viene sin el campo de importe cashback, se agrega */
		if ( (iRet==268) && (atoi(TipoMensajePrice)==1200) )
		{
			strncpy(szData+268, "000000000000", 12);
			strncpy(szHeader+2, "0286", 4);
		}
	}
#endif
	/* Cierra el socket */
	LogAlarm.Put( 5, "TCPDaemon[%09d]: Cierra socket con sucursal\n", iNroPid);
	iRet=inSocket->Close();
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) en cierre de socket\n", iNroPid, inSocket->GetErrno());
		PTable.RemPid(iNroPid);
		return NOOK; 
	}

	/* Se obtiene el código de mensaje Price, para enviarlo al Servidor */
	iTamResta = 0;
	sprintf(TipoMensajePrice,"%4.4s",szData+31);
	switch(atoi(TipoMensajePrice))
	{
		case 1200: /* Venta On-line */
			iCodMsg=EVENT_PRICEONLINE;
//			/* Si no usa campos 34 y 63 en la respuesta se corta la longitud del mensaje */
//			/* para no enviar estos dos campos y mantener compatibilidad con version 220 */
//			if ( (strncmp(szData+180, "01", 2)!=0) &&
//				 (strncmp(szData+180, "02", 2)!=0) )
//			{
//				iTamResta = 28 + 40;
//				LogAlarm.Put( 0, "TCPDaemon[%09d]: iTamResta:[%02d]\n", iNroPid, iTamResta);
//			}	

#ifndef EMPRESA_3
			strncpy(szData+128, "001", 3); /* QUITAR AL IMPLEMENTAR EMPRESA 3 */
#endif
			break;
		case 1220: /* Venta Off-Line */
		case 1221: /* Reintento Venta Off-Line */
#ifndef EMPRESA_3
			strncpy(szData+128, "001", 3); /* QUITAR AL IMPLEMENTAR EMPRESA 3 */
#endif
			iCodMsg=EVENT_PRICEOFFLINE;
			break;
		case 1420: /* Anulacion de mensaje */

#ifndef EMPRESA_3
			strncpy(szData+ 89, "001", 3); /* QUITAR AL IMPLEMENTAR EMPRESA 3 */
#endif			
			iCodMsg=EVENT_PRICEREV;		/* Trx crédito */
			break;
		case 1421: /* Reintento anulación de mensaje */
#ifndef EMPRESA_3
			strncpy(szData+ 89, "001", 3); /* QUITAR AL IMPLEMENTAR EMPRESA 3 */
#endif
			iCodMsg=EVENT_PRICEANUL;
			break;
		default:
			iCodMsg=EVENT_NULL;
	}

	/* Finaliza si el mensaje es desconocido */
	if (iCodMsg==EVENT_NULL)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Mensaje (%4.4s)desconocido\n", iNroPid, TipoMensajePrice);
		PTable.RemPid(iNroPid);
		return NOOK;
	}

	/* Loguea tipo de mensaje y cabecera */
	memset(szAuxStr1, 0, sizeof(szAuxStr1));
	strncpy(szAuxStr1, szData,27);
	LogAlarm.Put( 0, "TCPDaemon[%09d]: Envia mensaje [%s] [%s]\n", iNroPid, TipoMensajePrice, szAuxStr1);

	/* Envia mensaje al Daemon */
	iRet=EQueue.SendMsg(iCodMsg, szData, (atoi(szHeader+2))-HEADER_SIZE);
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) en escritura al Daemon\n", iNroPid, EQueue.GetErrno());
		PTable.RemPid(iNroPid);
		return NOOK;
	}

	/* Espera la respuesta */
	LogAlarm.Put( 0, "TCPDaemon[%09d]: Espera respuesta...\n", iNroPid);
	iLongitud=iRet=SQueue.GetMsg(iNroPid, szData, sizeof(szData));
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) al esperar respuesta\n", iNroPid, SQueue.GetErrno());
		PTable.RemPid(iNroPid);
		return NOOK;
	}

	/* Recupera el puerto donde responder */
	sprintf(szAuxStr1, "TCPPortRta%s", szHeaderAux);
	iRet=Cfg.GetItem("TCPDaemon", szAuxStr1, szAuxStr2);
	if (iRet!=0)
	{
		LogAlarm.Put(0, "TCPDaemon[%09d]: Error al leer configuracion [%s]\n", iNroPid, szAuxStr1);
		PTable.RemPid(iNroPid);
		return NOOK;
	}

	/* Arma header de respuesta */
	sprintf(szHeaderLen, "%04d", iLongitud+HEADER_SIZE-iTamResta);
	memset(szHeader, 0, sizeof(szHeader));
	strncpy(szHeader, szHeaderAux, strlen(szHeaderAux));
	strcat(szHeader, szHeaderLen);

	/* Abre socket hacia sucursal */
	LogAlarm.Put( 5, "TCPDaemon[%09d]: Abre socket hacia sucursal\n", iNroPid);
	iRet=RSocket.Open();
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) en apertura de socket\n", iNroPid, RSocket.GetErrno());
		PTable.RemPid(iNroPid);
		return NOOK;
	}

	/* Se conecta a sucursal */
	strcpy(szAuxStr1, inSocket->GetRemoteAddress());
	LogAlarm.Put( 0, "TCPDaemon[%09d]: Conecta con sucursal hacia %s:%d\n", iNroPid, szAuxStr1, atoi(szAuxStr2));
	iCont=1;
	do
	{
		iRet=RSocket.Connect(szAuxStr1, atoi(szAuxStr2));
		if (iRet==NOOK)
		{
			LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) al conectar socket de respuesta. Intento %d\n", 
								iNroPid, RSocket.GetErrno(), iCont);
			iCont++;
			sleep(2);
		}
	} while ((iRet==NOOK) && (iCont<=5));

	/* Verifica si establecio conexion con sucursal */
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error de conexion con sucursal. Mensaje descartado\n", iNroPid);
		RSocket.Close();
		PTable.RemPid(iNroPid);
		return NOOK;
	}
	
	/* Escribe header a sucursal */
	LogAlarm.Put( 5, "TCPDaemon[%09d]: Conecta con sucursal desde %s:%d\n",
						iNroPid, RSocket.GetLocalAddress(), RSocket.GetLocalPort());
	LogAlarm.Put( 5, "TCPDaemon[%09d]: Envia cabecera a sucursal [%*.*s]\n", iNroPid, HEADER_SIZE,HEADER_SIZE,szHeader);
	iRet=RSocket.Write(szHeader, HEADER_SIZE);
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) al escribir cabecera de respuesta\n", iNroPid, RSocket.GetErrno());
		RSocket.Close();
		PTable.RemPid(iNroPid);
		return NOOK;
	}

	/* Escribe mensaje a sucursal */
	LogAlarm.Put( 0, "TCPDaemon[%09d]: Envia respuesta a sucursal [%*.*s]\n", iNroPid, HEADER_SIZE,HEADER_SIZE,szHeader);
	iRet=RSocket.Write(szData, iLongitud-iTamResta);
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) al escribir mensaje de respuesta\n", iNroPid, RSocket.GetErrno());
		RSocket.Close();
		PTable.RemPid(iNroPid);
		return NOOK;
	}
	
	/* Cierra socket de conexion con sucursal */
	LogAlarm.Put( 5, "TCPDaemon[%09d]: Cierra socket con sucursal\n", iNroPid);
	iRet=RSocket.Close();
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) al cerrar socket con sucursal\n", iNroPid, RSocket.GetErrno());
	}

	/* Borra el PID de la tabla */
	LogAlarm.Put( 5, "TCPDaemon[%09d]: Borra proceso de tabla de PIDs\n", iNroPid);
	iRet=PTable.RemPid(iNroPid);
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "TCPDaemon[%09d]: Error (%d) al eliminar PID\n", iNroPid, PTable.GetErrno());
	}
	LogAlarm.Put(0, "TCPDaemon[%09d]: Finaliza proceso de mensaje. %s", iNroPid, currentTimeLog());
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvShutDown                                                 |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra y destruye todos los objetos creados para poder     |
|                 | finalizar la aplicacion correctamente                      |
+-----------------+------------------------------------------------------------+
*/
void pvShutDown(int sig_id=0)
{
	int iRet;

	/* Borra la tabla de PIDs */
	LogAlarm.Put( 0, "ShutDown(): Borra PID de tabla (%d)\n",getpid());
	iRet=PTable.RemPid(getpid());
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "ShutDown(): Error (%d) al borrar PID de tabla\n", PTable.GetErrno());
		exit(1);
	}
	
	/* Finaliza si concluyo por signal */
	if (sig_id!=0)
	{
		LogAlarm.Put( 0, "ShutDown(): Proceso Concluido por Signal= %d\n", sig_id);
		exit(0);
	}
	
	/* Cierra el socket */
	LogAlarm.Put( 0, "ShutDown(): Cierra socket\n");
	iRet=LSocket.Close();
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "ShutDown(): Error (%d) al cerrar socket\n", LSocket.GetErrno());
	}

	/* Destruye objeto contador de hijos */
	LogAlarm.Put( 0, "ShutDown(): Destruye contador de hijos\n");
	QHijos.Destroy();

	/* Finaliza aplicacion */
	LogAlarm.Put( 0, "ShutDown(): Hecho.\n");
	exit(0);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvInit                                                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea e inicializa todos los objetos necesarios para poder  |
|                 | comenzar la ejecucion del proceso                          |
+-----------------+------------------------------------------------------------+
*/
void pvInit()
{
	int		ShMID;
	int		iRet;
	char	szAuxStr1[100];		
	char	szAuxStr2[100];		
    long	TimeOutOpen = 20;      

	/* Inicia las señales ante eventos externos */
	signal(SIGINT , pvShutDown);
	signal(SIGTERM, pvShutDown);
	signal(SIGALRM, pvShutDown);

	/* Recupera el timeout para apertura de colas */
	if (Cfg.GetItem("General", "OpenTimeout", szAuxStr1) == 0)
	{
		TimeOutOpen=atol(szAuxStr1); 
	}

	/* Recupera el nombre del archivo de log */
	Cfg.GetItem("TCPDaemon", "LogName", szAuxStr1);
	LogAlarm.Open(szAuxStr1);

	/* Recupera el nivel de log */
	Cfg.GetItem("TCPDaemon","LogLevel",szAuxStr1);
	LogAlarm.SetLevel(atol(szAuxStr1));

	/* Loguea hora de comienzo */
    LogAlarm.Put( 0,"Init(): Comienzo [%s] - %s", NUM_VERSION, currentTimeLog()); 

    /* Abre cola de sockets */
    Cfg.GetItem("TCPDaemon", "QueueID", szAuxStr1);
    LogAlarm.Put( 5, "Init(): Abre cola de mensajes [Id:%s]\n", szAuxStr1);  
    iRet=SQueue.Open(atol(szAuxStr1), TimeOutOpen);
    if (iRet==NOOK)
    {
		LogAlarm.Put( 0, "Init(): Error (%d) al abrir cola de mensajes\n", SQueue.GetErrno());
		exit(1);
    }

	/* Recupera el identificador de la cola de mensajes para enviar */
	Cfg.GetItem("VisaDaemon", "QueueID", szAuxStr1);

	/* Abre la cola de mensajes para enviar */
	LogAlarm.Put( 0, "Init(): Abre cola de eventos...[%s]\n", szAuxStr1);
	iRet = EQueue.Open(atoi(szAuxStr1));
	if (iRet == NOOK)
	{
		LogAlarm.Put( 0, "Init(): Error al abrir cola de eventos (%d)\n", EQueue.GetErrno());
		exit(1);
	}

	/* Abre socket */
	LogAlarm.Put( 0, "Init(): Abre socket de entrada...\n");
	iRet=LSocket.Open();
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "Init(): Error (%d) al abrir socket de entrada\n", LSocket.GetErrno());
		exit(1);
	}

	/* Recupera datos para la tabla de PIDs */ 
	Cfg.GetItem("PIDTable", "ShMemID", szAuxStr1);
	ShMID=atoi(szAuxStr1);
	Cfg.GetItem("PIDTable", "MaxEntries", szAuxStr1);

	/* Abre tabla de PIDs */
	LogAlarm.Put( 0, "Init(): Abre tabla de PIDs...(%d) [%s]\n",	ShMID, szAuxStr1);
	while (PTable.Open(ShMID, atoi(szAuxStr1))==NOOK)
	{
		if (PTable.GetErrno()!=ENOENT)
		{
			LogAlarm.Put( 0, "Init(): Error (%d) al abrir tabla de PIDs\n", PTable.GetErrno());
			exit(1);
		}
	}

	/* Agrega PID a la tabla */
	LogAlarm.Put( 0, "Init(): Agrega PID a tabla de PIDs\n");
	iRet=PTable.PutPid(getpid());
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "Init(): Error (%d) al agregar PID\n", PTable.GetErrno());
		exit(1);
	}

	/* Recupera el tamaño de la lista para el socket */
	Cfg.GetItem("TCPDaemon", "TCPPort", szAuxStr1);
	LogAlarm.Put( 0, "Init(): Espera conexiones entrantes\n");

	/* Configura la lista del socket */
	iRet=LSocket.ListenAny(atoi(szAuxStr1));
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "Init(): Error (%d)!!\n", LSocket.GetErrno());
		exit(1);
	}

	/* Recupera la cantidad maxima de hijos */
	Cfg.GetItem("TCPDaemon", "TopeHijos", szAuxStr1);
	TOPEHIJOS=atoi(szAuxStr1);
	
	/* Recupera los datos para el objeto contador de hijos */
	Cfg.GetItem("TCPDaemon", "HijosID", szAuxStr1);
	Cfg.GetItem("TCPDaemon", "HijosPerms", szAuxStr2);
	
	/* Crea el objeto contador de hijos */
	iRet= QHijos.Create(atoi(szAuxStr1),atoi(szAuxStr2));
	if (iRet==NOOK)
	{
		LogAlarm.Put( 0, "Init(): Error al crear el espacio de memoria para "
					     " el contador de la cantidad de procesos hijos. \n"); 
		exit(1); 
	}

	/* Setea el contador de hijos */
	LogAlarm.Put( 0, "Init(): Tope de hijos [%d]\n", TOPEHIJOS); 
	QHijos.SetQChild();
	LogAlarm.Put( 0, "Init(): Valor inicial de QHijos [%d] \n", QHijos.GetQChild());
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion principal de la aplicacion                         |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int main(int argc, char *argv[])
{
	int iRet;

	/* Inicia los objetos */
	pvInit();

	/* Repite por siempre */
	while (1)
	{
		/* Si hay demasiados procesos hijos corriendo */
		if (QHijos.GetQChild() > TOPEHIJOS)
		{ 
			sleep(10); 
			LogAlarm.Put( 0, "TCPDaemon[000000000]: Demasiados procesos. Durmiendo. %s", currentTimeLog());
		}
		else
		{
			/* Espera una conexion */
			inSocket=LSocket.Accept();
			LogAlarm.Put( 0, "TCPDaemon[000000000]: Acepta conexion...\n");

			/* Lanza proceso hijo de atencion */
			if (fork()==0)
			{  
				/* Incrementa la cantidad de procesos en ejecucion */
				iRet=QHijos.IncQChild();
				if (iRet==NOOK)
				{
					LogAlarm.Put( 0, "TCPDaemon[000000000]: Error al incrementar QHijos. \n");
				}
				
				/* Procesa el mensaje */
				LogAlarm.Put( 0, "TCPDaemon[000000000]: Proceso Hijo: [%d]\n",QHijos.GetQChild());
				piProcesaMensaje();
	
				/* Decrementa la cantidad de procesos en ejecucion */
				iRet=QHijos.DecQChild();
				if (iRet==NOOK)
				{
					LogAlarm.Put( 0, "TCPDaemon[000000000]: Error al decrementar QHijos. \n");
				}

				/* Cierra el objeto hijo */
				QHijos.Close();

				/* Finaliza el proceso hijo */
				exit(0);
			} 
			else
			{
				/* Finaliza los hijos que terminaron */
				for (iRet=0; iRet<10; iRet++)
				{
					waitpid(-1, NULL, WNOHANG);
				}

				/* Cierra el socket del proceso hijo */
				LogAlarm.Put( 0, "TCPDaemon[000000000]: Numero de procesos hijos [%d]\n", QHijos.GetQChild());
				iRet=inSocket->Close();
			}
		}
	}
	
	/* Finaliza la aplicacion */
	pvShutDown();
	return OK;
}
