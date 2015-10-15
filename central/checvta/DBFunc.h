#ifdef OS_Linux
#pragma pack(1)
#endif

#define	OK		0
#define NOOK	-1
#define NOF     -2

typedef struct
{
   char nro_suc[4];
   char nro_caj_sf[6];
   char dir_vta[60]; 
   char caj_dev[6];
   char nro_caj_fic[6];
   long tot_cre;
   long tot_vta;
   long cant_vta;
   int  max_pendi;
   int  cant_pendi;
   char szFechaProc[9];
   char min_fec_trx[9];
   char max_fec_trx[9];
} str_datos_suc;

typedef struct 
{
	char	nro_ca[3];
	char	nro_suc[4];
	char	nro_caj_sf[6];
	char	nro_caj[6];
	char	nro_trx[5];
	char	cod_tar[3];
	char	nro_tar[21];
	char	cod_aut[7];
	char	monto[15];
	char	fec_trx[7];
	char	cod_trx[3];
	char	plan_sf[3];
	char	nro_cuo[4];
	char	hora[5];
	char	rowidvt[30];
	char    rowidcr[30];
	char 	fec_vtacard[30];
	char	nro_lot[4];
	char	cod_pro[7];
	char 	fec_loc[11];
	char	nro_caj_ca[9];
	char	nro_com[16];
	char	plan_iso[2];
	char	cod_mon[4];
	char	cod_est[3];
	char	nro_trc[7];
	char	fec_ven[5];
	char	nro_tic_ori[5]; 
	char	mod_env[4];
	char	mod_ing[3];
	char	fec_ori[11];
	char	nro_tic[5];
	char	nro_ref[13];
	char	nro_caj_ori[6];
	int		flag;
	char	nro_caj_fic[6];
} str_data_trx;

typedef struct
{
	double Monto;
	char   Fecha[7];
	char   Caja;
} str_logueo;

#define varcpy(X,Y)             { strncpy(X.arr,Y , sizeof(X.arr));  \
				  X.len=(sizeof(X.arr) < strlen(Y) ? \
                                  sizeof(X.arr) : strlen(Y)); }
#define varcpy2(X,Y)            { strncpy(X, Y.arr, Y.len);\
				  X[Y.len]=0; }
int SqlErrno;

int DBConnect();
int DBObtFecha(char *fecing, char *fecsal);
int DBCargaTablaVta(str_datos_suc *datos_suc, char* fecpro);
int DBObtDatosSuc(str_datos_suc *datos_suc);
int DBSumVtaCard(str_datos_suc *datos_suc);
int DBSumCredito(str_datos_suc *datos_suc, char *fec_pro);
int DBOpenCurVta();
int DBObtNextVta(str_data_trx *data_trx);
int DBVerifCred(str_data_trx *data_trx, str_datos_suc *datos_suc, str_logueo *datos_log);
int DBIncCred(str_data_trx *data_trx);
int DBCompTrx(str_data_trx *data_trx, str_datos_suc *datos_suc);
int DBUpdRegistroCredito(str_data_trx *data_trx);
int DBCommit();
int DBSumCreFecVta(str_datos_suc *datos_suc, char *fecpro);
int DBTruncate();
int DBCloseCurVta();
int DBActFechaConc (str_datos_suc *datos_suc, char *fecpro); 
