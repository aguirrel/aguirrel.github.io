
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
    "abierto.pc"
};


static unsigned long sqlctx = 68947;


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
   unsigned char  *sqhstv[4];
   unsigned int   sqhstl[4];
            short *sqindv[4];
   unsigned int   sqharm[4];
   unsigned int   *sqharc[4];
} sqlstm = {8,4};

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
"select nro_ca ,descrip  from cred_cas            ";
typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static const short sqlcud0[] =
{8,4130,
2,0,0,1,0,0,539,37,0,3,3,0,1,0,1,9,0,0,1,9,0,0,1,10,0,0,
28,0,0,2,49,0,521,82,0,0,0,0,1,0,
42,0,0,2,0,0,525,85,0,2,0,0,1,0,2,9,0,0,2,9,0,0,
64,0,0,3,152,0,516,92,0,4,2,0,1,0,2,3,0,0,2,3,0,0,1,9,0,0,1,9,0,0,
94,0,0,2,0,0,525,103,0,2,0,0,1,0,2,9,0,0,2,9,0,0,
116,0,0,2,0,0,527,110,0,0,0,0,1,0,
};


#line 1 "abierto.pc"
#ifndef OK
  #define OK    0
  #define NOOK  -1
#endif

#define USERID_LEN   20
#define PASSWD_LEN   20

extern int	SqlErrno;

typedef struct
{
    char	userid[USERID_LEN+1];
    char	passwd[PASSWD_LEN+1];
} str_login;

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

#line 18 "abierto.pc"

/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 19 "abierto.pc"

    /* VARCHAR	userid[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } userid;
#line 20 "abierto.pc"

    /* VARCHAR	passwd[20]; */ 
struct { unsigned short len; unsigned char arr[20]; } passwd;
#line 21 "abierto.pc"

    /* VARCHAR     descrip[30]; */ 
struct { unsigned short len; unsigned char arr[30]; } descrip;
#line 22 "abierto.pc"

    /* VARCHAR     nro_ca[2]; */ 
struct { unsigned short len; unsigned char arr[2]; } nro_ca;
#line 23 "abierto.pc"

    /* VARCHAR     fecha[4]; */ 
struct { unsigned short len; unsigned char arr[4]; } fecha;
#line 24 "abierto.pc"

/* EXEC SQL END DECLARE SECTION; */ 
#line 25 "abierto.pc"



int open_db(str_login *data)
{
int SqlErrno;

    strcpy(userid.arr, data->userid);
    userid.len=strlen(data->userid);
    strcpy(passwd.arr, data->passwd);
    passwd.len=strlen(data->passwd);

    /* EXEC SQL
	CONNECT :userid IDENTIFIED BY :passwd; */ 
#line 38 "abierto.pc"

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

#line 38 "abierto.pc"

    if (sqlca.sqlcode!=OK)
    {
	SqlErrno=sqlca.sqlcode;
        printf("Error al conectarse a la Base de Datos\n");
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


void main(int argc,char *argv[])
{

int cont;
long monto;
  

    if (argc<2)
       {
       printf("Uso: %s <mmdd>\n",argv[0]);
       exit(0);
       }

    init();

       strcpy((char *)fecha.arr,argv[1]);
       fecha.len=strlen((char *)fecha.arr);

    /* EXEC SQL
        DECLARE cursor_ca CURSOR FOR
           SELECT nro_ca, descrip
           FROM cred_cas; */ 
#line 80 "abierto.pc"

        
    /* EXEC SQL
        OPEN cursor_ca; */ 
#line 83 "abierto.pc"

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

#line 83 "abierto.pc"


    /* EXEC SQL 
        FETCH cursor_ca
        INTO :nro_ca, :descrip; */ 
#line 87 "abierto.pc"

{
    struct sqlexd sqlstm;

    sqlstm.sqlvsn = 8;
    sqlstm.arrsiz = 3;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )42;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)0;
    sqlstm.sqhstv[0] = (unsigned char  *)&nro_ca;
    sqlstm.sqhstl[0] = (unsigned int  )4;
    sqlstm.sqindv[0] = (         short *)0;
    sqlstm.sqharm[0] = (unsigned int  )0;
    sqlstm.sqhstv[1] = (unsigned char  *)&descrip;
    sqlstm.sqhstl[1] = (unsigned int  )32;
    sqlstm.sqindv[1] = (         short *)0;
    sqlstm.sqharm[1] = (unsigned int  )0;
    sqlstm.sqphsv = sqlstm.sqhstv;
    sqlstm.sqphsl = sqlstm.sqhstl;
    sqlstm.sqpind = sqlstm.sqindv;
    sqlstm.sqparm = sqlstm.sqharm;
    sqlstm.sqparc = sqlstm.sqharc;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 87 "abierto.pc"


    while (sqlca.sqlcode==0)
    {
    
        /* EXEC SQL
            SELECT count(*),sum(decode(cod_trx,'00',monto,-monto))
            INTO :cont,:monto
            FROM cred_trx
            WHERE nro_ca=:nro_ca 
                  AND cod_est='02'
                  AND substr(fec_loc,1,4)=:fecha; */ 
#line 98 "abierto.pc"

{
        struct sqlexd sqlstm;

        sqlstm.sqlvsn = 8;
        sqlstm.arrsiz = 4;
        sqlstm.stmt = "select count(*)  ,sum(decode(cod_trx,'00',monto,(-mon\
to))) into :b0,:b1  from cred_trx where ((nro_ca=:b2 and cod_est='02') and sub\
str(fec_loc,1,4)=:b3)";
        sqlstm.iters = (unsigned int  )1;
        sqlstm.offset = (unsigned int  )64;
        sqlstm.selerr = (unsigned short)0;
        sqlstm.cud = sqlcud0;
        sqlstm.sqlest = (unsigned char  *)&sqlca;
        sqlstm.sqlety = (unsigned short)0;
        sqlstm.sqhstv[0] = (unsigned char  *)&cont;
        sqlstm.sqhstl[0] = (unsigned int  )4;
        sqlstm.sqindv[0] = (         short *)0;
        sqlstm.sqharm[0] = (unsigned int  )0;
        sqlstm.sqhstv[1] = (unsigned char  *)&monto;
        sqlstm.sqhstl[1] = (unsigned int  )4;
        sqlstm.sqindv[1] = (         short *)0;
        sqlstm.sqharm[1] = (unsigned int  )0;
        sqlstm.sqhstv[2] = (unsigned char  *)&nro_ca;
        sqlstm.sqhstl[2] = (unsigned int  )4;
        sqlstm.sqindv[2] = (         short *)0;
        sqlstm.sqharm[2] = (unsigned int  )0;
        sqlstm.sqhstv[3] = (unsigned char  *)&fecha;
        sqlstm.sqhstl[3] = (unsigned int  )6;
        sqlstm.sqindv[3] = (         short *)0;
        sqlstm.sqharm[3] = (unsigned int  )0;
        sqlstm.sqphsv = sqlstm.sqhstv;
        sqlstm.sqphsl = sqlstm.sqhstl;
        sqlstm.sqpind = sqlstm.sqindv;
        sqlstm.sqparm = sqlstm.sqharm;
        sqlstm.sqparc = sqlstm.sqharc;
        sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 98 "abierto.pc"



         printf("\t%.15s Trx abiertas  %8d    por %10.2lf\n", descrip.arr, cont,(long double)monto/100);

         /* EXEC SQL 
             FETCH cursor_ca
             INTO :nro_ca, :descrip; */ 
#line 105 "abierto.pc"

{
         struct sqlexd sqlstm;

         sqlstm.sqlvsn = 8;
         sqlstm.arrsiz = 4;
         sqlstm.iters = (unsigned int  )1;
         sqlstm.offset = (unsigned int  )94;
         sqlstm.cud = sqlcud0;
         sqlstm.sqlest = (unsigned char  *)&sqlca;
         sqlstm.sqlety = (unsigned short)0;
         sqlstm.sqhstv[0] = (unsigned char  *)&nro_ca;
         sqlstm.sqhstl[0] = (unsigned int  )4;
         sqlstm.sqindv[0] = (         short *)0;
         sqlstm.sqharm[0] = (unsigned int  )0;
         sqlstm.sqhstv[1] = (unsigned char  *)&descrip;
         sqlstm.sqhstl[1] = (unsigned int  )32;
         sqlstm.sqindv[1] = (         short *)0;
         sqlstm.sqharm[1] = (unsigned int  )0;
         sqlstm.sqphsv = sqlstm.sqhstv;
         sqlstm.sqphsl = sqlstm.sqhstl;
         sqlstm.sqpind = sqlstm.sqindv;
         sqlstm.sqparm = sqlstm.sqharm;
         sqlstm.sqparc = sqlstm.sqharc;
         sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 105 "abierto.pc"


     }
         printf("\n");

     /* EXEC SQL
         CLOSE cursor_ca; */ 
#line 111 "abierto.pc"

{
     struct sqlexd sqlstm;

     sqlstm.sqlvsn = 8;
     sqlstm.arrsiz = 4;
     sqlstm.iters = (unsigned int  )1;
     sqlstm.offset = (unsigned int  )116;
     sqlstm.cud = sqlcud0;
     sqlstm.sqlest = (unsigned char  *)&sqlca;
     sqlstm.sqlety = (unsigned short)0;
     sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}

#line 111 "abierto.pc"

 
}
