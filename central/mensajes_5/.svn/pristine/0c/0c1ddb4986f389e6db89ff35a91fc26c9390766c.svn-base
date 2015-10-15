#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Price.C                                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Definicion de la clase Price para el manejo de los         |
|                 | mensajes provenientes del TPV                              | 
+-----------------+------------------------------------------------------------+
*/
#include "Price.h"
#include "Daemons.h"
#include "Includes.h"
extern "C" 
{
	#include "DBTrxOff.h"
}

Config CfgFec("cfgfec");

/*******************************/
int Conv2Greg(char *FJ, char *FG)
{	/* Convierte de Juliano a Gregoriano */ 
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
/************************/
int Price_Msg::FunConUni()
{	/* Funcion para conversion de Unipago */	
	char FecJul[4];
	char FecGre[5];
	char C2Ini[30];
	char C2Fin[30];
	char Trx[7]; 

	strcpy(C2Ini,this->GetField(4));
	strncpy(FecJul,C2Ini+2,3);
	FecJul[3]=0;
	strncpy(Trx,C2Ini+6,6);
	Trx[6]=0;

	Conv2Greg(FecJul,FecGre);

	sprintf(C2Fin,"%s%s",FecGre,Trx);
	this->PutField(4,C2Fin);

	return OK;
}
/**************************/
int Price_Msg::CargaFunCon()
{	/* Funcion que carga las funciones de conversion */
	int i; 

	VFuncConv[0].Func=NULL;
	VFuncConv[1].Func=NULL;

	for (i=0;i<MAX_FUNC_CONV;i++)
		VFuncConv[i].Func=NULL;
	VFuncConv[2].Func=&Price_Msg::FunConUni;
	return OK;
}
/********************************/
int Price_Msg::ConvMsgPrice(int i)
{
	TFunConv F;

	if (VFuncConv[i].Func==NULL )
		return OK; 

	(this->*VFuncConv[i].Func)();
	return OK;
}
/***********************************/
int Price_Msg::ObtTipoMsg(char* data)
{
	char aux_str[10];

	strncpy(aux_str,&data[31],4);
	aux_str[4]=0;

	return atoi(aux_str);
}
/***********************************/
int Price_Msg::ObtTipoRta(char* data)
{
	char aux_str[10];

	strncpy(aux_str,&data[35],4);
	aux_str[4]=0;
	return atoi(aux_str);
}
/********************/
Price_Msg::Price_Msg()
{
	UseTrackIyCvc = 0; // DS29052001
	sprintf(szNroCA, "??");
	this->CargaFunCon();
	type=NULL;
	version=0;
	int i=0;
	for (i=0;i<MAX_CAMPOS_PRICE;i++)
		 Buffer[i][0]=0; 	
}
/********************************/
Price_Msg::Price_Msg(int msg_type)
{
    char aux_str[10];

	UseTrackIyCvc = 0; // DS29052001
	sprintf(szNroCA, "??");

    this->CargaFunCon();
    type=msg_type;
    version=0;
    int i=0;
    for (i=0;i<MAX_CAMPOS_PRICE;i++)
         Buffer[i][0]=0; 
    switch (msg_type)
    {
        case TYPE_PRICE_VENTA:
        case TYPE_PRICE_VENTA_LOCAL:
        case TYPE_PRICE_VENTA_LOCAL_BIS:
        case TYPE_PRICE_ANUL_VENTA:
        case TYPE_PRICE_ANUL_VENTA_BIS:
           sprintf(aux_str,"%04d",msg_type);
           PutField(3,aux_str);
           break;
        case TYPE_PRICE_VENTA_RTA:
        case TYPE_PRICE_VENTA_LOCAL_RTA:
        case TYPE_PRICE_ANUL_VENTA_RTA:
           sprintf(aux_str,"%04d",msg_type);
           PutField(5,aux_str);
           break;
    }
}
/*******************************************/
Price_Msg::Price_Msg(str_price_msg price_msg) 
{
	int i;
	int found;
	int j;
	char aux_str[1024]; /* DS29052001 */
	char data[1024];
	int desp;

	UseTrackIyCvc = 0; // DS29052001
	sprintf(szNroCA, "??");

	this->CargaFunCon();
	for (i=0;i<MAX_CAMPOS_PRICE;i++)
	Buffer[i][0]=0; 
	memcpy(data,(char *)&price_msg,sizeof(price_msg));

	version=VER22;
	type=ObtTipoMsg(data);

	i=0;
	found=0;
	while ((FormCamposPrice[i].version!=0) &&
	((FormCamposPrice[i].version!=version) || 
	(FormCamposPrice[i].tipo_msg!=type)))
		i++;


	if (FormCamposPrice[i].version==0)
		return;

	j=0;
	desp=0;

	while (j<FormCamposPrice[i].CanCampos)
	{
		memset(aux_str, 0x00, sizeof(aux_str));
		memcpy(aux_str, &data[desp], FormCamposPrice[i].order_campos[j].longi);
		desp=desp+FormCamposPrice[i].order_campos[j].longi;
		if (strlen(aux_str)>0)
			PutField(FormCamposPrice[i].order_campos[j].campo,aux_str);
		else
			NewPutField(FormCamposPrice[i].order_campos[j].campo,aux_str, FormCamposPrice[i].order_campos[j].longi);
		j++;
	}
	return;
}
/*****************************************/
int Price_Msg::PutField(int nro,char *data)
{
	strcpy(Buffer[nro],data);
	return OK;
}
/******************************************************/
int Price_Msg::NewPutField(int nro, char *data, int len)
{
	memcpy(Buffer[nro], data, len);
	Buffer[nro][len] = 0;
	return OK;
}
/********************************/
char* Price_Msg::GetField(int nro)
{
	static char data[500];

	strcpy(data,Buffer[nro]);
	return data;
}
/***************************************/
void Price_Msg::GetPinPad(char *szPinPad)
{
	static char aux_str[50];

	memset(aux_str, 0, sizeof(aux_str));
	switch(type)
	{
		case TYPE_PRICE_VENTA:
			memcpy(aux_str, Buffer[18], PINPAD_LEN);
			break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			memcpy(aux_str, Buffer[30], PINPAD_LEN);
			break;
		default:
			break;
	}
	memcpy(szPinPad, aux_str, PINPAD_LEN);
}
/************************/
int Price_Msg::UsePinPad()
{
	int iHayPinPad = 0;
	static char aux_str[50];
	char szNull[9];

	memset(aux_str, 0, sizeof(aux_str));
	memset(szNull,  0, sizeof(szNull));
	switch(type)
	{
		case TYPE_PRICE_VENTA:
			memcpy(aux_str, Buffer[18], PINPAD_LEN);
			if (    (memcmp(aux_str, PINPAD_NULO, PINPAD_LEN) != 0)
				 && (memcmp(aux_str, szNull,      PINPAD_LEN) != 0)   )
			{
				iHayPinPad = 1;
			}
			break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			memcpy(aux_str, Buffer[30], PINPAD_LEN);
			if (   (memcmp(aux_str, PINPAD_NULO, PINPAD_LEN) != 0)
				&& (memcmp(aux_str, szNull,      PINPAD_LEN) != 0)   )
			{
				iHayPinPad = 1;
			}
			break;
		default:
			break;
	}
	return (iHayPinPad);
}
/**************************/
char* Price_Msg::GetNroTar()
{
    static char aux_str[500];
    int cont;
    strcpy(aux_str, GetField(4));
	 
    /* Eliminar los blancos en el campo nro_tar del msg. price. */
    cont=0;
    while ((aux_str[cont]==' ') && (cont<strlen(aux_str)))
	   cont++;
    while ((aux_str[cont]!=' ') && (cont<strlen(aux_str)))
           cont++;
    if (aux_str[cont]==' ')
           aux_str[cont]='\0';

    if ((aux_str[0]!=0) && (aux_str[0]!=' '))
        return 	aux_str;
    else
    {
        strcpy(aux_str,ObtTarTrk2(GetField(10)));
	return aux_str;
    }
}
/***************************************/
int Price_Msg::GetMsgString(char *msg_string)
{
	int i,j;
	int LonTot;
	char aux_str[1024];   /* DS29052001 */
	char aux_str2[1024];  /* DS29052001 */
	int found;

	version=VER22;

	i=0;
	found=0;
	while ((FormCamposPrice[i].version!=0) &&
		 ((FormCamposPrice[i].version!=version) || 
		  (FormCamposPrice[i].tipo_msg!=type)))
	   i++;

	if (FormCamposPrice[i].version==0)
	  return NOOK;

	msg_string[0]=0;
	LonTot=0;
	for (j=0;j<FormCamposPrice[i].CanCampos;j++)
	{
	   strcpy(aux_str,GetField(FormCamposPrice[i].order_campos[j].campo));
	   sprintf(aux_str2,"%*.*s",FormCamposPrice[i].order_campos[j].longi,
			 FormCamposPrice[i].order_campos[j].longi,aux_str);
	   strcat(msg_string,aux_str2);
	   LonTot=LonTot+FormCamposPrice[i].order_campos[j].longi;
	}
	return LonTot;
}
/**************************************************/
int Price_Msg::GetHeapData(str_heap_data *heap_data)
{
	/*char szAux[3+1];*/

	switch(type)
	{
		case TYPE_PRICE_VENTA:
			memcpy((char *)&heap_data->header,GetField(1),strlen(GetField(1)));
			/*        memcpy((char *)&heap_data->planta,GetField(12),strlen(GetField(12)));*/
			memcpy((char *)&heap_data->nro_term,GetField(16),strlen(GetField(16)));
			memcpy((char *)&heap_data->nro_trans,GetField(11),strlen(GetField(11)));
			memcpy((char *)&heap_data->cod_tar,GetField(17),strlen(GetField(17)));
			/*sprintf(szAux, "%03d", atoi(GetField(13)));
			memcpy((char *)&heap_data->planta, szAux, strlen(szAux)-1 );*/ /*Empr03*/
			break;
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			memcpy((char *)&heap_data->header,GetField(1),strlen(GetField(1)));
			/*		memcpy((char *)&heap_data->planta,GetField(12),strlen(GetField(12)));*/
			memcpy((char *)&heap_data->nro_term,GetField(16),strlen(GetField(16)));
			memcpy((char *)&heap_data->nro_trans,GetField(11),strlen(GetField(11)));
			memcpy((char *)&heap_data->cod_tar,GetField(17),strlen(GetField(17)));
			/*sprintf(szAux, "%03d", atoi(GetField(13)));
			memcpy((char *)&heap_data->planta, szAux, strlen(szAux)-1 );*/ /*Empr03*/
			break;
		case TYPE_PRICE_VENTA_RTA:
		case TYPE_PRICE_VENTA_LOCAL_RTA:
		case TYPE_PRICE_ANUL_VENTA_RTA:
			memcpy((char *)&heap_data->header,GetField(1),strlen(GetField(1)));
			memcpy((char *)&heap_data->nro_term,GetField(16),strlen(GetField(1)));
			memcpy((char *)&heap_data->nro_trans,GetField(11),strlen(GetField(11)));
			break;
	}
	return OK;
}
/*************************************************/   
int Price_Msg::PutHeapData(str_heap_data heap_data)
{
	char aux_header[30];
	char aux_term[10];
	char aux_tran[10];

	memcpy(aux_header,heap_data.header,sizeof(heap_data.header));
	aux_header[sizeof(heap_data.header)]=0;
	memcpy(aux_term,heap_data.nro_term,sizeof(heap_data.nro_term));
	aux_term[sizeof(heap_data.nro_term)]=0;
	memcpy(aux_tran,heap_data.nro_trans,sizeof(heap_data.nro_trans));
	aux_tran[sizeof(heap_data.nro_trans)]=0;

	PutField(1,aux_header);
	switch (type)
	{
		case TYPE_PRICE_VENTA:
		   PutField(16,aux_term);
		   PutField(11,aux_tran);
		   break;
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
		   PutField(16,aux_term);
		   PutField(11,aux_tran);
		   break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
		   PutField(9,aux_tran);
		   PutField(13,aux_term);
		   break;
		case TYPE_PRICE_VENTA_RTA:
		case TYPE_PRICE_VENTA_LOCAL_RTA:
		case TYPE_PRICE_ANUL_VENTA_RTA:
		   PutField(3,aux_term);
		   PutField(4,aux_tran);
		   break;
		default:
		   return NOOK;
	}
	return OK;
}
/************************************/
char *Price_Msg::ObtTarTrk2(char * t2)
{	// Funcion que obtiene el numero de tarjeta a partir del track2
    static char aux_str1[30];
    int cont;

    //Obtener el numero de tarjeta del track2
    strcpy(aux_str1, GetField(10)); 
    cont=0;
    while ((cont<strlen(aux_str1)) && (aux_str1[cont]!='D') && 
            (aux_str1[cont]!='='))
            cont++;
    aux_str1[cont]=0;
    return aux_str1;
}
/*****************************************/
int Price_Msg::GetDBData(TrxData& trx_data)
{
    char        aux_str[21];
    char        aux_str1[4];
    unsigned int                cont;
	char		szAnioTrx[5];

	strcpy(aux_str, GetField(4));
    
	/* Eliminar los blancos en el campo nro_tar del msg. price. */
	cont=0;
	while ((aux_str[cont]==' ') && (cont<strlen(aux_str)))
		cont++;
	while ((aux_str[cont]!=' ') && (cont<strlen(aux_str)))
		cont++;
	if (aux_str[cont]==' ')
		aux_str[cont]='\0';

	if ((aux_str[0]!=0) && (aux_str[0]!=' ')) 
		trx_data.NroTar(aux_str);
	else
		trx_data.NroTar(ObtTarTrk2(GetField(10)));

	trx_data.CodEst("00");
	trx_data.Monto(GetField(6));
	trx_data.Retiro(GetField(32));
	trx_data.FecLoc(GetField(7)+2);

	/* Copia el año de la transaccion */
	memset(szAnioTrx, 0, sizeof(szAnioTrx));
	strcpy(szAnioTrx, "20");
	strncat(szAnioTrx, GetField(7), 2);
	trx_data.AnoTrx(szAnioTrx);

	char aux_str4[30];
	switch (type)
	{
	case TYPE_PRICE_VENTA:
		trx_data.CodTar(GetField(17));
		trx_data.NroTrx(GetField(11));
		trx_data.ModEnv("00");
		trx_data.ModEnv(aux_str4);
		sprintf(aux_str4, "%03d", atoi(GetField(13))); /*Empr03*/
		trx_data.NroSuc(aux_str4);
		sprintf(aux_str4, "%05d", atoi(GetField(15)));
		trx_data.NroCajSf(aux_str4);
		trx_data.NroCaj(GetField(16));

		trx_data.FecVen(GetField(8));
		trx_data.NroTic(GetField(11));
		trx_data.CodSeg(GetCodSeg());
		if (this->ValidoTrack2(GetField(10))==OK)
			trx_data.Track2(GetField(10));
		else
			trx_data.Track2("");
		trx_data.Track1(GetField(30)); /* DS29052001 */
		return OK;
	case TYPE_PRICE_VENTA_LOCAL:
	case TYPE_PRICE_VENTA_LOCAL_BIS:
		trx_data.CodTar(GetField(17));
		trx_data.NroTrx(GetField(11));
		trx_data.ModEnv("01");
		sprintf(aux_str4, "%03d", atoi(GetField(13))); /*Empr03*/
		trx_data.NroSuc(aux_str4);
		sprintf(aux_str4, "%05d", atoi(GetField(15)));
		trx_data.NroCajSf(aux_str4);
		trx_data.NroCaj(GetField(16));
		trx_data.CodSeg(GetCodSeg());
		if (this->ValidoTrack2(GetField(10))==OK)
			trx_data.Track2(GetField(10));
		else
			trx_data.Track2("");
		trx_data.Track1(GetField(30)); /* DS29052001 */
		trx_data.FecVen(GetField(8));
		trx_data.NroTic(GetField(11));
		return OK;
	case TYPE_PRICE_ANUL_VENTA:
	case TYPE_PRICE_ANUL_VENTA_BIS:
		trx_data.CodTar(GetField(14));
		trx_data.NroTrx(GetField(9));
		trx_data.ModEnv("00");
		sprintf(aux_str4, "%03d", atoi(GetField(11))); /*Empr03*/               
		trx_data.NroSuc(aux_str4);
		trx_data.NroCaj(GetField(13));
		trx_data.NroTic(GetField(9));
		trx_data.NroTicOri(GetField(16));
		return OK;
	default:
		break;
	}
} 
/****************************/
char *Price_Msg::GetTerminal()
{
	static char         aux_str[50];
	switch(type)
	{
		case TYPE_PRICE_VENTA:
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			strcpy(aux_str, GetField(16));
			break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			strcpy(aux_str, GetField(13));
			break;
		case TYPE_PRICE_VENTA_RTA:
		case TYPE_PRICE_VENTA_LOCAL_RTA:
		case TYPE_PRICE_ANUL_VENTA_RTA:
			strcpy(aux_str, GetField(3));
			break;
	}
	return aux_str;
}
/****************************/
char *Price_Msg::GetSucursal()
{
	static char         aux_str[50];
	switch(type)
	{
		case TYPE_PRICE_VENTA:
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			sprintf(aux_str, "%03d", atoi(GetField(15))); /*Empr03*/
			break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			sprintf(aux_str, "%03d", atoi(GetField(12))); /*Empr03*/
			break;
		case TYPE_PRICE_VENTA_RTA:
		case TYPE_PRICE_VENTA_LOCAL_RTA:
		case TYPE_PRICE_ANUL_VENTA_RTA:
			strcpy(aux_str, "000");
			break;
	}
	return aux_str;
}
/*******************************/
char *Price_Msg::GetTransaccion()
{
	static char aux_str[50];
	switch(type)
	{
		case TYPE_PRICE_VENTA:
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			strcpy(aux_str, GetField(11));
			break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			strcpy(aux_str, GetField(9));
			break;
		case TYPE_PRICE_VENTA_RTA:
		case TYPE_PRICE_VENTA_LOCAL_RTA:
		case TYPE_PRICE_ANUL_VENTA_RTA:
			strcpy(aux_str, GetField(4));
			break;
	}
	return aux_str;
}
/***************************/
char *Price_Msg::GetEmpresa()
{
	static char         aux_str[50];
	switch(type)
	{
		case TYPE_PRICE_VENTA:
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			sprintf(aux_str, "%03d", atoi(GetField(13)));
			break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			sprintf(aux_str, "%03d", atoi(GetField(11)));
			break;
		default:
			strcpy(aux_str, "");
	}
	return aux_str;
}
/**************************/
char *Price_Msg::GetCodTar()
{
	static char         aux_str[50];
	switch(type)
	{
		case TYPE_PRICE_VENTA:
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			strcpy(aux_str, GetField(17));
			break;
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			strcpy(aux_str, GetField(14));
			break;
		default:
			strcpy(aux_str, "");
	}
	return aux_str;
}
/***************************************/
int Price_Msg::ValidoTrack2 (char *campo)  
{	/* Funcion: la cual  controla que esta correcto el campo TrackII del Msg. price */
	int  i;

	if (campo[0] == ' ' )
	{  // Esta vacio el campo.
		return NOOK;
	}
	i=0;
	while ((campo[i] != '=') && (campo[i] != 'D') && (i<strlen(campo)) )
		i++;

	if (i == strlen(campo))
	{
		return NOOK;
	}
	return OK;
}
/************************/
void Price_Msg::InvertirFecha()
{ 
	char fec_aux[5];
	char fec_fin[5];
	
	strcpy(fec_aux,GetField(8));
	fec_fin[0]=fec_aux[2];
	fec_fin[1]=fec_aux[3];
	fec_fin[2]=fec_aux[0];
	fec_fin[3]=fec_aux[1];
	fec_fin[4]=0;
	PutField(8,fec_fin);
}
/***************************/
Price_Msg::operator Iso_Msg()
{
	char         aux_str1[100];
	char         aux_str2[100];
	char         aux_str4[100];
	char         aux_str5[100];
	char         aux_str9[100];
	char         aux_cfg[50];
	char         cod_seg[4];
	int          usePlan;
	unsigned int         cont;
	Iso_Msg      iso_msg;
	int		iCont;
	int		iValidarTrackI;
	char	szFormaIngreso[10];
	int		iUsaCashBack;

	iso_msg.SetNroCA8583(szNroCA);

	switch (type)
	{
		case TYPE_PRICE_VENTA:
			switch (atoi(GetField(5)))
			{
				case 1:
				case 3:
				case 8: /* Pago de resumen */
					iso_msg=Iso_Msg(TYPE_VISA_VENTA);
					iso_msg.SetNroCA8583(szNroCA);
					break;
				case 2:
				case 4:
				case 9: /* Devol Pago de resumen */
					iso_msg=Iso_Msg(TYPE_VISA_DEVOL);
					iso_msg.SetNroCA8583(szNroCA);
					break;
				default:
					return NULL;
			} 
			iso_msg.PutField(4, GetField(6));
			
			/* Tk#7118 - Monto cashback */
			iUsaCashBack = 1;
			sprintf(aux_str1, GetField(32));
			for (iCont=0; iCont<12; iCont++)
				if ( (aux_str1[iCont]<'0') || (aux_str1[iCont]>'9') )
					iUsaCashBack = 0;
			if ( (iUsaCashBack) && (atol(aux_str1)>0) )
			{
				sprintf(aux_str2, "012%012s", aux_str1);
				iso_msg.PutField(54, aux_str2);
			}
			
			iso_msg.PutField(12, GetField(7)+6);
			iso_msg.PutField(13, GetField(7)+2);
			
			/* Modificaion: 06/05/98
			Autor: Denise Lucien
			Objetivo: Evitar  que los  mensajes en que la tarjeta fue deslizada y no 
			trae  el TrackII, se considere manual   */
			strcpy(aux_str1, GetField(9));
			strcpy(aux_str9, GetField(10));

			if (((aux_str1[0] == '0' ) || (aux_str1[0] == '2' )) 
				&& (ValidoTrack2(aux_str9)!=OK) )
			{  // No viene el trackII
				PutField(9,"1");
				strcpy(aux_str1, GetField(9));
			}
			
			strcpy(szFormaIngreso, GetField(9));
			
			switch (szFormaIngreso[0])
			{
				case '0': // Ingreso por banda 
				case '2':           
					
					/* Forma de ingreso */
					switch (DBTipoProtocoloISO(szNroCA))
					{
					case PROT_ISO_AMEX:
						iso_msg.PutField(22, "062");
						break;
					default:
						iso_msg.PutField(22, "022");
						break;
					}		

					strcpy(aux_str1, GetField(10));
					cont=0;
					while ((aux_str1[cont]!=' ') && (aux_str1[cont]!='?') && 
						(cont<strlen(aux_str1)))
						cont++;
					strncpy(aux_str5, aux_str1, cont); 
					aux_str5[cont]='\0';
					sprintf(aux_str2, "%02d%s", strlen(aux_str5), aux_str5);
					iso_msg.PutField(35, aux_str2);
					break;
					
				case '1': // Ingreso manual
				case '3':
					
					/* Forma de ingreso */
					switch (DBTipoProtocoloISO(szNroCA))
					{
					case PROT_ISO_AMEX:
						iso_msg.PutField(22, "072");
						break;
					default:
						iso_msg.PutField(22, "012");
						break;
					}		
				
					iso_msg.PutField(14, GetField(8));
					strcpy(aux_str1, GetField(4));
					cont=0;
					while ((aux_str1[cont]!=' ') && (cont<strlen(aux_str1)))
						cont++;
					if (aux_str1[cont]==' ')
						aux_str1[cont]=0;
					sprintf(aux_str2, "%02d%s", strlen(aux_str1), aux_str1);
					iso_msg.PutField(2, aux_str2);
					break;
				case '4': // Ingreso Web
					iso_msg.PutField(22, "810");
					iso_msg.PutField(14, GetField(8));
					strcpy(aux_str1, GetField(4));
					cont=0;
					while ((aux_str1[cont]!=' ') && (cont<strlen(aux_str1)))
						cont++;
					if (aux_str1[cont]==' ')
						aux_str1[cont]=0;
					sprintf(aux_str2, "%02d%s", strlen(aux_str1), aux_str1);
					iso_msg.PutField(2, aux_str2);
					break;

			} /* Hasta aqui el switch de forma de captura */
			
			/* Manejo del Track I DS29052001 */
			strcpy(aux_str9, GetField(30));
			iValidarTrackI= ValidoTrack1(aux_str9);
			switch (iValidarTrackI)
			{
				case 0: /* Debe enviarse */
					if ((szFormaIngreso[0]=='0')||(szFormaIngreso[0]=='2'))
					{
						cont=76-1;
						while ((cont>=0)&&(aux_str9[cont]==' '))
							cont--;
						aux_str9[cont+1]= 0;
						sprintf(aux_str2, "%02d%s", strlen(aux_str9), aux_str9);
						iso_msg.PutField(45, aux_str2);
					}
					break;
				case 1: /* Debe enviarse pero no pudo leerse */ 
					//sprintf(aux_str2, "%02d%s", strlen(aux_str9), aux_str9);
					//iso_msg.PutField(45, aux_str2);
					// Se manda flag si ingreso es por banda
					switch (DBTipoProtocoloISO(szNroCA))
					{
					case PROT_ISO_AMEX:
						break;
					default:
						if ((szFormaIngreso[0]=='0')||(szFormaIngreso[0]=='2'))
						{
							sprintf(aux_str2, "0011");
							iso_msg.PutField(46, aux_str2);
						}
						break;
					}
					break;
				case 2: /* No debe enviarse */
					break;
			}
			// Se envía el código de seguridad siempre que:
			// Se envía el track I 
			// Se usa codigo de seguridad y el ingreso es manual
			if ( (iValidarTrackI!=2) ||
				((UseCodSeg==1)&&((szFormaIngreso[0]=='1')||
				                  (szFormaIngreso[0]=='3')||
								  (szFormaIngreso[0]=='4'))) )
			{
				memset(aux_str1, 0, sizeof(aux_str1));
				strcpy(aux_str1,GetField(29));
				iCont=8;
				while ( (iCont>=0) &&
					((aux_str1[iCont]>='0')&&(aux_str1[iCont]<='9')) )
				{
					iCont--;
				}
				if (iCont!=8) 
				{
					iCont++;
					strcpy(aux_str2, &aux_str1[iCont]);
					sprintf(aux_str1,"%03d%s",strlen(aux_str2),aux_str2);
					iso_msg.PutField(55,aux_str1);
				}
			}
			
			iso_msg.PutField(25, "00");
			
			strcpy(aux_str1, GetField(11));
			sprintf(aux_str2, "004%04d", atoi(aux_str1));
			iso_msg.PutField(62, aux_str2);      
			
			break;
			
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			
			switch (atoi(GetField(5)))
			{
				case 1:
				case 3:
				case 8: /* Pago de resumen */
					iso_msg=TYPE_VISA_VENTA_LOCAL;
					break;
				case 2:
				case 4:
				case 9: /* Devol Pago de resumen */
					iso_msg=TYPE_VISA_DEVOL_LOCAL;
					break;
				default:
					return NULL;
			} 
			
			
			iso_msg.PutField(4, GetField(6));
			iso_msg.PutField(12, GetField(7)+6);
			iso_msg.PutField(13, GetField(7)+2);
			
			/* Fecha de captura */
			switch (DBTipoProtocoloISO(szNroCA))
			{
			case PROT_ISO_AMEX:
				break;
			default:
				iso_msg.PutField(17, GetField(7)+2);
				break;
			}		
			/* DS29052001 */
			strcpy(szFormaIngreso, GetField(9));
			strcpy(aux_str9, GetField(10));
			if (((szFormaIngreso[0] == '0' ) || (szFormaIngreso[0] == '2' )) 
				&& (ValidoTrack2(aux_str9)!=OK) )
			{  // No viene el trackII
				PutField(9,"1");
				strcpy(szFormaIngreso, GetField(9));
			}		
			
			switch (szFormaIngreso[0])
			{
				case '0':
				case '2':
					iso_msg.PutField(22, "022");
					strcpy(aux_str1, GetField(10));
					cont=0;
					while ((aux_str1[cont]!=' ')&&(cont<strlen(aux_str1)))
						cont++;
					strncpy(aux_str5, aux_str1, cont);
					aux_str5[cont]='\0';
					sprintf(aux_str2, "%02d%s", strlen(aux_str5), aux_str5);
					iso_msg.PutField(35, aux_str2);					
					break;
					
				case '1':
				case '3':
					iso_msg.PutField(22, "012");
					iso_msg.PutField(14, GetField(8));
					strcpy(aux_str1, GetField(4));
					cont=0;
					while ((aux_str1[cont]!=' ')&&(cont<strlen(aux_str1)))
						cont++;
					if (aux_str1[cont]==' ')
						aux_str1[cont]=0;
					sprintf(aux_str2, "%02d%s", strlen(aux_str1), aux_str1);
					iso_msg.PutField(2, aux_str2);
					break;					
				case '4':
					iso_msg.PutField(22, "810");
					iso_msg.PutField(14, GetField(8));
					strcpy(aux_str1, GetField(4));
					cont=0;
					while ((aux_str1[cont]!=' ')&&(cont<strlen(aux_str1)))
						cont++;
					if (aux_str1[cont]==' ')
						aux_str1[cont]=0;
					sprintf(aux_str2, "%02d%s", strlen(aux_str1), aux_str1);
					iso_msg.PutField(2, aux_str2);
					break;						
			} /* Hasta aqui el switch de forma de captura */
			
			/* Manejo del Track I DS29052001 */
			strcpy(aux_str9, GetField(30));
			iValidarTrackI= ValidoTrack1(aux_str9);
			switch (iValidarTrackI)
			{
				case 0: /* Debe enviarse */
               if ((szFormaIngreso[0]=='0')||(szFormaIngreso[0]=='2'))
					{
						cont=76-1;
						while ((cont>=0)&&(aux_str9[cont]==' '))
							cont--;
						aux_str9[cont+1]= 0;
						sprintf(aux_str2, "%02d%s", strlen(aux_str9), aux_str9);
						iso_msg.PutField(45, aux_str2);
					}
					break;
				case 1: /* Debe enviarse pero no pudo leerse */ 
					//sprintf(aux_str2, "%02d%s", strlen(aux_str9), aux_str9);
					//iso_msg.PutField(45, aux_str2);
					switch (DBTipoProtocoloISO(szNroCA))
					{
					case PROT_ISO_AMEX:
						break;
					default:
						if ((szFormaIngreso[0]=='0')||(szFormaIngreso[0]=='2'))
						{
							sprintf(aux_str2, "0011");
							iso_msg.PutField(46, aux_str2);
						}
						break;
					}
					break;
				case 2: /* No debe enviarse */
					break;
			}		
			// Se envía el código de seguridad siempre que se debe enviar track I 
			if (iValidarTrackI!=2) 
			{
				memset(aux_str1, 0, sizeof(aux_str1));
				strcpy(aux_str1,GetField(29));
				iCont=8;
				while ( (iCont>=0) &&
					((aux_str1[iCont]>='0')&&(aux_str1[iCont]<='9')) )
				{
					iCont--;
				}
				if (iCont!=8) 
				{
					iCont++;
					strcpy(aux_str2, &aux_str1[iCont]);
					sprintf(aux_str1,"%03d%s",strlen(aux_str2),aux_str2);
					iso_msg.PutField(55,aux_str1);
				}
			}

			iso_msg.PutField(25, "00");		
			iso_msg.PutField(38, GetField(18)+2);		
			switch (DBTipoProtocoloISO(szNroCA))
			{
			case PROT_ISO_AMEX:
				break;
			default:
				iso_msg.PutField(48, "003001");
				break;
			}
			iso_msg.PutField(49, "032");
			
			strcpy(aux_str1, GetField(11));
			sprintf(aux_str2, "004%04d", atoi(aux_str1));
			iso_msg.PutField(62, aux_str2);
			
			break;
			
		case TYPE_PRICE_ANUL_VENTA:
		case TYPE_PRICE_ANUL_VENTA_BIS:
			switch (atoi(GetField(5)))
			{
				case 1:
				case 3:
				case 8: /* Pago de resumen */
					iso_msg=TYPE_VISA_ANUL_VENTA;
					break;
				case 2:
				case 4:
				case 9: /* Devol Pago de resumen */
					iso_msg=TYPE_VISA_ANUL_DEVOL;
					break;
				default:
					return NULL;
			} 
			strcpy(aux_str1, GetField(4));
			cont=0;
			while ((aux_str1[cont]!=' ') && (cont<strlen(aux_str1)))
				cont++;
			if (aux_str1[cont]==' ')
				aux_str1[cont]=0;
			sprintf(aux_str2, "%02d%s", strlen(aux_str1), aux_str1);
			iso_msg.PutField(2, aux_str2);
			
			iso_msg.PutField(4, GetField(6));
			
			iso_msg.PutField(12, GetField(7)+6);
			iso_msg.PutField(13, GetField(7)+2);
			
			iso_msg.PutField(25, "00");
			iso_msg.PutField(49, "032");
			
			strcpy(aux_str1, GetField(9));
			sprintf(aux_str2, "004%04d", atoi(aux_str1));
			iso_msg.PutField(62, aux_str2);
			break;

			case TYPE_PRICE_VENTA_RTA:
				break;

			case TYPE_PRICE_VENTA_LOCAL_RTA:
				break;

			case TYPE_PRICE_ANUL_VENTA_RTA:
				break;
	}
	return iso_msg;
}
/********************/
void Price_Msg::SetCodSeg()
{
	UseCodSeg=1;
}
/**********************/
void Price_Msg::SetNoCodSeg()
{
	UseCodSeg=0;
}
/**************************/
char *Price_Msg::GetCodSeg()
{
	static char cod_seg[40];
	char  mod_ing[5];
	char  aux_str[50]; 
	int 	 iCont;

	cod_seg[0] = 0;

	strcpy(mod_ing,GetField(9));

	if  ( (UseTrackIyCvc == 1) || // DS29052001
		((((!strcmp(mod_ing,"1")) || (!strcmp(mod_ing,"3")) || (!strcmp(mod_ing,"4"))) 
		&& (UseCodSeg==1))) )
	{
		strcpy(aux_str,GetField(29));
		iCont=8;
		while ( (iCont>=0) &&
		((aux_str[iCont]>='0')&&(aux_str[iCont]<='9')) )
		{
			iCont--;
		}
		if (iCont!=8) 
		{
			iCont++;
			strcpy(cod_seg, &aux_str[iCont]);
		}
	}
	return cod_seg;
}
/**************************/
char *Price_Msg::ObtPlanSf()
{
	static char plan[3];
	if (version==VER22)
	{
		switch (type) 
		{
		case TYPE_PRICE_VENTA:
			strcpy(plan,GetField(22));
			break;
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			strcpy(plan,GetField(25));
			break;
		default:
			strcpy(plan,"00");
		}
		return plan;
	}
	strcpy(plan,"00");
	return plan;
}
/**************************/
char *Price_Msg::ObtCuotas()
{
	static char cuotas[5];

	if (version==VER22)
	{
		switch (type) 
		{
		case TYPE_PRICE_VENTA:
			if (!strcmp(GetField(23),"000"))
				strcpy(cuotas,"001");
			else 
				strcpy(cuotas,GetField(23));
			break;
		case TYPE_PRICE_VENTA_LOCAL:
		case TYPE_PRICE_VENTA_LOCAL_BIS:
			if (!strcmp(GetField(26),"000"))
				strcpy(cuotas,"001");
			else
				strcpy(cuotas,GetField(26));
			break;
		default:
			strcpy(cuotas,"001"); 
		}
		return cuotas;
	}
	else
	{   
		strcpy(cuotas,"001");
		return cuotas;
	}
}
/***************************************/
int Price_Msg::ValidoTrack1 (char *campo)  
{
	int  iRta;

	if (strcmp(&campo[30] , "                                             1")==0)
	{ 
		iRta= 1;
		UseTrackIyCvc = 1; // DS29052001
	}
	else
	{
		if (  (strcmp(&campo[30] , "                                             2")==0) ||   (strcmp(&campo[30] , "                                              ")==0)  )
		{
			iRta= 2;
			UseTrackIyCvc = 0; // DS29052001
		}
		else
		{
			iRta= 0;
			UseTrackIyCvc = 1; // DS29052001
		}
	}
	return iRta;
}
/***************************************/
void Price_Msg::SetNroCAPrice(char *szParamCA)
{
	memset(szNroCA, 0, sizeof(szNroCA));
	memcpy(szNroCA, szParamCA, sizeof(szNroCA)-1);
}
