
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
    "depuaux.pc"
};


static unsigned long sqlctx = 72715;


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
   unsigned char  *sqhstv[3];
   unsigned int   sqhstl[3];
            short *sqindv[3];
   unsigned int   sqharm[3];
   unsigned int   *sqharc[3];
} sqlstm = {8,3};

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

 static const char *sq0002 = 
"select nro_caj  from cred_term  order by nro_caj           ";
typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static const short sqlcud0[] =
{8,4130,
2,0,0,1,0,0,539,69,0,3,3,0,1,0,1,9,0,0,1,9,0,0,1,10,0,0,
28,0,0,2,59,0,521,127,0,0,0,0,1,0,
42,0,0,2,0,0,525,130,0,1,0,0,1,0,2,9,0,0,
60,0,0,3,109,0,515,144,0,1,1,0,1,0,1,9,0,0,
78,0,0,2,0,0,525,157,0,1,0,0,1,0,2,9,0,0,
96,0,0,4,0,0,543,174,0,0,0,0,1,0,
110,0,0,5,0,0,541,179,0,0,0,0,1,0,
124,0,0,6,664,0,515,182,0,1,1,0,1,0,1,9,0,0,
142,0,0,7,0,0,543,254,0,0,0,0,1,0,
156,0,0,8,0,0,541,259,0,0,0,0,1,0,
170,0,0,9,120,0,514,262,0,1,1,0,1,0,1,9,0,0,
188,0,0,10,0,0,543,272,0,0,0,0,1,0,
202,0,0,11,0,0,541,277,0,0,0,0,1,0,
216,0,0,2,0,0,525,280,0,1,0,0,1,0,2,9,0,0,
234,0,0,2,0,0,527,293,0,0,0,0,1,0,
};


#line 1 "depuaux.pc"
/************************************************************************/
/*									*/
/* depuaux.pc	    : Funcion SQL para realizar la depuracion           */
/*		      de la tabla cred_trx. 				*/
/*									*/
/* Proyecto	    : Autorizacion de tarjetas de credito - StoreFlow.	*/
/* Cliente	    : Coto						*/
/* Autor	    : Rolando Loustric					*/
/* Fecha	    : 21/08/96						*/
/* Compilador	    : C++						*/
/* Base de datos    : Oracle						*/
/*									*/
/************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef OK
  #define OK    0
  #define NOOK  -1
#endif

#define USERID_LEN       20
#define PASSWD_LEN       20

#define varcpy(X,Y)		{ strncpy(X.arr,Y , sizeof(X.arr));  \
				  X.len=(sizeof(X.arr) < strlen(Y) ? \
				       sizeof(X.arr) : strlen(Y)); }
#define varcpy2(X,Y)		{ strncpy(X, Y.arr, Y.len);\
				  X[Y.len]=0; }

typedef struct
{
    char	userid[USERID_LEN+1];
    char	passwd[PASSWD_LEN+1];
} str_login;


int open_db(str_login *data);
void init();
void getFechaActual(char *fecha);

/* EXEC SQL INCLUDE SQLCA;
 */ 
#line 1 "/u01/app/oracle/product/7.3.2/precomp/public/sqlca.h"
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

#line 45 "depuaux.pc"

long              ret;
int               code;
int               SqlErrno;
char              hor[11];
char              caj[6];


/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 53 "depuaux.pc"

    /* VARCHAR	userid[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } userid;
#line 54 "depuaux.pc"

    /* VARCHAR	passwd[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } passwd;
#line 55 "depuaux.pc"

    /* VARCHAR     nro_caj[5]; */ 
struct { unsigned short len; unsigned char arr[5]; } nro_caj;
#line 56 "depuaux.pc"

/* EXEC SQL END DECLARE SECTION; */ 
#line 57 "depuaux.pc"



int open_db(str_login *data)
{
int SqlErrno;

    strcpy(userid.arr, data->userid);
    userid.len=strlen(data->userid);
    strcpy(passwd.arr, data->passwd);
    passwd.len=strlen(data->passwd);

    /* EXEC SQL
	CONNECT :userid IDENTIFIED BY :passwd; */ 
#line 70 "depuaux.pc"

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

#line 70 "depuaux.pc"


    if (sqlca.sqlcode!=OK)
        {
	SqlErrno=sqlca.sqlcode;
	return NOOK;
        }
    SqlErrno=OK;
    return OK;
}

void init()
{
    str_login	login_data;

    strcpy(login_data.userid, "u601");
    strcpy(login_data.passwd, "u601");
    open_db(&login_data);
}

void getFechaActual(char *hora)
{
char fecha[11];
struct tm	*newtime;
time_t	        ltime;

    time(&ltime);
    newtime=localtime(&ltime);

    sprintf(fecha, "%02d%02d%02d%02d%02d", newtime->tm_mday, 
	newtime->tm_mon+1, newtime->tm_hour, newtime->tm_min, 
	newtime->tm_sec);

    sprintf(hora, "%2.2s:%2.2s:%2.2s", &fecha[4], &fecha[6], &fecha[8]); 

}


int main()
{
time_t hIni;
FILE    *fd;
  
    if ((fd = fopen("depuracion.log", "w")) == NULL) 
    {
	printf("error en apertura de log\n");
 	exit(1);
    }

    init();
    
    /* EXEC SQL
        DECLARE cursor_term CURSOR FOR
            SELECT nro_caj
            FROM cred_term
            ORDER BY nro_caj; */ 
#line 125 "depuaux.pc"


    /* EXEC SQL
        OPEN cursor_term; */ 
#line 128 "depuaux.pc"

{
    struct sqlexd sqlstm;

    sqlstm.sqlvsn = 8;
    sqlstm.arrsiz = 3;
    sqlstm.stmt = sq0002;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )28;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)0;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 128 "depuaux.pc"


    /* EXEC SQL 
        FETCH cursor_term
           INTO :nro_caj; */ 
#line 132 "depuaux.pc"

{
    struct sqlexd sqlstm;

    sqlstm.sqlvsn = 8;
    sqlstm.arrsiz = 3;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )42;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)0;
    sqlstm.sqhstv[0] = (unsigned char  *)&nro_caj;
    sqlstm.sqhstl[0] = (unsigned int  )7;
    sqlstm.sqindv[0] = (         short *)0;
    sqlstm.sqharm[0] = (unsigned int  )0;
    sqlstm.sqphsv = sqlstm.sqhstv;
    sqlstm.sqphsl = sqlstm.sqhstl;
    sqlstm.sqpind = sqlstm.sqindv;
    sqlstm.sqparm = sqlstm.sqharm;
    sqlstm.sqparc = sqlstm.sqharc;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 132 "depuaux.pc"


    code = sqlca.sqlcode;

    varcpy2(caj, nro_caj);

    while(code==0)
    {

       fprintf(fd, "Numero de caja a depurar [%s] \n", caj);
       printf("Numero de caja a depurar [%s] \n", caj);

       /* EXEC SQL
           INSERT INTO hist_trx1
                   SELECT * FROM cred_trx 
                      WHERE nro_caj = :nro_caj AND
                            cod_est = '04' AND 
                            fec_loc like '0414%'; */ 
#line 149 "depuaux.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 3;
       sqlstm.stmt = "insert into hist_trx1 select *  from cred_trx where ((\
nro_caj=:b0 and cod_est='04') and fec_loc like '0414%')";
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )60;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlstm.sqhstv[0] = (unsigned char  *)&nro_caj;
       sqlstm.sqhstl[0] = (unsigned int  )7;
       sqlstm.sqindv[0] = (         short *)0;
       sqlstm.sqharm[0] = (unsigned int  )0;
       sqlstm.sqphsv = sqlstm.sqhstv;
       sqlstm.sqphsl = sqlstm.sqhstl;
       sqlstm.sqpind = sqlstm.sqindv;
       sqlstm.sqparm = sqlstm.sqharm;
       sqlstm.sqparc = sqlstm.sqharc;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 149 "depuaux.pc"



       if (sqlca.sqlcode==-1)
       {
           fprintf(fd, "Error en Insert (Tabla hist_trx1) %ld\n",sqlca.sqlcode);
           fprintf(fd, "caja: [%s]\n", caj);

           /* EXEC SQL 
              FETCH cursor_term
                 INTO :nro_caj; */ 
#line 159 "depuaux.pc"

{
           struct sqlexd sqlstm;

           sqlstm.sqlvsn = 8;
           sqlstm.arrsiz = 3;
           sqlstm.iters = (unsigned int  )1;
           sqlstm.offset = (unsigned int  )78;
           sqlstm.cud = sqlcud0;
           sqlstm.sqlest = (unsigned char  *)&sqlca;
           sqlstm.sqlety = (unsigned short)0;
           sqlstm.sqhstv[0] = (unsigned char  *)&nro_caj;
           sqlstm.sqhstl[0] = (unsigned int  )7;
           sqlstm.sqindv[0] = (         short *)0;
           sqlstm.sqharm[0] = (unsigned int  )0;
           sqlstm.sqphsv = sqlstm.sqhstv;
           sqlstm.sqphsl = sqlstm.sqhstl;
           sqlstm.sqpind = sqlstm.sqindv;
           sqlstm.sqparm = sqlstm.sqharm;
           sqlstm.sqparc = sqlstm.sqharc;
           sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 159 "depuaux.pc"

     
           varcpy2(caj, nro_caj);

           code = sqlca.sqlcode;

           continue;
       }
           

       if (sqlca.sqlcode!=0)
       {
           fprintf(fd, "Error en Insert (Tabla hist_trx1) %ld\n",sqlca.sqlcode);
           printf("Error en Insert (Tabla hist_trx1) %ld\n",sqlca.sqlcode);

           /* EXEC SQL
               ROLLBACK ; */ 
#line 175 "depuaux.pc"

{
           struct sqlexd sqlstm;

           sqlstm.sqlvsn = 8;
           sqlstm.arrsiz = 3;
           sqlstm.iters = (unsigned int  )1;
           sqlstm.offset = (unsigned int  )96;
           sqlstm.cud = sqlcud0;
           sqlstm.sqlest = (unsigned char  *)&sqlca;
           sqlstm.sqlety = (unsigned short)0;
           sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 175 "depuaux.pc"
 
           exit(1);
       }

       /* EXEC SQL
           COMMIT; */ 
#line 180 "depuaux.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 3;
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )110;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 180 "depuaux.pc"


       /* EXEC SQL 
          INSERT INTO rech_trx
(NRO_SUC,
NRO_CAJ,
NRO_TRX,
MOD_ENV,
COD_TAR,
NRO_CA,
COD_TRX,
COD_EST,
CAN_RET,
NRO_LOT,
COD_PRO,
MONTO,
NRO_TAR,
NRO_TRC,
FEC_LOC,
FEC_VEN,
MOD_ING,
TRACKII,
NRO_TIC_ORI,
FEC_ORI,
NRO_TIC,
FEC_ENV,
FEC_RTA,
COD_AUT,
NRO_REF,
COD_RTA,
FEC_VTACARD,
FEC_CIE,
NRO_CAJ_SF,
NRO_CAJ_ORI)
                 SELECT 
NRO_SUC,
NRO_CAJ,
NRO_TRX,
MOD_ENV,
COD_TAR,
NRO_CA,
COD_TRX,
COD_EST,
CAN_RET,
NRO_LOT,
COD_PRO,
MONTO,
NRO_TAR,
NRO_TRC,
FEC_LOC,
FEC_VEN,
MOD_ING,
TRACKII,
NRO_TIC_ORI,
FEC_ORI,
NRO_TIC,
FEC_ENV,
FEC_RTA,
COD_AUT,
NRO_REF,
COD_RTA,
FEC_VTACARD,
FEC_CIE,
NRO_CAJ_SF,
NRO_CAJ_ORI
FROM cred_trx
                     WHERE nro_caj = :nro_caj AND 
                           cod_est IN ('03','10', '12', 'E3', 'E4') AND
                           fec_loc like '0414%'; */ 
#line 248 "depuaux.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 3;
       sqlstm.stmt = "insert into rech_trx(NRO_SUC,NRO_CAJ,NRO_TRX,MOD_ENV,C\
OD_TAR,NRO_CA,COD_TRX,COD_EST,CAN_RET,NRO_LOT,COD_PRO,MONTO,NRO_TAR,NRO_TRC,FE\
C_LOC,FEC_VEN,MOD_ING,TRACKII,NRO_TIC_ORI,FEC_ORI,NRO_TIC,FEC_ENV,FEC_RTA,COD_\
AUT,NRO_REF,COD_RTA,FEC_VTACARD,FEC_CIE,NRO_CAJ_SF,NRO_CAJ_ORI)select NRO_SUC \
,NRO_CAJ ,NRO_TRX ,MOD_ENV ,COD_TAR ,NRO_CA ,COD_TRX ,COD_EST ,CAN_RET ,NRO_LO\
T ,COD_PRO ,MONTO ,NRO_TAR ,NRO_TRC ,FEC_LOC ,FEC_VEN ,MOD_ING ,TRACKII ,NRO_T\
IC_ORI ,FEC_ORI ,NRO_TIC ,FEC_ENV ,FEC_RTA ,COD_AUT ,NRO_REF ,COD_RTA ,FEC_VTA\
CARD ,FEC_CIE ,NRO_CAJ_SF ,NRO_CAJ_ORI  from cred_trx where ((nro_caj=:b0 and \
cod_est in ('03','10','12','E3','E4')) and fec_loc like '0414%')";
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )124;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlstm.sqhstv[0] = (unsigned char  *)&nro_caj;
       sqlstm.sqhstl[0] = (unsigned int  )7;
       sqlstm.sqindv[0] = (         short *)0;
       sqlstm.sqharm[0] = (unsigned int  )0;
       sqlstm.sqphsv = sqlstm.sqhstv;
       sqlstm.sqphsl = sqlstm.sqhstl;
       sqlstm.sqpind = sqlstm.sqindv;
       sqlstm.sqparm = sqlstm.sqharm;
       sqlstm.sqparc = sqlstm.sqharc;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 248 "depuaux.pc"


       if (sqlca.sqlcode!=0)
       {
           fprintf(fd, "Error en Insert (Tabla rech_trx) %ld\n",sqlca.sqlcode);
           printf("Error en Insert (Tabla rech_trx) %ld\n",sqlca.sqlcode);
           /* EXEC SQL
               ROLLBACK ; */ 
#line 255 "depuaux.pc"

{
           struct sqlexd sqlstm;

           sqlstm.sqlvsn = 8;
           sqlstm.arrsiz = 3;
           sqlstm.iters = (unsigned int  )1;
           sqlstm.offset = (unsigned int  )142;
           sqlstm.cud = sqlcud0;
           sqlstm.sqlest = (unsigned char  *)&sqlca;
           sqlstm.sqlety = (unsigned short)0;
           sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 255 "depuaux.pc"
 
           exit(1);
       }

       /* EXEC SQL
           COMMIT; */ 
#line 260 "depuaux.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 3;
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )156;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 260 "depuaux.pc"


       /* EXEC SQL
           DELETE FROM cred_trx
              WHERE nro_caj = :nro_caj AND
                    cod_est IN ('04', '03', '07', '10', '12', 'E3', 'E4') AND
                    fec_loc like '0414%'; */ 
#line 266 "depuaux.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 3;
       sqlstm.stmt = "delete from cred_trx  where ((nro_caj=:b0 and cod_est \
in ('04','03','07','10','12','E3','E4')) and fec_loc like '0414%')";
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )170;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlstm.sqhstv[0] = (unsigned char  *)&nro_caj;
       sqlstm.sqhstl[0] = (unsigned int  )7;
       sqlstm.sqindv[0] = (         short *)0;
       sqlstm.sqharm[0] = (unsigned int  )0;
       sqlstm.sqphsv = sqlstm.sqhstv;
       sqlstm.sqphsl = sqlstm.sqhstl;
       sqlstm.sqpind = sqlstm.sqindv;
       sqlstm.sqparm = sqlstm.sqharm;
       sqlstm.sqparc = sqlstm.sqharc;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 266 "depuaux.pc"


       if ((sqlca.sqlcode!=0) && (sqlca.sqlcode!=1403))
       {
           fprintf(fd, "Error en Delete (Tabla cred_trx) %ld\n",sqlca.sqlcode);
           printf("Error en Delete (Tabla cred_trx) %ld\n",sqlca.sqlcode);
           /* EXEC SQL
               ROLLBACK; */ 
#line 273 "depuaux.pc"

{
           struct sqlexd sqlstm;

           sqlstm.sqlvsn = 8;
           sqlstm.arrsiz = 3;
           sqlstm.iters = (unsigned int  )1;
           sqlstm.offset = (unsigned int  )188;
           sqlstm.cud = sqlcud0;
           sqlstm.sqlest = (unsigned char  *)&sqlca;
           sqlstm.sqlety = (unsigned short)0;
           sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 273 "depuaux.pc"

           exit(1);
       }
 
       /* EXEC SQL
           COMMIT; */ 
#line 278 "depuaux.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 3;
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )202;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 278 "depuaux.pc"

 
       /* EXEC SQL 
           FETCH cursor_term
              INTO :nro_caj; */ 
#line 282 "depuaux.pc"

{
       struct sqlexd sqlstm;

       sqlstm.sqlvsn = 8;
       sqlstm.arrsiz = 3;
       sqlstm.iters = (unsigned int  )1;
       sqlstm.offset = (unsigned int  )216;
       sqlstm.cud = sqlcud0;
       sqlstm.sqlest = (unsigned char  *)&sqlca;
       sqlstm.sqlety = (unsigned short)0;
       sqlstm.sqhstv[0] = (unsigned char  *)&nro_caj;
       sqlstm.sqhstl[0] = (unsigned int  )7;
       sqlstm.sqindv[0] = (         short *)0;
       sqlstm.sqharm[0] = (unsigned int  )0;
       sqlstm.sqphsv = sqlstm.sqhstv;
       sqlstm.sqphsl = sqlstm.sqhstl;
       sqlstm.sqpind = sqlstm.sqindv;
       sqlstm.sqparm = sqlstm.sqharm;
       sqlstm.sqparc = sqlstm.sqharc;
       sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 282 "depuaux.pc"

     
       varcpy2(caj, nro_caj);

       code = sqlca.sqlcode;

       hIni=time(&hIni);
       fprintf(fd, "finalizando caja: %s", asctime(localtime(&hIni)));

    } /* Fin del while ppal. */

    /* EXEC SQL
        CLOSE cursor_term; */ 
#line 294 "depuaux.pc"

{
    struct sqlexd sqlstm;

    sqlstm.sqlvsn = 8;
    sqlstm.arrsiz = 3;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )234;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)0;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 294 "depuaux.pc"

    
    hIni=time(&hIni);
    fprintf(fd, "\nDepuracion Finalizada %s\n", asctime(localtime(&hIni)));
    close (fd);  
    return OK;

}
