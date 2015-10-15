#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef DAEMONS_H
#define DAEMONS_H

#ifndef OK
    #define OK			0
#endif
#ifndef NOOK
    #define NOOK		-1
#endif

#define VERSION_ISO			"Coto.Arg"
#define VERSION_NRO			"A04"
#define CONFIG_FILE			"credito2.ini"

/* Eventos ordenados por prioridad.Nota: la prioridad del evento
   es igual a su codigo  */

#define LEN_TIMESTAMP			12

#define EVENT_NULL				0	/* Evento nulo, no deberia existir	*/


#define EVENT_SHUTDOWN			05	/* Bajar el Daemon principal		*/

#define EVENT_ECHOTEST_RTA		10	/* Respuesta de mensaje de Test		*/
#define EVENT_ECHOTEST			12	/* Mensaje de Test con ctro. aut.	*/
#define EVENT_TIMEOUT_ECHOTEST	14	/* Time out esperando respta.		*/


#define EVENT_PRICEONLINE_RTA	20	/* Respuesta de centro autorizador	*/
#define EVENT_PRICEONLINE		22	/* Msj a centro autorizador			*/
#define EVENT_TIMEOUT			24	/* Time out esperando respta.		*/
#define EVENT_PRICEREV		 	28	/* Evento de reverso de trx			*/
#define EVENT_CIERRE_RTA		30	/* Respuesta de cierre de lote		*/
#define EVENT_CIERRE			32	/* Cierre de lote					*/
#define EVENT_CIERRE_B			34	/* Cierre de lote (Batch Upload)	*/
#define EVENT_TIMEOUT_CIERRE	36	/* Time out esperando respta.		*/
#define EVENT_TIMEOUT_CIERRE_B	38	/* Time out esperando respta.		*/
#define EVENT_BATCH_RTA			40	/* Respuesta de batch upload		*/
#define EVENT_BATCH				42	/* Batch upload						*/
#define EVENT_TIMEOUT_BATCH		44	/* Time out esperando respta.		*/
#define EVENT_SNDREVERSO_RTA	50	/* Envia reverso					*/
#define EVENT_SNDREVERSO		52	/* Envia reverso					*/
#define EVENT_TIMEOUT_REVERSO	54	/* Envia reverso					*/
#define EVENT_RTATRXOFFLINE		60	/* Mensaje de Rta Autorz local		*/ 
#define EVENT_SNDTRXOFFLINE		62	/* Mensaje de autorizacion local	*/
#define EVENT_TMTTRXOFFLINE		64	/* Timeout en mensaje Offline		*/ 
#define EVENT_PRICEOFFLINE		70	/* Mensaje PRICE de auto. local		*/
#define EVENT_PRICEANUL			80	/* Msj a centro autorizador			*/
/* #define EVENT_PRICEANUL_RTA	6*/	/* Respuesta de centro autorizador	*/
#define EVENT_SNDTRXANUL		100	/* evento de envio de anulacion		*/
#define EVENT_SNDTRXANUL_RTA	101	/* evento de rta de anulacion		*/
#define EVENT_SNDTRXANUL_TO		102	/* timeout de anulacion				*/
#define EVENT_TIMEOUT_ANULACION 103      
#define EVENT_REVTRXANUL		110 /* Reverso de trx de anulacion		*/
#define EVENT_TIMEOUT_REV_ANUL  112 /* Evento de TO de reverso de anul	*/
#define EVENT_REVTRXANUL_RTA	113

#define MOD_AUT_LEN			2
#define FEC_TRX_LEN			8
#define CVC_LEN				9 /* Mod DS 09/02/2001 CVC AMEX Pasar a 9 */
#define COD_MON_LEN			3
#define COD_CON_CIE_LEN		1
#define PLAN_ISO_LEN		1
#define PLAN_SF_LEN			2
#define NRO_CUO_LEN			3
#define NRO_CAJ_CA_LEN		8
#define NRO_COM_LEN			15
#define COD_TAR_LEN			2
#define NRO_TRX_LEN			4
#define NRO_SUC_LEN			3
#define NRO_CAJ_LEN			5
#define NRO_TAR_LEN			20
#define COD_SEG_LEN			9 /* Mod DS 09/02/2001 CVC AMEX Pasar a 9 */	
#define FEC_VEN_LEN			4
#define COD_AUT_LEN			6
#define NRO_TIC_LEN			NRO_TRX_LEN
#define COD_TRX_LEN			2
#define MOD_ENV_LEN			2
#define COD_RTA_LEN			2
#define COD_EST_LEN			2
#define MONTO_LEN			12
#define RETIRO_LEN			12
#define FEC_ENV_LEN			10
#define FEC_RTA_LEN			10
#define CAN_RET_LEN			4
#define NRO_TRC_LEN			6
#define NRO_LOT_LEN			3
#define MOD_ING_LEN			3
#define NRO_REF_LEN			12
#define NRO_TIC_ORI_LEN		NRO_TIC_LEN
#define NRO_CAJ_ORI_LEN		NRO_CAJ_LEN
#define FEC_ORI_LEN			6
#define COD_PRO_LEN			6
#define FEC_LOC_LEN			10
#define LEN_TIP_ERR			10
#define LEN_COD_ERR			10
#define LEN_STA_ERR			2	
#define CAN_LEN				4
#define MON_LEN				12
#define CAN_VEN_LEN			CAN_LEN
#define MON_VEN_LEN			MON_LEN
#define CAN_DEV_LEN			CAN_LEN
#define MON_DEV_LEN			MON_LEN
#define CAN_ANU_LEN			CAN_LEN
#define MON_ANU_LEN			MON_LEN
/* Pago de resumen */
#define CAN_PAG_LEN			CAN_LEN
#define MON_PAG_LEN			MON_LEN
#define CAN_DEP_LEN			CAN_LEN
#define MON_DEP_LEN			MON_LEN
#define CAN_ANP_LEN			CAN_LEN
#define MON_ANP_LEN			MON_LEN
/* Retiros cashback */
#define MON_RET_LEN			MON_LEN
#define MON_ANR_LEN			MON_LEN

/******************************/
/* Configuracion p/borrar     */
/* track en proceso de cierre */
#define DEL_DAT_LEN			1
/******************************/
/* Configuracion p/insertar   */
/* o no el registro de cierre */
#define AGR_CIE_LEN			1
/******************************/

#define TRACK2_LEN 			37	
#define TRACK1_LEN			76 /* DS29052001 */
#define NRO_CA_LEN			2
#define COD_CA_LEN			25
#define NRO_CAJ_CA_LEN		8
#define NRO_COM_LEN			15
#define COD_RED_LEN			3
#define DES_CA_LEN			30
#define NRO_CAJ_SF_LEN		5
#define TIP_CIE_LEN			5
#define FEC_CIERRE_LEN		8
/* Modificacion PINPAD */
#define RTA_CA_LEN			3
#define PINPAD_LEN			8
#define	PINPAD_NULO			"00000000"
#define PINPAD_BD_NULO		"3030303030303030"
#define LON_CAMPO_13		18
#define COD_CTA_LEN			1 /* Pinpad2 */ 
#define LON_CAMPO_34		28
#define LON_CAMPO_63		40
#define ANO_TRX_LEN			4

#define FECHA_DIF_NUL		"000000"
	
/* TipoProd[3]='002' CantSubCampos[4]='0001' LongSubCampo[3]='004' IdentSubCampo[3]='009' DatoSubCampo[1]='1' */
#define CAMPO_NRO_CUENTA	"00200010040091"	
/* TipoProd[3]='008' CantSubCampos[4]='0001' LongSubCampo[3]='009' IdentSubCampo[3]='025' DatoSubCampo[1]='DDMMAA' */
#define CAMPO_FECHA_DIFE	"0080001009025"

#define FTO_VER				"%-08s%-01s%-01s%c%-03s%-04s%-06s "

#define NO_ANUL				'0'
#define SI_ANUL				'1'

typedef struct
{
    char	cod_tar[COD_TAR_LEN+1];
    char	nro_ca[NRO_CA_LEN+1];
    char	nro_suc[NRO_SUC_LEN+1];
    char	nro_caj[NRO_CAJ_LEN+1];
} str_ping_data;

char *currentTimestamp(void); 
char *currentTimeLog(void); 

/* DS 17/04/2001 para informar al TPV quien autoriza */
#define FLAG_ONLINE		"1"   /* Autoriza el centro autorizador */
#define FLAG_OFFLINE	"2"   /* Autoriza credito */
#define FLAG_TIMEOUT	"3"   /* Autoriza credito pero por timeout */

#endif
