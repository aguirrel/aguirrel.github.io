#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | IniX25.C                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso que inicializa el canal X25 para un determinado    |
|                 | centro autorizador                                         | 
+-----------------+------------------------------------------------------------+
*/
#include "Queues.h"
#include "Config.h"
#include "Daemons.h"

Config		Cfg(CONFIG_FILE);
Queue		X;
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion principal de la aplicacion                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int main(int argc,char **argv)
{
    char szQueueID[100];
    char szQueuePerm[100];
	char szRespuesta[1024];
	char szConsulta[16];
    char szSep[] = "#";
	char *ptrToken;
	int  iRet;
	int  iCentro;
	int  iConfig;
	int  iCreados; 
	int  iActivos; 
	int  iBloqueados;
	int	 iEscLinea;
	int  iSalida = 0;
	int  iParamCentro = -1;

	if ( (argc==1) || ((argc==2)&&(strlen(argv[1])==2)) )
	{
		/*Toma el numero de centro */
		if (argc==2)
		{
			iParamCentro = atoi(argv[1]);
		}

		/* Abre cola de mensajes */
		Cfg.GetItem("CAVisaDaemon", "QueueID", szQueueID);
		Cfg.GetItem("CAVisaDaemon", "QueuePerm", szQueuePerm);
		iRet = X.Open(atol(szQueueID), atol(szQueuePerm));

		/* Envia consulta de canales */
		sprintf(szConsulta, "C");
		iRet = X.SendMsg(99, szConsulta, strlen(szConsulta));
		if (iRet==0)
		{
			/* Espera respuesta */
			memset(szRespuesta, 0, sizeof(szRespuesta));
			iRet = X.GetMsg(98, szRespuesta, sizeof(szRespuesta));
			if (iRet > 0)
			{
				/* Imprime el encabezado */
				printf("\n");
				printf("    CENTRO        CANALES       CANALES      CANALES     CANALES  \n");
				printf("  AUTORIZADOR   CONFIGURADOS   LEVANTADOS   ACTIVADOS   BLOQUEADOS\n");

				/* Procesa respuesta */
				ptrToken = strtok(szRespuesta, szSep );
				while( ptrToken != NULL )
				{
					sscanf(ptrToken, "%02d-%02d-%02d-%02d-%02d", 
								&iCentro, &iConfig, &iCreados, &iActivos, &iBloqueados);
					ptrToken = strtok( NULL, szSep );

					/* Si hay parametro pero no coincide no escribe linea */
					iEscLinea = 1;
					if ( (iParamCentro!=-1) && (iParamCentro!=iCentro) )
					{
						iEscLinea = 0;
					}
										
					if (iEscLinea)
					{
						printf("      %02d            %02d            %02d           %02d          %02d\n",
							iCentro, iConfig, iCreados, iActivos, iBloqueados);
						iSalida += iActivos;
					}
				}
				printf("\n");
			}
			else
			{
				printf("Error al recibir respuesta\n");
				iSalida = 0;
			}
		}
		else
		{
			printf("Error al enviar solicitud\n");
			iSalida = 0;
		}
	}
	else
	{	
		printf("Error en parametros\n");
		printf("Uso: CfgX25 [opcional NroCA] : Muestra informacion de los canales\n");	
		iSalida = 0;
	}
	return (iSalida);
}
    
    


 
