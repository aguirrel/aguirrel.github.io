#ifdef OS_Linux
#pragma pack(1)
#endif

/************************************************************************/
/*									*/
/* SendRev.C	    : Proceso que envia al Centro Autorizador las       */
/*                    transacciones pendientes 				*/
/*									*/
/* Proyecto	    : Autorizacion de tarjetas de credito - StoreFlow.	*/
/* Cliente	    : Coto						*/
/* Autor	    : Lalo Steinmann					*/
/* Fecha	    : 17/02/95						*/
/* Compilador	    : C++						*/
/* Base de datos    : Oracle						*/
/*									*/
/************************************************************************/
#include "Config.h"
#include "Table.h"

extern "C"
{
#include "DBServices.h"
}

DataBase        DB;

int main(int argc, char *argv[])
{
	int  ret=0;
	char nro_ca[3]={0};
	char comando[64];

	while (1)
	{
		do
		{
			ret= getNextCentro(nro_ca);
			if (ret==OK)
			{
				memset(comando, 0, sizeof(comando));
				sprintf(comando,"SendAnul %02d 2>/dev/null", atoi(nro_ca));
				system(comando);
				sleep(1);
			} 	
		} while (ret==OK);
		sleep(30);
	} 
	return OK;
}
