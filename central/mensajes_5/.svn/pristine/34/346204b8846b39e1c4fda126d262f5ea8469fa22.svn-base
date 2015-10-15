#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef H8583_H
#define H8583_H

class Iso_Msg;

#include "Price.h"
#include "Data.h"

#ifdef TRUE
#undef  TRUE
#endif
#ifdef  FALSE
#undef  FALSE
#endif
#define TRUE	1==1
#define FALSE	0==1

#define	TYPE_VISA_VENTA					2000	/* 000000 */
#define	TYPE_VISA_VENTA_RTA				2100
#define	TYPE_VISA_DEVOL					2001	/* 200000 */ 
#define	TYPE_VISA_DEVOL_RTA				2101
#define	TYPE_VISA_ANUL_VENTA			2002	/* 020000 */
#define	TYPE_VISA_ANUL_VENTA_RTA		2102
#define	TYPE_VISA_ANUL_DEVOL			2003	/* 220000 */
#define	TYPE_VISA_ANUL_DEVOL_RTA		2103

#define	TYPE_VISA_VENTA_LOCAL			2200	/* 000000 */
#define	TYPE_VISA_VENTA_LOCAL_RTA		2300
#define	TYPE_VISA_DEVOL_LOCAL			2201	/* 200000 */
#define	TYPE_VISA_DEVOL_LOCAL_RTA		2301
#define	TYPE_VISA_ANUL_VENTA_LOCAL		2202	/* 020000 */
#define	TYPE_VISA_ANUL_VENTA_LOCAL_RTA	2302
#define	TYPE_VISA_ANUL_DEVOL_LOCAL		2203	/* 220000 */
#define	TYPE_VISA_ANUL_DEVOL_LOCAL_RTA	2303

#define TYPE_VISA_REV_VENTA				4000	/* 000000 */
#define TYPE_VISA_REV_VENTA_RTA			4100
#define TYPE_VISA_REV_DEVOL				4001	/* 200000 */
#define TYPE_VISA_REV_DEVOL_RTA			4101
#define TYPE_VISA_REV_ANUL_VENTA		4002	/* 020000 */
#define TYPE_VISA_REV_ANUL_VENTA_RTA	4102
#define TYPE_VISA_REV_ANUL_DEVOL		4003	/* 220000 */
#define TYPE_VISA_REV_ANUL_DEVOL_RTA	4103

#define TYPE_VISA_BATCH					3200
#define TYPE_VISA_BATCH_RTA				3300
#define	TYPE_VISA_CIERRE				5000	/* 920000 */
#define	TYPE_VISA_CIERRE_RTA			5100
#define TYPE_VISA_CIERRE_B				5001    /* 960000 */
#define	TYPE_VISA_CIERRE_B_RTA			5101

#define	TYPE_VISA_TEST					8000	/* 990000 */
#define	TYPE_VISA_TEST_RTA				8100

#define OPCODE_VISA_VENTA				0
#define OPCODE_VISA_DEVOL				200000
#define OPCODE_VISA_ANUL_VENTA			20000
#define OPCODE_VISA_ANUL_DEVOL			220000

#define OPCODE_VISA_CIERRE				920000
#define OPCODE_VISA_CIERRE_B			960000

#define OPCODE_VISA_TEST				990000

#define OPCODE_VISA_VTA_CA_P			1000	/* Venta Posnet comun CA-$    */	
#define OPCODE_VISA_VTA_CC_P			2000	/* Venta Posnet comun CC-$    */	
#define OPCODE_VISA_VTA_CA_D			8000	/* Venta Posnet comun CA-D    */		
#define OPCODE_VISA_VTA_CC_D			9000	/* Venta Posnet comun CC-D    */

#define OPCODE_VISA_DEV_CA_P			201000	/* Devol Posnet comun CA-$    */
#define OPCODE_VISA_DEV_CC_P			202000	/* Devol Posnet comun CC-$    */
#define OPCODE_VISA_DEV_CA_D			208000	/* Devol Posnet comun CA-D    */
#define OPCODE_VISA_DEV_CC_D			209000	/* Devol Posnet comun CC-D    */

#define OPCODE_CASHB_POS_VTA_CA_P		91000	/* Venta cashback Posnet CA-$ */
#define OPCODE_CASHB_POS_VTA_CC_P		92000	/* Venta cashback Posnet CC-$ */		
#define OPCODE_CASHB_POS_VTA_CA_D		98000	/* Venta cashback Posnet CA-D */			
#define OPCODE_CASHB_POS_VTA_CC_D		99000	/* Venta cashback Posnet CC-D */

#define OPCODE_CASHB_POS_ANU_CA_P		211000	/* Anula cashback Posnet CA-$ */
#define OPCODE_CASHB_POS_ANU_CC_P		212000	/* Anula cashback Posnet CC-$ */
#define OPCODE_CASHB_POS_ANU_CA_D		218000	/* Anula cashback Posnet CA-D */
#define OPCODE_CASHB_POS_ANU_CC_D		219000	/* Anula cashback Posnet CC-D */

#define OPCODE_CASHB_VISA_VENTA_P		90000	/* Venta cashback VISA        */
#define OPCODE_CASHB_VISA_ANULA_P		140000	/* Anula cashback VISA        */

#define OPCODE_VISA_ANUL_VTA_CA_P		21000	/* Anula Posnet comun CA-$    */
#define OPCODE_VISA_ANUL_VTA_CC_P		22000	/* Anula Posnet comun CC-$    */
#define OPCODE_VISA_ANUL_VTA_CA_D		28000	/* Anula Posnet comun CA-D    */
#define OPCODE_VISA_ANUL_VTA_CC_D		29000	/* Anula Posnet comun CC-D    */

#define OPCODE_VISA_ANUL_DEV_CA_P		221000
#define OPCODE_VISA_ANUL_DEV_CC_P		222000
#define OPCODE_VISA_ANUL_DEV_CA_D		228000
#define OPCODE_VISA_ANUL_DEV_CC_D		229000

/* Pago de resumen */
#define OPCODE_VISA_PAGO_RESUMEN		    10	/* Pago resumen */
#define OPCODE_VISA_DEVO_RESUMEN		200010	/* Devolucion resumen */
#define OPCODE_VISA_ANUP_RESUMEN		 20010	/* Anulacion de pago resumen */
#define OPCODE_VISA_ANUD_RESUMEN		220010	/* Anulacion de devolucion resumen */

#define OPCODE_VISA_VEN_CASHBACK		 90000  /* VISA Transaccion de venta + cashback */
#define OPCODE_VISA_ANU_CASHBACK		140000  /* VISA Anulacion de venta + cashback */

#define OPCODE_POSN_VEN_CASHBACK		 90000  /* POSNET Transaccion de venta + cashback */
#define OPCODE_POSN_ANU_CASHBACK		210000  /* POSNET Anulacion de venta + cashback */

#define PINPAD_CA_N						0		/* Cuenta por defecto nula  */
#define	PINPAD_CA_P						1		/* Caja de ahorros pesos    */
#define	PINPAD_CC_P						2		/* Cuenta corriente pesos   */
#define	PINPAD_CA_D						8		/* Caja de ahorro dolares   */
#define	PINPAD_CC_D						9		/* Cuenta corriente dolares */

#define LVAR				1
#define	LFIX				2


#define TASCII				1
#define TBCD				2
#define FBCD				3

typedef struct
{
   int	bit;
   int	type;
   int	len;
   int  format;
} str_iso_fields;

typedef struct
{
   int 	type;
   long	min1;
   long	min2;
   long max1;
   long max2;
} str_fields_mask;

class Iso_Msg
{
protected:
   //  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   //  Ojo no se pueden separar los siguientes 5 campos debido a la 
   // Implemenacion de las rutinas de base..

   int		type;
   long		bitmap1;
   long		bitmap2;
   char		data[2000];
   int		data_len;
   char		szNroCA[3];						/* ATENCION: NO USARLO EN LOS CONSTRUCTORES    */
  
   int GetFormat(int nro);					/* Devuelve el formato del registro ASCII/BCD  */
   int GetType(int nro);					/* Devuelve el tipo de registro LFIX o LVAR    */
   int GetLen(int nro);						/* Devuelve el largo del registro	           */
   int GetLenVar(int nro);
   int GetFieldOfs(int nro);				/* Devuelve el offset del registro	           */
   int GetNextFieldOfs(int nro);

public:
   Iso_Msg();								/* Constructores		                       */
   Iso_Msg(int msg_type);
   Iso_Msg(char *msg_string, int msg_len);
   int PutField(int nro, char *data);		/* Agrega/modifica un registro	               */
   char *GetField(int nro);					/* Devuelve el contenido de un reg             */
   int ChkField(int nro);					/* Verifica si esta o no un reg.               */
   int ChkValid(int nro);					/* Verifica si es un registro para ese mensaje */
   int ChkReady();							/* Verifica si estan los registros mandatorios */
   int GetMsgType();						/* Devuelve el tipo de mensaje	               */
   void SetMsgType(int iTipo);				/* Setea el tipo de mensaje                    */
   int GetMsgString(char *msg_string);		/* Devuelve el msj. armado (ISO)               */
   int GetDBData(BatchData& batch_data);
   int GetDBData(TrxData& trx_data);
   int Aprob();								/* Devuelve si esta aprobado o no              */
   operator Price_Msg();
   void SetNroCA8583(char *szParamCA);
   int EsAnulacionPinpad();
};

#endif
