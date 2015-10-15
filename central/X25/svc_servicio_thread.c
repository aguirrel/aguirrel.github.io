#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <NLchar.h>
#include <x25sdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>	
#include <pthread.h>

#define LINK_NAME		"x25s0"				/*Default link*/
#define CALLING_ADDR	"41700047"			/*Calling Network User Address */
#define CALLED_ADDR		"28990000633208"  	/*POSNET valor viejo*/

/*#define CALLED_ADDR 		"28990000632055"*/ /*POSNET*/

/*#define CALLED_ADDR		"410700321000"*/		/*AMEX*/

#define MAX_LON_MSG		1024
#define PUERTO_TCP		5000

#define SEPARADOR	 		0x1c
#define LON_DATOS			4
#define LON_SEPARADOR	1
#define LON_ID_CAMPO	2
#define LON_NULL          4
#define STR_NULL          "NULL"
#define LARGO_DATOS_EFT		80
#define DATOS_EFT					81
#define COD_RESPUESTA_EFT	82

#define REQ_ESTADO		"027000"
#define LON_REQ_ESTADO	6

#define REQ_VERSION    "027100"
#define LON_REQ_VERSION  6

#define VERSION_COMUNICACIONES_X25 "Version 1.0"

#define MAX_LON_RTA_ESTADO	256

typedef struct
{
	char *pszDatos;
	int iLonDatos;
	int iSocket;
}ST_PARAMETROS;

/*Tipo de dato para la region critica*/
typedef pthread_mutex_t REGION_CRITICA;

REGION_CRITICA RegionCritica;
int iNumConcurrencias = 0;	/*Contador de concurrencias*/
int iNumRechazadas = 0;			/*Contador de llamadas rechazadas*/
int iNumLlamadas = 0;   		/*Contador de llamadas*/

/*Funcion que envia por X25*/
void *pvEnviaMensajeX25(void *pParam);
int iArmaRespuesta(char *pszRtaIn, int iLonRtaIn, char *pszRtaOut, int *piLonRtaOut, int iCodRta);

/*Declaracion de las funciones de manejo de regiones criticas*/
/*PASAR A UNA LIBRERIA*/
int iInicializarRegCritica(REGION_CRITICA *pRC);
void iEliminarRegCritica(REGION_CRITICA *pRC);
int iEntrarRegCritica(REGION_CRITICA *pRC);
int iSalirRegCritical(REGION_CRITICA *pRC);

FILE * fdX;

int main(int argc, char *argv[])
{
	int rc;             /* Used for return codes.*/
	int	iSockFd	= 0;
	int iNewSockFd = 0;

	/*Estructuras*/
	struct cb_msg_struct		cb_msg;
	struct cb_link_name_struct	cb_link_name;

	struct sockaddr_in			stServAddr;   
	struct sockaddr_in			stCliAddr;

	/*Varialbes*/
	char 	strMsgInt[MAX_LON_MSG];
	int 	iIndice	= 0;
	int 	ret		= 0;
	int 	i 		= 0;
	int 	readLen = 0;
	int		iParar	= 0;
	int 	iCliLen	= 0;
	char szBuffer[MAX_LON_MSG]; 
	int iLargoBuffer = 0;
	ST_PARAMETROS *pstParam;
	pthread_t stthread;

	/*Inicializa la region critica*/
	iInicializarRegCritica(&RegionCritica);

	/* Inicializacion de l vinculo x25 */
	cb_link_name.flags = X25FLG_LINK_NAME;
	cb_link_name.link_name = LINK_NAME;
	
	rc = x25_init(&cb_link_name);
	if (rc < 0)
	{
		printf("%s: Error en x25_init : x25_errno = %d errno = %d\n",argv[0],x25_errno,errno);
		/*Elimina la region critica inicializada*/
		iEliminarRegCritica(&RegionCritica);
		return(1);
	}
	else
	{
		/*Se pone a escuchar en el puerto*/
		if((iSockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("Error al crear el socket\n");
		}/*end if*/
    
		memset(&stServAddr, 0, sizeof(stServAddr));
		stServAddr.sin_family      = AF_INET;
		stServAddr.sin_port        = htons(PUERTO_TCP);
		stServAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if(bind(iSockFd, (struct sockaddr *) &stServAddr, sizeof(stServAddr)) < 0)
		{
			printf("Error al intentar hacer el bind()");
		}/*end if*/
		
		/*Escucha mensajes en el puerto*/
		listen(iSockFd, 5);

		while(!iParar)
		{
			iCliLen = sizeof(stCliAddr);
			iNewSockFd = accept(iSockFd, (struct sockaddr *) &stCliAddr, &iCliLen);

 		 	memset(szBuffer, 0, sizeof(szBuffer));
			if(iNewSockFd < 0)
			{
				printf("Error en el accept()");
				iParar = 1;
				continue;
			}/*end if*/ 

			/*Toma el mensaje del Socket*/
			iLargoBuffer = recv(iNewSockFd, szBuffer, sizeof(szBuffer), 0);

			printf("Largo del mensaja:[%d]\n", iLargoBuffer);
			printf("Mensaje TCP In:[");
			for (i = 0 ; i <= iLargoBuffer ; i++)
			{
				printf("%c", szBuffer[i]);
			}/*end for*/

  			printf("]\n");
 
			/*Crea la estructura para los parametros*/
			pstParam = (ST_PARAMETROS*) malloc(sizeof(ST_PARAMETROS));
			if(pstParam == NULL)
			{
				printf("Error al intentar alocar memoria para la estructura\n");
				/*Elimina la region critica inicializada*/
				iEliminarRegCritica(&RegionCritica);
				return(-1);
			}/*end if*/
	
			/*Copia los datos*/
			
			pstParam->iLonDatos = iLargoBuffer;
			pstParam->iSocket = iNewSockFd;
			pstParam->pszDatos = (char*) malloc(iLargoBuffer);
			if(pstParam->pszDatos == NULL)
			{
				printf("Error al inentar alocar datos para la variable\n");	
				/*Elimina la region critica inicializada*/
				iEliminarRegCritica(&RegionCritica);
				return(-1);
			}/*end if*/
		
			memset(pstParam->pszDatos, 0, iLargoBuffer);	
		
			memcpy(pstParam->pszDatos, szBuffer, iLargoBuffer);
				
			/*Crea el thread que envia el mensaje por X25*/
			pthread_create(&stthread, NULL, pvEnviaMensajeX25, (void*)pstParam);

		}/*end while()*/ 
	
		/*Cierra el socket de escucha*/
		close(iNewSockFd);

		/*Termina la conexcion X25*/
		ret = x25_term(&cb_link_name);
		printf("Cierra X25 x25_term = %d\n",ret);
	}

	/*Elimina la region critica inicializada*/
	iEliminarRegCritica(&RegionCritica);

	return(0);
}	

void *pvEnviaMensajeX25(void *pParam)
{
	int conn_id;        /* Used to identify the call after making the call.    */
	int ctr_id;         /* Counter identifier to be associated with this call. */
	int ctr_num	= 1;    /* The number of counters in counter array.            */
	int ret	= 0;
	int i = 0;
	int j = 0;
	int iCantidadEnvio = 0;
	char szLonDatos[LON_DATOS + 1];
	int iLonDatos = 0;

	struct ctr_array_struct	ctr_array[1];
	struct cb_call_struct		cb_call;
	struct cb_clear_struct	cb_clear;
	struct cb_data_struct		cb_data;
	struct cb_msg_struct 		cb_msg;

	ST_PARAMETROS *pstParam;

	char szRta[MAX_LON_MSG];
	int iLonRta = 0;

	printf("Dentro del thread, inicio\n");

	memset(szRta, 0, sizeof(szRta));

	pstParam = (ST_PARAMETROS*)pParam;
	
	memset(szLonDatos, 0, sizeof(szLonDatos));

	printf("  Desglosa el mensaje\n");

	/*Incrementa el numero de concurrencias*/
	iEntrarRegCritica(&RegionCritica);
	iNumConcurrencias++;
	iSalirRegCritica(&RegionCritica);

	/*************************************/
	/*Verifica si es un mensaje de estado*/
	/*************************************/
	if(memcmp(pstParam->pszDatos, REQ_ESTADO, LON_REQ_ESTADO) == 0)
	{
		iTrataReqEstado(pstParam->iSocket);

		/*Decrementa el numero de concurrencias*/
		iEntrarRegCritica(&RegionCritica);
		iNumConcurrencias--;
		iSalirRegCritica(&RegionCritica);

		/*Sale del thread*/
		return;
	}/*end if*/
	
	/*Verifica si es un mensaje de Version*/
  if(memcmp(pstParam->pszDatos, REQ_VERSION, LON_REQ_VERSION) == 0)
  {
    iTrataReqVersion(pstParam->iSocket);
		return;
	}/*end if*/



	/*Obtiene el largo de los datos a enviar*/
	for(i = LON_ID_CAMPO, j = 0 ; pstParam->pszDatos[i] != SEPARADOR; i++, j++)
	{
		szLonDatos[j] = pstParam->pszDatos[i];
	}/*end for*/

	iLonDatos = atoi(szLonDatos);

	printf("	Largo de los datos [%d]\n", iLonDatos);

	/*Obtiene el identificador de llamada*/
	ctr_id = x25_ctr_get();

	cb_call.flags = X25FLG_LINK_NAME;      /* Set flag for using linkname.  */
	cb_call.link_name = LINK_NAME;

	cb_call.flags |= X25FLG_CALLING_ADDR;  /* Set flags for calling addr    */
	cb_call.calling_addr = CALLING_ADDR;

	cb_call.flags |= X25FLG_CALLED_ADDR;   /* Set flag for called address   */
	cb_call.called_addr = CALLED_ADDR;

	cb_call.flags |= X25FLG_D_BIT;

	conn_id = x25_call(&cb_call,ctr_id);
	if(conn_id == -1)
	{
		printf("Error en x25_call : x25_errno = %d errno = %d\n", x25_errno,errno);

		/*Decrementa el numero de concurrencias*/
		iEntrarRegCritica(&RegionCritica);
		iNumConcurrencias--;
		iSalirRegCritica(&RegionCritica);

		return;
	}/*end if*/

	/*Incrementa el numero de llamadas*/
	iEntrarRegCritica(&RegionCritica);
	iNumLlamadas++;
	iSalirRegCritica(&RegionCritica);


	ctr_array[0].flags = X25FLG_CTR_ID;
	ctr_array[0].flags |= X25FLG_CTR_VALUE;
	ctr_array[0].ctr_id = ctr_id;
	ctr_array[0].ctr_value = 0;

	ret = x25_ctr_wait(ctr_num,ctr_array);

	ret = x25_receive(&conn_id,&cb_msg);  /* Receive the incoming message.  */
	
	/*Evalua el retorno de la llamada*/
	if(cb_msg.msg_type == X25_CALL_CONNECTED)
	{
		printf("	Conexion lograda, Envia el mensaje\n");	

		cb_data.flags = X25FLG_DATA;
		cb_data.data_len = iLonDatos;
		cb_data.data = pstParam->pszDatos + LON_ID_CAMPO + LON_DATOS + LON_SEPARADOR + LON_ID_CAMPO;
		
		/*Envia el mensaje*/
		ret = x25_send(conn_id,&cb_data);

		ret = x25_ctr_wait(ctr_num,ctr_array);   /* Wait for acknowledgement. */

		ret = x25_receive(&conn_id,&cb_msg);     /* Get acknowledgement.      */
		
		/*Evalua el retorno del envio de datos*/
		switch(cb_msg.msg_type)
		{
			case X25_DATA:
				printf("	Respuesta X25 [");
				for (i = 0 ; i <= cb_msg.msg_point.cb_data->data_len ; i++)
				{
					printf("%c",cb_msg.msg_point.cb_data->data[i]);
				}/*end for*/
				printf("]\n");
				break;

			case X25_CLEAR_INDICATION:
				printf("Se informo Clear Indication\n");
				printf	(	
							"X25: Causa (%d)  Diagnostico (%d)\n",
							cb_msg.msg_point.cb_clear->cause,
							cb_msg.msg_point.cb_clear->diagnostic
						);
				/*Incrementa el numero de llamadas*/
				iEntrarRegCritica(&RegionCritica);
				iNumRechazadas++;
				iSalirRegCritica(&RegionCritica);

				break;

			case X25_DATA_ACK:
				printf("Se informo un Data Ack\n");
				break;

			default:
				printf("Mensaje no interpretable\n");
				break;
		}/*end switch()*/

		cb_clear.flags = X25FLG_CAUSE;
		cb_clear.flags |= X25FLG_DIAGNOSTIC;
		cb_clear.cause = 0;            /* The CCITT code for DTE-originated   */
		cb_clear.diagnostic = 0;       /* No further information              */

		ret = x25_call_clear(conn_id,&cb_clear,(struct cb_msg_struct *)NULL);

		/*Arma la respuesta para responder al origen*/
		iArmaRespuesta(
			cb_msg.msg_point.cb_data->data,
			cb_msg.msg_point.cb_data->data_len,
			szRta,
			&iLonRta,
			0);

		
		printf("  Responde al origen.\n");

		/*
		printf("  Respuesta TCP al Origen[");
		for(i = 0 ; i <= iLonRta; i++)
		{
			printf("%c",szRta[i]);
		}
		printf("]\n");
		*/

    /*Envia la respuesta al proceso que origino la conexion*/
    iCantidadEnvio = send(pstParam->iSocket, szRta, iLonRta, 0);
		fflush(stdout);
	}
	else
	{
		if(cb_msg.msg_type == X25_CLEAR_INDICATION)
		{
			printf(
						"Llamada no aceptada. Causa = 0x%02x Diagnostico = 0x%02x\n",
						cb_msg.msg_point.cb_clear->cause,
						cb_msg.msg_point.cb_clear->diagnostic
					);
		}/*end if*/

	  printf("  Arma respuesta de error.\n");	

		/*Arma la respuesta para responder al origen*/
		iArmaRespuesta( NULL, 0, szRta, &iLonRta, 1);
		
		printf("	Envia el mensaje.\n");

  	/*Envia la respuesta con un mensaje de error*/
  	iCantidadEnvio = send(pstParam->iSocket, szRta, iLonRta, 0);
	}/*end if*/

	/*Libera la memoria tomada anteriormente*/
	free(pstParam->pszDatos);	
	free(pstParam);
	
	ret = x25_ctr_remove(ctr_id);

	/*Decrementa el numero de concurrencias*/
	iEntrarRegCritica(&RegionCritica);
	iNumConcurrencias--;
	iSalirRegCritica(&RegionCritica);

	return;
}

int iArmaRespuesta(char *pszRtaIn, int iLonRtaIn, char *pszRtaOut, int *piLonRtaOut, int iCodRta)
{
	int iOffset = 0;

	sprintf(pszRtaOut, "%d", LARGO_DATOS_EFT);
	iOffset = LON_ID_CAMPO;
	if(pszRtaIn != NULL)
	{
		sprintf(pszRtaOut + iOffset, "%04d", iLonRtaIn);
	}
	else
	{
		sprintf(pszRtaOut + iOffset, "%04d", LON_NULL);
	}/*end if*/

	iOffset += LON_DATOS;

	sprintf(pszRtaOut + iOffset, "%c", SEPARADOR);
	iOffset++;

	sprintf(pszRtaOut + iOffset, "%d", DATOS_EFT);
  iOffset += LON_ID_CAMPO;
	if(iLonRtaIn > 0)
	{
		memcpy(pszRtaOut + iOffset, pszRtaIn, iLonRtaIn);
		iOffset += iLonRtaIn;
	}
	else
	{
		sprintf(pszRtaOut + iOffset, "%s", STR_NULL);
		iOffset += LON_NULL;
	}/*end if*/

  sprintf(pszRtaOut + iOffset, "%c", SEPARADOR);
  iOffset++;

  sprintf(pszRtaOut + iOffset, "%d", COD_RESPUESTA_EFT);
  iOffset += LON_ID_CAMPO;
  sprintf(pszRtaOut + iOffset, "%02d", iCodRta);
  iOffset += 2;
  sprintf(pszRtaOut + iOffset, "%c", SEPARADOR);
  iOffset++;

	*piLonRtaOut = iOffset;
	return(0);
}

/*Funcion que trata el mensaje de estado*/
int iTrataReqEstado(unsigned int uiSocketConexion)
{
	int iLargoMensaje = 0;
	int iRetorno = 0;
	int iConcurrenciasLocal = 0;
	int iLlamadasLocal = 0;
	int iRechazadasLocal = 0;

	char szRta[MAX_LON_RTA_ESTADO];

	memset(szRta, 0, sizeof(szRta));

	/*Obtiene el numero de concurrencias actual*/
	iEntrarRegCritica(&RegionCritica);
	iConcurrenciasLocal = iNumConcurrencias;
	iLlamadasLocal = iNumLlamadas;
	iRechazadasLocal = iNumRechazadas;
	iSalirRegCritica(&RegionCritica);

	/*Arma la rta. de estado*/
	sprintf(szRta, "027000%c50Concurrentes:%03d;Llamadas X25:%03d;Rechazadas:%03d%c",
	SEPARADOR, iConcurrenciasLocal, iLlamadasLocal, iRechazadasLocal, SEPARADOR);

	iLargoMensaje = strlen(szRta);

	iRetorno = send(uiSocketConexion, szRta, iLargoMensaje, 0);
	if(iRetorno < iLargoMensaje)
	{
	}/*end if*/
	return(0);
}

/*Funcion que trata el mansaje de Version*/
int iTrataReqVersion(unsigned int uiSocketConexion)
{
	int iLargoMensaje = 0;
	int iRetorno = 0;
	char szRta[MAX_LON_RTA_ESTADO];

	memset(szRta, 0, sizeof(szRta));

	/*Arma la rta. de estado*/
	sprintf(szRta, "027100%c51%s%c", SEPARADOR, VERSION_COMUNICACIONES_X25, SEPARADOR);
	iLargoMensaje = strlen(szRta);

	iRetorno = send(uiSocketConexion, szRta, iLargoMensaje, 0);
  if(iRetorno < iLargoMensaje)
  {
  }/*end if*/

	return(0);
}



/*Funciones para el manejo de regiones criticas*/
/*PASAR A UNA LIBRERIA*/
int iInicializarRegCritica(REGION_CRITICA *pRC)
{
	int iRetValue;
	pthread_mutexattr_t attrMutex;

	/*UNIX*/
	pthread_mutexattr_init(&attrMutex);
	iRetValue=pthread_mutex_init(pRC,&attrMutex);

	return(iRetValue);
}

void iEliminarRegCritica(REGION_CRITICA *pRC)
{
	pthread_mutex_destroy(pRC);
}

int iEntrarRegCritica(REGION_CRITICA *pRC)
{
	int iRetValue;

	/*UNIX*/
	iRetValue=pthread_mutex_lock(pRC);

	return(iRetValue);
}

int iSalirRegCritica(REGION_CRITICA *pRC)
{
	int iRetValue;

	/*UNIX*/
	iRetValue=pthread_mutex_unlock(pRC);

	return(iRetValue);
}

