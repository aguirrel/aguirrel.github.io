#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef PRICE_H
#define PRICE_H

class Price_Msg;

#define MAX_CAMPOS_PRICE 50 /* DS29052001 */
#define MAX_FUNC_CONV 10
#define MAX_LONG 80 /* DS29052001 */ 

#define VER1	1
#define VER2	2
#define VER22	3

#include "Includes.h"
#include "8583.h"
#include "Data.h"
#include "Config.h"

#ifndef OK
#define OK			0
#define NOOK		-1
#endif

#define TYPE_PRICE_VENTA			1200
#define TYPE_PRICE_VENTA_RTA		1210
#define TYPE_PRICE_VENTA_LOCAL 		1220
#define TYPE_PRICE_VENTA_LOCAL_BIS	1221
#define TYPE_PRICE_VENTA_LOCAL_RTA	1230
#define TYPE_PRICE_ANUL_VENTA		1420
#define TYPE_PRICE_ANUL_VENTA_BIS	1421
#define TYPE_PRICE_ANUL_VENTA_RTA	1430

typedef  int (Price_Msg::*TFunConv)();
typedef struct SFunConv
{
    TFunConv Func;
};

typedef struct
{
   int campo;
   int longi;
} str_campo;



typedef struct
{
   int  msg_type;
   int  field;
   int  type;
} str_price_fields;

typedef struct
{
   long         orig_pid;
   char         header[27];
   char         plan_sf[3];
   char         nro_term[5];
   char         nro_trans[4];
   char         cod_tar[2];
   char         cod_pla[2];
   char         nro_cuo[3];
   char         nro_caj_ca[9];
   char		    anul_onl[2]; /* Anulacion online */
   char		    plan_iso[2]; 
   char			nro_com[16];
   char			trans_ori[5];
   char			fecha_ori[7];
   char	    	nro_tar[20];
   char         rowid[50+1]; /* rowid de trx devolucion original */
} str_heap_data;


typedef struct
{
   int version;
   int  tipo_msg;
   int CanCampos;
   str_campo order_campos[MAX_CAMPOS_PRICE];
} str_form_price;


static str_form_price FormCamposPrice[MAX_CAMPOS_PRICE] =
{ 
{VER22, TYPE_PRICE_VENTA,31,{{1,27},{2,4},{3,4},{4,19},{5,2},{6,12},{7,12},
                           {8,4},{9,1},{10,37},{11,4},{12,3},{13,2},{14,3},
                           {15,3},{16,5},{17,2},{18,8},{22,2},{23,3},{24,1},
                           {20,6},{25,4},{26,3},{27,5},{28,4},{21,2},
			   {29,9},{30,76},{31,1},{32,12}}}, /* Pinpad2. Agregado {31,1} */

 {VER22, TYPE_PRICE_VENTA_RTA,18,{{1,27},{2,3},{3,5},{4,4},{5,4},{6,2},{7,19},
                           {8,2},{9,6},{10,6},{11,15},{12,25},{15,2},{16,12},
				{13,18},{14,25},{17,28},{18,40}}}, /* Modificacion PINPAD ( {13,20} -> {13,18} ) */

 {VER22, TYPE_PRICE_VENTA_LOCAL,29,{{1,27},{2,4},{3,4},{4,19},{5,2},{6,12},
                           {7,12},{8,4},{9,1},{10,37},{11,4},{12,3},{13,2},
			   {14,3},{15,3},{16,5},{17,2},{18,6},{19,2},{24,8},
			   {25,2},{26,3},{27,1},{22,6},{23,3},
			   {29,9},{30,76},{32,5},{33,4}}}, 
 {VER22, TYPE_PRICE_VENTA_LOCAL_BIS,29,{{1,27},{2,4},{3,4},{4,19},{5,2},{6,12},
                           {7,12},{8,4},{9,1},{10,37},{11,4},{12,3},{13,2},
			   {14,3},{15,3},{16,5},{17,2},{18,6},{19,2},{24,8},
			   {25,2},{26,3},{27,1},{22,6},{23,3},
			   {29,9},{30,76},{32,5},{33,4}}}, 
 {VER22, TYPE_PRICE_VENTA_LOCAL_RTA,13,{{1,27},{2,3},{3,5},{4,4},{5,4},{6,2},
                           {7,6},{8,19},{9,2},{10,15},{11,25},{12,60},{13,3}}},
{VER22, TYPE_PRICE_ANUL_VENTA,24,{{1,27},{2,4},{3,4},{4,19},{5,2},{6,12},{7,12} /* Modificacion PINPAD ( Cant.Campos 22 -> 23 ) */
		           ,{8,3},{9,4},{10,3},{11,2},{12,3},{13,5},{14,2},
                           {15,4},{16,4},{17,12},{18,3},{19,5},{20,42},{21,3},
			   {29,9},{30,8},{31,1}}}, /* Modificacion PINPAD ( Agregado {30,8} ) */ /* Pinpad2. Agregado {31,1} */
 {VER22, TYPE_PRICE_ANUL_VENTA_BIS,24,{{1,27},{2,4},{3,4},{4,19},{5,2},{6,12}, /* Modificacion PINPAD ( Cant.Campos 22 -> 23 ) */
                          {7,12},{8,3},{9,4},{10,3},{11,2},{12,3},{13,5},{14,2},
                          {15,4},{16,4},{17,12},{18,3},{19,5},{20,42},{21,3},
			  {29,9},{30,8},{31,1}}}, /* Modificacion PINPAD ( Agregado {30,8} ) */ /* Pinpad2. Agregado {31,1} */
 {VER22, TYPE_PRICE_ANUL_VENTA_RTA,13,{{1,27},{2,3},{3,5},{4,4},{5,4},{6,2},
                          {7,6},{8,19},{9,2},{10,15},{11,25},{12,60},{13,3}}},
 {0,0}
};

typedef struct 
{
   char data[500];
} str_price_msg;

class Price_Msg
{
protected:
    int		type;
    int		version;
    int		UseCodSeg;
	char	szNroCA[3];							/* ATENCION: NO USARLO EN LOS CONSTRUCTORES */
    int		UseTrackIyCvc;  /* DS29052001 */
    char	Buffer[MAX_CAMPOS_PRICE][MAX_LONG];
    SFunConv    VFuncConv[MAX_FUNC_CONV];

private: 
    int ObtTipoMsg(char* data); 
    int ObtTipoRta(char* data); 
    int ValidoTrack2(char *data); 
    int ValidoTrack1(char *data);
    char *ObtTarTrk2(char *);
    char *GetCodSeg();
    int CargaFunCon(); 
    int FunConUni();

public:
    Price_Msg();
    Price_Msg(int msg_type);
    Price_Msg(str_price_msg data);

    int PutField(int nro,char *data);
	int NewPutField(int nro, char *data, int len);

    char *GetField(int nro);
    char *GetNroTar();
    int GetMsgString(char *msg_string);
    int GetHeapData(str_heap_data *heap_data);
    int PutHeapData(str_heap_data heap_data);
    int GetDBData(TrxData& trx_data);
    char *GetTerminal();
    char *GetTransaccion();
	char *GetSucursal();
    char *GetEmpresa();
    char *GetCodTar();
    char *ObtCuotas();
    char *ObtPlanSf();
    void SetCodSeg();
    void SetNoCodSeg();
    void InvertirFecha();
    int ConvMsgPrice(int i); 
    operator Iso_Msg();
	/* Modificacion PINPAD */
	void GetPinPad(char *szPinPad);
	int UsePinPad();
	void SetNroCAPrice(char*);
};

#endif
