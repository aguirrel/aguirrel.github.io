#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | CACStatus.C                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso para obtener el estado de los canales de los       |
|                 | centros autorizadores                                      | 
+-----------------+------------------------------------------------------------+
*/
#include "CACStat.h"
#include "Config.h"
#include "Daemons.h"
#include "Includes.h"

#ifndef NOOK
    #define NOOK		ERROR
#endif
#ifndef OK
    #define OK			0
#endif

char		aux_str1[100];
int			ret;
CACStatus	CACS;
Config		Cfg(CONFIG_FILE);

#define		NUM_VERSION			"v1.46 02-Jul-2013 (3 min)"

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | init                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Inicializacion de estructura CACs                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void init()
{
	Cfg.GetItem("CACStatVisa", "ShMemID", aux_str1);
	while (CACS.Open(atoi(aux_str1))==NOOK)
	if (CACS.GetErrno()!=ENOENT)
	{
		printf("Error accediendo al CACS flag (%d)!!\n", CACS.GetErrno());
		exit(1);
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | shut_down                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierre de la estructura CACs                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void shut_down()
{
	ret=CACS.Close();
	if (ret==NOOK)
	{
		printf("Error cerrando CACS flag (%d)!!\n", CACS.GetErrno());
	}
	exit(0);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PrintStatus                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Impresion del estado de los canales                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void PrintStatus(int channel)
{
    printf("\nStatus de CA     : %02d\n",channel);
    printf("X.25 On-line     : %s\n", (CACS.IsUp(channel)     ? "SI" : "NO"));
    if (CACS.IsCierre(channel))
		printf("Cierre en proceso: SI (Sucursal: %d Caja: %d)\n", 
	    CACS.GetSuc(channel), CACS.GetCaj(channel)); 
    else
		printf("Cierre en proceso: NO\n");
    printf("Ping activo      : %s\n\n", (CACS.IsPing(channel)   ? "SI" : "NO"));
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PrintUsage                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Impresion de la forma de ejecucion                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void PrintUsage(char *bin_name)
{
    printf("Version: %s\n", NUM_VERSION);
	printf("Uso:\n\t%s cod_tar [up|normal]\n\n"
	"\t    Nro_CA  - Numero de Centro Autorizador\n"
	"\t    up      - Fuerza a on-line el estado del canal X.25 \n"
	"\t    down    - Fuerza a off-line el estado del canal X.25 \n"
	"\t    normal  - Fuerza fin de cierre de lote\n\n", bin_name);
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
	int		cont;

	if ((argc<2) || (argc>3))
	PrintUsage(argv[0]);
	else
	{
		if ((atoi(argv[1])<0) || (atoi(argv[1])>MAX_CAS))
		{
			printf("\nError Nro_CA fuera de rango (0-%d)!!\n\n", MAX_CAS);
			exit(1);
		}
		init();
		if (argc==3)
		{
			if (strcmp(argv[2], "up")==0) 
			{
				if (CACS.Up(atoi(argv[1]))==NOOK)
					printf("Error cambiando estado del X.25 (%d)\n", 
						CACS.GetErrno());
			}
			else
				if (strcmp(argv[2], "normal")==0)
				{
					if (CACS.Normal(atoi(argv[1]))==NOOK)
						printf("Error cambiando estado del X.25 (%d)\n", 
							CACS.GetErrno());
				}
				else
					if (strcmp(argv[2], "down")==0)
					{
						if (CACS.Down(atoi(argv[1]))==NOOK)
							printf("Error cambiando estado del X.25 (%d)\n", 
								CACS.GetErrno());
					}
					else
					{
						PrintUsage(argv[0]);
						shut_down();
					}
		}
		PrintStatus(atoi(argv[1]));
		shut_down();
	}
	return OK;
}

