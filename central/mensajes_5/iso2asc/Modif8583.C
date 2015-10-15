#ifdef OS_Linux
#pragma pack(1)
#endif

/************************************************************************/
/*									*/
/* 8583.C	    : Definicion de la clase 8583 			*/
/*									*/
/* Proyecto	    : Autorizacion de tarjetas de credito - StoreFlow.	*/
/* Cliente	    : Coto						*/
/* Autor	    : Lalo Steinmann					*/
/* Fecha	    : 17/02/95						*/
/* Compilador	    : C++						*/
/* Base de datos    : Oracle						*/
/*									*/
/************************************************************************/

#include "Modif8583.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define	BYTE_1			0x18
#define BYTE_2			0x10
#define	BYTE_3			0x08
#define BYTE_4			0x00

#define MASK_1			0xFF000000L
#define MASK_2			0x00FF0000L
#define MASK_3			0x0000FF00L
#define MASK_4			0x000000FFL

extern int	SqlErrno;

str_descripcion_tipo_mensaje iso_desc_tipo[]=
{
	{ TYPE_VISA_VENTA,                "000000", "0200 Requerimiento de compra on-line" },

	{ TYPE_VISA_DEVOL,                "201000", "0200 Req devolución on-line (Pinpad CA-$)" },
	{ TYPE_VISA_DEVOL,                "202000", "0200 Req devolución on-line (Pinpad CC-$)" },
	{ TYPE_VISA_DEVOL,                "208000", "0200 Req devolución on-line (Pinpad CA-U$S)" },
	{ TYPE_VISA_DEVOL,                "209000", "0200 Req devolución on-line (Pinpad CC-U$S)" },

	{ TYPE_VISA_DEVOL_RTA,            "201000", "0210 Rta devolucion on-line (Pinpad CA-$)" },
	{ TYPE_VISA_DEVOL_RTA,            "202000", "0210 Rta devolucion on-line (Pinpad CC-$)" },
	{ TYPE_VISA_DEVOL_RTA,            "208000", "0210 Rta devolucion on-line (Pinpad CA-U$S)" },
	{ TYPE_VISA_DEVOL_RTA,            "209000", "0210 Rta devolucion on-line (Pinpad CC-U$S)" },

	/* Requerimiento compra Pinpad */
	{ TYPE_VISA_VENTA,                "001000", "0200 Req compra on-line (Pinpad CA-$)" },
	{ TYPE_VISA_VENTA,                "002000", "0200 Req compra on-line (Pinpad CC-$)" },
	{ TYPE_VISA_VENTA,                "008000", "0200 Req compra on-line (Pinpad CA-U$S)" },
	{ TYPE_VISA_VENTA,                "009000", "0200 Req compra on-line (Pinpad CC-U$S)" },
	/* Respuesta compra Pinpad */
	{ TYPE_VISA_VENTA_RTA,            "001000", "0210 Rta compra on-line (Pinpad CA-$)" },
	{ TYPE_VISA_VENTA_RTA,            "002000", "0210 Rta compra on-line (Pinpad CC-$)" },
	{ TYPE_VISA_VENTA_RTA,            "008000", "0210 Rta compra on-line (Pinpad CA-U$S)" },
	{ TYPE_VISA_VENTA_RTA,            "009000", "0210 Rta compra on-line (Pinpad CC-U$S)" },
	/* Requerimiento compra CashBack */
	{ TYPE_VISA_VENTA,                "091000", "0200 Req compra on-line (CashBack P CA-$)" },
	{ TYPE_VISA_VENTA,                "092000", "0200 Req compra on-line (CashBack P CC-$)" },
	{ TYPE_VISA_VENTA,                "098000", "0200 Req compra on-line (CashBack P CA-U$S)" },
	{ TYPE_VISA_VENTA,                "099000", "0200 Req compra on-line (CashBack P CC-U$S)" },
	{ TYPE_VISA_VENTA,                "090000", "0200 Req compra on-line (CashBack V)" },
	/* Respuesta compra CashBack */
	{ TYPE_VISA_VENTA_RTA,            "091000", "0210 Rta compra on-line (CashBack P CA-$)" },
	{ TYPE_VISA_VENTA_RTA,            "092000", "0210 Rta compra on-line (CashBack P CC-$)" },
	{ TYPE_VISA_VENTA_RTA,            "098000", "0210 Rta compra on-line (CashBack P CA-U$S)" },
	{ TYPE_VISA_VENTA_RTA,            "099000", "0210 Rta compra on-line (CashBack P CC-U$S)" },
	{ TYPE_VISA_VENTA_RTA,            "090000", "0210 Rta compra on-line (CashBack V)" },
	/* Requerimiento anulacion CashBack */
	{ TYPE_VISA_ANUL_VENTA,           "211000", "0200 Req anulación compra on-line (CashBack P CA-$)" },
	{ TYPE_VISA_ANUL_VENTA,           "212000", "0200 Req anulación compra on-line (CashBack P CC-$)" },
	{ TYPE_VISA_ANUL_VENTA,           "218000", "0200 Req anulación compra on-line (CashBack P CA-U$S)" },
	{ TYPE_VISA_ANUL_VENTA,           "219000", "0200 Req anulación compra on-line (CashBack P CC-U$S)" },
	{ TYPE_VISA_ANUL_VENTA,           "140000", "0200 Req anulación compra on-line (CashBack V)" },
	/* Respuesta anulacion CashBack */
	{ TYPE_VISA_ANUL_VENTA_RTA,       "211000", "0210 Rta anulación compra on-line (CashBack P CA-$)" },
	{ TYPE_VISA_ANUL_VENTA_RTA,       "212000", "0210 Rta anulación compra on-line (CashBack P CC-$)" },
	{ TYPE_VISA_ANUL_VENTA_RTA,       "218000", "0210 Rta anulación compra on-line (CashBack P CA-U$S)" },
	{ TYPE_VISA_ANUL_VENTA_RTA,       "219000", "0210 Rta anulación compra on-line (CashBack P CC-U$S)" },
	{ TYPE_VISA_ANUL_VENTA_RTA,       "140000", "0210 Rta anulación compra on-line (CashBack V)" },
	/* Requerimiento reverso CashBack */
	{ TYPE_VISA_REV_VENTA,            "091000", "0400 Req reverso venta (CashBack P CA-$)" },
	{ TYPE_VISA_REV_VENTA,            "092000", "0400 Req reverso venta (CashBack P CC-$)" },
	{ TYPE_VISA_REV_VENTA,            "098000", "0400 Req reverso venta (CashBack P CA-U$S)" },
	{ TYPE_VISA_REV_VENTA,            "099000", "0400 Req reverso venta (CashBack P CC-U$S)" },
	{ TYPE_VISA_REV_VENTA,            "090000", "0400 Req reverso venta (CashBack V)" },
	/* Respuesta reverso CashBack */
	{ TYPE_VISA_REV_VENTA_RTA,        "091000", "0410 Rta reverso venta (CashBack P CA-$)" },
	{ TYPE_VISA_REV_VENTA_RTA,        "092000", "0410 Rta reverso venta (CashBack P CC-$)" },
	{ TYPE_VISA_REV_VENTA_RTA,        "098000", "0410 Rta reverso venta (CashBack P CA-U$S)" },
	{ TYPE_VISA_REV_VENTA_RTA,        "099000", "0410 Rta reverso venta (CashBack P CC-U$S)" },
	{ TYPE_VISA_REV_VENTA_RTA,        "090000", "0410 Rta reverso venta (CashBack V)" },
	/* Requerimiento reverso anulacion CashBack */
	{ TYPE_VISA_REV_ANUL_VENTA,       "211000", "0400 Req reverso anulación venta (CashBack P CA-$)" },
	{ TYPE_VISA_REV_ANUL_VENTA,       "212000", "0400 Req reverso anulación venta (CashBack P CC-$)" },
	{ TYPE_VISA_REV_ANUL_VENTA,       "218000", "0400 Req reverso anulación venta (CashBack P CA-U$S)" },
	{ TYPE_VISA_REV_ANUL_VENTA,       "219000", "0400 Req reverso anulación venta (CashBack P CC-U$S)" },
	{ TYPE_VISA_REV_ANUL_VENTA,       "140000", "0400 Req reverso anulación venta (CashBack V)" },
	/* Respuesta reverso anulacion CashBack */
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,   "211000", "0410 Rta reverso anulación venta (CashBack P CA-$)" },
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,   "212000", "0410 Rta reverso anulación venta (CashBack P CC-$)" },
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,   "218000", "0410 Rta reverso anulación venta (CashBack P CA-U$S)" },
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,   "219000", "0410 Rta reverso anulación venta (CashBack P CC-U$S)" },
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,   "140000", "0410 Rta reverso anulación venta (CashBack V)" },

	{ TYPE_VISA_DEVOL,                "200000", "0200 Req devolución on-line" },
	{ TYPE_VISA_ANUL_VENTA,           "020000", "0200 Req anulación compra on-line" },
	{ TYPE_VISA_ANUL_DEVOL,           "220000", "0200 Req anulación devolución on-line" },

	{ TYPE_VISA_VENTA_RTA,            "000000", "0210 Rta compra on-line" },
	{ TYPE_VISA_DEVOL_RTA,            "200000", "0210 Rta devolución on-line" },
	{ TYPE_VISA_ANUL_VENTA_RTA,       "020000", "0210 Rta anulación de compra on-line" },
	{ TYPE_VISA_ANUL_DEVOL_RTA,       "220000", "0210 Rta anulación de devolución on-line" },

	{ TYPE_VISA_VENTA_LOCAL,          "000000", "0220 Req compra off-line" },
	{ TYPE_VISA_DEVOL_LOCAL,          "200000", "0220 Req devolución off-line" },
	{ TYPE_VISA_ANUL_VENTA_LOCAL,     "020000", "0220 Req anulación compra off-line" },
	{ TYPE_VISA_ANUL_DEVOL_LOCAL,     "220000", "0220 Req anulación devolución off-line" },

	{ TYPE_VISA_VENTA_LOCAL_RTA,      "000000", "0230 Rta compra off-line" },
	{ TYPE_VISA_DEVOL_LOCAL_RTA,      "200000", "0230 Rta devolución off-line" },
	{ TYPE_VISA_ANUL_VENTA_LOCAL_RTA, "020000", "0230 Rta anulación compra off-line" },
	{ TYPE_VISA_ANUL_DEVOL_LOCAL_RTA, "220000", "0230 Rta anulación devolución off-line" },

	{ TYPE_VISA_CIERRE,               "920000", "0500 Req cierre de lote (antes del batch upload)" },
	{ TYPE_VISA_CIERRE_RTA,           "920000", "0510 Rta cierre de lote (antes del batch upload)" },

	{ TYPE_VISA_CIERRE_B,             "960000", "0500 Req cierre de lote (después del batch upload)" },
	{ TYPE_VISA_CIERRE_B_RTA,         "960000", "0510 Rta cierre de lote (después del batch upload)" },

	{ TYPE_VISA_TEST,                 "990000", "0800 Req echo test" },
	{ TYPE_VISA_TEST_RTA,             "990000", "0810 Rta echo test" },

	{ TYPE_VISA_REV_VENTA,            "000000", "0400 Req reverso de venta" },
	{ TYPE_VISA_REV_VENTA_RTA,        "000000", "0410 Rta reverso de venta" },
	{ TYPE_VISA_REV_DEVOL,            "200000", "0400 Req reverso de devolución" },
	{ TYPE_VISA_REV_DEVOL_RTA,        "200000", "0410 Rta reverso de devolución" },
	{ TYPE_VISA_REV_ANUL_VENTA,       "020000", "0400 Req reverso anulación venta" },
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,   "020000", "0410 Rta reverso anulación venta " },
	{ TYPE_VISA_REV_ANUL_DEVOL,       "220000", "0400 Req reverso anulación devolución" },
	{ TYPE_VISA_REV_ANUL_DEVOL_RTA,   "220000", "0410 Rta reverso anulación devolución" },

	{ TYPE_VISA_BATCH,                "xxxxxx", "0320 Req Batch Upload" },
	{ TYPE_VISA_BATCH_RTA,            "xxxxxx", "0330 Rta Batch Upload" },	

	/* Pago de resumen */	
	{ TYPE_VISA_VENTA,                "000010", "0200 Req pago on-line (Cobro TCI)" },
	{ TYPE_VISA_DEVOL,                "200010", "0200 Req devolución on-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_VENTA,           "020010", "0200 Req anulación pago on-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_DEVOL,           "220010", "0200 Req anulación devolución on-line (Cobro TCI)" },

	{ TYPE_VISA_VENTA_RTA,            "000010", "0210 Rta pago on-line (Cobro TCI)" },
	{ TYPE_VISA_DEVOL_RTA,            "200010", "0210 Rta devolución on-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_VENTA_RTA,       "020010", "0210 Rta anulación pago on-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_DEVOL_RTA,       "220010", "0210 Rta anulación devolución on-line (Cobro TCI)" },

	{ TYPE_VISA_VENTA_LOCAL,          "000010", "0220 Req pago off-line (Cobro TCI)" },
	{ TYPE_VISA_DEVOL_LOCAL,          "200010", "0220 Req devolución off-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_VENTA_LOCAL,     "020010", "0220 Req anulación pago off-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_DEVOL_LOCAL,     "220010", "0220 Req anulación devolución off-line (Cobro TCI)" },

	{ TYPE_VISA_VENTA_LOCAL_RTA,      "000010", "0230 Rta pago off-line (Cobro TCI)" },
	{ TYPE_VISA_DEVOL_LOCAL_RTA,      "200010", "0230 Rta devolución off-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_VENTA_LOCAL_RTA, "020010", "0230 Rta anulación pago off-line (Cobro TCI)" },
	{ TYPE_VISA_ANUL_DEVOL_LOCAL_RTA, "220010", "0230 Rta anulación devolución off-line (Cobro TCI)" },

	{ TYPE_VISA_REV_VENTA,            "000010", "0400 Req reverso pago (Cobro TCI)" },
	{ TYPE_VISA_REV_VENTA_RTA,        "000010", "0410 Rta reverso pago (Cobro TCI)" },
	{ TYPE_VISA_REV_DEVOL,            "200010", "0400 Req reverso devolución (Cobro TCI)" },
	{ TYPE_VISA_REV_DEVOL_RTA,        "200010", "0410 Rta reverso devolución (Cobro TCI)" },
	{ TYPE_VISA_REV_ANUL_VENTA,       "020010", "0400 Req reverso anulación pago (Cobro TCI)" },
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,   "020010", "0410 Rta reverso anulación pago (Cobro TCI)" },
	{ TYPE_VISA_REV_ANUL_DEVOL,       "220010", "0400 Req reverso anulación devolución (Cobro TCI)" },
	{ TYPE_VISA_REV_ANUL_DEVOL_RTA,   "220010", "0410 Rta reverso anulación devolución (Cobro TCI)" },

	{ 0 , 0 , 0 }
};

str_descripcion_campo iso_descripcion[]=
{
	{ 2, "Número de cuenta / tarjeta"},
	{ 3, "Código de procesamiento"},
	{ 4, "Importe de la transacción"},
	{ 7, "Fecha y hora de transmisión"},
	{11, "Número de trace del sistema"},
	{12, "Hora local de la transacción"},
	{13, "Fecha local de la transacción"},
	{14, "Fecha de expiración"},
	{15, "Fecha de cierre"},
	{17, "Fecha de captura"},
	{19, "Codigo de pais"},
	{22, "Modo de ingreso en la pos"},
	{23, "Card Secuence Number (PAN)"},
	{24, "Identificación de la red"},
	{25, "Código de condición de la pos"},
	{34, "Numero de cuenta"},
	{35, "Datos del track II"},
	{37, "Retrieval reference number"},
	{38, "Código de autorización"},
	{39, "Código de respuesta"},
	{41, "Identificación de la terminal"},
	{42, "Identificación del comercio"},
	{45, "Datos del track I"},
	{46, "Track I no leido"},
	{48, "Cuotas / Datos originales"},
	{49, "Código de moneda"},
	{52, "Pin / Working key"},
	{54, "Importe de retiro cashback"},
	{55, "Código de seguridad de la tarjeta"},
	{59, "Campos adicionales"},
	{60, "Versión de soft de la aplicación"},
	{62, "Número de ticket"},
	{63, "Mensaje del host/batch upl/cierre"},
    { 0,  0 }	
};


str_iso_fields iso_fields[]=
{
    { 2, LVAR,  2, TBCD   }, 
    { 3, LFIX,  6, TBCD   }, 
    { 4, LFIX, 12, TBCD   }, 
    { 7, LFIX, 10, TBCD   },
    {11, LFIX,  6, TBCD   }, 
    {12, LFIX,  6, TBCD   }, 
    {13, LFIX,  4, TBCD   }, 
    {14, LFIX,  4, TBCD   },
    {15, LFIX,  4, TBCD   }, 
    {17, LFIX,  4, TBCD   }, 
    {19, LFIX,  3, TBCD   }, 
    {22, LFIX,  3, TBCD   }, 
    {23, LFIX,  3, TBCD   }, 
    {24, LFIX,  3, TBCD   },
    {25, LFIX,  2, TBCD   }, 
	{34, LVAR,  2, TASCII },
    {35, LVAR,  2, TBCD   }, 
    {37, LFIX, 12, TASCII }, 
    {38, LFIX,  6, TASCII },
    {39, LFIX,  2, TASCII }, 
    {41, LFIX,  8, TASCII }, 
    {42, LFIX, 15, TASCII }, 
	{45, LVAR,  2, TASCII },
	{46, LVAR,  3, TASCII },
    {48, LVAR,  3, TASCII },
    {49, LFIX,  3, TASCII }, 
	{52, LFIX,  8, TASCII }, 
	{54, LVAR,  3, TASCII }, 
	{55, LVAR,  3, TASCII }, 
	{59, LVAR,  3, TASCII },
    {60, LVAR,  3, TASCII }, 
    {62, LVAR,  3, TASCII }, 
    {63, LVAR,  3, TASCII },
    { 0,    0,  0, 0 }
};

str_fields_mask fields_mask[]=
{
	{ TYPE_VISA_VENTA,					0x723C0580, 0x00C18214, 0x723C0780, 0x20CD9634},	/* Modificacion PINPAD */
	{ TYPE_VISA_DEVOL,					0x723C0580, 0x00C18014, 0x723C0580, 0x2CCD9634},	/* Modificacion PINPAD */
	{ TYPE_VISA_ANUL_VENTA,				0x723C0580, 0x08C18014, 0x723C0580, 0x28C19634},	/* Modificacion PINPAD */
	{ TYPE_VISA_ANUL_DEVOL,				0x723C0580, 0x08C18014, 0x723C0580, 0x28C19634},	/* Modificacion PINPAD */
	{ TYPE_VISA_VENTA_RTA,				0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_DEVOL_RTA,				0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_ANUL_VENTA_RTA,			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_ANUL_DEVOL_RTA,			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_VENTA_LOCAL,			0x723C8580, 0x04C18014, 0x723C8580, 0x24C18234},   /* DS11062002 dice 0x24cd8214 */
	{ TYPE_VISA_DEVOL_LOCAL,            0x723C8580, 0x04C18014, 0x723C8580, 0x24C18234},   /* DS11062002 dice 0x24cd8214 */
	{ TYPE_VISA_ANUL_VENTA_LOCAL,		0x723C8580, 0x0CC18014, 0x723C8580, 0x0CC18034},
	{ TYPE_VISA_ANUL_DEVOL_LOCAL,		0x723C8580, 0x0CC18014, 0x723C8580, 0x0CC18034},
	{ TYPE_VISA_VENTA_LOCAL_RTA,		0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_DEVOL_LOCAL_RTA,		0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_ANUL_VENTA_LOCAL_RTA,	0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_ANUL_DEVOL_LOCAL_RTA,	0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_BATCH,					0x723C0580, 0x0CC18016, 0x723C8580, 0x0CC18416},
	{ TYPE_VISA_BATCH_RTA,				0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_VENTA,				0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_DEVOL,				0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_ANUL_VENTA,			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_ANUL_DEVOL,			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_VENTA_RTA,			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_DEVOL_RTA,			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_ANUL_VENTA_RTA,		0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_REV_ANUL_DEVOL_RTA,		0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_CIERRE,					0x22220100, 0x00C00012, 0x22220100, 0x00C00012},
	{ TYPE_VISA_CIERRE_B,				0x22220100, 0x00C00012, 0x22220100, 0x00C00012},
	{ TYPE_VISA_CIERRE_RTA,				0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_CIERRE_B_RTA,			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
	{ TYPE_VISA_TEST,					0x22200100, 0x00C00000, 0x22200100, 0x00C00010},
	{ TYPE_VISA_TEST_RTA,				0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},    
	{ 0,								0x00000000, 0x00000000, 0x00000000, 0x00000000}
};

/* ===================== Funciones auxiliares ===================== */
#define	getbit(n)	(0x80000000>>((n)-1))
#define getbyte(x,y)	( ( (x) >> (8 * (4 - (y)) ) ) & 0xFF)


int ascii2bcd(char *str, int len)
{
    static char	bcd_str[100];
    int		cont;
    int		bcd_len;
    int		ascii_len;

    memset(bcd_str, 0, sizeof(bcd_str));

    bcd_len=len/2+len%2;
    ascii_len=len;
    for (cont=bcd_len-1; cont>=0; cont--)
    {
	bcd_str[cont]=(str[ascii_len-1+(cont-bcd_len+1)*2]-0x30);
	if ((ascii_len-2+(cont-bcd_len+1)*2)>=0)
	    bcd_str[cont]|=(str[ascii_len-2+(cont-bcd_len+1)*2]-0x30)*0x10;
    }
    memcpy(str, bcd_str, bcd_len);
    return bcd_len;
}

int ascii2bcd_2(char *str, int len)
{
    static char	bcd_str[100];
    int		cont;
    int		bcd_len;
    int		ascii_len;

    memset(bcd_str, 0, sizeof(bcd_str));

    bcd_len=len/2+len%2;
    ascii_len=len;
    for (cont=0; cont<bcd_len; cont++)
    {
	bcd_str[cont]=(str[cont*2]-0x30)*0x10;
	if ((cont<(bcd_len-1))||(len%2==0))
	    bcd_str[cont]|=(str[(cont*2)+1]-0x30);
    }
    memcpy(str, bcd_str, bcd_len);
    return bcd_len;
}

int bcd2ascii(char *str, int bcd_len)
{
    static char	ascii_str[100];
    int		cont;
    int		ascii_len;

    memset(ascii_str, 0, sizeof(ascii_str));
    ascii_len=bcd_len*2;

    for(cont=0; cont<ascii_len; cont+=2)
    {
		ascii_str[cont]=(str[cont/2]&0x00F0)/0x10+0x30;
		if (ascii_str[cont]==0x3F) 
			ascii_str[cont]= 'F';
		ascii_str[cont+1]=(str[cont/2]&0x000F)+0x30;
		if (ascii_str[cont+1]==0x3F) 
			ascii_str[cont+1]= 'F';
    }

    memcpy(str, ascii_str, ascii_len+1);
    return ascii_len;
}
/* ===================== Metodos ===================== */
/*-----------------------------------------------------------------*\
|  Funcion     : Iso_Msg()                                          |
|-------------------------------------------------------------------|
|  Argumentos  : Ninguno.                                           |
|-------------------------------------------------------------------|
|  Descripcion : Constructor.                                       |
\*-----------------------------------------------------------------*/
Iso_Msg::Iso_Msg()
{
   type=0;
   bitmap1=0;
   bitmap2=0;
   data_len=0;
   memset (data, 0, sizeof(data));
}

/*-----------------------------------------------------------------*\
|  Funcion     : Iso_Msg()                                          |
|-------------------------------------------------------------------|
|  Argumentos  : 2.                                                 |
|-------------------------------------------------------------------|
|  char *msg_string : Mensaje en formato ISO (Visa).                |
|-------------------------------------------------------------------|
|  int msg_len : Largo del mensaje.				    |
|-------------------------------------------------------------------|
|  Descripcion : Constructor.                                       |
\*-----------------------------------------------------------------*/
Iso_Msg::Iso_Msg(char *msg_string, int msg_len)
{
    char	type_aux[5];
	long	lCodiProc;
	char	szCodiProc[7];

    if (msg_len<10)		// Si el largo del mensaje es menor al largo
	return;			// del tipo de mensaje + bitmap, no es valido.

    memset (type_aux, 0, sizeof(type_aux));
    memset (data, 0, sizeof(data));
    data_len=msg_len-10;

    bitmap1=( (msg_string[2] << BYTE_1) & MASK_1 ) |
	    ( (msg_string[3] << BYTE_2) & MASK_2 ) |
	    ( (msg_string[4] << BYTE_3) & MASK_3 ) | 
	    ( (msg_string[5] << BYTE_4) & MASK_4 );
    bitmap2=( (msg_string[6] << BYTE_1) & MASK_1 ) | 
	    ( (msg_string[7] << BYTE_2) & MASK_2 ) |
	    ( (msg_string[8] << BYTE_3) & MASK_3 ) | 
	    ( (msg_string[9] << BYTE_4) & MASK_4 );

    memcpy(data, &msg_string[10], msg_len-10);

    memcpy (type_aux, msg_string, 2);
    bcd2ascii(type_aux, 2);

    sprintf(szCodiProc, "%s", GetField(3));
	lCodiProc = atol(GetField(3));

	/* Pago de resumen */
	lCodiProc = (lCodiProc==OPCODE_VISA_PAGO_RESUMEN)    ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_DEVO_RESUMEN)    ? OPCODE_VISA_DEVOL      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUP_RESUMEN)    ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUD_RESUMEN)    ? OPCODE_VISA_ANUL_DEVOL : lCodiProc;

	lCodiProc = (lCodiProc==OPCODE_VISA_VTA_CA_P)        ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_VTA_CC_P)        ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_VTA_CA_D)        ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_VTA_CC_D)        ? OPCODE_VISA_VENTA      : lCodiProc;

	lCodiProc = (lCodiProc==OPCODE_VISA_DEV_CA_P)        ? OPCODE_VISA_DEVOL      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_DEV_CC_P)        ? OPCODE_VISA_DEVOL      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_DEV_CA_D)        ? OPCODE_VISA_DEVOL      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_DEV_CC_D)        ? OPCODE_VISA_DEVOL      : lCodiProc;
	
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_VTA_CA_P)   ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_VTA_CC_P)   ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_VTA_CA_D)   ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_VTA_CC_D)   ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_DEV_CC_D)   ? OPCODE_VISA_ANUL_DEVOL : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_DEV_CC_D)   ? OPCODE_VISA_ANUL_DEVOL : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_DEV_CC_D)   ? OPCODE_VISA_ANUL_DEVOL : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_VISA_ANUL_DEV_CC_D)   ? OPCODE_VISA_ANUL_DEVOL : lCodiProc;

	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_VTA_CA_P)   ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_VTA_CC_P)   ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_VTA_CA_D)   ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_VTA_CC_D)   ? OPCODE_VISA_VENTA      : lCodiProc;

	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_ANU_CA_P)  ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_ANU_CC_P)  ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_ANU_CA_D)  ? OPCODE_VISA_ANUL_VENTA : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_CASHB_POS_ANU_CC_D)  ? OPCODE_VISA_ANUL_VENTA : lCodiProc;

	lCodiProc = (lCodiProc==OPCODE_CASHB_VISA_VENTA_P)  ? OPCODE_VISA_VENTA      : lCodiProc;
	lCodiProc = (lCodiProc==OPCODE_CASHB_VISA_ANULA_P)  ? OPCODE_VISA_ANUL_VENTA : lCodiProc;

	switch (atoi(type_aux))
    {
	case TYPE_VISA_VENTA/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_VENTA: 
		    type=TYPE_VISA_VENTA;
		    break;
		case OPCODE_VISA_DEVOL: 
		    type=TYPE_VISA_DEVOL;
		    break;
		case  OPCODE_VISA_ANUL_VENTA:   
		    type=TYPE_VISA_ANUL_VENTA;
		    break;
		case OPCODE_VISA_ANUL_DEVOL: 
		    type=TYPE_VISA_ANUL_DEVOL;
		    break;
	    }
	    break;
	case TYPE_VISA_VENTA_LOCAL/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_VENTA: 
		    type=TYPE_VISA_VENTA_LOCAL;
		    break;
		case OPCODE_VISA_DEVOL: 
		    type=TYPE_VISA_DEVOL_LOCAL;
		    break;
		case  OPCODE_VISA_ANUL_VENTA:  
		    type=TYPE_VISA_ANUL_VENTA_LOCAL;
		    break;
		case OPCODE_VISA_ANUL_DEVOL:  
		    type=TYPE_VISA_ANUL_DEVOL_LOCAL;
		    break;
	    }
	    break;
	case TYPE_VISA_REV_VENTA/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_VENTA: 
		    type=TYPE_VISA_REV_VENTA;
		    break;
		case OPCODE_VISA_DEVOL: 
		    type=TYPE_VISA_REV_DEVOL;
		    break;
		case  OPCODE_VISA_ANUL_VENTA:  
		    type=TYPE_VISA_REV_ANUL_VENTA;
		    break;
		case OPCODE_VISA_ANUL_DEVOL:  
		    type=TYPE_VISA_REV_ANUL_DEVOL;
		    break;
	    }
	    break;
	case TYPE_VISA_VENTA_RTA/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_VENTA: 
		    type=TYPE_VISA_VENTA_RTA;
		    break;
		case OPCODE_VISA_DEVOL: 
		    type=TYPE_VISA_DEVOL_RTA;
		    break; 
		case OPCODE_VISA_ANUL_VENTA: 
		    type=TYPE_VISA_ANUL_VENTA_RTA;
		    break;
		case OPCODE_VISA_ANUL_DEVOL:
		    type=TYPE_VISA_ANUL_DEVOL_RTA;
		    break;
	    }
            break;
	case TYPE_VISA_VENTA_LOCAL_RTA/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_VENTA: 
		    type=TYPE_VISA_VENTA_LOCAL_RTA;
		    break;
		case OPCODE_VISA_DEVOL: 
		    type=TYPE_VISA_DEVOL_LOCAL_RTA;
		    break;
		case OPCODE_VISA_ANUL_VENTA:
		    type=TYPE_VISA_ANUL_VENTA_LOCAL_RTA;
		    break;
		case OPCODE_VISA_ANUL_DEVOL:
		    type=TYPE_VISA_ANUL_DEVOL_LOCAL_RTA;
		    break;
	    }
            break;
	case TYPE_VISA_REV_VENTA_RTA/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_VENTA: 
		    type=TYPE_VISA_REV_VENTA_RTA;
		    break;
		case OPCODE_VISA_DEVOL: 
		    type=TYPE_VISA_REV_DEVOL_RTA;
		    break;
		case  OPCODE_VISA_ANUL_VENTA: 
		    type=TYPE_VISA_REV_ANUL_VENTA_RTA;
		    break;
		case OPCODE_VISA_ANUL_DEVOL: 
		    type=TYPE_VISA_REV_ANUL_DEVOL_RTA;
		    break;
	    }
	    break;
	case TYPE_VISA_CIERRE/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_CIERRE: 
		    type=TYPE_VISA_CIERRE;
		    break;
		case OPCODE_VISA_CIERRE_B: 
		    type=TYPE_VISA_CIERRE_B;
		    break;
	    }
	    break;
	case TYPE_VISA_CIERRE_RTA/10:
	    switch (lCodiProc)
	    {
		case OPCODE_VISA_CIERRE: 
		    type=TYPE_VISA_CIERRE_RTA;
		    break;
		case OPCODE_VISA_CIERRE_B: 
		    type=TYPE_VISA_CIERRE_B_RTA;
		    break;
	    }
	    break;
	case TYPE_VISA_BATCH/10:
	    type=TYPE_VISA_BATCH;
	    break;
	case TYPE_VISA_BATCH_RTA/10:
	    type=TYPE_VISA_BATCH_RTA;
	    break;
	case TYPE_VISA_TEST/10:
            type=TYPE_VISA_TEST;
	    break;
	case TYPE_VISA_TEST_RTA/10:
	    type=TYPE_VISA_TEST_RTA;
	    break;
	}

	PutField(3, szCodiProc);
}
/*-----------------------------------------------------------------*\
|  Funcion     : ChkField()                                         |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Nurmero de campo.                                  |
|-------------------------------------------------------------------|
|  Return      : TRUE si esta, FALSE en caso contrario.             |
|-------------------------------------------------------------------|
|  Descripcion : Verifica si esta el campo especificado.            |
\*-----------------------------------------------------------------*/
int	Iso_Msg::ChkField(int nro)
{
   if (nro>32)
      return ((bitmap2 & getbit(nro-32))==getbit(nro-32));
   else
      return ((bitmap1 & getbit(nro))==getbit(nro));
}

/*-----------------------------------------------------------------*\
|  Funcion     : ChkReady()                                         |
|-------------------------------------------------------------------|
|  Argumentos  : Ninguno.                                           |
|-------------------------------------------------------------------|
|  Return      : TRUE si estan todos los campos mandatorios.        |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Verifica que esten todos los campos mandatorios    |
|                para ese mensaje.                                  |
\*-----------------------------------------------------------------*/
int Iso_Msg::ChkReady()
{
   int	cont=0;
   while ((fields_mask[cont].type!=type) && (fields_mask[cont].type!=0))
      cont++;
   if (fields_mask[cont].type==0)
      return -1;

   return (((fields_mask[cont].min1 & bitmap1)==fields_mask[cont].min1) &&
      ((fields_mask[cont].min2 & bitmap2)==fields_mask[cont].min2));
}

/*-----------------------------------------------------------------*\
|  Funcion     : ChkValid()                                         |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Nuero de campo.                                    |
|-------------------------------------------------------------------|
|  Return      : TRUE si es valido, FALSE en caso contrario.        |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Verifica que es el campo especificado sea valido   |
|                para ese mensaje.                                  |
\*-----------------------------------------------------------------*/
int Iso_Msg::ChkValid(int nro)
{
   int	cont=0;
   while ((fields_mask[cont].type!=type) && (fields_mask[cont].type!=0))
      cont++;
   if (fields_mask[cont].type==0)
      return -1;
   if (nro>32)
      return ((fields_mask[cont].max2 & getbit(nro-32))==getbit(nro-32));
   else
      return ((fields_mask[cont].max1 & getbit(nro))==getbit(nro));
}

/*-----------------------------------------------------------------*\
|  Funcion     : GetFormat()                                        |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Numero de campo.                                   |
|-------------------------------------------------------------------|
|  Return      : Formato del campo (ASCII/BCD).                     |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Devuelve el tipo del campo:                        |
|                * ASCII					    |
|                * BCD						    |
\*-----------------------------------------------------------------*/
int Iso_Msg::GetFormat(int nro)
{
   int	cont=0;

   while (iso_fields[cont].bit!=0)
   {
      if (iso_fields[cont].bit==nro)
         return iso_fields[cont].format;
      cont++;
   }
   return FALSE;
}

char *Iso_Msg::GetDescripcionCampo(int nro)
{
   int	cont=0;

   while (iso_descripcion[cont].iNroCampo!=0)
   {
      if (iso_descripcion[cont].iNroCampo==nro)
         return iso_descripcion[cont].szDescripcion;
      cont++;
   }
   return FALSE;
}

char *Iso_Msg::GetDescripcionTipoMensaje()
{
   int	cont=0;
   static char szError[64];

   while (iso_desc_tipo[cont].iTipoMensaje!=0)
   {
	  if ( (iso_desc_tipo[cont].iTipoMensaje==type) &&
		   ((type==TYPE_VISA_BATCH)||(type==TYPE_VISA_BATCH_RTA)) )
		   return iso_desc_tipo[cont].szDescripcion;
      if ( (iso_desc_tipo[cont].iTipoMensaje==type) &&
		   (strcmp(iso_desc_tipo[cont].szCodProceso, GetField(3))==0) )	
		  return iso_desc_tipo[cont].szDescripcion;
      cont++;
   }
   sprintf(szError, "[%s] Descripción no encontrada", GetField(3));
   return (szError);
}

/*-----------------------------------------------------------------*\
|  Funcion     : GetType()                                          |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Numero de campo.                                   |
|-------------------------------------------------------------------|
|  Return      : Tipo de campo (largo fijo/variable).               |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Devuelve el tipo del campo:                        |
|                LVAR=Largo variable                                |
|                LFIX=Largo fijo.                                   |
\*-----------------------------------------------------------------*/
int Iso_Msg::GetType(int nro)
{
   int	cont=0;

   while (iso_fields[cont].bit!=0)
   {
      if (iso_fields[cont].bit==nro)
         return iso_fields[cont].type;
      cont++;
   }
   return FALSE;
}

/*-----------------------------------------------------------------*\
|  Funcion     : GetLen()                                           |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Numero de campo.                                   |
|-------------------------------------------------------------------|
|  Return      : Devuelve el largo del campo.                       |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Devuelve el largo del campo especificado o la      |
|                cantidad de digitos en caso de que el campo sea de |
|                largo variable.                                    |
\*-----------------------------------------------------------------*/
int Iso_Msg::GetLen(int nro)
{
   int	cont=0;

   while (iso_fields[cont].bit!=0)
   {
      if (iso_fields[cont].bit==nro)
         return iso_fields[cont].len;
      cont++;
   }
   return FALSE;
}

/*-----------------------------------------------------------------*\
|  Funcion     : GetFieldOfs()                                      |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Numero de campo.                                   |
|-------------------------------------------------------------------|
|  Return      : Devuelve el offset del campo o -1 en caso de error.|
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Devuelve el offset del campo especificado dentro   |
|                del mensaje.                                       |
\*-----------------------------------------------------------------*/
int Iso_Msg::GetFieldOfs(int nro)
{
    int		offset=0;
    int		cont;
    char	aux_str[5];
    int		aux_len;

    if (!ChkField(nro))
	return -1;

    for (cont=0; cont<nro; cont++)
    {
	if (ChkField(cont))
	{
	    if (GetType(cont)==LVAR)
	    {
		aux_len=GetLen(cont)/2+GetLen(cont)%2;
		memcpy(aux_str, &data[offset], aux_len);
		bcd2ascii(aux_str, aux_len);
		if (GetFormat(cont)==TBCD)
		    offset+=atoi(aux_str)/2+atoi(aux_str)%2;
		else
		    offset+=atoi(aux_str);
		offset+=aux_len;
	    }
	    else
	    {
		if (GetFormat(cont)==TBCD)
		    offset+=GetLen(cont)/2+GetLen(cont)%2;
		else
		    offset+=GetLen(cont);
	    }
	}
    }

    return offset;
}

/*-----------------------------------------------------------------*\
|  Funcion     : GetNextFieldOfs()                                  |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Numero de campo.                                   |
|-------------------------------------------------------------------|
|  Return      : Devuelve el offset del campo o -1 en caso de error.|
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Devuelve el offset del campo siguiente al 			|
|                especificado dentro del mensaje.                   |
\*-----------------------------------------------------------------*/
int Iso_Msg::GetNextFieldOfs(int nro)
{
   int	cont;
   for (cont=nro+1; cont<64; cont++)
   {
      if (ChkField(cont))
         return GetFieldOfs(cont);
   }
   return data_len;
}

/*-----------------------------------------------------------------*\
|  Funcion     : PutField()                                         |
|-------------------------------------------------------------------|
|  Argumentos  : 2.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Numero de campo.                                   |
|-------------------------------------------------------------------|
|  char *data  : Datos.                                             |
|-------------------------------------------------------------------|
|  Return      : 0 si no hubo error, -1 en caso contrario.          |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Agrega o modifica el campo especificado al mensaje.|
|								    |
|          +---- HHHHHH (nuevos datos)				    |
|          v							    |
|  xxxxxxxxZZZZZZZyyyyyyyy					    |
|								    |
|          	1. Offset inicio nuevos datos.		    	    |
|               2. Longitud de Z.				    |
|               3. Longitud de H.				    |
|               4. Longitud real de los datos.			    |
|                						    |
|  A. Mov desde la possicion (1+2) (4-(1+2)) bytes a buf. temporal. |
|  B. Insertar en la posicion (1) (3) bytes (datos nuevos).	    |
|  C. Mov del buf. temporal a la pos. (1+3) (4-(1+2)) bytes.	    |
\*-----------------------------------------------------------------*/
int Iso_Msg::PutField(int nro, char *new_data_str)
{
    int		new_data_ofs=0;
    int		new_data_len=0;
    int		old_data_len=0;
    int		aux_len;
    int		cont;
    char	aux_str[5];
    char	temp[1000]={0};
    char	*new_data;

    if (!ChkValid(nro))
	return -1;

    /* Copio new_data_str a new_data ya que lo vamos a modificar */
    aux_len=GetLen(nro);
    if (GetType(nro)==LVAR)
    {
	memcpy(aux_str, new_data_str, aux_len);
	aux_str[aux_len]=0;
	new_data_len+=atoi(aux_str);
    }
    new_data_len+=aux_len;
    new_data=(char *)malloc(new_data_len+1);
    memset(new_data, 0, new_data_len+1);
    memcpy(new_data, new_data_str, new_data_len);
    new_data_len=0;

    /* Obtengo el largo que va a ocupar el dato en el area de datos */
    if (GetType(nro)==LVAR)
    {
	aux_len=GetLen(nro)/2+GetLen(nro)%2;
	memcpy(aux_str, new_data, GetLen(nro));
	aux_str[GetLen(nro)]=0;
	if (GetFormat(nro)==TBCD)
	    new_data_len+=atoi(aux_str)/2+atoi(aux_str)%2;
	else
	    new_data_len+=atoi(aux_str);
	new_data_len+=aux_len;
    }
    else
    {
	if (GetFormat(nro)==TBCD)
	    new_data_len+=GetLen(nro)/2+GetLen(nro)%2;
	else
	    new_data_len+=GetLen(nro);
    }

    /* Obtengo el largo que ocupaba el valor anterior */
    if (ChkField(nro)==FALSE)
    {
	new_data_ofs=GetNextFieldOfs(nro);
	old_data_len=0;
    }
    else
    {
	new_data_ofs=GetFieldOfs(nro);
	if (GetType(nro)==LVAR)
	{
	    aux_len=GetLen(nro)/2+GetLen(nro)%2;
	    memcpy(aux_str, &data[new_data_ofs], aux_len);
	    bcd2ascii(aux_str, aux_len);
	    if (GetFormat(nro)==TBCD)
		old_data_len+=atoi(aux_str)/2+atoi(aux_str)%2;
	    else
		old_data_len+=atoi(aux_str);
	    old_data_len+=aux_len;
	}
	else
	{
	    if (GetFormat(nro)==TBCD)
		old_data_len+=GetLen(nro)/2+GetLen(nro)%2;
	    else
		old_data_len+=GetLen(nro);
	}
    }

    /* Si es necesario convierto los datos a formato BCD */
    if (GetType(nro)==LVAR)
    {
	aux_len=GetLen(nro)/2+GetLen(nro)%2;
	memcpy(aux_str, new_data, GetLen(nro));
	aux_str[GetLen(nro)]=0;
	ascii2bcd(new_data, GetLen(nro));
	for (cont=0; cont<atoi(aux_str); cont++)
	new_data[aux_len+cont]=new_data[GetLen(nro)+cont];
	if (GetFormat(nro)==TBCD)
	    ascii2bcd_2(&new_data[aux_len], atoi(aux_str));
    }
    else
    {
	if (GetFormat(nro)==TBCD)
	    ascii2bcd(new_data, GetLen(nro));
    }

    memcpy(temp, &data[new_data_ofs+old_data_len],
	data_len-(new_data_ofs+old_data_len));
    memset(&data[new_data_ofs], 0, data_len-new_data_ofs);
    memcpy(&data[new_data_ofs], new_data, new_data_len);
    memcpy(&data[new_data_ofs+new_data_len], temp,
	data_len-(new_data_ofs+old_data_len));

    data_len+=new_data_len-old_data_len;

    if (nro>32)
	bitmap2=bitmap2|getbit(nro-32);
    else
	bitmap1=bitmap1|getbit(nro);

    free(new_data);
    return 0;
}

/*-----------------------------------------------------------------*\
|  Funcion     : GetField()                                         |
|-------------------------------------------------------------------|
|  Argumentos  : 1.                                                 |
|-------------------------------------------------------------------|
|  int nro     : Numero de campo.                                   |
|-------------------------------------------------------------------|
|  Return      : Puntero a un string, NULL si no esta el campo.     |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Devuelve un puntero a un string conteniendo los    |
|                datos de ese campo. En caso de que ese campo no    |
|                contenga datos devuelve un puntero a NULL.         |
\*-----------------------------------------------------------------*/
char *Iso_Msg::GetField(int nro)
{
    static char	field_data[1003];
    int		offset;
    int		aux_len;
    int		len=0;
    int		cont;
	int		iAdd=0;
    

    if (!ChkField(nro))
	return NULL;

    memset(field_data, 0, sizeof(field_data));
    offset=GetFieldOfs(nro);
    if (GetType(nro)==LVAR)
    {
		aux_len=GetLen(nro)/2+GetLen(nro)%2;
		memcpy(field_data, &data[offset], aux_len);
		bcd2ascii(field_data, aux_len);
		if (GetLen(nro)%2!=0)
			for (cont=0; cont<GetLen(nro); cont++)
			field_data[cont]=field_data[cont+1];
		field_data[GetLen(nro)]=0;
		len=atoi(field_data);

		if (GetFormat(nro)==TBCD)
		{
			memcpy(&field_data[GetLen(nro)], &data[offset+aux_len], 
			len/2+len%2);
			bcd2ascii(&field_data[GetLen(nro)], len/2+len%2);
			if (GetLen(nro)%2!=0)
			for (cont=0; cont<len; cont++)
				field_data[GetLen(nro)+cont]=field_data[GetLen(nro)+cont+1];
			if (field_data[GetLen(nro)+len] == 'F')
				iAdd = 1;
			field_data[GetLen(nro)+len+iAdd]=0;
		}
		else
		{
			memcpy(&field_data[GetLen(nro)], &data[offset+aux_len], len);
			field_data[GetLen(nro)+len]=0;
		}
    }
    else
    {
		if (GetFormat(nro)==TBCD)
		{
			aux_len=GetLen(nro)/2+GetLen(nro)%2;
			memcpy(field_data, &data[offset], aux_len);
			bcd2ascii(field_data, aux_len);
			if (GetLen(nro)%2!=0)
	    		for (cont=0; cont<GetLen(nro); cont++)
				field_data[cont]=field_data[cont+1];
			field_data[GetLen(nro)]=0;
		}
		else
		{
			memcpy(field_data, &data[offset], GetLen(nro));
			field_data[GetLen(nro)]=0;
		}
    }

    return field_data;
}

int Iso_Msg::GetMsgType()
{
   return type;
}

/*-----------------------------------------------------------------*\
|  Funcion     : GetMsgString()                                     |
|-------------------------------------------------------------------|
|  Argumentos  : Ninguno.                                           |
|-------------------------------------------------------------------|
|  Return      : Puntero a un string, NULL en caso de error.        |
|-------------------------------------------------------------------|
|  Llamada por :                                                    |
|-------------------------------------------------------------------|
|  Descripcion : Devuelve un puntero a un string conteniendo el     |
|                mensaje armado. En caso de que falte un campo      |
|                devuelve un puntero a NULL.                        |
\*-----------------------------------------------------------------*/
int Iso_Msg::GetMsgString(char *msg_string)
{
   char		aux_str[5];
   int		tot_len=0;
   int		len;
   /* No hacemos mas el chequeo para poder pasar mensajes a medio armar */
/*
   if (!ChkReady())
      return 0;
*/

    sprintf(aux_str, "%04d", type/10);
    len=ascii2bcd(aux_str, strlen(aux_str));
    memcpy(&msg_string[tot_len], aux_str, len);
    tot_len=len;

    msg_string[2]=(char)(bitmap1 & MASK_1 ) >> BYTE_1;
    msg_string[3]=(char)(bitmap1 & MASK_2 ) >> BYTE_2;
    msg_string[4]=(char)(bitmap1 & MASK_3 ) >> BYTE_3;
    msg_string[5]=(char)(bitmap1 & MASK_4 ) >> BYTE_4;
    msg_string[6]=(char)(bitmap2 & MASK_1 ) >> BYTE_1;
    msg_string[7]=(char)(bitmap2 & MASK_2 ) >> BYTE_2;
    msg_string[8]=(char)(bitmap2 & MASK_3 ) >> BYTE_3;
    msg_string[9]=(char)(bitmap2 & MASK_4 ) >> BYTE_4;

    tot_len+=sizeof(bitmap1);
    tot_len+=sizeof(bitmap2);
    memcpy(&msg_string[tot_len], data, data_len);
    tot_len+=data_len;
    return tot_len;
}

