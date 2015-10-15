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

#define LINK_NAME		"x25s0"				/*Default link*/
#define CALLING_ADDR	"41700047"			/*Calling Network User Address */
#define CALLED_ADDR		"28990000633208"  	/*POSNET*/
/*#define CALLED_ADDR		"410700321000"*/		/*AMEX*/
#define MAX_LON_MSG		1024
#define PUERTO_TCP		5000

/*Funcion que envia por X25*/
int iEnviaMensajeX25(int iNuevoSocket);

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

	/* Inicializacion de l vinculo x25 */
	cb_link_name.flags = X25FLG_LINK_NAME;
	cb_link_name.link_name = LINK_NAME;
	
	rc = x25_init(&cb_link_name);
	if (rc < 0)
	{
		printf("%s: Error en x25_init : x25_errno = %d errno = %d\n",argv[0],x25_errno,errno);
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

			if(iNewSockFd < 0)
			{
				printf("Error en el accept()");
				iParar = 1;
				continue;
			}/*end if*/
			
			/*Envia el mensaje por X25*/
			iEnviaMensajeX25(iNewSockFd);

		}/*end while()*/ 
	
		/*Cierra el socket de escucha*/
		close(iNewSockFd);

		/*Termina la conexcion X25*/
		ret = x25_term(&cb_link_name);
		printf("Cierra X25 x25_term = %d\n",ret);
	}
	return(0);
}	

int iEnviaMensajeX25(int iNuevoSocket)
{
	int conn_id;        /* Used to identify the call after making the call.    */
	int ctr_id;         /* Counter identifier to be associated with this call. */
	int ctr_num	= 1;    /* The number of counters in counter array.            */
	int iLargoBuffer = 0;
	int ret	= 0;
	int i = 0;
	int iCantidadEnvio = 0;

	struct ctr_array_struct	ctr_array[1];
	struct cb_call_struct		cb_call;
	struct cb_clear_struct	cb_clear;
	struct cb_data_struct		cb_data;
	struct cb_msg_struct 		cb_msg;

	char szBuffer[MAX_LON_MSG];

	memset(szBuffer, 0, sizeof(szBuffer));

	/*Toma el mensaje del Socket*/
	iLargoBuffer = recv(iNuevoSocket, szBuffer, sizeof(szBuffer), 0);
	printf("Mensaje TCP In:[");
  for (i = 0 ; i <= iLargoBuffer ; i++)
  {
  	printf("%c", szBuffer[i]);
  }/*end for*/

	printf("]\n");

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
		return(-1);
	}/*end if*/

	ctr_array[0].flags = X25FLG_CTR_ID;
	ctr_array[0].flags |= X25FLG_CTR_VALUE;
	ctr_array[0].ctr_id = ctr_id;
	ctr_array[0].ctr_value = 0;

	ret = x25_ctr_wait(ctr_num,ctr_array);

	ret = x25_receive(&conn_id,&cb_msg);  /* Receive the incoming message.  */
	
	/*Evalua el retorno de la llamada*/
	if(cb_msg.msg_type == X25_CALL_CONNECTED)
	{
		cb_data.flags = X25FLG_DATA;
		cb_data.data_len = iLargoBuffer;
		cb_data.data = szBuffer;

		ret = x25_send(conn_id,&cb_data);

		ret = x25_ctr_wait(ctr_num,ctr_array);   /* Wait for acknowledgement. */

		ret = x25_receive(&conn_id,&cb_msg);     /* Get acknowledgement.      */
		
		/*Evalua el retorno del envio de datos*/
		switch(cb_msg.msg_type)
		{
			case X25_DATA:
				printf("Mensaje TCP Out [");
				for (i = 0 ; i <= cb_msg.msg_point.cb_data->data_len ; i++)
				{
					printf("%c",cb_msg.msg_point.cb_data->data[i]);
				}/*end for*/
				printf("]\n");
				if( (fdX=fopen("svcRta.log","a"))!=NULL )
				{
					fwrite(cb_msg.msg_point.cb_data->data,sizeof(char),cb_msg.msg_point.cb_data->data_len,fdX);
					fclose(fdX);
				}
				break;

			case X25_CLEAR_INDICATION:
				printf("Se informo Clear Indication\n");
				printf	(	
							"X25: Causa (%d)  Diagnostico (%d)\n",
							cb_msg.msg_point.cb_clear->cause,
							cb_msg.msg_point.cb_clear->diagnostic
						);
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

    /*Envia la respuesta al proceso que origino la conexion*/
    iCantidadEnvio = send(iNuevoSocket, cb_msg.msg_point.cb_data->data, cb_msg.msg_point.cb_data->data_len, 0);
		fflush(stdout);

	}
	else
	{
		if(cb_msg.msg_type == X25_CLEAR_INDICATION)
		{
			printf	(
						"Llamada no aceptada. Causa = 0x%02x Diagnostico = 0x%02x\n",
						cb_msg.msg_point.cb_clear->cause,
						cb_msg.msg_point.cb_clear->diagnostic
					);
		}/*end if*/
	}/*end if*/


	ret = x25_ctr_remove(ctr_id);

	return(0);
}
