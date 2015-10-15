#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+------------------------------------------------------------------------------+
|                   Copyright (C) 2003 - COTO CICSA                            |
+-----------------+------------------------------------------------------------+
| Fuente          | ChecVta.c                                                  |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| Descripcion     | Proceso de conciliacion de movimientos de tarjetas entre   |
|                 | el Sistema de Crédito y las sucursales                     |
+-----------------+------------------------------------------------------------+
*/
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "DBFunc.h"

#define VERSION_APLIC "1.47"
#define ULTIMA_MODIF  "13-Dic-2013"

char MensajeError[18][50]= 
{
	"Proceso Exitoso. Totales coinciden", 					/* 00 */
    "Proceso Exitoso. Totales coinciden (BatchUpload)",		/* 01 */
	"En desuso luego de sacar incorporacion fantasmas",     /* -- */
	"En desuso luego de sacar incorporacion fantasmas",     /* -- */
	"Error al sumar luego del batchupload",			        /* 04 */
	"Error al conectar a base de datos",	 				/* 05 */
	"Error al obtener fecha a procesar",				    /* 06 */
	"Error al obtener datos de la sucursal", 		        /* 07 */
	"Error al obtener suma de vtacard", 					/* 08 */
	"Error al obtener suma de credito",					    /* 09 */
	"Error al incorporar pendientes",        			    /* 10 */
	"Error al actualizar fecha de conciliacion", 	        /* 11 */
	"Error al actualizar transacciones de credito",         /* 12 */
	"Error al truncar tabla diaria (vtacard_dia_new)",      /* 13 */
	"Error al existir demasiadas pendientes",		        /* 14 */
    "Error al cargar tabla (vtacard_dia_new)",              /* 15 */
	"Error al llamar al proceso (mal parametros)",			/* 16 */
	"Error al intentar abrir archivo de log"                /* 17 */
};
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TimeStamp                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion para obtener la hora del sistema                   |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
char*  TimeStamp()
{
	static char buffer[100];
	struct tm *newtime;
	time_t ltime;

	time(&ltime);
	newtime=localtime(&ltime);

	sprintf(buffer,"%02d:%02d:%02d",newtime->tm_hour,newtime->tm_min,newtime->tm_sec);

	return buffer;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | debug                                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion para loguear resultados generales de la corrida    |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int debug (char *fmt, ...)
{
	FILE *fd;
	va_list args;
	char szNombreArchivo[256];

	sprintf(szNombreArchivo,"%s/log/conciliacion.log",getenv("HOME"));
	if ( (fd=fopen(szNombreArchivo,"a")) != NULL )
	{
		va_start(args,fmt);
		vfprintf(fd, fmt, args);
		fclose(fd);
		va_end(args);
	}
	else
	{
		return NOOK;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | debugdetalle                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion para loguear resultados detallados de la corrida   |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int debugdetalle (char *fmt, ...)
{
	FILE *fd;
	va_list args;
	char szNombreArchivo[256];

	sprintf(szNombreArchivo,"%s/log/chequeo.log",getenv("HOME"));
	if ( (fd=fopen(szNombreArchivo,"a")) != NULL )
	{
		va_start(args,fmt);
		vfprintf(fd, fmt, args);
		fclose(fd);
		va_end(args);
	}
	else
	{
		return NOOK;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | EscribirResultado                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Loguea el resultado de la ejecucion en el archivo          |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
void EscribirResultado(str_datos_suc *datos_suc, int iMuchasPend)
{
	if (!iMuchasPend)
	{
		debug("Cant de Pend [%03d] \n", datos_suc->cant_pendi);
		debug("Total Vtacard [%8.2f]         ",(float)datos_suc->tot_vta/100);
		debug("Total Credito [%8.2f]\n",(float)datos_suc->tot_cre/100);
		debug("Totales coinciden\n");
	}
	else
	{
		debug("Cant de Pend [SUP] \n");		
		debug("Totales de pendientes permitidas superado\n");		
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Exit                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion de salida del proceso                              |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
void Exit(int iError, str_datos_suc *datos_suc)
{
	DBLoguearResultado(datos_suc, iError, (char*)&MensajeError[iError]);
	exit (iError);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | IncPend                                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Incorporacion de pendientes                                |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int IncPend(str_datos_suc *datos_suc, char *fecpro)
{
    int ret,cant_pendi;
    str_data_trx datos_trx; 
	double Monto; 
    char szSituacion[64];
    str_logueo datos_log;
    time_t hini;
    char aux_str[200];

    /* Loguea comienzo de ejecucion */
	time(&hini);
    strcpy(aux_str, asctime(localtime(&hini)));
    aux_str[strlen(aux_str)-1]=0;
    debugdetalle("Comienzo del chequeo detallado: %s\n", aux_str);
    
	/* Inicia variables */
    cant_pendi=0;
    strcpy(datos_trx.fec_vtacard,fecpro);
    
	/* Abre cursor de ventas sobre las transacciones de sucursal */
	ret=DBOpenCurVta();
    if (ret!=OK)
    {
		debug("Error [%d] al abrir cursor de Vtacard\n", SqlErrno);
		debugdetalle("Error [%d] al abrir cursor de Vtacard\n", SqlErrno);
    }

	/* Repite el ciclo mientras hay transacciones y no hay error */
    while (ret==OK)
    {
		/* Obtiene una transaccion */
		ret=DBObtNextVta(&datos_trx);
		if (ret!=OK) 
		{
			if (ret!=NOF)
			{
				debug("Error [%d] al obtener siguiente venta de Vtacard\n", SqlErrno);
				debugdetalle("Error [%d] al obtener siguiente venta de Vtacard\n", SqlErrno);
			}
			break;
        }

		/* Verifica si se encuentra en credito */
		ret=DBVerifCred(&datos_trx, datos_suc, &datos_log);
		if (ret==NOOK)
		{
			debug("Error [%d] al verificar transaccion\n", SqlErrno);
			debugdetalle("Error [%d] al verificar transaccion\n", SqlErrno);
			break;
		}

		/* Prepara la situacion para loguear en el archivo */	
		if ( atol(datos_trx.monto) == (long)datos_log.Monto ) 
		{
			if ( datos_log.Caja == 'F' )
			{
				sprintf(szSituacion, "CAJA FICTICIA. ");
			}
			else
			{
				memset(szSituacion, 0, sizeof(szSituacion));
			}
			if ( strcmp(datos_log.Fecha, &fecpro[2]) == 0 )
			{
				strcat(szSituacion, "OK");
			}
			else
			{
				strcat(szSituacion, "DIFIERE FECHA. OK");
			}
		}
		else
		{	
			if (datos_log.Monto == -1)
			{
				sprintf(szSituacion, "PENDIENTE A INCORPORAR");
			}
			else
			{
				sprintf(szSituacion, "DIFIERE MONTO DE COMPRA");
			}
		}

		/* Loguea datos detallados de la transaccion */
        debugdetalle("\nEmp[%03s] Suc[%05s] Ter[%05s] Trx[%04s] Fecha[%.6s]\n",
			datos_trx.nro_suc, datos_trx.nro_caj_sf, datos_trx.nro_caj,	datos_trx.nro_trx, datos_trx.fec_trx);
		debugdetalle("Monto Vtacard[%8.2f] Monto Credito[%8.2f] %s\n",	
			(float)atol(datos_trx.monto)/100, (float)datos_log.Monto/100, szSituacion ); 

		/* Si no se encuentra en credito */
		if (ret==NOF)
		{
			/* Loguea pendiente a incorporar */
			debug("  -Pend: Emp[%03s] Suc[%05s] Ter[%05s] Trx[%04s] Monto[%8.2f] Fecha[%.6s]\n",
					datos_trx.nro_suc, datos_trx.nro_caj_sf,datos_trx.nro_caj,
					datos_trx.nro_trx, (float)atol(datos_trx.monto)/100, datos_trx.fec_trx);
		
			/* Obtiene los datos faltantes de la transaccion */
			ret=DBCompTrx(&datos_trx, datos_suc);
			if (ret==NOOK)
			{
				debugdetalle("      Plan[%s] Tarjeta[%s] Numero[%s]\n", datos_trx.plan_sf, 
									datos_trx.cod_tar, datos_trx.nro_tar); 
                debugdetalle("Error [%d] obteniendo datos de transaccion a insertar\n", SqlErrno);
                debug("Error [%d] obteniendo datos de transaccion a insertar\n", SqlErrno);
				break;
			}
			
			/* Inserta la transaccion en credito */
			ret=DBIncCred(&datos_trx);
			if (ret==NOOK)
			{
				debug("Error [%d] al insertar transaccion pendiente en Credito\n", SqlErrno);
				debugdetalle("Error [%d] al insertar transaccion pendiente en Credito\n", SqlErrno);
				break;
			}
			cant_pendi++;
        }
		else
		{
			/* Marca la transaccion en credito */
			ret=DBUpdRegistroCredito(&datos_trx);
			if (ret==NOOK)
			{
				debug("Error [%d] al actualizar fecha en Credito\n", SqlErrno);
				debugdetalle("Error [%d] al actualizar fecha en Credito\n", SqlErrno);
				break;
			}		
        }
    }

	/* Cierra el cursor y retorna */
    if (ret==NOF)
    {
		ret=DBCloseCurVta();
		datos_suc->cant_pendi=cant_pendi;
		return OK;
    }
	else
	{
		return NOOK;
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
main(int argc, char** argv)
{
	int iParam;
	char FecPro[20];
	time_t hIni; 
	char *arg;
	char aux_fec[30];
	char aux_suc[30];
	char aux_emp[30];
	int ret;
	str_datos_suc datos_suc;
	char aux_str[200];
	int iMuchasPend = 0;
	
	/* Verifica correccion en cantidad de parametros */
	if (argc!=4)
	{
		printf("Error en parametros:\n");
		printf("%s -fDD/MM/AAAA -sXXXXX -eXXX\n", argv[0]);
		printf("         f:Fecha\n");
		printf("         s:Sucursal\n");
		printf("         e:Empresa\n");
		Exit(16, &datos_suc);
	}

	/* Loguea comienzo de ejecucion */
	time(&hIni);
	strcpy(aux_str,asctime(localtime(&hIni)));
	aux_str[strlen(aux_str)-1]=0;
	ret = debug("\n\n\Programa Arrancado [%s]\n", aux_str);
	if ( ret == NOOK )
	{
		printf("Error al intentar abrir el Archivo de Log\n");
		Exit(17, &datos_suc);
	}
	debugdetalle("\nForma de ejecucion [%s %s %s %s]\n", argv[0],argv[1],argv[2], argv[3]);
	debug("Forma de ejecucion [%s %s %s %s]\n", argv[0],argv[1],argv[2], argv[3]);
	debug("Version de Aplicacion: %s\n", VERSION_APLIC);
	debug("Ultima modificacion: %s\n", ULTIMA_MODIF);

	/* Verifica correccion en parametros */
	for (iParam = 1; iParam < argc; iParam++)
	{
		arg = argv[iParam];
		if (*arg != '-')
			continue;
	
		switch (arg[1])
		{
			case 'f': 
				strcpy(aux_fec, &arg[2]);
				break;
			case 's':
				strcpy(aux_suc, &arg[2]);
				break;
			case 'e': 
				strcpy(aux_emp, &arg[2]);
				break;
			default:
				break;
		}
	}

	/* Se conecta a la base de datos */
	ret=DBConnect();
	if (ret!=OK)
	{
		debug("Error conectando a la base [%d]!!! - Abortando \n",SqlErrno);
		Exit(5, &datos_suc);
	}

	/* Obtiene fecha de proceso */
	ret=DBObtFecha(aux_fec, FecPro);
	if (ret!=OK)
	{
		debug("No se puede obtener fecha a procesar [%d] - Abortando\n\n\n", SqlErrno);
		Exit(6, &datos_suc);
	}

	/* Obtiene datos de la sucursal */
	strcpy(datos_suc.szFechaProc, FecPro);
	strcpy(datos_suc.nro_caj_sf , aux_suc);
	strcpy(datos_suc.nro_suc    , aux_emp);
	ret=DBObtDatosSuc(&datos_suc);
	if (ret!=OK)
	{
		debug("No se pueden obtener datos de la sucursal [%d] - Abortando\n\n\n",
		SqlErrno);
		Exit(7, &datos_suc);
	}

	/* Trunca la tabla */
	ret=DBTruncate();
	if (ret!=OK)
	{
		debug("Error [%d] truncando tabla vtacard_dia_new !!! - Abortando \n", SqlErrno);
		Exit(13, &datos_suc);
	}

	/* Carga tabla en funcion del archivo de entrada */
	ret=DBCargaTablaVta(&datos_suc, FecPro);
	if (ret!=OK)
	{
		debug("No se puede cargar tabla vtacard_dia_new [%d]. Abortando \n\n\n", SqlErrno);
		Exit(15, &datos_suc);
	}
	
	/* Obtiene los totales de la sucursal */
	debug("  Suma de Totales Comienza %s\n",TimeStamp());
	ret=DBSumVtaCard(&datos_suc);
	if (ret!=OK)
	{
		debug("No se puede obtener la suma de vtacard [%d]\n",SqlErrno);
		Exit(8, &datos_suc);
	}

	/* Obtiene los totales de credito */
	ret=DBSumCredito(&datos_suc, FecPro);
	if (ret!=OK)
	{
		debug("No se puede obtener la suma de credito [%d]\n",SqlErrno);
		Exit(9, &datos_suc);
	}

	/* Loguea resultados de consulta */
	debug("  Suma de Totales Finaliza %s\n",TimeStamp());
	debug("Total Vtacard [%8.2f]         ",(float)datos_suc.tot_vta/100);
	debug("Total Credito [%8.2f]\n",(float)datos_suc.tot_cre/100);

	/* Verifica si los totales coinciden */
	if (datos_suc.tot_cre!=datos_suc.tot_vta)
	{
		/* TOTALES NO COINCIDEN */
		debug("Totales no coinciden - Checkeo detallado \n");
		debug("  Comienzo de Incorporacion de pendientes %s\n",TimeStamp());
		
		/* Incorpora las pendientes */
		ret= IncPend(&datos_suc,FecPro);
		if (ret==NOOK)
		{
			datos_suc.cant_pendi = 0;
			debug("  Error en incorporacion de pendientes. Abortando!!!\n");
			Exit(10, &datos_suc); 
		}
		debug("  Fin de Incorporacion de pendientes      %s\n",TimeStamp());
		debug("  Total de pendientes: %d, permitidas: %d\n", datos_suc.cant_pendi,datos_suc.max_pendi);

		/* Verifica si supera el maximo permitido */
		if (datos_suc.cant_pendi>datos_suc.max_pendi)
		{
			datos_suc.cant_pendi = 0;
			debug("  Error Demasiadas pendientes!!! - Abortando Pendientes\n");
			DBRollback();
			iMuchasPend = 1;
		}
		else
		{
			/* Obtiene nuevamente los totales de credito */
			debug("  Nueva suma de totales                   %s\n",TimeStamp());
			ret=DBSumCreFecVta(&datos_suc,FecPro);
			if (ret!=OK)
			{
				debug("No se puede obtener la suma de credito [%d]\n",SqlErrno);
				Exit(9, &datos_suc);
			}

			/* Loguea los totales */
			debugdetalle("\nTotal Vtacard [%8.2f]\n",(float)datos_suc.tot_vta/100);
			debugdetalle("Total Credito [%8.2f]\n\n",(float)datos_suc.tot_cre/100);

			/* Si los totales no coinciden, sale por error */
			if (datos_suc.tot_cre!=datos_suc.tot_vta)
			{
				debug("Total de credito no coincide con vtacard!!! - Abortando\n");
				debug("Verifique datos de configuracion en tablas.\n");
				Exit(4, &datos_suc);
			}
	
			/* Actualiza la fecha de conciliacion */
			debug("  Actualizacion de transacciones          %s\n",TimeStamp());
			ret=DBActFechaConc (&datos_suc, FecPro); 
			if (ret!=OK)
			{
				debug("  Error [%d] actualizando fecha de conciliacion en cred_trx\n", SqlErrno);
				Exit(11, &datos_suc);
			}

			/* Confirma movimientos */
			ret=DBCommit();
		}

		/* Actualiza la fecha de conciliacion */
		ret=DBActFechaConc (&datos_suc, FecPro); 
		if (ret!=OK)
		{
			debug("  Error [%d] actualizando fecha de conciliacion en cred_trx\n", SqlErrno);
			Exit(11, &datos_suc);
		}

		/* Confirma movimientos */
		ret=DBCommit();

		/* Loguea el resultado en el archivo de log */
		EscribirResultado(&datos_suc, iMuchasPend);
			
		/* Finaliza el proceso */
		debug("Finalizacion del proceso %s\n",TimeStamp());
		if (iMuchasPend ==1)	Exit(14, &datos_suc);
		Exit(1, &datos_suc);
	}
	else
	{
		/* TOTALES COINCIDEN */
		debug("Totales coinciden - Actualizando transacciones\n");

		/* Actualiza transacciones en credito */
		ret=DBUpdCredito(&datos_suc,FecPro);
		if (ret!=OK)
		{ 
			debug("Error actualizando transacciones de credito [%d]\n",SqlErrno);
			Exit(12, &datos_suc);
		}

		/* Confirma movimientos */
		ret=DBCommit();

		/* Finaliza el proceso */
		debug("Finalizacion del proceso %s\n",TimeStamp());  
		Exit(0, &datos_suc);
	}
}
