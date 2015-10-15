
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned long magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[56];
};
static struct sqlcxp sqlfpn =
{
    55,
    "/home/codigos/coto/v220/ts001070/kap/source/ts001070.pc"
};


static unsigned long sqlctx = 256868661;


static struct sqlexd {
   unsigned int   sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   unused;
            short *cud;
   unsigned char  *sqlest;
            char  *stmt;
   unsigned char  **sqphsv;
   unsigned int   *sqphsl;
            short **sqpind;
   unsigned int   *sqparm;
   unsigned int   **sqparc;
   unsigned char  *sqhstv[6];
   unsigned int   sqhstl[6];
            short *sqindv[6];
   unsigned int   sqharm[6];
   unsigned int   *sqharc[6];
} sqlstm = {8,6};

/* Prototypes */
extern sqlcxt (/*_ void **, unsigned long *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlcx2t(/*_ void **, unsigned long *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlbuft(/*_ void **, char * _*/);
extern sqlgs2t(/*_ void **, char * _*/);
extern sqlorat(/*_ void **, unsigned long *, void * _*/);

/* Forms Interface */
static int IAPSUCC = 0;
static int IAPFAIL = 1403;
static int IAPFTL  = 535;
extern void sqliem(/*_ char *, int * _*/);

 static char *sq0004 = 
"select T6041500.CEMPRESA ,T6041500.CTERMINL ,T6041500.NCTRTERM ,SUBSTR(TO_CH\
AR(T6040900.FCTRENTR,'DD/MM/YYYY'),1,10) ,((SUBSTR(T6040900.HCTRENTR,1,2)||':'\
)||SUBSTR(HCTRENTR,3,2)) ,DECODE(T6041500.CTIPPAGO,'01',T6041500.ITARJDOC,(-T6\
041500.ITARJDOC))  from T6041500 ,T6040900 where (((T6041500.CCOPATER=:b0 and \
T6040900.CINCIDEN='000') and T6040900.CTERMINL=T6041500.CTERMINL) and T6040900\
.NCTRTERM=T6041500.NCTRTERM) order by 1,2,3,4,5            ";
typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static short sqlcud0[] =
{8,4130,
2,0,0,1,40,0,516,81,0,1,0,0,1,0,2,9,0,0,
20,0,0,2,127,0,516,91,0,3,0,0,1,0,2,9,0,0,2,9,0,0,2,9,0,0,
46,0,0,3,58,0,516,106,0,2,1,0,1,0,2,9,0,0,1,9,0,0,
68,0,0,4,447,0,521,158,0,1,1,0,1,0,1,9,0,0,
86,0,0,4,0,0,525,176,0,6,0,0,1,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,4,0,
0,
};


/*
+------------------------------------------------------------------------------+
|                   Copyright (C) 2003 - COTO CICSA                            |
+-----------------+------------------------------------------------------------+
| F. Fuente       | ts001070.pc                                                |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
| Descripcion     | Reporte detallado por transaccion de movimientos PAGOFACIL |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#define NOMPROC		"ts001070"
#define NOMLIST		"LS001070"

#define TERMIN		1
#define EMPRES		2
#define SUCURS		3

#define MAX_LINEAS	68

/* EXEC SQL INCLUDE SQLCA;
 */ 
/*
 * $Header: sqlca.h,v 1.3 1994/12/12 19:27:27 jbasu Exp $ sqlca.h 
 */

/* Copyright (c) 1985,1986 by Oracle Corporation. */
 
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
         /* b4  */ long    sqlabc;
         /* b4  */ long    sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ long    sqlerrd[6];
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


/* EXEC SQL BEGIN DECLARE SECTION; */ 

	/* VARCHAR cterminl[4]; */ 
struct { unsigned short len; unsigned char arr[4]; } cterminl;

	/* VARCHAR cempresa[2]; */ 
struct { unsigned short len; unsigned char arr[2]; } cempresa;
  
	/* VARCHAR hctrentr[5]; */ 
struct { unsigned short len; unsigned char arr[5]; } hctrentr;

	/* VARCHAR fctrentr[10]; */ 
struct { unsigned short len; unsigned char arr[10]; } fctrentr;

	/* VARCHAR nctrterm[4]; */ 
struct { unsigned short len; unsigned char arr[4]; } nctrterm;

	float	itarjdoc;
/* EXEC SQL END DECLARE SECTION; */ 


/* Definicion de tipos */
typedef struct
{
	char dcentros[31];
	char frctproc[ 9];
	char sysdate [ 9];
	char systime [ 6];
	char dcopater[21];
} TipoDATOSCABECERA;

/* Variables globales */
TipoDATOSCABECERA pstDatosCab;
int iLineas=0;
FILE *fdListado=NULL;

/* Prototipos de funciones */
int		piDBLeerFetch(void);
int		piDBPrepararRegistros(char *);
int		piDBLeerDatosCabecera(char *);
void	pvSalir(int);
void	pvImprimirRegistro();
void	pvImprimirTotal(int, int, float, char*, char*);
void	pvImprimirCabecera();
void	pvPuntosNumero (double, char *, int);

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | piDBLeerDatosCabecera                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Recupera los datos de la tabla de control de StoreFlow     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int piDBLeerDatosCabecera(char *szCobroPago) 
{
	/* EXEC SQL BEGIN DECLARE SECTION; */ 

		/* VARCHAR dcentros[22]; */ 
struct { unsigned short len; unsigned char arr[22]; } dcentros;

		/* VARCHAR frctproc[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } frctproc;

		/* VARCHAR sysdate[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } sysdate;

		/* VARCHAR systime[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } systime;

		/* VARCHAR dcopater[21]; */ 
struct { unsigned short len; unsigned char arr[21]; } dcopater;

		/* VARCHAR ccopater[2]; */ 
struct { unsigned short len; unsigned char arr[2]; } ccopater;

	/* EXEC SQL END DECLARE SECTION; */ 


	/* EXEC SQL 
		SELECT dcentros
		INTO :dcentros
		FROM t6022600; */ 

{
 struct sqlexd sqlstm;

 sqlstm.sqlvsn = 8;
 sqlstm.arrsiz = 1;
 sqlstm.stmt = "select dcentros into :b0  from t6022600 ";
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )2;
 sqlstm.selerr = (unsigned short)0;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)0;
 sqlstm.sqhstv[0] = (unsigned char  *)&dcentros;
 sqlstm.sqhstl[0] = (unsigned int  )24;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqharm[0] = (unsigned int  )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



	if (sqlca.sqlcode  != 0)
	{
		return (-1);
	}

	/* EXEC SQL 
		SELECT	TO_CHAR(frctproc, 'DD/MM/YY' ),
				TO_CHAR(sysdate , 'DD/MM/YY' ),
				TO_CHAR(sysdate , 'HH24:MI'  )
		INTO :frctproc, :sysdate, :systime
		FROM T6040800; */ 

{
 struct sqlexd sqlstm;

 sqlstm.sqlvsn = 8;
 sqlstm.arrsiz = 3;
 sqlstm.stmt = "select TO_CHAR(frctproc,'DD/MM/YY') ,TO_CHAR(sysdate ,'DD/MM\
/YY') ,TO_CHAR(sysdate ,'HH24:MI') into :b0,:b1,:b2  from T6040800 ";
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )20;
 sqlstm.selerr = (unsigned short)0;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)0;
 sqlstm.sqhstv[0] = (unsigned char  *)&frctproc;
 sqlstm.sqhstl[0] = (unsigned int  )22;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqharm[0] = (unsigned int  )0;
 sqlstm.sqhstv[1] = (unsigned char  *)&sysdate;
 sqlstm.sqhstl[1] = (unsigned int  )22;
 sqlstm.sqindv[1] = (         short *)0;
 sqlstm.sqharm[1] = (unsigned int  )0;
 sqlstm.sqhstv[2] = (unsigned char  *)&systime;
 sqlstm.sqhstl[2] = (unsigned int  )22;
 sqlstm.sqindv[2] = (         short *)0;
 sqlstm.sqharm[2] = (unsigned int  )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



	if (sqlca.sqlcode != 0)
	{
		return (-1);	
	}

	memcpy( ccopater.arr, szCobroPago, 2);
	ccopater.len = 2;

	/* EXEC SQL 
		SELECT DCOPATER
		INTO :dcopater
		FROM T6196000
		WHERE CCOPATER = :ccopater; */ 

{
 struct sqlexd sqlstm;

 sqlstm.sqlvsn = 8;
 sqlstm.arrsiz = 3;
 sqlstm.stmt = "select DCOPATER into :b0  from T6196000 where CCOPATER=:b1";
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )46;
 sqlstm.selerr = (unsigned short)0;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)0;
 sqlstm.sqhstv[0] = (unsigned char  *)&dcopater;
 sqlstm.sqhstl[0] = (unsigned int  )23;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqharm[0] = (unsigned int  )0;
 sqlstm.sqhstv[1] = (unsigned char  *)&ccopater;
 sqlstm.sqhstl[1] = (unsigned int  )4;
 sqlstm.sqindv[1] = (         short *)0;
 sqlstm.sqharm[1] = (unsigned int  )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



	if (sqlca.sqlcode != 0)
	{
		return (-1);	
	}

	memcpy( pstDatosCab.dcentros, dcentros.arr, dcentros.len );
	memcpy( pstDatosCab.frctproc, frctproc.arr, 8 );
	memcpy( pstDatosCab.sysdate,  sysdate.arr , 8 );
	memcpy( pstDatosCab.systime,  systime.arr , 5 );

	memset( pstDatosCab.dcopater, ' ', sizeof(dcopater.arr));
	memcpy( pstDatosCab.dcopater + (20-dcopater.len)/2 , dcopater.arr, dcopater.len);
	return (0);
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | piDBPrepararRegistros                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Declara y abre el cursor de registros conteniendo la       |
|                 | informacion a mostrar en el listado                        | 
+-----------------+------------------------------------------------------------+
*/
int piDBPrepararRegistros(char *szCobroPago)
{
	/* EXEC SQL BEGIN DECLARE SECTION; */ 

		/* VARCHAR ccopater[2]; */ 
struct { unsigned short len; unsigned char arr[2]; } ccopater;

	/* EXEC SQL END DECLARE SECTION; */ 


	memcpy( ccopater.arr, szCobroPago, 2);
	ccopater.len = 2;

	/* EXEC SQL 
		DECLARE CursorPagoFacil CURSOR FOR
		SELECT  T6041500.CEMPRESA, 
				T6041500.CTERMINL, 
				T6041500.NCTRTERM, 
				SUBSTR(TO_CHAR(T6040900.FCTRENTR, 'DD/MM/YYYY'),1,10),
				SUBSTR(T6040900.HCTRENTR, 1, 2)||':'||SUBSTR(HCTRENTR, 3,2),
				DECODE(T6041500.CTIPPAGO, '01', T6041500.ITARJDOC, -T6041500.ITARJDOC)
		FROM    T6041500, T6040900
		WHERE 	T6041500.CCOPATER = :ccopater AND
				T6040900.CINCIDEN = '000' AND
				T6040900.CTERMINL = T6041500.CTERMINL AND
				T6040900.NCTRTERM = T6041500.NCTRTERM
		ORDER BY 1,2,3,4,5 ; */ 

				
	/* EXEC SQL OPEN CursorPagoFacil; */ 

{
 struct sqlexd sqlstm;

 sqlstm.sqlvsn = 8;
 sqlstm.arrsiz = 3;
 sqlstm.stmt = sq0004;
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )68;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)0;
 sqlstm.sqhstv[0] = (unsigned char  *)&ccopater;
 sqlstm.sqhstl[0] = (unsigned int  )4;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqharm[0] = (unsigned int  )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	
	if (sqlca.sqlcode != 0 )
	{
		return (-1);
	}
	return (0);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | piDBLeerFetch                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Lee un registro de movimiento                              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int piDBLeerFetch (void )
{
	/* EXEC SQL
		FETCH CursorPagoFacil     
			INTO	:cempresa, :cterminl, :nctrterm, 
					:fctrentr, :hctrentr, :itarjdoc; */ 

{
 struct sqlexd sqlstm;

 sqlstm.sqlvsn = 8;
 sqlstm.arrsiz = 6;
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )86;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)0;
 sqlstm.sqhstv[0] = (unsigned char  *)&cempresa;
 sqlstm.sqhstl[0] = (unsigned int  )4;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqharm[0] = (unsigned int  )0;
 sqlstm.sqhstv[1] = (unsigned char  *)&cterminl;
 sqlstm.sqhstl[1] = (unsigned int  )6;
 sqlstm.sqindv[1] = (         short *)0;
 sqlstm.sqharm[1] = (unsigned int  )0;
 sqlstm.sqhstv[2] = (unsigned char  *)&nctrterm;
 sqlstm.sqhstl[2] = (unsigned int  )6;
 sqlstm.sqindv[2] = (         short *)0;
 sqlstm.sqharm[2] = (unsigned int  )0;
 sqlstm.sqhstv[3] = (unsigned char  *)&fctrentr;
 sqlstm.sqhstl[3] = (unsigned int  )12;
 sqlstm.sqindv[3] = (         short *)0;
 sqlstm.sqharm[3] = (unsigned int  )0;
 sqlstm.sqhstv[4] = (unsigned char  *)&hctrentr;
 sqlstm.sqhstl[4] = (unsigned int  )7;
 sqlstm.sqindv[4] = (         short *)0;
 sqlstm.sqharm[4] = (unsigned int  )0;
 sqlstm.sqhstv[5] = (unsigned char  *)&itarjdoc;
 sqlstm.sqhstl[5] = (unsigned int  )4;
 sqlstm.sqindv[5] = (         short *)0;
 sqlstm.sqharm[5] = (unsigned int  )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



	if (sqlca.sqlcode !=0 && sqlca.sqlcode != 1403) 
	{
		return (-1);
	}

	if (sqlca.sqlcode == 1403)
	{
		return (1);
	}
	else
	{
	    return (0);
	}
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvImprimirCabecera                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Imprime la cabecera del listado en el archivo              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void pvImprimirCabecera()
{
	char *pcPagina = "000";
	static int iPagina = 0 ;
	int iAux;
	static int iPrimeraVez = 1;

	char ppcCabecera[6][80]={
	{"                                                              FECHA  :         "}, 
	{"FECHA DE PROCESO....:                  MOVIMIENTOS            HORA   :         "}, 
	{"CODIGO INFORME......: XXXXXXXX    XXXXXXXXXXXXXXXXXXXX        PAGINA :         "}, 
	{"                                                                               "}, 
	{"EMPRESA     TERMINAL     TRANSACCION       FECHA        HORA        IMPORTE    "}, 
	{"=======     ========     ===========     ==========     =====    =============="}};

	iPagina++;
	sprintf( pcPagina, "%d", iPagina );
	iLineas=6;
	memcpy( ppcCabecera[0], pstDatosCab.dcentros, strlen(pstDatosCab.dcentros));
	memcpy( ppcCabecera[0] + 71 , pstDatosCab.sysdate , 8);

	memcpy( ppcCabecera[1] + 22 , pstDatosCab.frctproc, 8); 
	memcpy( ppcCabecera[1] + 71 , pstDatosCab.systime , 5);
	memcpy( ppcCabecera[2] + 22 , NOMLIST, 8);
	memcpy( ppcCabecera[2] + 34 , pstDatosCab.dcopater, 20);
	memcpy( ppcCabecera[2] + 71 , pcPagina,              3);
	
	if (iPrimeraVez==1)
	{
		iPrimeraVez = 0;
	}
	else
	{
		fprintf( fdListado, "\n\14" ); /* salto de pagina */
	}

	for (iAux = 0 ; iAux < 6  ;iAux++ )
	{
		fprintf(fdListado, "\n%s",ppcCabecera[ iAux ] );
	} 
	fflush(fdListado);
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvSalir                                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Finaliza la aplicacion escribiendo el archivo de log       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void pvSalir(int iError)
{
	FILE *fError;
	time_t	lTime;
	char szArchivo[64];
	char szMensaje[7][64] =	{
								{"Finalizacion correcta"},
								{"Error en parametros de llamada"},
								{"Error de conexion a Oracle"},
								{"Error en apertura de archivo"},
								{"Error al leer datos de cabecera"},
								{"Error al abrir cursor"},
								{"Error al leer cursor"},
							};

	sprintf(szArchivo, "%s/tmp/%s.log", getenv("NAPA_HOME"), NOMPROC);

	if ((fError = fopen(szArchivo, "wt"))!=NULL)
	{
		time(&lTime);
		fprintf(fError, "%s\n", ctime(&lTime));
		fprintf(fError, "%s\n", szMensaje[iError]);
		fflush(fError);
		fclose(fError);
	}
	if (fdListado!=NULL) fclose(fdListado);
	exit(iError);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvImprimirRegistro                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Imprime un registro de movimiento en el archivo            |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void pvImprimirRegistro()
{
	char szAux1[64];
	char szAux2[64];
	char szLinea[133];

	/* Salta de pagina si corresponde */
	if ((iLineas > MAX_LINEAS) || (iLineas==0))
	{
		pvImprimirCabecera();
	}
	
	/* Imprime el movimiento */
	memset (szLinea, ' ', sizeof(szLinea));
	szLinea[sizeof(szLinea)-1] = 0x00;
	memcpy(szLinea + 00, cempresa.arr, cempresa.len);
	memcpy(szLinea + 12, cterminl.arr, cterminl.len);
	memcpy(szLinea + 25, nctrterm.arr, nctrterm.len);
	memcpy(szLinea + 41, fctrentr.arr, fctrentr.len);
	memcpy(szLinea + 56, hctrentr.arr, hctrentr.len);
	pvPuntosNumero(itarjdoc, szAux1, 2);
	sprintf(szAux2, "%14.14s", szAux1);
	memcpy(szLinea + 65, szAux2, 14);
	fprintf(fdListado, "\n%s", szLinea);
	fflush(fdListado);
	iLineas ++;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvImprimirTotal                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Imprime un registro de total (por terminal, empresa o      |
|                 | total) en el archivo                                       | 
+-----------------+------------------------------------------------------------+
*/
void pvImprimirTotal(int iTipoTotal, int iCantidad, float fImporte,
					 char *szEmpresa, char *szTerminal)
{
	char szAux1[64];
	char szAux2[64];
	char szLinea[133];

	if (iLineas > MAX_LINEAS)
	{
		pvImprimirCabecera();
	}

	/* Imprime raya de totales */
	memset(szLinea, ' ', sizeof(szLinea));
	szLinea[sizeof(szLinea)-1] = 0x00;
	memcpy(szLinea+25, "----", 4);
	memcpy(szLinea+66, "--------------", 14);
	fprintf(fdListado, "\n%s", szLinea); 
	iLineas++;

	/* Imprime totales por terminal */
	memset (szLinea, ' ', sizeof(szLinea));
	szLinea[sizeof(szLinea)-1] = 0x00;
	switch (iTipoTotal)
	{
		case TERMIN:
			memcpy(szLinea, "TOTAL TERMINAL XXXX:", 20);
			memcpy(szLinea + 15, szTerminal, 4);
			break;
		case EMPRES:
			memcpy(szLinea, "TOTAL EMPRESA XX:   ", 20);
			memcpy(szLinea + 14, szEmpresa, 2);
			break;
		case SUCURS:
			memcpy(szLinea, "TOTAL SUCURSAL:     ", 20);
			break;
	}
	sprintf(szAux1, "%4d", iCantidad);
	memcpy(szLinea + 25, szAux1, 4);
	pvPuntosNumero(fImporte, szAux1, 2);
	sprintf(szAux2, "%14.14s", szAux1);
	memcpy(szLinea + 65, szAux2, 14);
	fprintf(fdListado, "\n%s\n", szLinea);
	iLineas++;
	iLineas++;
	if (iTipoTotal==EMPRES)
	{	
		iLineas= 0;
	}
	fflush(fdListado);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso principal de la aplicacion                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void main (int argc, char **argv)
{
	/* Definicion de variables */
	char szAux[64];
	char szAux2[64];
	char szUltEmpr[3];
	char szUltTerm[5];
	char szNomProceso[80];
	int iFinalizar=0;
	char szCobroPago[3];
	char szArchivo[64];

	/* Inicia variables contadoras */
	int iCantTerminal = 0;
	float fImpoTerminal = 0;
	int iCantEmpresa = 0;
	float fImpoEmpresa = 0;
	int iCantTotal = 0;
	float fImpoTotal = 0;

	/* Verifica que exista el parametro */
	if ( (argc!=2) || (strlen(argv[1])!=2) )
	{
		pvSalir (1);
	}
	else
	{
		sprintf(szCobroPago, argv[1]);
	}
	
	/* Conecta a base de datos */
	strcpy(szNomProceso,argv[0]);
	QuitaPath (szNomProceso);
	if (HacerCONNECT (szNomProceso) != 0)
	{
		pvSalir (2);
	}
		
	/* Crea el archivo de listado */
	sprintf(szArchivo, "%s_%s", NOMLIST, szCobroPago);
	if ((fdListado = fopen(szArchivo, "w+") )==NULL)
	{
		pvSalir (3);
	}

	/* Lee datos para informar en la cabecera */
	if (piDBLeerDatosCabecera(szCobroPago) != 0)
	{
		pvSalir (4);
	}
	
	/* Abre cursor */
	if (piDBPrepararRegistros(szCobroPago) != 0)
	{
		pvSalir (5);
	}

	/* Imprime la cabecera */
	pvImprimirCabecera();
	
	/* Lee primer registro */
	iFinalizar = piDBLeerFetch();
	if ( iFinalizar == 1)
	{ 
		/* No hay datos */
		fprintf(fdListado, "\n                    *** NO HAY DATOS PARA ESTE DOCUMENTO ***");
		pvSalir(0);
	}
	else
	{
		/* Error en lectura del cursor */
		if (iFinalizar == -1)
		{
			pvSalir(6);
		}
	}

	/* Inicializa empresa y terminal */
	memset(szUltEmpr, 0, sizeof(szUltEmpr));
	memcpy(szUltEmpr, cempresa.arr, cempresa.len);
	memset(szUltTerm, 0, sizeof(szUltTerm));
	memcpy(szUltTerm, cterminl.arr, cterminl.len);
	
	do
	{
		/* Verifica si se cambia de empresa, terminal */
		if ( (memcmp(szUltEmpr, cempresa.arr, cempresa.len)!=0) ||
			 (memcmp(szUltTerm, cterminl.arr, cterminl.len)!=0) )
		{	
			/* Imprime totales por terminal */
			pvImprimirTotal(TERMIN, iCantTerminal, fImpoTerminal, szUltEmpr, szUltTerm);

			/* Si cambia la empresa */
			if (memcmp(szUltEmpr, cempresa.arr, cempresa.len)!=0)
			{
				/* Imprime totales por empresa */
				pvImprimirTotal(EMPRES, iCantEmpresa, fImpoEmpresa, szUltEmpr, szUltTerm);

				/*Inicia las variables para nueva empresa */
				iCantEmpresa = 0;
				fImpoEmpresa = 0;
			}
			
			/* Inicia variables para nueva terminal */
			iCantTerminal = 0;
			fImpoTerminal = 0;
			
			/* Carga la nueva empresa y terminal */
			memset(szUltEmpr, 0, sizeof(szUltEmpr));
			memcpy(szUltEmpr, cempresa.arr, cempresa.len);
			memset(szUltTerm, 0, sizeof(szUltTerm));
			memcpy(szUltTerm, cterminl.arr, cterminl.len);
		}	

		/* Actualiza los contadores de la terminal */
		iCantTerminal++;
		fImpoTerminal += itarjdoc;

		/* Actualiza los contadores de la empresa */
		iCantEmpresa++;
		fImpoEmpresa += itarjdoc;

		/* Actualiza los contadores globales */
		iCantTotal++;
		fImpoTotal += itarjdoc;

		/* Imprime registro */		
		pvImprimirRegistro();

	} while((iFinalizar = piDBLeerFetch())==0);

	/* Verifica si ultimo fetch fue error de lectura */
	if (iFinalizar == -1)
	{
		pvSalir(6);
	}

	/* Imprime totales por terminal */
	pvImprimirTotal(TERMIN, iCantTerminal, fImpoTerminal, szUltEmpr, szUltTerm);

	/* Imprime totales por empresa */
	pvImprimirTotal(EMPRES, iCantEmpresa, fImpoEmpresa, szUltEmpr, szUltTerm);

	/* Imprime totales por empresa */
	pvImprimirTotal(SUCURS, iCantTotal, fImpoTotal, szUltEmpr, szUltTerm);
							
	/* Retorna correctamente */
	pvSalir(0);
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | pvPuntosNumero                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Convierte el numero a cadena para imprimirlo con el        |
|                 | formato que corresponde                                    | 
+-----------------+------------------------------------------------------------+
*/
void pvPuntosNumero (double numero, char *cadena, int dec)
{
	char buff[30], aux[30], *b, *a;
	int c, ncarac, decimal=0;
	int negativo=0;

	int Coma;
	char szAux[2];

	memset(szAux,'\0',2);
	BuscaParametro("etc/procesos.cfg","FORMATO_DECIMAL:",szAux);
	if (!strcmp(szAux,"c") || !strcmp(szAux,"C")) 
	{
		Coma = 1;
	}
	else 
	{
		Coma = 0;
	}

	memset (aux,0,30);
	memset (buff,0,30);
	b = buff + 28;
	if (numero < 0)
	{
		numero = numero * (-1);
		negativo = 1;
	}
	else 
	{
		negativo = 0;
	}
	sprintf (aux, "%.*lf", dec, numero);
	a=aux + strlen(aux) - 1;
	if (dec)
	{
		decimal = 1;
		for (;((*a!='.') && (*a != ','));a--,b--)
		*b=*a;
		if (Coma == 1)
		{
			*b=',';
		}
		else 
		{
			*b='.';
		}
		a--;
		b--;
	}
	ncarac = strlen (aux) - (dec+decimal);
	for (c=1;c<=ncarac;a--,b--)
	{
		*b=*a;
		if (!(c%3))
		{
			b--;
			if(Coma == 1)
			{
				*b='.';
			}
			else
			{
				*b=',';
			}
		}
		c++;
	}
	b++;
	if ((*b=='.') || (*b==',')) 
	{
		b++;
	}
	if (negativo)
	{
		b--;
		*b = '-';
	}
	memset(cadena,'\0',sizeof(cadena));
	strcpy(cadena,b);
}



