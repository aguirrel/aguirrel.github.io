
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,1,0,
};

/* Binding to PL/SQL Records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[63];
};
static struct sqlcxp sqlfpn =
{
    62,
    "/codigos/coto/v220/servofer/src/ChecDif//kap/source/ChecDif.pc"
};


static unsigned int sqlctx = 1654138395;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
            short *cud;
   unsigned char  *sqlest;
            char  *stmt;
   sqladts *sqladtp;
   sqltdss *sqltdsp;
   unsigned char  **sqphsv;
   unsigned long  *sqphsl;
            int   *sqphss;
            short **sqpind;
            int   *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
   unsigned int   sqlcmax;
   unsigned int   sqlcmin;
   unsigned int   sqlcincr;
   unsigned int   sqlctimeout;
   unsigned int   sqlcnowait;
            int   sqfoff;
   unsigned int   sqcmod;
   unsigned int   sqfmod;
   unsigned char  *sqhstv[20];
   unsigned long  sqhstl[20];
            int   sqhsts[20];
            short *sqindv[20];
            int   sqinds[20];
   unsigned long  sqharm[20];
   unsigned long  *sqharc[20];
   unsigned short  sqadto[20];
   unsigned short  sqtdso[20];
} sqlstm = {12,20};

/* SQLLIB Prototypes */
extern sqlcxt (/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlcx2t(/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlbuft(/*_ void **, char * _*/);
extern sqlgs2t(/*_ void **, char * _*/);
extern sqlorat(/*_ void **, unsigned int *, void * _*/);

/* Forms Interface */
static int IAPSUCC = 0;
static int IAPFAIL = 1403;
static int IAPFTL  = 535;
extern void sqliem(/*_ char *, int * _*/);

 static char *sq0003 = 
"08','10','11','12','19','21','25','43','57')) order by NRO_SUC,NRO_CAJ_SF,NR\
O_CAJ,NRO_TRX,COD_EST            ";

 static char *sq0004 = 
"select 'C' ,NRO_SUC ,NRO_CAJ ,NRO_TRX ,NRO_CA ,NRO_CAJ_SF ,COD_TAR ,NRO_TAR \
,MONTO ,FEC_LOC ,MOD_ENV ,MOD_ING ,PLAN_SF ,NRO_COM ,COD_EST ,NRO_CAJ_CA ,COD_\
TRX ,NVL(FEC_VTACARD,'    ') ,NVL(COD_AUT,'      ') ,'1'  from CRED_TRX where \
((FEC_LOC>=:b0 and FEC_LOC<=:b1) and ((((((COD_EST<>:b2 and NRO_SUC=:b3) and N\
RO_CAJ_SF=:b4) and NRO_CAJ=:b5) and NRO_TRX=:b6) and NRO_TAR=:b7) and MONTO=:b\
8))           ";

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* CUD (Compilation Unit Data) Array */
static short sqlcud0[] =
{12,4130,1,0,0,
5,0,0,1,0,0,539,234,0,0,4,4,0,1,0,1,9,0,0,1,10,0,0,1,10,0,0,1,10,0,0,
36,0,0,2,79,0,516,247,0,0,2,1,0,1,0,1,9,0,0,2,9,0,0,
59,0,0,3,1133,0,521,298,0,0,6,6,0,1,0,1,9,0,0,1,9,0,0,1,9,0,0,1,9,0,0,1,9,0,0,
1,9,0,0,
98,0,0,3,0,0,525,308,0,0,20,0,0,1,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,
9,0,0,2,9,0,0,2,9,0,0,2,4,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,
2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,
193,0,0,4,402,0,521,343,0,0,9,9,0,1,0,1,9,0,0,1,9,0,0,1,9,0,0,1,9,0,0,1,9,0,0,
1,9,0,0,1,9,0,0,1,9,0,0,1,4,0,0,
244,0,0,4,0,0,525,351,0,0,20,0,0,1,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,
9,0,0,2,9,0,0,2,9,0,0,2,4,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,
2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,
339,0,0,4,0,0,527,369,0,0,0,0,0,1,0,
354,0,0,3,0,0,527,382,0,0,0,0,0,1,0,
};


#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+------------------------------------------------------------------------------+
|                   Copyright (C) 2003 - COTO CICSA                            |
+-----------------+------------------------------------------------------------+
| F. Fuente       | ChecDif.pc                                                 |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| Descripcion     | Proceso que genera un archivo con las posibles             |
|                 | transacciones con problemas en CREDITO                     |
+-----------------+------------------------------------------------------------+
*/
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

/* EXEC SQL INCLUDE SQLCA;
 */ 
/*
 * $Header: sqlca.h,v 1.3 1994/12/12 19:27:27 jbasu Exp $ sqlca.h 
 */

/* Copyright (c) 1985,1986, 1998 by Oracle Corporation. */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */


#define NOMBPROCESO "ChecDif"

#define FOK	0
#define FIN	1
#define SIG	2

#define ARCHLOG	'L'
#define ARCHIVO	'A'

/* Definiciones de Oracle */
#define CPVAR(X)			X.arr 
#define CPMEM(X)			X.arr[X.len] = 0
#define SQLCODE				abs(sqlca.sqlcode)
#define VAR_TO_STR( Y, X )  ( memcpy( Y, X.arr, X.len ), Y[X.len]=0x00 )
#define STR_TO_VAR( Y, X )  ( Y.len = (unsigned short)strlen( X ), \
                                   memcpy( Y.arr, X, Y.len ) )

/* Prototipos de funciones */
void pvFinLog(FILE **prLogStream);
void pvLog( FILE *, char, int, const char *, ... );
int  piIniLog(FILE **prLogStream, char * szNombreArchivo);
void pvArmarLinea(char *, int );

/* EXEC SQL BEGIN DECLARE SECTION; */ 

	double		MONTO;
	/* VARCHAR		CUSUARIO	[8+1]; */ 
struct { unsigned short len; unsigned char arr[9]; } CUSUARIO;

	/* VARCHAR		NRO_SUC		[3+1]; */ 
struct { unsigned short len; unsigned char arr[4]; } NRO_SUC;

	/* VARCHAR		NRO_CAJ		[5+1]; */ 
struct { unsigned short len; unsigned char arr[6]; } NRO_CAJ;

	/* VARCHAR		NRO_TRX		[4+1]; */ 
struct { unsigned short len; unsigned char arr[5]; } NRO_TRX;

	/* VARCHAR		NRO_CA		[2+1]; */ 
struct { unsigned short len; unsigned char arr[3]; } NRO_CA;

	/* VARCHAR		NRO_CAJ_SF	[5+1]; */ 
struct { unsigned short len; unsigned char arr[6]; } NRO_CAJ_SF;
  
	/* VARCHAR		COD_TAR		[2+1]; */ 
struct { unsigned short len; unsigned char arr[3]; } COD_TAR;

	/* VARCHAR		NRO_TAR	   [20+1]; */ 
struct { unsigned short len; unsigned char arr[21]; } NRO_TAR;

	/* VARCHAR		FEC_LOC    [10+1]; */ 
struct { unsigned short len; unsigned char arr[11]; } FEC_LOC;

	/* VARCHAR		MOD_ENV		[2+1]; */ 
struct { unsigned short len; unsigned char arr[3]; } MOD_ENV;

	/* VARCHAR 	MOD_ING		[2+1]; */ 
struct { unsigned short len; unsigned char arr[3]; } MOD_ING;

	/* VARCHAR		PLAN_SF		[2+1]; */ 
struct { unsigned short len; unsigned char arr[3]; } PLAN_SF;

	/* VARCHAR		NRO_COM	   [15+1]; */ 
struct { unsigned short len; unsigned char arr[16]; } NRO_COM;

	/* VARCHAR		COD_EST		[2+1]; */ 
struct { unsigned short len; unsigned char arr[3]; } COD_EST;

	/* VARCHAR		NRO_CAJ_CA	[8+1]; */ 
struct { unsigned short len; unsigned char arr[9]; } NRO_CAJ_CA;

	/* VARCHAR		FECHA_MIN  [10+1]; */ 
struct { unsigned short len; unsigned char arr[11]; } FECHA_MIN;

	/* VARCHAR		FECHA_MAX  [10+1]; */ 
struct { unsigned short len; unsigned char arr[11]; } FECHA_MAX;

	/* VARCHAR		MMDD_MIN	[4+1]; */ 
struct { unsigned short len; unsigned char arr[5]; } MMDD_MIN;

	/* VARCHAR		MMDD_MAX	[4+1]; */ 
struct { unsigned short len; unsigned char arr[5]; } MMDD_MAX;

	/* VARCHAR		COD_TRX		[2+1]; */ 
struct { unsigned short len; unsigned char arr[3]; } COD_TRX;

	/* VARCHAR		MOVIMIE		[1+1]; */ 
struct { unsigned short len; unsigned char arr[2]; } MOVIMIE;

	/* VARCHAR		FEC_VTACARD [4+1]; */ 
struct { unsigned short len; unsigned char arr[5]; } FEC_VTACARD;

	/* VARCHAR		COD_AUT		[6+1]; */ 
struct { unsigned short len; unsigned char arr[7]; } COD_AUT;

	/* VARCHAR		ASO_REG		[1+1]; */ 
struct { unsigned short len; unsigned char arr[2]; } ASO_REG;

/* EXEC SQL END DECLARE SECTION; */ 


/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvIniLog                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion para abrir el archivo de log                       |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int piIniLog(FILE **prLogStream, char * szNombreArchivo)
{
	int iRet= 0;
	char szArchivo[128];

	if ((*prLogStream = fopen(szNombreArchivo, "wt"))==NULL)
	{
		iRet = -1;
	}
	return (iRet);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvFinLog                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion para cerrar el archivo de log                      |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
void pvFinArch(FILE **prLogStream)
{
	if (*prLogStream)
	{
		fclose(*prLogStream);
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvLog                                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion para escribir los mensajes en el archivo de log    |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
void pvLog( FILE *prLogStream, char cLog, int iCont, const char * szMascara, ... )
{
    char szBuffer[1500], szBuffer2[1500], szFecha[12], szHora[10];
    va_list pcListaParametros;

    struct tm *newtime;
    time_t long_time;

    /* Obtiene  la fecha y hora */
    time( &long_time );                
    newtime = localtime( &long_time ); 
    sprintf(szFecha, "%02d/%02d/%04d", newtime->tm_mday, newtime->tm_mon+1, 
			1900+newtime->tm_year);
    sprintf(szHora, "%02d:%02d:%02d", newtime->tm_hour, newtime->tm_min, 
			newtime->tm_sec);
    
    va_start( pcListaParametros, szMascara );

	/* Guarda la información */
    if( prLogStream )  
    {               
            if (cLog==ARCHLOG)
			{
				sprintf( szBuffer, "%10.10s %8.8s %s\n", szFecha, szHora, szMascara );
            }
			else
			{
				sprintf( szBuffer, "%s\n", szMascara );
			}
			vsprintf( szBuffer2, szBuffer, pcListaParametros );
			fprintf( prLogStream, szBuffer2 );
            fflush( prLogStream );
    }

	/* Finaliza el proceso si es error critico */
	if ( ( iCont==FIN ) || ( iCont==FOK ) )
	{
		if ( prLogStream )
		{
			fclose(prLogStream);
		}
		exit(iCont);
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvArmarLinea                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Arma linea para escribir en el archivo                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void pvArmarLinea(char *szLinea, int iLong)
{
	CPMEM(NRO_SUC); CPMEM(NRO_CAJ); CPMEM(NRO_TRX); CPMEM(NRO_CA); CPMEM(NRO_CAJ_SF);
	CPMEM(COD_TAR); CPMEM(NRO_TAR); CPMEM(FEC_LOC); CPMEM(MOD_ENV); CPMEM(MOVIMIE);
	CPMEM(MOD_ING); CPMEM(PLAN_SF); CPMEM(NRO_COM); CPMEM(COD_EST); CPMEM(NRO_CAJ_CA);
	CPMEM(COD_TRX); CPMEM(ASO_REG); CPMEM(COD_AUT); CPMEM(FEC_VTACARD);

	memset(szLinea, 0, sizeof(iLong));
	sprintf(szLinea, "%01.1s|%03.3s|%05.5s|%05.5s|%04.4s|%02.2s|"
					 "%02.2s|%020.20s|%011.02f|%010.10s|%02.2s|"
					 "%03.3s|%02.2s|%015.15s|%08.8s|%02.2s|%06.6s|%04.4s|%02.2s|%01.1s" ,
						CPVAR(MOVIMIE),
						CPVAR(NRO_SUC), CPVAR(NRO_CAJ_SF), CPVAR(NRO_CAJ), CPVAR(NRO_TRX), CPVAR(NRO_CA), 
						CPVAR(COD_TAR), CPVAR(NRO_TAR), MONTO/100, CPVAR(FEC_LOC), CPVAR(MOD_ENV),
						CPVAR(MOD_ING), CPVAR(PLAN_SF), CPVAR(NRO_COM), CPVAR(NRO_CAJ_CA),
						CPVAR(COD_TRX), CPVAR(COD_AUT), CPVAR(FEC_VTACARD), CPVAR(COD_EST), CPVAR(ASO_REG) );		
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso principal de la aplicacion                         |
|                 | Parametros: [Fecha de las operaciones MMDD]                | 
|                 | Ejemplo: ChecDif 0612                                      | 
+-----------------+------------------------------------------------------------+
*/
void main (int argc, char **argv)
{
	/* Definicion de variables */
	int iError= 0;
	FILE *fdLog = NULL;
	FILE *fdSal = NULL;
	char szLinea[512];

	/* Abre archivo de log */
	sprintf(szLinea, "%s%s", NOMBPROCESO, ".log");
	if (piIniLog(&fdLog, szLinea))
	{
		printf("Error al abrir archivo de log\n");
		exit(1);
	}

	/* Inicia proceso */
	pvLog(fdLog, ARCHLOG, SIG, "Inicio de proceso");

	/* Verifica que exista el parametro */
	if ( (argc!=2) || (strlen(argv[1])!=4) )
	{
		pvLog(fdLog, ARCHLOG, FIN, "Error en parametros. Sintaxis: MMDD");
	}
	
	/* Abre archivo de salida */ 
	sprintf(szLinea, "%s%s%s", NOMBPROCESO, argv[1], ".txt");
	if (piIniLog(&fdSal, szLinea))
	{
		pvLog(fdLog, ARCHLOG, FIN, "Error al abrir archivo de salida");
	}

	/* Muestra el parametro de ejecucion */
	pvLog(fdLog, ARCHLOG, SIG, "Parametro de ejecucion: %s", argv[1]);

	/* Obtiene usuario y password */
	STR_TO_VAR(CUSUARIO, "/");

	/* Se conecta a BD */
	/* EXEC SQL
		CONNECT :CUSUARIO; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 4;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.iters = (unsigned int  )10;
 sqlstm.offset = (unsigned int  )5;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)256;
 sqlstm.occurs = (unsigned int  )0;
 sqlstm.sqhstv[0] = (unsigned char  *)&CUSUARIO;
 sqlstm.sqhstl[0] = (unsigned long )11;
 sqlstm.sqhsts[0] = (         int  )11;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqinds[0] = (         int  )0;
 sqlstm.sqharm[0] = (unsigned long )0;
 sqlstm.sqadto[0] = (unsigned short )0;
 sqlstm.sqtdso[0] = (unsigned short )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqphss = sqlstm.sqhsts;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqpins = sqlstm.sqinds;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlstm.sqpadto = sqlstm.sqadto;
 sqlstm.sqptdso = sqlstm.sqtdso;
 sqlstm.sqlcmax = (unsigned int )100;
 sqlstm.sqlcmin = (unsigned int )2;
 sqlstm.sqlcincr = (unsigned int )1;
 sqlstm.sqlctimeout = (unsigned int )0;
 sqlstm.sqlcnowait = (unsigned int )0;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

 
	
	/* Verifica conexion */
	if (SQLCODE!=0)
	{
		pvLog(fdLog, ARCHLOG, FIN, "Error al conectar a BD");
	}

	/* Toma el MMDD inicial */
	STR_TO_VAR(MMDD_MIN, argv[1]);

	/* Arma el MMDD final */
	/* EXEC SQL
		SELECT SUBSTR(TO_CHAR(TO_DATE(:MMDD_MIN, 'MMDD')+1,'MMDD'),1,4) 
		INTO :MMDD_MAX	
		FROM DUAL; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 4;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.stmt = "select SUBSTR(TO_CHAR((TO_DATE(:b0,'MMDD')+1),'MMDD'),1,4) i\
nto :b1  from DUAL ";
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )36;
 sqlstm.selerr = (unsigned short)0;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)256;
 sqlstm.occurs = (unsigned int  )0;
 sqlstm.sqhstv[0] = (unsigned char  *)&MMDD_MIN;
 sqlstm.sqhstl[0] = (unsigned long )7;
 sqlstm.sqhsts[0] = (         int  )0;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqinds[0] = (         int  )0;
 sqlstm.sqharm[0] = (unsigned long )0;
 sqlstm.sqadto[0] = (unsigned short )0;
 sqlstm.sqtdso[0] = (unsigned short )0;
 sqlstm.sqhstv[1] = (unsigned char  *)&MMDD_MAX;
 sqlstm.sqhstl[1] = (unsigned long )7;
 sqlstm.sqhsts[1] = (         int  )0;
 sqlstm.sqindv[1] = (         short *)0;
 sqlstm.sqinds[1] = (         int  )0;
 sqlstm.sqharm[1] = (unsigned long )0;
 sqlstm.sqadto[1] = (unsigned short )0;
 sqlstm.sqtdso[1] = (unsigned short )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqphss = sqlstm.sqhsts;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqpins = sqlstm.sqinds;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlstm.sqpadto = sqlstm.sqadto;
 sqlstm.sqptdso = sqlstm.sqtdso;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



	/* Verifica correccion */
	if (SQLCODE!=0)
	{
		pvLog(fdLog, ARCHLOG, FIN, "Error al obtener fecha");
	}
	
	/* Obtiene la fecha inicial */
	CPMEM(MMDD_MIN);
	sprintf(szLinea, "%s060000", CPVAR(MMDD_MIN));
	STR_TO_VAR(FECHA_MIN, szLinea);
	
	/* Obtiene la fecha final */
	CPMEM(MMDD_MAX);
	sprintf(szLinea, "%s055959", CPVAR(MMDD_MAX));
	STR_TO_VAR(FECHA_MAX, szLinea);
			
	/* Recupera todas las empresas y los dias de resguardo */
	/* EXEC SQL
		DECLARE CursorTrx CURSOR FOR
			SELECT 'C' , NRO_SUC  ,  NRO_CAJ , NRO_TRX  , NRO_CA   , NRO_CAJ_SF , 
						 COD_TAR  ,  NRO_TAR , MONTO    , FEC_LOC  , MOD_ENV    , 
						 MOD_ING  ,  PLAN_SF , NRO_COM  , COD_EST  , NRO_CAJ_CA ,
						 COD_TRX  ,  NVL(FEC_VTACARD,'    ')       , NVL(COD_AUT,'      ')    , '0'   
			FROM CRED_TRX
			WHERE ( (FEC_LOC >= :FECHA_MIN) AND (FEC_LOC <= :FECHA_MAX) ) AND
				  ( ( COD_EST IN ('01','08','10','11','12','19','21','25','43','57') ) OR
					( (COD_EST = '04') AND (FEC_VTACARD IS NULL) ) )
				UNION		
			SELECT 'A' , NRO_SUC  ,  NRO_CAJ , NRO_TRX  , NRO_CA   , NRO_CAJ_SF , 
						 COD_TAR  ,  NRO_TAR , MONTO    , FEC_LOC  , MOD_ENV    , 
						 MOD_ING  ,  PLAN_SF , NRO_COM  , COD_EST  , NRO_CAJ_CA ,
						 COD_TRX  ,  NVL(FEC_VTACARD,'    ')       , NVL(COD_AUT,'      ')    , '0'
			FROM CRED_ANUL_TRX
			WHERE ( (FEC_LOC >= :FECHA_MIN) AND (FEC_LOC <= :FECHA_MAX) ) AND
				  ( ( COD_EST IN ('01','08','10','11','12','19','21','25','43','57') ) )
				UNION
			SELECT 'R' , NRO_SUC  ,  NRO_CAJ , NRO_TRX  , NRO_CA   , NRO_CAJ_SF , 
						 COD_TAR  ,  NRO_TAR , MONTO    , FEC_LOC  , MOD_ENV    , 
						 MOD_ING  ,  PLAN_SF , NRO_COM  , COD_EST  , NRO_CAJ_CA ,
						 COD_TRX  ,  NVL(FEC_VTACARD,'    ')       , NVL(COD_AUT,'      ')    , '0'
			FROM CRED_REV_ANUL_TRX
			WHERE ( (FEC_LOC >= :FECHA_MIN) AND (FEC_LOC <= :FECHA_MAX) ) AND
				  ( ( COD_EST IN ('01','08','10','11','12','19','21','25','43','57') ) )
			ORDER BY NRO_SUC, NRO_CAJ_SF, NRO_CAJ, NRO_TRX, COD_EST; */ 



	/* EXEC SQL
		OPEN CursorTrx; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 6;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlbuft((void **)0, 
   "select 'C' ,NRO_SUC ,NRO_CAJ ,NRO_TRX ,NRO_CA ,NRO_CAJ_SF ,COD_TAR ,NRO_\
TAR ,MONTO ,FEC_LOC ,MOD_ENV ,MOD_ING ,PLAN_SF ,NRO_COM ,COD_EST ,NRO_CAJ_CA\
 ,COD_TRX ,NVL(FEC_VTACARD,'    ') ,NVL(COD_AUT,'      ') ,'0'  from CRED_TR\
X where ((FEC_LOC>=:b0 and FEC_LOC<=:b1) and (COD_EST in ('01','08','10','11\
','12','19','21','25','43','57') or (COD_EST='04' and FEC_VTACARD is null ))\
) union select 'A' ,NRO_SUC ,NRO_CAJ ,NRO_TRX ,NRO_CA ,NRO_CAJ_SF ,COD_TAR ,\
NRO_TAR ,MONTO ,FEC_LOC ,MOD_ENV ,MOD_ING ,PLAN_SF ,NRO_COM ,COD_EST ,NRO_CA\
J_CA ,COD_TRX ,NVL(FEC_VTACARD,'    ') ,NVL(COD_AUT,'      ') ,'0'  from CRE\
D_ANUL_TRX where ((FEC_LOC>=:b0 and FEC_LOC<=:b1) and COD_EST in ('01','08',\
'10','11','12','19','21','25','43','57')) union select 'R' ,NRO_SUC ,NRO_CAJ\
 ,NRO_TRX ,NRO_CA ,NRO_CAJ_SF ,COD_TAR ,NRO_TAR ,MONTO ,FEC_LOC ,MOD_ENV ,MO\
D_ING ,PLAN_SF ,NRO_COM ,COD_EST ,NRO_CAJ_CA ,COD_TRX ,NVL(FEC_VTACARD,'    \
') ,NVL(COD_AUT,'      ') ,'0'  from CRED_REV_ANUL_TRX where ((FEC_LOC>=:b0 \
and FEC_LOC<=:b1) and COD_EST in ('01','");
 sqlstm.stmt = sq0003;
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )59;
 sqlstm.selerr = (unsigned short)1;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)256;
 sqlstm.occurs = (unsigned int  )0;
 sqlstm.sqcmod = (unsigned int )0;
 sqlstm.sqhstv[0] = (unsigned char  *)&FECHA_MIN;
 sqlstm.sqhstl[0] = (unsigned long )13;
 sqlstm.sqhsts[0] = (         int  )0;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqinds[0] = (         int  )0;
 sqlstm.sqharm[0] = (unsigned long )0;
 sqlstm.sqadto[0] = (unsigned short )0;
 sqlstm.sqtdso[0] = (unsigned short )0;
 sqlstm.sqhstv[1] = (unsigned char  *)&FECHA_MAX;
 sqlstm.sqhstl[1] = (unsigned long )13;
 sqlstm.sqhsts[1] = (         int  )0;
 sqlstm.sqindv[1] = (         short *)0;
 sqlstm.sqinds[1] = (         int  )0;
 sqlstm.sqharm[1] = (unsigned long )0;
 sqlstm.sqadto[1] = (unsigned short )0;
 sqlstm.sqtdso[1] = (unsigned short )0;
 sqlstm.sqhstv[2] = (unsigned char  *)&FECHA_MIN;
 sqlstm.sqhstl[2] = (unsigned long )13;
 sqlstm.sqhsts[2] = (         int  )0;
 sqlstm.sqindv[2] = (         short *)0;
 sqlstm.sqinds[2] = (         int  )0;
 sqlstm.sqharm[2] = (unsigned long )0;
 sqlstm.sqadto[2] = (unsigned short )0;
 sqlstm.sqtdso[2] = (unsigned short )0;
 sqlstm.sqhstv[3] = (unsigned char  *)&FECHA_MAX;
 sqlstm.sqhstl[3] = (unsigned long )13;
 sqlstm.sqhsts[3] = (         int  )0;
 sqlstm.sqindv[3] = (         short *)0;
 sqlstm.sqinds[3] = (         int  )0;
 sqlstm.sqharm[3] = (unsigned long )0;
 sqlstm.sqadto[3] = (unsigned short )0;
 sqlstm.sqtdso[3] = (unsigned short )0;
 sqlstm.sqhstv[4] = (unsigned char  *)&FECHA_MIN;
 sqlstm.sqhstl[4] = (unsigned long )13;
 sqlstm.sqhsts[4] = (         int  )0;
 sqlstm.sqindv[4] = (         short *)0;
 sqlstm.sqinds[4] = (         int  )0;
 sqlstm.sqharm[4] = (unsigned long )0;
 sqlstm.sqadto[4] = (unsigned short )0;
 sqlstm.sqtdso[4] = (unsigned short )0;
 sqlstm.sqhstv[5] = (unsigned char  *)&FECHA_MAX;
 sqlstm.sqhstl[5] = (unsigned long )13;
 sqlstm.sqhsts[5] = (         int  )0;
 sqlstm.sqindv[5] = (         short *)0;
 sqlstm.sqinds[5] = (         int  )0;
 sqlstm.sqharm[5] = (unsigned long )0;
 sqlstm.sqadto[5] = (unsigned short )0;
 sqlstm.sqtdso[5] = (unsigned short )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqphss = sqlstm.sqhsts;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqpins = sqlstm.sqinds;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlstm.sqpadto = sqlstm.sqadto;
 sqlstm.sqptdso = sqlstm.sqtdso;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



	if (SQLCODE!=0)
	{
		pvLog(fdLog, ARCHLOG, FIN, "Error al abrir cursor [%d]", SQLCODE);
	}

	do
	{
		/* EXEC SQL
			FETCH CursorTrx INTO
				:MOVIMIE ,
				:NRO_SUC , :NRO_CAJ , :NRO_TRX , :NRO_CA  , :NRO_CAJ_SF ,
				:COD_TAR , :NRO_TAR , :MONTO   , :FEC_LOC , :MOD_ENV    , 
				:MOD_ING , :PLAN_SF , :NRO_COM , :COD_EST , :NRO_CAJ_CA ,
				:COD_TRX , :FEC_VTACARD,         :COD_AUT , :ASO_REG; */ 

{
  struct sqlexd sqlstm;
  sqlstm.sqlvsn = 12;
  sqlstm.arrsiz = 20;
  sqlstm.sqladtp = &sqladt;
  sqlstm.sqltdsp = &sqltds;
  sqlstm.iters = (unsigned int  )1;
  sqlstm.offset = (unsigned int  )98;
  sqlstm.selerr = (unsigned short)1;
  sqlstm.cud = sqlcud0;
  sqlstm.sqlest = (unsigned char  *)&sqlca;
  sqlstm.sqlety = (unsigned short)256;
  sqlstm.occurs = (unsigned int  )0;
  sqlstm.sqfoff = (         int )0;
  sqlstm.sqfmod = (unsigned int )2;
  sqlstm.sqhstv[0] = (unsigned char  *)&MOVIMIE;
  sqlstm.sqhstl[0] = (unsigned long )4;
  sqlstm.sqhsts[0] = (         int  )0;
  sqlstm.sqindv[0] = (         short *)0;
  sqlstm.sqinds[0] = (         int  )0;
  sqlstm.sqharm[0] = (unsigned long )0;
  sqlstm.sqadto[0] = (unsigned short )0;
  sqlstm.sqtdso[0] = (unsigned short )0;
  sqlstm.sqhstv[1] = (unsigned char  *)&NRO_SUC;
  sqlstm.sqhstl[1] = (unsigned long )6;
  sqlstm.sqhsts[1] = (         int  )0;
  sqlstm.sqindv[1] = (         short *)0;
  sqlstm.sqinds[1] = (         int  )0;
  sqlstm.sqharm[1] = (unsigned long )0;
  sqlstm.sqadto[1] = (unsigned short )0;
  sqlstm.sqtdso[1] = (unsigned short )0;
  sqlstm.sqhstv[2] = (unsigned char  *)&NRO_CAJ;
  sqlstm.sqhstl[2] = (unsigned long )8;
  sqlstm.sqhsts[2] = (         int  )0;
  sqlstm.sqindv[2] = (         short *)0;
  sqlstm.sqinds[2] = (         int  )0;
  sqlstm.sqharm[2] = (unsigned long )0;
  sqlstm.sqadto[2] = (unsigned short )0;
  sqlstm.sqtdso[2] = (unsigned short )0;
  sqlstm.sqhstv[3] = (unsigned char  *)&NRO_TRX;
  sqlstm.sqhstl[3] = (unsigned long )7;
  sqlstm.sqhsts[3] = (         int  )0;
  sqlstm.sqindv[3] = (         short *)0;
  sqlstm.sqinds[3] = (         int  )0;
  sqlstm.sqharm[3] = (unsigned long )0;
  sqlstm.sqadto[3] = (unsigned short )0;
  sqlstm.sqtdso[3] = (unsigned short )0;
  sqlstm.sqhstv[4] = (unsigned char  *)&NRO_CA;
  sqlstm.sqhstl[4] = (unsigned long )5;
  sqlstm.sqhsts[4] = (         int  )0;
  sqlstm.sqindv[4] = (         short *)0;
  sqlstm.sqinds[4] = (         int  )0;
  sqlstm.sqharm[4] = (unsigned long )0;
  sqlstm.sqadto[4] = (unsigned short )0;
  sqlstm.sqtdso[4] = (unsigned short )0;
  sqlstm.sqhstv[5] = (unsigned char  *)&NRO_CAJ_SF;
  sqlstm.sqhstl[5] = (unsigned long )8;
  sqlstm.sqhsts[5] = (         int  )0;
  sqlstm.sqindv[5] = (         short *)0;
  sqlstm.sqinds[5] = (         int  )0;
  sqlstm.sqharm[5] = (unsigned long )0;
  sqlstm.sqadto[5] = (unsigned short )0;
  sqlstm.sqtdso[5] = (unsigned short )0;
  sqlstm.sqhstv[6] = (unsigned char  *)&COD_TAR;
  sqlstm.sqhstl[6] = (unsigned long )5;
  sqlstm.sqhsts[6] = (         int  )0;
  sqlstm.sqindv[6] = (         short *)0;
  sqlstm.sqinds[6] = (         int  )0;
  sqlstm.sqharm[6] = (unsigned long )0;
  sqlstm.sqadto[6] = (unsigned short )0;
  sqlstm.sqtdso[6] = (unsigned short )0;
  sqlstm.sqhstv[7] = (unsigned char  *)&NRO_TAR;
  sqlstm.sqhstl[7] = (unsigned long )23;
  sqlstm.sqhsts[7] = (         int  )0;
  sqlstm.sqindv[7] = (         short *)0;
  sqlstm.sqinds[7] = (         int  )0;
  sqlstm.sqharm[7] = (unsigned long )0;
  sqlstm.sqadto[7] = (unsigned short )0;
  sqlstm.sqtdso[7] = (unsigned short )0;
  sqlstm.sqhstv[8] = (unsigned char  *)&MONTO;
  sqlstm.sqhstl[8] = (unsigned long )sizeof(double);
  sqlstm.sqhsts[8] = (         int  )0;
  sqlstm.sqindv[8] = (         short *)0;
  sqlstm.sqinds[8] = (         int  )0;
  sqlstm.sqharm[8] = (unsigned long )0;
  sqlstm.sqadto[8] = (unsigned short )0;
  sqlstm.sqtdso[8] = (unsigned short )0;
  sqlstm.sqhstv[9] = (unsigned char  *)&FEC_LOC;
  sqlstm.sqhstl[9] = (unsigned long )13;
  sqlstm.sqhsts[9] = (         int  )0;
  sqlstm.sqindv[9] = (         short *)0;
  sqlstm.sqinds[9] = (         int  )0;
  sqlstm.sqharm[9] = (unsigned long )0;
  sqlstm.sqadto[9] = (unsigned short )0;
  sqlstm.sqtdso[9] = (unsigned short )0;
  sqlstm.sqhstv[10] = (unsigned char  *)&MOD_ENV;
  sqlstm.sqhstl[10] = (unsigned long )5;
  sqlstm.sqhsts[10] = (         int  )0;
  sqlstm.sqindv[10] = (         short *)0;
  sqlstm.sqinds[10] = (         int  )0;
  sqlstm.sqharm[10] = (unsigned long )0;
  sqlstm.sqadto[10] = (unsigned short )0;
  sqlstm.sqtdso[10] = (unsigned short )0;
  sqlstm.sqhstv[11] = (unsigned char  *)&MOD_ING;
  sqlstm.sqhstl[11] = (unsigned long )5;
  sqlstm.sqhsts[11] = (         int  )0;
  sqlstm.sqindv[11] = (         short *)0;
  sqlstm.sqinds[11] = (         int  )0;
  sqlstm.sqharm[11] = (unsigned long )0;
  sqlstm.sqadto[11] = (unsigned short )0;
  sqlstm.sqtdso[11] = (unsigned short )0;
  sqlstm.sqhstv[12] = (unsigned char  *)&PLAN_SF;
  sqlstm.sqhstl[12] = (unsigned long )5;
  sqlstm.sqhsts[12] = (         int  )0;
  sqlstm.sqindv[12] = (         short *)0;
  sqlstm.sqinds[12] = (         int  )0;
  sqlstm.sqharm[12] = (unsigned long )0;
  sqlstm.sqadto[12] = (unsigned short )0;
  sqlstm.sqtdso[12] = (unsigned short )0;
  sqlstm.sqhstv[13] = (unsigned char  *)&NRO_COM;
  sqlstm.sqhstl[13] = (unsigned long )18;
  sqlstm.sqhsts[13] = (         int  )0;
  sqlstm.sqindv[13] = (         short *)0;
  sqlstm.sqinds[13] = (         int  )0;
  sqlstm.sqharm[13] = (unsigned long )0;
  sqlstm.sqadto[13] = (unsigned short )0;
  sqlstm.sqtdso[13] = (unsigned short )0;
  sqlstm.sqhstv[14] = (unsigned char  *)&COD_EST;
  sqlstm.sqhstl[14] = (unsigned long )5;
  sqlstm.sqhsts[14] = (         int  )0;
  sqlstm.sqindv[14] = (         short *)0;
  sqlstm.sqinds[14] = (         int  )0;
  sqlstm.sqharm[14] = (unsigned long )0;
  sqlstm.sqadto[14] = (unsigned short )0;
  sqlstm.sqtdso[14] = (unsigned short )0;
  sqlstm.sqhstv[15] = (unsigned char  *)&NRO_CAJ_CA;
  sqlstm.sqhstl[15] = (unsigned long )11;
  sqlstm.sqhsts[15] = (         int  )0;
  sqlstm.sqindv[15] = (         short *)0;
  sqlstm.sqinds[15] = (         int  )0;
  sqlstm.sqharm[15] = (unsigned long )0;
  sqlstm.sqadto[15] = (unsigned short )0;
  sqlstm.sqtdso[15] = (unsigned short )0;
  sqlstm.sqhstv[16] = (unsigned char  *)&COD_TRX;
  sqlstm.sqhstl[16] = (unsigned long )5;
  sqlstm.sqhsts[16] = (         int  )0;
  sqlstm.sqindv[16] = (         short *)0;
  sqlstm.sqinds[16] = (         int  )0;
  sqlstm.sqharm[16] = (unsigned long )0;
  sqlstm.sqadto[16] = (unsigned short )0;
  sqlstm.sqtdso[16] = (unsigned short )0;
  sqlstm.sqhstv[17] = (unsigned char  *)&FEC_VTACARD;
  sqlstm.sqhstl[17] = (unsigned long )7;
  sqlstm.sqhsts[17] = (         int  )0;
  sqlstm.sqindv[17] = (         short *)0;
  sqlstm.sqinds[17] = (         int  )0;
  sqlstm.sqharm[17] = (unsigned long )0;
  sqlstm.sqadto[17] = (unsigned short )0;
  sqlstm.sqtdso[17] = (unsigned short )0;
  sqlstm.sqhstv[18] = (unsigned char  *)&COD_AUT;
  sqlstm.sqhstl[18] = (unsigned long )9;
  sqlstm.sqhsts[18] = (         int  )0;
  sqlstm.sqindv[18] = (         short *)0;
  sqlstm.sqinds[18] = (         int  )0;
  sqlstm.sqharm[18] = (unsigned long )0;
  sqlstm.sqadto[18] = (unsigned short )0;
  sqlstm.sqtdso[18] = (unsigned short )0;
  sqlstm.sqhstv[19] = (unsigned char  *)&ASO_REG;
  sqlstm.sqhstl[19] = (unsigned long )4;
  sqlstm.sqhsts[19] = (         int  )0;
  sqlstm.sqindv[19] = (         short *)0;
  sqlstm.sqinds[19] = (         int  )0;
  sqlstm.sqharm[19] = (unsigned long )0;
  sqlstm.sqadto[19] = (unsigned short )0;
  sqlstm.sqtdso[19] = (unsigned short )0;
  sqlstm.sqphsv = sqlstm.sqhstv;
  sqlstm.sqphsl = sqlstm.sqhstl;
  sqlstm.sqphss = sqlstm.sqhsts;
  sqlstm.sqpind = sqlstm.sqindv;
  sqlstm.sqpins = sqlstm.sqinds;
  sqlstm.sqparm = sqlstm.sqharm;
  sqlstm.sqparc = sqlstm.sqharc;
  sqlstm.sqpadto = sqlstm.sqadto;
  sqlstm.sqptdso = sqlstm.sqtdso;
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



		iError = SQLCODE;
		
		switch (iError)
		{
			case 0:
				pvArmarLinea(szLinea, sizeof(szLinea));
						
				pvLog(fdSal, ARCHIVO, SIG, szLinea);				

				/* Si el estado no es aprobada, busca si existe otro registro para esa transaccion */
				if (strcmp(CPVAR(COD_EST), "04") != 0)
				{
					/* Declara el cursor */
					/* EXEC SQL
						DECLARE CursorErrores CURSOR FOR
							SELECT 'C' , 
									NRO_SUC  ,  NRO_CAJ , NRO_TRX  , NRO_CA   , NRO_CAJ_SF , 
									COD_TAR  ,  NRO_TAR , MONTO    , FEC_LOC  , MOD_ENV    , 
									MOD_ING  ,  PLAN_SF , NRO_COM  , COD_EST  , NRO_CAJ_CA ,
									COD_TRX  ,  NVL(FEC_VTACARD,'    ')       , NVL(COD_AUT,'      ') , '1'
							FROM CRED_TRX
							WHERE ( (FEC_LOC>=:FECHA_MIN) AND (FEC_LOC<=:FECHA_MAX) ) AND
								  ( (COD_EST != :COD_EST) AND (NRO_SUC = :NRO_SUC) AND (NRO_CAJ_SF = :NRO_CAJ_SF) AND
									(NRO_CAJ=:NRO_CAJ) AND (NRO_TRX = :NRO_TRX) AND (NRO_TAR=:NRO_TAR) AND
									(MONTO=:MONTO) ) ; */ 


					/* Abre el cursor */
					/* EXEC SQL
						OPEN CursorErrores; */ 

{
     struct sqlexd sqlstm;
     sqlstm.sqlvsn = 12;
     sqlstm.arrsiz = 20;
     sqlstm.sqladtp = &sqladt;
     sqlstm.sqltdsp = &sqltds;
     sqlstm.stmt = sq0004;
     sqlstm.iters = (unsigned int  )1;
     sqlstm.offset = (unsigned int  )193;
     sqlstm.selerr = (unsigned short)1;
     sqlstm.cud = sqlcud0;
     sqlstm.sqlest = (unsigned char  *)&sqlca;
     sqlstm.sqlety = (unsigned short)256;
     sqlstm.occurs = (unsigned int  )0;
     sqlstm.sqcmod = (unsigned int )0;
     sqlstm.sqhstv[0] = (unsigned char  *)&FECHA_MIN;
     sqlstm.sqhstl[0] = (unsigned long )13;
     sqlstm.sqhsts[0] = (         int  )0;
     sqlstm.sqindv[0] = (         short *)0;
     sqlstm.sqinds[0] = (         int  )0;
     sqlstm.sqharm[0] = (unsigned long )0;
     sqlstm.sqadto[0] = (unsigned short )0;
     sqlstm.sqtdso[0] = (unsigned short )0;
     sqlstm.sqhstv[1] = (unsigned char  *)&FECHA_MAX;
     sqlstm.sqhstl[1] = (unsigned long )13;
     sqlstm.sqhsts[1] = (         int  )0;
     sqlstm.sqindv[1] = (         short *)0;
     sqlstm.sqinds[1] = (         int  )0;
     sqlstm.sqharm[1] = (unsigned long )0;
     sqlstm.sqadto[1] = (unsigned short )0;
     sqlstm.sqtdso[1] = (unsigned short )0;
     sqlstm.sqhstv[2] = (unsigned char  *)&COD_EST;
     sqlstm.sqhstl[2] = (unsigned long )5;
     sqlstm.sqhsts[2] = (         int  )0;
     sqlstm.sqindv[2] = (         short *)0;
     sqlstm.sqinds[2] = (         int  )0;
     sqlstm.sqharm[2] = (unsigned long )0;
     sqlstm.sqadto[2] = (unsigned short )0;
     sqlstm.sqtdso[2] = (unsigned short )0;
     sqlstm.sqhstv[3] = (unsigned char  *)&NRO_SUC;
     sqlstm.sqhstl[3] = (unsigned long )6;
     sqlstm.sqhsts[3] = (         int  )0;
     sqlstm.sqindv[3] = (         short *)0;
     sqlstm.sqinds[3] = (         int  )0;
     sqlstm.sqharm[3] = (unsigned long )0;
     sqlstm.sqadto[3] = (unsigned short )0;
     sqlstm.sqtdso[3] = (unsigned short )0;
     sqlstm.sqhstv[4] = (unsigned char  *)&NRO_CAJ_SF;
     sqlstm.sqhstl[4] = (unsigned long )8;
     sqlstm.sqhsts[4] = (         int  )0;
     sqlstm.sqindv[4] = (         short *)0;
     sqlstm.sqinds[4] = (         int  )0;
     sqlstm.sqharm[4] = (unsigned long )0;
     sqlstm.sqadto[4] = (unsigned short )0;
     sqlstm.sqtdso[4] = (unsigned short )0;
     sqlstm.sqhstv[5] = (unsigned char  *)&NRO_CAJ;
     sqlstm.sqhstl[5] = (unsigned long )8;
     sqlstm.sqhsts[5] = (         int  )0;
     sqlstm.sqindv[5] = (         short *)0;
     sqlstm.sqinds[5] = (         int  )0;
     sqlstm.sqharm[5] = (unsigned long )0;
     sqlstm.sqadto[5] = (unsigned short )0;
     sqlstm.sqtdso[5] = (unsigned short )0;
     sqlstm.sqhstv[6] = (unsigned char  *)&NRO_TRX;
     sqlstm.sqhstl[6] = (unsigned long )7;
     sqlstm.sqhsts[6] = (         int  )0;
     sqlstm.sqindv[6] = (         short *)0;
     sqlstm.sqinds[6] = (         int  )0;
     sqlstm.sqharm[6] = (unsigned long )0;
     sqlstm.sqadto[6] = (unsigned short )0;
     sqlstm.sqtdso[6] = (unsigned short )0;
     sqlstm.sqhstv[7] = (unsigned char  *)&NRO_TAR;
     sqlstm.sqhstl[7] = (unsigned long )23;
     sqlstm.sqhsts[7] = (         int  )0;
     sqlstm.sqindv[7] = (         short *)0;
     sqlstm.sqinds[7] = (         int  )0;
     sqlstm.sqharm[7] = (unsigned long )0;
     sqlstm.sqadto[7] = (unsigned short )0;
     sqlstm.sqtdso[7] = (unsigned short )0;
     sqlstm.sqhstv[8] = (unsigned char  *)&MONTO;
     sqlstm.sqhstl[8] = (unsigned long )sizeof(double);
     sqlstm.sqhsts[8] = (         int  )0;
     sqlstm.sqindv[8] = (         short *)0;
     sqlstm.sqinds[8] = (         int  )0;
     sqlstm.sqharm[8] = (unsigned long )0;
     sqlstm.sqadto[8] = (unsigned short )0;
     sqlstm.sqtdso[8] = (unsigned short )0;
     sqlstm.sqphsv = sqlstm.sqhstv;
     sqlstm.sqphsl = sqlstm.sqhstl;
     sqlstm.sqphss = sqlstm.sqhsts;
     sqlstm.sqpind = sqlstm.sqindv;
     sqlstm.sqpins = sqlstm.sqinds;
     sqlstm.sqparm = sqlstm.sqharm;
     sqlstm.sqparc = sqlstm.sqharc;
     sqlstm.sqpadto = sqlstm.sqadto;
     sqlstm.sqptdso = sqlstm.sqtdso;
     sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



					/* Procesa los registros */
					if (SQLCODE==0)
					{
						while (SQLCODE==0)
						{
							/* EXEC SQL
								FETCH CursorErrores INTO
										:MOVIMIE ,
										:NRO_SUC , :NRO_CAJ , :NRO_TRX , :NRO_CA  , :NRO_CAJ_SF ,
										:COD_TAR , :NRO_TAR , :MONTO   , :FEC_LOC , :MOD_ENV    , 
										:MOD_ING , :PLAN_SF , :NRO_COM , :COD_EST , :NRO_CAJ_CA ,
										:COD_TRX , :FEC_VTACARD        , :COD_AUT , :ASO_REG; */ 

{
       struct sqlexd sqlstm;
       sqlstm.sqlvsn = 12;
       sqlstm.arrsiz = 20;
       sqlstm.sqladtp = &sqladt;
       sqlstm.sqltdsp = &sqltds;
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )244;
       sqlstm.selerr = (unsigned short)1;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)256;
       sqlstm.occurs = (unsigned int  )0;
       sqlstm.sqfoff = (         int )0;
       sqlstm.sqfmod = (unsigned int )2;
       sqlstm.sqhstv[0] = (unsigned char  *)&MOVIMIE;
       sqlstm.sqhstl[0] = (unsigned long )4;
       sqlstm.sqhsts[0] = (         int  )0;
       sqlstm.sqindv[0] = (         short *)0;
       sqlstm.sqinds[0] = (         int  )0;
       sqlstm.sqharm[0] = (unsigned long )0;
       sqlstm.sqadto[0] = (unsigned short )0;
       sqlstm.sqtdso[0] = (unsigned short )0;
       sqlstm.sqhstv[1] = (unsigned char  *)&NRO_SUC;
       sqlstm.sqhstl[1] = (unsigned long )6;
       sqlstm.sqhsts[1] = (         int  )0;
       sqlstm.sqindv[1] = (         short *)0;
       sqlstm.sqinds[1] = (         int  )0;
       sqlstm.sqharm[1] = (unsigned long )0;
       sqlstm.sqadto[1] = (unsigned short )0;
       sqlstm.sqtdso[1] = (unsigned short )0;
       sqlstm.sqhstv[2] = (unsigned char  *)&NRO_CAJ;
       sqlstm.sqhstl[2] = (unsigned long )8;
       sqlstm.sqhsts[2] = (         int  )0;
       sqlstm.sqindv[2] = (         short *)0;
       sqlstm.sqinds[2] = (         int  )0;
       sqlstm.sqharm[2] = (unsigned long )0;
       sqlstm.sqadto[2] = (unsigned short )0;
       sqlstm.sqtdso[2] = (unsigned short )0;
       sqlstm.sqhstv[3] = (unsigned char  *)&NRO_TRX;
       sqlstm.sqhstl[3] = (unsigned long )7;
       sqlstm.sqhsts[3] = (         int  )0;
       sqlstm.sqindv[3] = (         short *)0;
       sqlstm.sqinds[3] = (         int  )0;
       sqlstm.sqharm[3] = (unsigned long )0;
       sqlstm.sqadto[3] = (unsigned short )0;
       sqlstm.sqtdso[3] = (unsigned short )0;
       sqlstm.sqhstv[4] = (unsigned char  *)&NRO_CA;
       sqlstm.sqhstl[4] = (unsigned long )5;
       sqlstm.sqhsts[4] = (         int  )0;
       sqlstm.sqindv[4] = (         short *)0;
       sqlstm.sqinds[4] = (         int  )0;
       sqlstm.sqharm[4] = (unsigned long )0;
       sqlstm.sqadto[4] = (unsigned short )0;
       sqlstm.sqtdso[4] = (unsigned short )0;
       sqlstm.sqhstv[5] = (unsigned char  *)&NRO_CAJ_SF;
       sqlstm.sqhstl[5] = (unsigned long )8;
       sqlstm.sqhsts[5] = (         int  )0;
       sqlstm.sqindv[5] = (         short *)0;
       sqlstm.sqinds[5] = (         int  )0;
       sqlstm.sqharm[5] = (unsigned long )0;
       sqlstm.sqadto[5] = (unsigned short )0;
       sqlstm.sqtdso[5] = (unsigned short )0;
       sqlstm.sqhstv[6] = (unsigned char  *)&COD_TAR;
       sqlstm.sqhstl[6] = (unsigned long )5;
       sqlstm.sqhsts[6] = (         int  )0;
       sqlstm.sqindv[6] = (         short *)0;
       sqlstm.sqinds[6] = (         int  )0;
       sqlstm.sqharm[6] = (unsigned long )0;
       sqlstm.sqadto[6] = (unsigned short )0;
       sqlstm.sqtdso[6] = (unsigned short )0;
       sqlstm.sqhstv[7] = (unsigned char  *)&NRO_TAR;
       sqlstm.sqhstl[7] = (unsigned long )23;
       sqlstm.sqhsts[7] = (         int  )0;
       sqlstm.sqindv[7] = (         short *)0;
       sqlstm.sqinds[7] = (         int  )0;
       sqlstm.sqharm[7] = (unsigned long )0;
       sqlstm.sqadto[7] = (unsigned short )0;
       sqlstm.sqtdso[7] = (unsigned short )0;
       sqlstm.sqhstv[8] = (unsigned char  *)&MONTO;
       sqlstm.sqhstl[8] = (unsigned long )sizeof(double);
       sqlstm.sqhsts[8] = (         int  )0;
       sqlstm.sqindv[8] = (         short *)0;
       sqlstm.sqinds[8] = (         int  )0;
       sqlstm.sqharm[8] = (unsigned long )0;
       sqlstm.sqadto[8] = (unsigned short )0;
       sqlstm.sqtdso[8] = (unsigned short )0;
       sqlstm.sqhstv[9] = (unsigned char  *)&FEC_LOC;
       sqlstm.sqhstl[9] = (unsigned long )13;
       sqlstm.sqhsts[9] = (         int  )0;
       sqlstm.sqindv[9] = (         short *)0;
       sqlstm.sqinds[9] = (         int  )0;
       sqlstm.sqharm[9] = (unsigned long )0;
       sqlstm.sqadto[9] = (unsigned short )0;
       sqlstm.sqtdso[9] = (unsigned short )0;
       sqlstm.sqhstv[10] = (unsigned char  *)&MOD_ENV;
       sqlstm.sqhstl[10] = (unsigned long )5;
       sqlstm.sqhsts[10] = (         int  )0;
       sqlstm.sqindv[10] = (         short *)0;
       sqlstm.sqinds[10] = (         int  )0;
       sqlstm.sqharm[10] = (unsigned long )0;
       sqlstm.sqadto[10] = (unsigned short )0;
       sqlstm.sqtdso[10] = (unsigned short )0;
       sqlstm.sqhstv[11] = (unsigned char  *)&MOD_ING;
       sqlstm.sqhstl[11] = (unsigned long )5;
       sqlstm.sqhsts[11] = (         int  )0;
       sqlstm.sqindv[11] = (         short *)0;
       sqlstm.sqinds[11] = (         int  )0;
       sqlstm.sqharm[11] = (unsigned long )0;
       sqlstm.sqadto[11] = (unsigned short )0;
       sqlstm.sqtdso[11] = (unsigned short )0;
       sqlstm.sqhstv[12] = (unsigned char  *)&PLAN_SF;
       sqlstm.sqhstl[12] = (unsigned long )5;
       sqlstm.sqhsts[12] = (         int  )0;
       sqlstm.sqindv[12] = (         short *)0;
       sqlstm.sqinds[12] = (         int  )0;
       sqlstm.sqharm[12] = (unsigned long )0;
       sqlstm.sqadto[12] = (unsigned short )0;
       sqlstm.sqtdso[12] = (unsigned short )0;
       sqlstm.sqhstv[13] = (unsigned char  *)&NRO_COM;
       sqlstm.sqhstl[13] = (unsigned long )18;
       sqlstm.sqhsts[13] = (         int  )0;
       sqlstm.sqindv[13] = (         short *)0;
       sqlstm.sqinds[13] = (         int  )0;
       sqlstm.sqharm[13] = (unsigned long )0;
       sqlstm.sqadto[13] = (unsigned short )0;
       sqlstm.sqtdso[13] = (unsigned short )0;
       sqlstm.sqhstv[14] = (unsigned char  *)&COD_EST;
       sqlstm.sqhstl[14] = (unsigned long )5;
       sqlstm.sqhsts[14] = (         int  )0;
       sqlstm.sqindv[14] = (         short *)0;
       sqlstm.sqinds[14] = (         int  )0;
       sqlstm.sqharm[14] = (unsigned long )0;
       sqlstm.sqadto[14] = (unsigned short )0;
       sqlstm.sqtdso[14] = (unsigned short )0;
       sqlstm.sqhstv[15] = (unsigned char  *)&NRO_CAJ_CA;
       sqlstm.sqhstl[15] = (unsigned long )11;
       sqlstm.sqhsts[15] = (         int  )0;
       sqlstm.sqindv[15] = (         short *)0;
       sqlstm.sqinds[15] = (         int  )0;
       sqlstm.sqharm[15] = (unsigned long )0;
       sqlstm.sqadto[15] = (unsigned short )0;
       sqlstm.sqtdso[15] = (unsigned short )0;
       sqlstm.sqhstv[16] = (unsigned char  *)&COD_TRX;
       sqlstm.sqhstl[16] = (unsigned long )5;
       sqlstm.sqhsts[16] = (         int  )0;
       sqlstm.sqindv[16] = (         short *)0;
       sqlstm.sqinds[16] = (         int  )0;
       sqlstm.sqharm[16] = (unsigned long )0;
       sqlstm.sqadto[16] = (unsigned short )0;
       sqlstm.sqtdso[16] = (unsigned short )0;
       sqlstm.sqhstv[17] = (unsigned char  *)&FEC_VTACARD;
       sqlstm.sqhstl[17] = (unsigned long )7;
       sqlstm.sqhsts[17] = (         int  )0;
       sqlstm.sqindv[17] = (         short *)0;
       sqlstm.sqinds[17] = (         int  )0;
       sqlstm.sqharm[17] = (unsigned long )0;
       sqlstm.sqadto[17] = (unsigned short )0;
       sqlstm.sqtdso[17] = (unsigned short )0;
       sqlstm.sqhstv[18] = (unsigned char  *)&COD_AUT;
       sqlstm.sqhstl[18] = (unsigned long )9;
       sqlstm.sqhsts[18] = (         int  )0;
       sqlstm.sqindv[18] = (         short *)0;
       sqlstm.sqinds[18] = (         int  )0;
       sqlstm.sqharm[18] = (unsigned long )0;
       sqlstm.sqadto[18] = (unsigned short )0;
       sqlstm.sqtdso[18] = (unsigned short )0;
       sqlstm.sqhstv[19] = (unsigned char  *)&ASO_REG;
       sqlstm.sqhstl[19] = (unsigned long )4;
       sqlstm.sqhsts[19] = (         int  )0;
       sqlstm.sqindv[19] = (         short *)0;
       sqlstm.sqinds[19] = (         int  )0;
       sqlstm.sqharm[19] = (unsigned long )0;
       sqlstm.sqadto[19] = (unsigned short )0;
       sqlstm.sqtdso[19] = (unsigned short )0;
       sqlstm.sqphsv = sqlstm.sqhstv;
       sqlstm.sqphsl = sqlstm.sqhstl;
       sqlstm.sqphss = sqlstm.sqhsts;
       sqlstm.sqpind = sqlstm.sqindv;
       sqlstm.sqpins = sqlstm.sqinds;
       sqlstm.sqparm = sqlstm.sqharm;
       sqlstm.sqparc = sqlstm.sqharc;
       sqlstm.sqpadto = sqlstm.sqadto;
       sqlstm.sqptdso = sqlstm.sqtdso;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



							if (SQLCODE==0)
							{
								pvArmarLinea(szLinea, sizeof(szLinea));

								pvLog(fdSal, ARCHIVO, SIG, szLinea);					
							}
						}
					}

					/* Cierra el cursor */
					/* EXEC SQL
						CLOSE CursorErrores; */ 

{
     struct sqlexd sqlstm;
     sqlstm.sqlvsn = 12;
     sqlstm.arrsiz = 20;
     sqlstm.sqladtp = &sqladt;
     sqlstm.sqltdsp = &sqltds;
     sqlstm.iters = (unsigned int  )1;
     sqlstm.offset = (unsigned int  )339;
     sqlstm.cud = sqlcud0;
     sqlstm.sqlest = (unsigned char  *)&sqlca;
     sqlstm.sqlety = (unsigned short)256;
     sqlstm.occurs = (unsigned int  )0;
     sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


				}
				break;
			case 1403:
				pvLog(fdLog, ARCHLOG, SIG, "Fin de recuperacion de registros");
				break;
			default:
				pvLog(fdLog, ARCHLOG, SIG, "Error al recuperar registros [%d]", SQLCODE);
		}
	} while (iError==0);

	/* Cierra el cursor */
	/* EXEC SQL
		CLOSE CursorTrx; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 20;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )354;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)256;
 sqlstm.occurs = (unsigned int  )0;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


								
	/* Retorna correctamente */
	pvLog(fdLog, ARCHLOG, FOK, "Fin de la aplicacion");

} 


