#ifdef OS_Linux
#pragma pack(1)
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Modif8583.h"


int main(int argc, char *argv[])
{
	FILE *fArchEnt=NULL;
	FILE *fArchSal=NULL;
	char szNombreArch[64];
	unsigned char cByte;
	int iContHexa;
	char szBuffer[512];
	int iLongMsje;
	int iCont;
	int iLong;
	char szAux[512];
	
	/* Verifica parametros */
	if (argc!=2)
	{	
		printf("Error en sintaxis de llamada:\n");
		printf("Parametro 1: Nombre del archivo 'dump' del mensaje\n");
		exit(-1);
	}
	
	/* Abre archivo de entrada */
	sprintf(szNombreArch, "%s.msg", argv[1]);
	if ((fArchEnt= fopen(szNombreArch, "rb"))==NULL)
	{
		printf("Error en apertura de archivo %s\n", szNombreArch);
		exit(-2);
	}

	/* Abre archivo de salida */
	sprintf(szNombreArch, "%s.asc", argv[1]);
	if ((fArchSal= fopen(szNombreArch, "w"))==NULL)
	{
		sprintf(szNombreArch, "%s/%s.asc", getenv("HOME"), argv[1]);
		if ((fArchSal= fopen(szNombreArch, "w"))==NULL)
		{
			printf("Error en apertura de archivo %s\n", szNombreArch);
			exit(-3);
		}
	}

	fprintf(fArchSal, "\nISO2ASC - TRADUCCION A FORMATO ASCII DE MENSAJES ISO\n\n");
	fprintf(fArchSal, "************************************************************************\n\n");

	/* Imprime el mensaje en formato Hexadecimal */
	fprintf(fArchSal, "Mensaje Original: \n======= =========\n\n");
	iContHexa= 0;
	while ( feof(fArchEnt)==0 )
	{
		cByte=fgetc(fArchEnt);
		if ( feof(fArchEnt)==0 )
		{
			fprintf(fArchSal, "%02X ", cByte);
			iContHexa++;
			if (iContHexa==16)
			{
				fseek(fArchEnt, -16, SEEK_CUR);		
				fprintf(fArchSal, "        ");
				for (iCont=0;iCont<16;iCont++)
				{
					cByte= fgetc(fArchEnt);
					if ((cByte>=32) &&(cByte<=126))
					{
						fprintf(fArchSal, "%c", cByte);
					}
					else
					{
						fprintf(fArchSal, ".");
					}
				}
				iContHexa= 0;
				fprintf(fArchSal, "\n");
			}
		}
	}

	if (iContHexa!=0)
	{
		for (iCont=0;iCont<16-iContHexa;iCont++)
		{
			fprintf(fArchSal, "   ");
		}
		fseek(fArchEnt, -iContHexa, SEEK_CUR);		
		fprintf(fArchSal, "        ");
		for (iCont=0;iCont<iContHexa;iCont++)
		{
			cByte= fgetc(fArchEnt);
			if ((cByte>=32) &&(cByte<=126))
			{
				fprintf(fArchSal, "%c", cByte);
			}
			else
			{
				fprintf(fArchSal, ".");
			}
		}
		fprintf(fArchSal, "\n");
	}
	fseek(fArchEnt,0, SEEK_SET);



	/* Imprime el mensaje traducido */
	fprintf(fArchSal, "\n************************************************************************\n\n");
	fprintf(fArchSal, "Mensaje Traducido: \n======= ==========\n\n");


	iLongMsje= fread(szBuffer, 1, sizeof(szBuffer), fArchEnt);
	Iso_Msg MensajeIso(szBuffer, iLongMsje);

	fprintf(fArchSal, "Tipo      : %s\n", MensajeIso.GetDescripcionTipoMensaje());
	fprintf(fArchSal, "Mapa      : ");
	for (iCont=1;iCont<=64;iCont++)
	{
		if ((iCont%8)==1)
		{
			if (iCont!=1) fprintf(fArchSal, "            ");
			fprintf(fArchSal, "Bits [%02d,%02d]", iCont, iCont+7);
			fprintf(fArchSal, "                      : [");
		}
		if (MensajeIso.ChkField(iCont))
		{
			fprintf(fArchSal, "1");
		}
		else
		{
			fprintf(fArchSal, "0");
		}
		if ((iCont %8)==0)
		{
			fprintf(fArchSal, "]\n");
		}
	}
	for (iCont=0;iCont<=64;iCont++)
	{
		if (MensajeIso.ChkField(iCont))
		{

			sprintf(szBuffer, MensajeIso.GetField(iCont));
			iLong= 0;
			while (iLong<(int)strlen(szBuffer))
			{
				memset(szAux, 0, sizeof(szAux));
				memcpy(szAux, &szBuffer[iLong], 22);
				if (iLong==0)
				{
					fprintf(fArchSal, "Campo %02d  : %-33s : [%s]\n", 
							   iCont, 
							   MensajeIso.GetDescripcionCampo(iCont),
							   szAux);
				}
				else
				{
					fprintf(fArchSal, "                                                [%s]\n", szAux);
				}
				iLong+=22;
			}
		}
	}
	fprintf(fArchSal, "\n************************************************************************\n\n");

	/* Cierra archivos y termina */
	fclose(fArchEnt);
	fclose(fArchSal);
	exit(0);

}
