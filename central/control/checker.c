
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
           char  filnam[11];
};
static const struct sqlcxp sqlfpn =
{
    10,
    "checker.pc"
};


static unsigned long sqlctx = 70427;


static struct sqlexd {
   unsigned int   sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   unused;
      const short *cud;
   unsigned char  *sqlest;
      const char  *stmt;
   unsigned char  **sqphsv;
   unsigned int   *sqphsl;
            short **sqpind;
   unsigned int   *sqparm;
   unsigned int   **sqparc;
   unsigned char  *sqhstv[9];
   unsigned int   sqhstl[9];
            short *sqindv[9];
   unsigned int   sqharm[9];
   unsigned int   *sqharc[9];
} sqlstm = {8,9};

/* Prototypes */
extern void sqlcxt (void **, unsigned long *,
                    struct sqlexd *, const struct sqlcxp *);
extern void sqlcx2t(void **, unsigned long *,
                    struct sqlexd *, const struct sqlcxp *);
extern void sqlbuft(void **, char *);
extern void sqlgs2t(void **, char *);
extern void sqlorat(void **, unsigned long *, void *);

/* Forms Interface */
static const int IAPSUCC = 0;
static const int IAPFAIL = 1403;
static const int IAPFTL  = 535;
extern void sqliem(char *, int *);

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static const short sqlcud0[] =
{8,4130,
2,0,0,1,0,0,539,156,0,3,3,0,1,0,1,9,0,0,1,9,0,0,1,10,0,0,
28,0,0,2,75,0,516,327,0,2,1,0,1,0,2,9,0,0,1,9,0,0,
50,0,0,3,57,0,516,370,0,2,1,0,1,0,2,9,0,0,1,9,0,0,
72,0,0,4,355,0,516,465,0,9,4,0,1,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,1,9,
0,0,1,9,0,0,1,9,0,0,1,9,0,0,
};


#line 1 "checker.pc"
#undef	_DEBUG_CHECKER 
#define _RELEASE_CHECKER 1

#define COD0 1

/************************************************************************/
/*									*/
/* checker.pc	    : Funcion para controlar tabla cred_trx		*/
/*		      contra la tabla de credito de sucursal.     	*/
/*									*/
/* Proyecto	    : Autorizacion de tarjetas de credito - StoreFlow.	*/
/* Cliente	    : Coto						*/
/* Autor	    : Rolando Loustric					*/
/* Fecha	    : 26/08/96						*/
/* Compilador	    : C++						*/
/* Base de datos    : Oracle						*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
#ifndef OK
  #define OK    0
  #define NOOK  -1
#endif

#define USERID_LEN   20
#define PASSWD_LEN   20
#define MAX_LONG_TARJ	20

extern int	SqlErrno;

struct trx 
{
char nro_caj[6];
char nro_trx[5];
char cod_tar[3];
char nro_tar[21];
char cod_aut[9];
char monto[15];
char fecha[7];
char hora[5];
char cuotas[4];
char plan[3];
char sitsend[3];
};

struct trx TrxRead;

typedef struct
{
    char	userid[USERID_LEN+1];
    char	passwd[PASSWD_LEN+1];
} str_login;
     
FILE *fsal;

/* EXEC SQL INCLUDE SQLCA;
 */ 
#line 1 "/home/oracle/app/oracle/product/7.3.3/precomp/public/sqlca.h"
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

#line 62 "checker.pc"

/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 63 "checker.pc"

    /* VARCHAR	userid[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } userid;
#line 64 "checker.pc"

    /* VARCHAR	passwd[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } passwd;
#line 65 "checker.pc"

    /* VARCHAR     nro_tar[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } nro_tar;
#line 66 "checker.pc"

    /* VARCHAR     cod_aut[6]; */ 
struct { unsigned short len; unsigned char arr[6]; } cod_aut;
#line 67 "checker.pc"

    /* VARCHAR     nro_caj[5]; */ 
struct { unsigned short len; unsigned char arr[5]; } nro_caj;
#line 68 "checker.pc"

    /* VARCHAR     nro_trx[4]; */ 
struct { unsigned short len; unsigned char arr[4]; } nro_trx;
#line 69 "checker.pc"

    /* VARCHAR     cod_tar[2]; */ 
struct { unsigned short len; unsigned char arr[2]; } cod_tar;
#line 70 "checker.pc"

    /* VARCHAR     monto[12]; */ 
struct { unsigned short len; unsigned char arr[12]; } monto;
#line 71 "checker.pc"

    /* VARCHAR     nro_lot[3]; */ 
struct { unsigned short len; unsigned char arr[3]; } nro_lot;
#line 72 "checker.pc"

    /* VARCHAR     mod_env[2]; */ 
struct { unsigned short len; unsigned char arr[2]; } mod_env;
#line 73 "checker.pc"

    /* VARCHAR     descrip[30]; */ 
struct { unsigned short len; unsigned char arr[30]; } descrip;
#line 74 "checker.pc"

    short       NULL1;
    /* Mod DS 03/01/2000 Multiples Tarjetas */
    /* VARCHAR     tarjeta[21]; */ 
struct { unsigned short len; unsigned char arr[21]; } tarjeta;
#line 77 "checker.pc"

#ifdef COD0
    /* VARCHAR     cod_aut_loc[7]; */ 
struct { unsigned short len; unsigned char arr[7]; } cod_aut_loc;
#line 79 "checker.pc"

#endif
/* EXEC SQL END DECLARE SECTION; */ 
#line 81 "checker.pc"


/*Funcion Auxiliar que convierte la fecha de Juliano a Gregoriano*/
int Conv2Greg(char *FJ, char *FG)
{
    int Months[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    int m,year;
    int FJul;
    time_t ltime;
    struct tm *newtime;

    FJul=atoi(FJ);
    m=1;

    time(&ltime);
    newtime=localtime(&ltime);
    year=newtime->tm_year+1900;

    if ((((year%4)==0) && ((year%100)!=0)) || ((year%1000)==0))
        Months[2]=29;

    while (FJul-Months[m]>0)
    {
        FJul-=Months[m];
        m++;
    }
    sprintf(FG,"%02d%02d",m,FJul);

    return OK;
}

/* Funcion para conversion de Unipago */
int FunConUni(char *Tarjeta)
{
  char FecJul[4];
  char FecGre[5];
  char C2Ini[30];
  char C2Fin[30];
  char Trx[7];

  strcpy(C2Ini,Tarjeta);
  strncpy(FecJul,C2Ini+2,3);
  FecJul[3]=0;
  strncpy(Trx,C2Ini+6,6);
  Trx[6]=0;

  Conv2Greg(FecJul,FecGre);

  sprintf(C2Fin,"%s%s",FecGre,Trx);
  sprintf(Tarjeta, C2Fin);

  return OK;
}

void getFechaActual(char *fecha)
{
struct tm	*newtime;
time_t	        ltime;

    time(&ltime);
    newtime=localtime(&ltime);

    sprintf(fecha, "%02d/%02d/%02d", newtime->tm_mday, 
	           newtime->tm_mon+1, newtime->tm_year);
}

int open_db(str_login *data)
{
int SqlErrno;

    strcpy((char *)userid.arr, data->userid);
    userid.len=strlen(data->userid);
    strcpy((char *)passwd.arr, data->passwd);
    passwd.len=strlen(data->passwd);

    /* EXEC SQL
	CONNECT :userid IDENTIFIED BY :passwd; */ 
#line 157 "checker.pc"

{
    struct sqlexd sqlstm;

    sqlstm.sqlvsn = 8;
    sqlstm.arrsiz = 3;
    sqlstm.iters = (unsigned int  )10;
    sqlstm.offset = (unsigned int  )2;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)0;
    sqlstm.sqhstv[0] = (unsigned char  *)&userid;
    sqlstm.sqhstl[0] = (unsigned int  )22;
    sqlstm.sqindv[0] = (         short *)0;
    sqlstm.sqharm[0] = (unsigned int  )0;
    sqlstm.sqhstv[1] = (unsigned char  *)&passwd;
    sqlstm.sqhstl[1] = (unsigned int  )22;
    sqlstm.sqindv[1] = (         short *)0;
    sqlstm.sqharm[1] = (unsigned int  )0;
    sqlstm.sqphsv = sqlstm.sqhstv;
    sqlstm.sqphsl = sqlstm.sqhstl;
    sqlstm.sqpind = sqlstm.sqindv;
    sqlstm.sqparm = sqlstm.sqharm;
    sqlstm.sqparc = sqlstm.sqharc;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 157 "checker.pc"

    if (sqlca.sqlcode!=OK)
       {
	  SqlErrno=sqlca.sqlcode;
          printf("error al conectarse a la Base de Datos\n");
          printf("Us [%*.*s] Pa [%*.*s]\n",passwd.len,passwd.len,
          passwd.arr,userid.len,userid.len,userid.arr);
          printf("Error [%d]\n",SqlErrno);
	  return NOOK;
       }
    SqlErrno=OK;
    return OK;
}

void init()
{
    str_login	login_data;

#ifdef _DEBUG_CHECKER
    strcpy(login_data.userid, "u606");
    strcpy(login_data.passwd, "u606");
#endif

#ifdef _RELEASE_CHECKER
    strcpy(login_data.userid, "u601");
    strcpy(login_data.passwd, "u601");
#endif

    open_db(&login_data);
}

int comp_cod_aut(char *cod1, char *cod2)
{
    return ((!strncmp(cod1, cod2, 6)) || (!strncmp(cod1, &cod2[2], 6)));
}

void encabe()
{

  fprintf(fsal, "\nEmisor       Nro Trx     Caja   Numero de tarjeta     Cod Aut     Fecha Trx  Lote  Envio  Monto  Estado\n");
  fprintf(fsal, "------       -------     ----   -----------------     -------     ---------  ----  -----  -----  ------\n\n");

}

void imp_totales(double TotNt, double TotAdm, int QTrxNt, int QTrxAdm)
{

  fprintf(fsal, "\n\n\t Totales Nueva Tienda : \n");
  fprintf(fsal, "\t\t\t\t Total de transacciones : %ld \n", QTrxNt);
  fprintf(fsal, "\t\t\t\t Monto Total : %6.2f \n", TotNt);
  fprintf(fsal, "\n\t Totales Administrador : \n");
  fprintf(fsal, "\t\t\t\t Total de transacciones : %ld \n", QTrxAdm);
  fprintf(fsal, "\t\t\t\t Monto Total : %6.2f \n", TotAdm);

}
   

int main (int argc, char *argv[])
{
#ifdef _DEBUG_CHECKER
FILE *fLogueo;
#endif

char buffer[256];
char buffer2[256];

FILE *fvtacard, *fanom, *fpending;
FILE *fvta, *fpend;
char fec[15];
int cont=0, nro, ret1, ret2;
int TrxNt=0, TrxNtTar=0, TrxAdm=0, TrxPend=0, TrxPendTar=0; 
double dMontoSumNt=0.0;
double dMontoSumNtTar=0.0;
double dMontoSumAdm=0.0;
double dMontoSumPend=0.0;
double dMontoSumPendTar=0.0;
char filename[100];
char desc[8];
char modo_envio[4];
char fecha_ant[5];
char cod_tar_ant[3];
char entrada[15];
int  cerrar=0;
/* Modificado 28/02/2000 
   Contador para ubicar la primera posicion del numero de tarjeta
   que es distinto a cero y compararla con el valor de la tabla.  */
int  i;
char cod_est_aux[3];

    if (argc<2)
    {
         printf ("\nUso checker [nombre-file]\n\n");
         exit(1);
    }

    init();

    strcpy(entrada, argv[1]); 
    strncpy(fecha_ant, &entrada[8], 4);
    printf("\nFecha [%s]\n", fecha_ant);
    
    printf("\nApertura de archivo [%s]\n", argv[1]);
    if ((fvtacard=fopen(argv[1], "r"))==NULL)
    {
         printf ("\nError: Apertura de archivo %s\n", argv[1]);
         exit(1);
    }

    strcpy(filename, "anomal.");
    strcat(filename, fecha_ant);
    printf("\nGenerando [%s] ...\n", filename);
    if ((fanom=fopen(filename, "w"))==NULL)
    {
         printf("\nError: Apertura de archivo %s\n", filename);
         exit(1);
    }
    
    strcpy(filename, "salida.");
    strcat(filename, fecha_ant);
    printf("\nGenerando [%s] ...\n", filename);
    if ((fsal=fopen(filename, "w"))==NULL)
    {
         printf("\nError: Apertura de archivo %s\n", filename);
         exit(1);
    }

    strcpy(filename, "pending.");
    strcat(filename, fecha_ant);
    printf("\nGenerando [%s] ...\n", filename);
    if ((fpending=fopen(filename, "w"))==NULL)
    {
         printf("\nError: Apertura de archivo %s\n", filename);
         exit(1);
    }
    
    getFechaActual(fec);

    fprintf(fsal, "\n\t\t\t\tListado de Control de Cierre de Lote \n\n");
    fprintf(fsal, "\t\t\t\t\t\t\t\t\t Fecha : %s \n", fec);
    encabe();

    strcpy(cod_tar_ant, "00");

    while(fgets(buffer,sizeof(buffer),fvtacard)!=NULL)
    {
       cont++;
       if (cont==60)
       {
          fprintf(fsal, "\f");
          encabe();
          cont=0;
       }

       strncpy(TrxRead.nro_caj, buffer, 5);
       strncpy(TrxRead.nro_trx, &buffer[5], 4);
       strncpy(TrxRead.cod_tar, &buffer[9], 2);
       strncpy(TrxRead.nro_tar, &buffer[11], 20);
       strncpy(TrxRead.cod_aut, &buffer[31], 8);
       strncpy(TrxRead.monto, &buffer[39], 14);
       strncpy(TrxRead.fecha, &buffer[53], 6);
       strncpy(TrxRead.hora, &buffer[67], 4);  
       strncpy(TrxRead.plan, &buffer[71], 2);  
       strncpy(TrxRead.cuotas, &buffer[73], 3);  

#ifdef COD0
   if ( strcmp(TrxRead.cod_aut, "000000") == 0 )
   {
       strcpy((char *)cod_tar.arr, TrxRead.cod_tar);
       cod_tar.len=strlen(TrxRead.cod_tar);

        /* EXEC SQL
        SELECT NVL(cod_aut_loc,'000000')
            INTO :cod_aut_loc
            FROM cred_tarj WHERE
                 Cod_tar=:cod_tar; */ 
#line 331 "checker.pc"

{
        struct sqlexd sqlstm;

        sqlstm.sqlvsn = 8;
        sqlstm.arrsiz = 3;
        sqlstm.stmt = "select NVL(cod_aut_loc,'000000') into :b0  from cred_\
tarj where Cod_tar=:b1";
        sqlstm.iters = (unsigned int  )1;
        sqlstm.offset = (unsigned int  )28;
        sqlstm.selerr = (unsigned short)0;
        sqlstm.cud = sqlcud0;
        sqlstm.sqlest = (unsigned char  *)&sqlca;
        sqlstm.sqlety = (unsigned short)0;
        sqlstm.sqhstv[0] = (unsigned char  *)&cod_aut_loc;
        sqlstm.sqhstl[0] = (unsigned int  )9;
        sqlstm.sqindv[0] = (         short *)0;
        sqlstm.sqharm[0] = (unsigned int  )0;
        sqlstm.sqhstv[1] = (unsigned char  *)&cod_tar;
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

#line 331 "checker.pc"


        if ((sqlca.sqlcode!=0) && (sqlca.sqlcode!=1403) && (sqlca.sqlcode!=-1403
))
        {
             strncpy(TrxRead.cod_aut, "000000",6);
        }
        else
        {
             strncpy(TrxRead.cod_aut, cod_aut_loc.arr,6);
        }
   }
#endif

       if (TrxRead.plan[0]=='\n')
          {
       strncpy(TrxRead.plan, "00",2);  
       strncpy(TrxRead.cuotas, "001",3);  
          }   
       strcpy((char *)cod_tar.arr, TrxRead.cod_tar);
       cod_tar.len=strlen(TrxRead.cod_tar);

       if (strcmp(TrxRead.cod_tar, cod_tar_ant))
       {
          /* Cambia el tipo de tarjeta */
          if (cerrar)
          {
            fprintf(fvta, "\n\n\t\t\t\tTotal de operaciones: %ld\n", TrxNtTar); 
            fprintf(fvta, "\t\t\t\tImporte total       : %.2f\n", dMontoSumNtTar); 
            fprintf(fpend, "\n\n\t\t\t\tTotal de operaciones: %ld\n", TrxPendTar); 
            fprintf(fpend, "\t\t\t\tImporte total       : %.2f\n", dMontoSumPendTar); 
            close(fpend);
            close(fvta);
          }
          else
          {
            cerrar=1;
          }

          /* EXEC SQL
              SELECT descrip
              INTO :descrip
              FROM cred_tarj
              WHERE cod_tar=:cod_tar; */ 
#line 374 "checker.pc"

{
          struct sqlexd sqlstm;

          sqlstm.sqlvsn = 8;
          sqlstm.arrsiz = 3;
          sqlstm.stmt = "select descrip into :b0  from cred_tarj where cod_t\
ar=:b1";
          sqlstm.iters = (unsigned int  )1;
          sqlstm.offset = (unsigned int  )50;
          sqlstm.selerr = (unsigned short)0;
          sqlstm.cud = sqlcud0;
          sqlstm.sqlest = (unsigned char  *)&sqlca;
          sqlstm.sqlety = (unsigned short)0;
          sqlstm.sqhstv[0] = (unsigned char  *)&descrip;
          sqlstm.sqhstl[0] = (unsigned int  )32;
          sqlstm.sqindv[0] = (         short *)0;
          sqlstm.sqharm[0] = (unsigned int  )0;
          sqlstm.sqhstv[1] = (unsigned char  *)&cod_tar;
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

#line 374 "checker.pc"


          printf("Salida del la consulta [%d] \n", sqlca.sqlcode);

          if (sqlca.sqlcode == !0)
          {   
             printf(" ERROR: No puede obtener informacion de cred_tarj  [%d]\n",
                     sqlca.sqlcode);
          } 
                
 
          memset(filename, 0, sizeof(filename)); 
          strncpy(filename, (char *)descrip.arr, 3);
          strcat(filename, "pend.");
          strcat(filename, fecha_ant);

          printf("\nGenerando [%s] ...\n", filename);
          if ((fpend=fopen(filename, "w"))==NULL)
          {
             printf ("\nError: Apertura de archivo %s\n", filename);
             exit(1);
          }

          fprintf(fpend, "\n\tCOTO C.I.C.S.A.\t\t\tPGM: checker\n"); 
          fprintf(fpend, "\tFecha: %s\n", fec);
          fprintf(fpend, "\n\n\t\t\tListado de pendientes por tarjeta\n"); 
          fprintf(fpend, "\n\n\t\tTarjeta: %s\n\n\n", descrip.arr); 

          memset(filename, 0, sizeof(filename)); 
          strncpy(filename, (char *)descrip.arr, 3);
          strcat(filename, "vta.");
          strcat(filename, fecha_ant);

          printf("\nGenerando [%s] ...\n", filename);
          if ((fvta=fopen(filename, "w"))==NULL)
          {
             printf ("\nError: Apertura de archivo %s\n", filename);
             exit(1);
          }

          fprintf(fvta, "\n\tCOTO C.I.C.S.A.\t\t\tPGM: checker\n"); 
          fprintf(fvta, "\tFecha: %s\n", fec);
          fprintf(fvta, "\n\n\t\t\tListado de ventas por tarjeta\n"); 
          fprintf(fvta, "\n\n\t\tTarjeta: %s\n\n\n", descrip.arr); 

          strcpy(cod_tar_ant, TrxRead.cod_tar);

/* Inicializo acumuladores de tipo de tarjetas */

          TrxNtTar=0;
          dMontoSumNtTar=0.0;
          TrxPendTar=0;
          dMontoSumPendTar=0.0;

       }

/* Acumulo totales para Nueva Tienda */
 
       TrxNt++;
       dMontoSumNt+=atof(TrxRead.monto);
       TrxNtTar++;
       dMontoSumNtTar+=atof(TrxRead.monto);
        
       strcpy((char *)nro_caj.arr, TrxRead.nro_caj);
       nro_caj.len=strlen(TrxRead.nro_caj);
       strcpy((char *)nro_trx.arr, TrxRead.nro_trx);
       nro_trx.len=strlen(TrxRead.nro_trx);
    
       /* Mod DS 03/01/2001 Multiples tarjetas */
       i=0;
       while ( (TrxRead.nro_tar[i]=='0') && (i<MAX_LONG_TARJ) )
             i++;
       strcpy((char *)tarjeta.arr, &TrxRead.nro_tar[i]);
       tarjeta.len=strlen(TrxRead.nro_tar)-i;

       /* Mod 23/01/2001 DS Conversion de tarjetas de unipago */
       /* para soportar multiples tarjetas                    */
       sprintf(cod_est_aux, "20");
       if (strncmp(cod_tar.arr, cod_est_aux,2)==0) 
       {
	   FunConUni(tarjeta.arr);
           tarjeta.len= strlen(tarjeta.arr);
       }

       memset(monto.arr, 0, sizeof(monto.arr));
	
	/*********************************************************************/
	/*MODIFICADO: Se agregaron los codigos de estador 33(vta-dev anulada)*/
	/*34(vta-dev anulada online) en el select, para tratar el envio de   */
	/*devoluciones como anulaciones [DS][DL][10-12-2000]                 */
	/*********************************************************************/
       /* EXEC SQL
           SELECT nro_tar, cod_aut, monto, nro_lot, mod_env
              INTO :nro_tar, :cod_aut:NULL1, :monto, :nro_lot, :mod_env
              FROM cred_trx
              WHERE nro_caj=:nro_caj and 
                    nro_trx=:nro_trx and
                    cod_tar=:cod_tar and
		    rtrim(nro_tar,' ')=:tarjeta and 
              (cod_est='00' or cod_est='02' or cod_est='04' or cod_est='12' or 
               cod_est='10' or cod_est='32' or cod_est='33' or cod_est='34' or
               cod_est='35' or cod_est='36' ); */ 
#line 475 "checker.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 9;
       sqlstm.stmt = "select nro_tar ,cod_aut ,monto ,nro_lot ,mod_env into \
:b0,:b1:b2,:b3,:b4,:b5  from cred_trx where ((((nro_caj=:b6 and nro_trx=:b7) a\
nd cod_tar=:b8) and rtrim(nro_tar,' ')=:b9) and (((((((((cod_est='00' or cod_e\
st='02') or cod_est='04') or cod_est='12') or cod_est='10') or cod_est='32') o\
r cod_est='33') or cod_est='34') or cod_est='35') or cod_est='36'))";
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )72;
       sqlstm.selerr = (unsigned short)0;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlstm.sqhstv[0] = (unsigned char  *)&nro_tar;
       sqlstm.sqhstl[0] = (unsigned int  )22;
       sqlstm.sqindv[0] = (         short *)0;
       sqlstm.sqharm[0] = (unsigned int  )0;
       sqlstm.sqhstv[1] = (unsigned char  *)&cod_aut;
       sqlstm.sqhstl[1] = (unsigned int  )8;
       sqlstm.sqindv[1] = (         short *)&NULL1;
       sqlstm.sqharm[1] = (unsigned int  )0;
       sqlstm.sqhstv[2] = (unsigned char  *)&monto;
       sqlstm.sqhstl[2] = (unsigned int  )14;
       sqlstm.sqindv[2] = (         short *)0;
       sqlstm.sqharm[2] = (unsigned int  )0;
       sqlstm.sqhstv[3] = (unsigned char  *)&nro_lot;
       sqlstm.sqhstl[3] = (unsigned int  )5;
       sqlstm.sqindv[3] = (         short *)0;
       sqlstm.sqharm[3] = (unsigned int  )0;
       sqlstm.sqhstv[4] = (unsigned char  *)&mod_env;
       sqlstm.sqhstl[4] = (unsigned int  )4;
       sqlstm.sqindv[4] = (         short *)0;
       sqlstm.sqharm[4] = (unsigned int  )0;
       sqlstm.sqhstv[5] = (unsigned char  *)&nro_caj;
       sqlstm.sqhstl[5] = (unsigned int  )7;
       sqlstm.sqindv[5] = (         short *)0;
       sqlstm.sqharm[5] = (unsigned int  )0;
       sqlstm.sqhstv[6] = (unsigned char  *)&nro_trx;
       sqlstm.sqhstl[6] = (unsigned int  )6;
       sqlstm.sqindv[6] = (         short *)0;
       sqlstm.sqharm[6] = (unsigned int  )0;
       sqlstm.sqhstv[7] = (unsigned char  *)&cod_tar;
       sqlstm.sqhstl[7] = (unsigned int  )4;
       sqlstm.sqindv[7] = (         short *)0;
       sqlstm.sqharm[7] = (unsigned int  )0;
       sqlstm.sqhstv[8] = (unsigned char  *)&tarjeta;
       sqlstm.sqhstl[8] = (unsigned int  )23;
       sqlstm.sqindv[8] = (         short *)0;
       sqlstm.sqharm[8] = (unsigned int  )0;
       sqlstm.sqphsv = sqlstm.sqhstv;
       sqlstm.sqphsl = sqlstm.sqhstl;
       sqlstm.sqpind = sqlstm.sqindv;
       sqlstm.sqparm = sqlstm.sqharm;
       sqlstm.sqparc = sqlstm.sqharc;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 475 "checker.pc"

	
#ifdef _DEBUG_CHECKER
	if ((fLogueo=fopen("LogChecker.log","a+"))!=NULL)
	{
		fprintf(fLogueo, "Caj: %s, Trx: %s, Cod: %s, Tar: %s, Lon: %d, Err: %d\n\n", nro_caj.arr, nro_trx.arr, cod_tar.arr, tarjeta.arr, tarjeta.len, sqlca.sqlcode);
		fclose(fLogueo);
	}
#endif
 
       if (NULL1==-1)
       {
          strncpy((char *)cod_aut.arr, "", sizeof(cod_aut.arr));
       }
 
       if ((sqlca.sqlcode!=0)&&(sqlca.sqlcode!=1403))
       {
          printf("Error en Select %ld \n", sqlca.sqlcode);
          exit(1);
       }
         
       else if (sqlca.sqlcode==1403)
            {



               memset(buffer2, '\0', strlen(buffer2));
               strncpy(buffer2, buffer, 61);
               strncat(buffer2, fecha_ant,4);
               strncat(buffer2, TrxRead.hora,4);
               strncat(buffer2, TrxRead.plan,2);   /*cuotas*/
               strncat(buffer2, TrxRead.cuotas,3);   /*cuotas*/
               strcat(buffer2, "\n");


               strcpy(desc, "PEND");     /* Caso 1 Transaccion pendiente */
               strcpy(modo_envio, "--");
               fprintf(fpending, "%s", buffer2);
               fprintf(fpend, "%s", buffer);

/* Acumulo totales de pendientes */

               TrxPend++;
               dMontoSumPend+=atof(TrxRead.monto);
               TrxPendTar++;
               dMontoSumPendTar+=atof(TrxRead.monto);

            }
            else    /* Comparacion de los campos cod_aut y nro_tar*/
            {

/* Acumulo totales para el Administrador */
           
               TrxAdm++;
               dMontoSumAdm+=atof((char *)monto.arr);
      
               /* Modificado 28/02/2000 
               Se obtiene la primera posici�n distinta de cero de la variable
               TrxRead.nro_tar para compararla con la obtenida por Oracle */   
               
               i=0;
               while ( (TrxRead.nro_tar[i]=='0') && (i<MAX_LONG_TARJ) )
                    i++;
            /* Se comenta esta linea para reemplazarla por la siguiente
               ret1=strncmp((char *)nro_tar.arr, &TrxRead.nro_tar[4], 16); */
               ret1=strncmp((char *)nro_tar.arr, &TrxRead.nro_tar[i], 
                                                 MAX_LONG_TARJ-i);
               /* Fin modificacion 28/02/2000 */
   
               ret2=comp_cod_aut((char *)cod_aut.arr, TrxRead.cod_aut);
                 
               if ((ret1!=0)&&(ret2!=0))   
               {
                  strcpy(desc, "ERROR");  /* Caso 2 No coincide nada*/
               }

               else if (ret2!=0)    /* Caso 3 No coincide cod_aut */
                    {   
                        strcpy(desc, "NOOK(1)");
                        fprintf(fanom, "%s", buffer);
                    }

                    else if (ret1!=0) /* Caso 4 No coincide nro_tar */
                         {
                            strcpy(desc, "NOOK(2)");
                            fprintf(fanom, "%s", buffer);
                         }
 
                         else /* Caso 5 Estado OK */
                         {
                            strcpy(desc, "(OK)");    /* Caso 2 */
                         }

             } /* Fin del else de la comparacion de los campos */

 
       if (!strncmp((char *)mod_env.arr,"00",2))
       {
           strcpy(modo_envio, "on");
       }
       else
       {
           strcpy(modo_envio, "off");
       }

       fprintf(fvta, "%s", buffer);

       /* Modificacion 28/02/2000 
          Se cambio el 4 al imprimir TrxRead.nro_tar por la variable i, 
          que contiene la primera posicion distinta de cero del nro tarjeta */

       fprintf(fsal, "%9s %7s %13s %19s %12s %11s %6s %5s %10s %8s\n",
                       TrxRead.cod_tar, TrxRead.nro_trx, TrxRead.nro_caj,
                       &TrxRead.nro_tar[i], TrxRead.cod_aut, TrxRead.fecha,
                       nro_lot.arr, modo_envio, TrxRead.monto, desc);

    } /* Fin del while ppal. */

    imp_totales(dMontoSumNt, dMontoSumAdm/100, TrxNt, TrxAdm);

    fprintf(fvta, "\n\n\t\t\t\tTotal de operaciones: %ld\n", TrxNtTar); 
    fprintf(fvta, "\t\t\t\tImporte total       : %.2f\n", dMontoSumNtTar); 

    fprintf(fpend, "\n\n\t\t\t\tTotal de operaciones: %ld\n", TrxPendTar); 
    fprintf(fpend, "\t\t\t\tImporte total       : %.2f\n", dMontoSumPendTar); 

    fclose(fvtacard);
    fclose(fanom);
    fclose(fpending);
    fclose(fsal);
    fclose(fvta);
    fclose(fpend);

}
