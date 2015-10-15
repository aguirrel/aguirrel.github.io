// DemoEFT.cpp : Defines the entry point for the console application.
//
#ifdef WIN_32
#include "stdafx.h"
#endif
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#ifdef WIN_32
#include "winsock2.h"
#else
#include "sys/socket.h"
#endif

#include "ConexSocket.h"
#include "Price.h"

#define HEADER_SIZE	6
#define FLAG_ONLINE	1

void TrataMsg(char * szMsgIn,int iMsgInLen,char * szMsgOut,int * iMsgOutLen);

char szCodAccion[3];
char szNumAuto[7];
char szNumLote[4];
char szCuponLote[18+1];

char * GetTerminal(char * szCaja, char * CodCA);
char * GetComercio(char * szSucursal, char * CodTar);
char * GetCA(char * CodTar);

int main(int argc, char* argv[])
{
	int iNewSock,iMsgInLen,iMsgOutLen;
	char szMsgIn[1024],szMsgOut[1024];
	char header[7],header_len[5],header_aux[3];
	char szDirIp[20];
	ConexSocket ComSrv;
	ConexSocket ComCnt;
/*	FILE * fdFile;*/
	char szAuxValor[7+1];
	char szAuxSalida[16];
	sprintf(szAuxValor, "11333");
	int iLong = 7;

	memset(szAuxSalida,  0 , sizeof(szAuxSalida));
	memset(szAuxSalida, '0', iLong);
	memcpy(&szAuxSalida[iLong-strlen(szAuxValor)], szAuxValor, strlen(szAuxValor));

	memset(szCodAccion, 0, sizeof(szCodAccion));
	memset(szNumAuto, 0, sizeof(szNumAuto));

	if (argc < 6)
	{
		printf("\n	%s requiere:\n",argv[0]);
		printf("/t1) Puerto de entrada.\n");
		printf("/t2) Puerto de salida.\n");
		printf("/t3) Codigo de respuesta(2 Bytes).\n");
		printf("/t4) Numero de autorizacion(6 Bytes).\n");
		printf("/t5) Numero de cupon/lote(3 Bytes).\n");
		return 0;
	}
	ComSrv.OpenServer(atoi(argv[1]));

	sprintf(szCodAccion, "%02d", atoi(argv[3]));
	sprintf(szNumAuto,   "%06d", atoi(argv[4]));
	sprintf(szNumLote,   "%03d", atoi(argv[5]));
	

	while(1)
	{
		printf("Espera conexion...\n");
		iNewSock = ComSrv.Accept();
		printf("Conexion aceptada\n");
		/* tomar el ip del que envia */
 		strcpy(szDirIp,ComSrv.GetIpClient());
		/* Descartar el HEADER en la lectura del mensaje */
		iMsgInLen = HEADER_SIZE;
		memset(header, 0, sizeof(header));
		memset(szMsgIn, 0, sizeof(szMsgIn));
		ComSrv.RecvMsg(header,&iMsgInLen);
		printf("Header: %s\n", header);
		iMsgInLen = ((atoi(header+2))-HEADER_SIZE);
		ComSrv.RecvMsg(szMsgIn,&iMsgInLen);
			
/*		if((fdFile = fopen("DemoPriceReq.log","a")) != NULL )
		{
			fwrite(szMsgIn,sizeof(char),iMsgInLen,fdFile);
			fprintf(fdFile,"\n");
			fclose(fdFile);
		}
*/
		/*************************************************/
		ComSrv.CloseClient();

		TrataMsg(szMsgIn,iMsgInLen,szMsgOut,&iMsgOutLen);
	
		memset(header_aux, 0, sizeof(header_aux));
		strncpy(header_aux, header, 2);
		sprintf(header_len, "%04d", iMsgOutLen + HEADER_SIZE);
		memset(header, 0, sizeof(header));
		strncpy(header, header_aux, strlen(header_aux));
        strcat(header, header_len);

		ComCnt.OpenClient(szDirIp,atoi(argv[2]));
		ComCnt.SendMsg(header, HEADER_SIZE);
		ComCnt.SendMsg(szMsgOut, iMsgOutLen);
		ComCnt.CloseServer();
/*		if((fdFile = fopen("DemoPriceRta.log","a")) != NULL )
		{
			fwrite(header,sizeof(char),HEADER_SIZE,fdFile);
			fprintf(fdFile,"\n");
			fwrite(szMsgOut,sizeof(char),iMsgOutLen,fdFile);
			fprintf(fdFile,"\n");
			fclose(fdFile);
		}
*/	}
	ComSrv.CloseServer();
	return 0;
}

void TrataMsg(char * szMsgIn,int iMsgInLen,char * szMsgOut,int * iMsgOutLen)
{
	Price_Msg * Price;
	Price_Msg * PriceRta;
	str_price_msg	price_msg_str;
	char aux_str[32];
	static int iUsaCuenta = 1;
	/*	FILE * fdFile;*/
	
	memcpy((char *)&price_msg_str, szMsgIn, sizeof(str_price_msg));
	Price = new Price_Msg(price_msg_str);

/*	if ( (fdFile = fopen("Price.log","a")) != NULL)
	{
		fprintf(fdFile,"\nMensaje nuevo: [%s]\n",Price->GetField(3));
		for (int i = 1 ; i < 29 ; i++)
			fprintf(fdFile,"Campo %d:[%s]\n",i,Price->GetField(i));
		fclose(fdFile);
	}
*/
	switch(atoi(Price->GetField(3)))
	{
	case TYPE_PRICE_VENTA:
		printf("Mensaje 1200\n");
		PriceRta = new Price_Msg(TYPE_PRICE_VENTA_RTA);
		PriceRta->PutField( 1, Price->GetField( 1));
		PriceRta->PutField( 3, Price->GetField(16));
		PriceRta->PutField( 4, Price->GetField(11));
	
		/*PriceRta->PutField( 6, "00");*/
		PriceRta->PutField( 6, szCodAccion);/*Se modifico el codigo de accion de retorno*/

		PriceRta->PutField( 7, Price->GetField( 4));
		PriceRta->PutField( 8, Price->GetField( 5));
		
		/*PriceRta->PutField( 9, "000000");*/
		PriceRta->PutField( 9, szNumAuto);/*Se modifico el numero de autorizacion*/

		PriceRta->PutField(10, "");
		PriceRta->PutField(11, "123456789");

		sprintf(szCuponLote, "%03d%01d        %03d   ", atoi(szNumLote), FLAG_ONLINE, atoi(szCodAccion));
		PriceRta->PutField(13, szCuponLote);/*Se Agrega el dato de cupon/lote*/
		PriceRta->PutField(16, "000000002550");
		
		sprintf(aux_str, "*                           ");
		if (iUsaCuenta==1)
		{
			sprintf(aux_str, "9876543210334455667712345678");
		}
		PriceRta->PutField(17, aux_str);
		
		sprintf(aux_str, ".             PLAN AHORA 12             ");
		PriceRta->PutField(18, aux_str);

		iUsaCuenta = iUsaCuenta==1 ? 0 : 1;

		*iMsgOutLen = PriceRta->GetMsgString(szMsgOut);
		break;
	case TYPE_PRICE_VENTA_LOCAL:
	case TYPE_PRICE_VENTA_LOCAL_BIS:
		printf("Mensaje 1220-1221\n");
		PriceRta = new Price_Msg(TYPE_PRICE_VENTA_LOCAL_RTA);
	    PriceRta->PutField(6, "00");
	    PriceRta->PutField(7, "      ");
	    strcpy(aux_str, Price->GetField(4));
	    while (strlen(aux_str)<21)
			strcat(aux_str, " ");
	    PriceRta->PutField(8, aux_str+2);
	    PriceRta->PutField( 1, Price->GetField(1));
	    PriceRta->PutField( 3, Price->GetField(16));
	    PriceRta->PutField( 4, Price->GetField(11));
	    PriceRta->PutField( 9, Price->GetField(5));
	    PriceRta->PutField(10, Price->GetField(42));
	    PriceRta->PutField(13, Price->GetField(23));
		*iMsgOutLen = PriceRta->GetMsgString(szMsgOut);
		break;
	case TYPE_PRICE_ANUL_VENTA:
	case TYPE_PRICE_ANUL_VENTA_BIS:
		printf("Mensaje 1420-1421\n");
		PriceRta = new Price_Msg(TYPE_PRICE_ANUL_VENTA_RTA);
		PriceRta->PutField( 1, Price->GetField( 1));
		PriceRta->PutField( 3, Price->GetField(13));
		PriceRta->PutField( 4, Price->GetField( 9));
		PriceRta->PutField( 8, Price->GetField( 4));
		PriceRta->PutField( 9, Price->GetField( 5));
		PriceRta->PutField(13, Price->GetField(21));
		PriceRta->PutField( 7, "000000");
	    PriceRta->PutField(10, "00000000");
	    PriceRta->PutField(11, "0000000000000000" );
		*iMsgOutLen = PriceRta->GetMsgString(szMsgOut);
		break;
	}
	
/*	if ( (fdFile = fopen("Price.log","a")) != NULL)
	{
		fprintf(fdFile,"Respuesta: [%s]\n",PriceRta->GetField(5));
		for (int i = 1 ; i < 17 ; i++)
			fprintf(fdFile,"Campo %d:[%s]\n",i,PriceRta->GetField(i));
		fclose(fdFile);
	}
*/
	delete Price;
	delete PriceRta;
}

char * GetTerminal(char * szCaja, char * CodCA)
{
	switch (atoi(CodCA))
	{
	case 20:
		return ("04");
		break;
	case 1:
		return ("02");
		break;
	case 2:
	case 8:
		return ("01");
		break;
	case 7:
		return ("05");
		break;
	default:
		return ("00");
		break;
	}
}

char * GetComercio(char * szSucursal, char * CodTar)
{
	switch(atoi(CodTar))
	{
	case 7:
		return ("9906706700");
		break;
	case 2:
		return ("123234");
		break;
	case 8:
		return ("100000001");
		break;
	case 20:
		return ("000000000055009");
		break;
	case 1:
		return ("00000013");
		break;
	default:
		return ("123456789");
		break;
	}
}

char * GetCA(char * CodTar)
{
	switch (atoi(CodTar))
	{
	case 20:
		return ("04");
		break;
	case 1:
		return ("02");
		break;
	case 2:
	case 8:
		return ("01");
		break;
	case 7:
		return ("05");
		break;
	default:
		return ("00");
		break;
	}
}
