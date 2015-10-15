#ifdef OS_Linux
#pragma pack(1)
#endif

#define NUM_VERSION "v1.01 26-Sep-2013"

/*
+------------------------------------------------------------------------------+
|                   Copyright (C) 2013 - COTO CICSA                            |
+-----------------+------------------------------------------------------------+
| F. Fuente       | PostCierreCASuc.C                                          |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| Descripcion     | Plancha los datos criticos de tarjetas                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/

/****************************************/
/* Formato del Log Visual: [ww,x,y,z]	*/
/* ww : Nivel en el arbol				*/
/* x  : Indicador de Nodo(0) u Hoja(1)  */
/* y  : Indicador de OK(0) o Error(1)   */
/* z  : Nivel de Log 0..3				*/
/****************************************/

/* Includes */
#include "Config.h"
#include "Daemons.h"
#include "Includes.h"
#include "Log.h"
#include "Table.h"
extern "C" 
{
	#include "DBServices.h"
	#include "DBTrxOff.h"
}

/* Definiciones */
#define ERROR_BASE_DE_DATO	-4	/* Error irrecuperable: Operaciones sobre base de datos      */
#define ERROR_CONFIG_TERMI	-5	/* Advertencia: Alguna terminal sin configurar               */

#define NOOK1				-9
#define NOOK				-1
#define OK					 0

/* Objetos globales */
BatchData		batch_data;
BatchTable		DBBatch;
Config			Cfg(CONFIG_FILE);
Log				LogVisual;
str_fet_term	term_data;

/* Variables externas */
extern int SqlErrno;

/* Variables globales */
char szArchivoTemporal[256];
char szArchivoFinal[256];

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ObtenerFechaCierre                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene la fecha actual de cierre                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *ObtenerFechaCierre()
{
	static char cTimestamp[40];
	struct tm *newtime;
	time_t ltime;
	
	time(&ltime);
	newtime=localtime(&ltime);
	sprintf(cTimestamp,"%04d%02d%02d",newtime->tm_year+1900,
		newtime->tm_mon+1,newtime->tm_mday);
	return cTimestamp;
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ProcesaPostCierre                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Lanza el planchado de datos de la terminal seleccionada    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int ProcesaPostCierre(str_fet_term term_data)
{
	int	retProc;
	
	/* Arma la estructura para consulta */
	batch_data.NroSuc(term_data.nro_suc);
	batch_data.NroCaj(term_data.nro_caj);
	batch_data.NroCA(term_data.nro_ca);
	batch_data.NroLot(term_data.nro_lot);
	batch_data.NroCom(term_data.nro_com);
	batch_data.NroCajCa(term_data.nro_caj_ca);

	/* Recupera los totales de la terminal */
	retProc = DBBatch.UpdatePostCierre(batch_data.GetUpdData());
	LogVisual.Put(3,"[04,1,0,3] Retorno de DBBatch.UpdatePostCierre:%d, GetErrno:%d\n", retProc, DBBatch.GetErrno());
	if (retProc)
	{
		LogVisual.Put(0, "[04,1,0,0] Error al actualizar datos criticos\n");
		return ERROR_BASE_DE_DATO;
	}
	return 0;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | vShutDown                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra las colas abiertas y finaliza la aplicacion         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void vShutDown(int iSignal)
{
	int		iRet= 0;
	int		iReintentos= 0;
	char	szComando[256];

	/* Loguea fin de proceso */
	LogVisual.Put(0,"[01,0,0,0] Fin de Proceso\n");
	
	/* Loguea fin de proceso */
	if (iSignal==0)
	{
		LogVisual.Put(0, "[02,1,0,0] Fin de proceso. Retorno:%d. %s", iSignal, currentTimeLog() );
	}
	else
	{
		LogVisual.Put(0, "[02,1,1,0] Fin de proceso. Retorno:%d. %s", iSignal, currentTimeLog() );
	}

	/* Concatena el archivo de log temporal al archivo final */
	iRet= 0;
	sprintf(szComando, "cat %s >> %s", szArchivoTemporal, szArchivoFinal);
	LogVisual.Put(3, "[02,1,0,3] Llamada al sistema para ejecutar comando:\n");
	LogVisual.Put(3, "[02,1,0,3] %s\n", szComando);
	do
	{
		if (iRet != 0) 
		{
			LogVisual.Put(3, "[02,1,1,3] Error al copiar archivo de Log\n"); 
			iReintentos++;
			sleep(5);
		}
		iRet= system(szComando);
	} while ((iRet!=0) && (iReintentos<5));
	
	/* Borra archivo temporal */
	if (iRet == 0)
	{
		sprintf(szComando, "rm -f %s\n", szArchivoTemporal);
		system(szComando);
	}
	
	/* Cierra archivo de log */
	LogVisual.Close();

	/* Retorna el resultado */
	exit(iSignal);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | vShutDownTerminal                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Llama a funcion de finalizacion de aplicacion              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void vShutDownTerminal(int iSignal=9)
{
	vShutDown(9);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | vInit                                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Inicia las variables de configuracion                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void vInit(char *nro_ca, char *nro_caj_sf, char *nro_suc)
{
	FILE	*fArch;
	char	szFecha[20]; 
	char	szComando[256];
	char	szAuxStr1[100];
	
	/* Toma las señales por si se aborta */ 
	signal(SIGINT, vShutDownTerminal);
	signal(SIGTERM, vShutDownTerminal);
	
	/*  Toma fecha de archivo fecha.dat (contiene la fecha del dia anterior) */
	if ((fArch=fopen("../log/fecha.dat", "r"))==NULL)
	{
		printf("\nError en apertura de archivo (fecha.dat)\n");
		exit(1);
	}
	fscanf(fArch, "%s", szFecha);
	fclose(fArch);
	
	/* Arma el nombre del archivo de log */
	if (Cfg.GetItem("AdminDaemon", "LogPostCierreCASuc", szAuxStr1)!=CFG_FOUND)
	{
		sprintf(szAuxStr1, "/online/log/cierre/LogPostCierre");
	}
	sprintf(szArchivoFinal, "%s%s.%s.%s", szAuxStr1, nro_ca, "SUC", szFecha); 
	sprintf(szArchivoTemporal, "LogPostCierre%s%s%s.%s.tmp", nro_ca, nro_suc, nro_caj_sf, szFecha);
	
	/* Borra el archivo de log si existe */
	sprintf(szComando, "rm -f %s\n", szArchivoTemporal);
	system(szComando);

	/* Abre archivo de log */
	LogVisual.Open(szArchivoTemporal);
	
	/* Configura el nivel de log */
	if (Cfg.GetItem("AdminDaemon", "LogLevelVisualCASuc", szAuxStr1)!=CFG_FOUND)
	{
		sprintf(szAuxStr1, "3");
	}
	LogVisual.SetLevel(atoi(szAuxStr1));

	/* Configura el nivel de log por pantalla */
	if (Cfg.GetItem("AdminDaemon", "LogLevelScreenVisualCASuc", szAuxStr1)!=CFG_FOUND)
	{
		sprintf(szAuxStr1, "3");
	}
	LogVisual.SetDebugLevel(atoi(szAuxStr1));

	/* Loguea el inicio del proceso */
	memset(szAuxStr1, 0, sizeof(szAuxStr1));
	strncpy(szAuxStr1, &szFecha[0],2);
	strcat(szAuxStr1, "/");
	strncat(szAuxStr1, &szFecha[2], 2);
	LogVisual.Put(0, "[00,0,0,0] Empresa:%03s - Sucursal:%05s - Centro:%02s - Fecha:%s\n", nro_suc, nro_caj_sf, nro_ca, szAuxStr1);
	LogVisual.Put(0, "[01,0,0,0] Inicio de Proceso\n");
	LogVisual.Put(0, "[02,1,0,0] PostCierreCASuc. %s\n", NUM_VERSION);

	/* Loguea finalizacion correcta de inicializacion */
	LogVisual.Put(0, "[02,1,0,0] Inicio finalizado correctamente\n");
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
	int		retTerminal;
	int		iErrorTerminal=0;
	char	szCajCA[16];
	int		iNroLot;
		
	/* Verifica correccion en parametros */
	if (    (  argc<4 )
		 || (  argc>6 ) 
	     || ( (argc==4) && (    (strlen(argv[1]) != (sizeof(term_data.nro_ca)     -1) ) 
		                     || (strlen(argv[2]) != (sizeof(term_data.nro_caj_sf) -1) ) 
							 || (strlen(argv[3]) != (sizeof(term_data.nro_suc)    -1) )  
                           )
			)
	     || ( (argc==5) && (    (strlen(argv[1]) != (sizeof(term_data.nro_ca)     -1) ) 
		                     || (strlen(argv[2]) != (sizeof(term_data.nro_caj_sf) -1) ) 
							 || (strlen(argv[3]) != (sizeof(term_data.nro_suc)    -1) )  
							 || (strlen(argv[4]) != (sizeof(term_data.nro_caj_ca) -1) )  
                           )
			)
       )
	{
		printf("\nUso:\t%s <Num.Cen> <Num.Suc> <Num.Emp> [<Num.CajCA>]\n\n", argv[0]);
		printf("\tNum.Cen - Numero de Centro Aut (%d digitos)\n"  , (sizeof(term_data.nro_ca)     -1));
		printf("\tNum.Suc - Numero de Sucursal   (%d digitos)\n"  , (sizeof(term_data.nro_caj_sf) -1));
		printf("\tNum.Emp - Numero de Empresa    (%d digitos)\n"  , (sizeof(term_data.nro_suc)    -1));
		printf("\tNum.Caj - Numero de CajaCA     (%d digitos)\n\n", (sizeof(term_data.nro_caj_ca) -1));
		exit(1);
	}
	else
	{
		memset(szCajCA, 0, sizeof(szCajCA));
		if (argc==5)
		{
			sprintf(szCajCA, argv[4]);
		}
	}
	
	/* Inicia variables de configuracion */
	vInit(argv[1],argv[2], argv[3]);
	
	/* Loguea hora de comienzo de proceso */
	LogVisual.Put(0, "[01,0,0,0] Actualizacion de operaciones. Pid:[%d]\n", getpid());
	LogVisual.Put(0, "[02,1,0,0] Comienzo de proceso:%s", currentTimeLog());
	
	/* Ciclo principal por terminal */
	do 
	{	/* Configura centro y sucursal a cerrar */
		strncpy(term_data.nro_ca    , argv[1], sizeof(term_data.nro_ca)     -1);
		strncpy(term_data.nro_caj_sf, argv[2], sizeof(term_data.nro_caj_sf) -1);
		strncpy(term_data.nro_suc   , argv[3], sizeof(term_data.nro_suc)    -1);
		strncpy(term_data.nro_caj_ca, szCajCA, sizeof(term_data.nro_caj_ca) -1);

		/* Recupera la terminal */
		retTerminal = fetch_termdata_ca_suc(&term_data);

		/* Actualiza nro de lote como el anterior al actual */
	
		iNroLot = atol(term_data.nro_lot);
		if ( iNroLot != 1 )
			sprintf(term_data.nro_lot, "%d", iNroLot - 1);
		else
			sprintf(term_data.nro_lot, "%d", 999);
			
		/* Error al recuperar terminal */
		if (retTerminal == NOOK)
		{
			LogVisual.Put(0, "[02,1,1,0] Error BD irrecuperable obteniendo datos de terminal. Cod=%d\n", SqlErrno);
			LogVisual.Put(0, "[01,1,1,0] CANCELADO PROCESO DE CIERRE POR ERRORES DE BASE DE DATOS\n");
			iErrorTerminal = ERROR_BASE_DE_DATO;
			break;
		}
		
		/* Error en terminal (mal configurada en CRED_CAS o CRED_COM_CIE) */
		if (retTerminal == NOOK1)
		{
			LogVisual.Put(0, "[02,0,0,0] Terminal:%s - CajaCA:%s - Lote:%s - CodConCie:%s\n",
							 term_data.nro_caj, term_data.nro_caj_ca, term_data.nro_lot, term_data.cod_con_cie);
			LogVisual.Put(0, "[03,1,1,0] Error. Verificar configuración en crédito\n");
			iErrorTerminal = ERROR_CONFIG_TERMI;
			continue;
		}

		/* No hay terminales */
		if (retTerminal == 1403)
		{
			continue;
		}

		/* Loguea datos de la terminal a cerrar */
		LogVisual.Put(0, "[02,0,0,0] Terminal:%s - CajaCA:%s - Lote:%s - CodConCie:%s\n",
						 term_data.nro_caj, term_data.nro_caj_ca, term_data.nro_lot, term_data.cod_con_cie);
		LogVisual.Put(1, "[03,1,0,1] NroSuc:%s - NroCom:%s - NroComCie:%s\n", 
						 term_data.nro_suc, term_data.nro_com, term_data.nro_com_cie);
		
		/* Procesa el planchado de la terminal */
		LogVisual.Put(0, "[03,0,0,0] ProcesaPostCierre\n");
		iErrorTerminal = ProcesaPostCierre(term_data);
		LogVisual.Put(3,"[04,1,0,3] Retorno de ProcesaPostCierre:%d\n", iErrorTerminal);
		
		/* Error critico: base de datos */
		if (iErrorTerminal == ERROR_BASE_DE_DATO)
		{
			LogVisual.Put(0,"[01,1,1,0] CANCELADO PROCESO DE CIERRE POR PROBLEMAS EN BASE DE DATOS\n");
			break;
		}

	} while (retTerminal != 1403);
	
	/* Finaliza */
	vShutDown(abs(iErrorTerminal));

	return OK;
}
