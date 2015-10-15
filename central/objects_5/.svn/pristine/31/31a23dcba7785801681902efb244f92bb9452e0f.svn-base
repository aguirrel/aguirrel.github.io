#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | LinkTCP.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Fuentes para el manejo de la conexion TCP                  |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
#include "Includes.h"
#include "LinkTCP.h"
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TCP::AddLong                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion interna del modulo.                                |
|                 | Agrega la longitud del mensaje al comienzo                 |
+-----------------+------------------------------------------------------------+
*/
int TCP::AddLong(char *sEnt, int iEnt, char *sSal, int *iSal)
{
	int iRet = -1;
	unsigned short iLon;

	if ( iEnt <= sizeof(sDat)-CAB_TCP )
	{
		iLon = (unsigned short) ( (iProt==SIN_CAB) ? iEnt : (iEnt+CAB_TCP) );
		memcpy(sSal, &iLon, CAB_TCP);
		memcpy(sSal+CAB_TCP, sEnt, iEnt);
		*iSal = iEnt + CAB_TCP;
		iRet = 0;
	}
	return (iRet);
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TCP::TCP                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creador de la clase, inicializador de variables            |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
TCP::TCP(char *szNull, int iTime, int iPrtc)
{
	iProt = iPrtc;
	iTOut = iTime;
	sysLastError = 0;
	tcpLastError = 0;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TCP::Open                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Abre socket para comunicacion con centro autorizador.      |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int TCP::Open(char *sDirecc, char *sPuerto, char *sIdenti)
{
	int iRet;

	sysLastError = 0;
	tcpLastError = 0;
	iRet = SockTCPIP.Open();
	if (iRet!=0)
	{
		tcpLastError = -1;	
	}
	else
	{
		iRet = SockTCPIP.Connect(sDirecc, atoi(sPuerto), iTOut);
		if (iRet != 0)
		{
			tcpLastError = -2;
		}
		else
		{
			if (strlen(sIdenti)>0)
			{
				if (AddLong(sIdenti, strlen(sIdenti), sDat, &iTam)==0)
				{
					iRet = SockTCPIP.Write(sDat, iTam);
					if (iRet)
					{
						tcpLastError = -3;
					}
				}
				else
				{
					tcpLastError = -4;
				}
			}
		}
	}
	if (tcpLastError)
		sysLastError = SockTCPIP.GetErrno();
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TCP::Close                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra la conexion TCP/IP P                                |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int TCP::Close()
{
	int iRet;

	sysLastError = 0;
	tcpLastError = 0;
	iRet = SockTCPIP.Shutdown(SHUT_RDWR);
	if (iRet)
	{
		tcpLastError = -5;
	}
	else
	{
		iRet = SockTCPIP.Close();
		if (iRet)
		{
			tcpLastError = -6;
		}
	}
	if (tcpLastError)
		sysLastError = SockTCPIP.GetErrno();
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TCP::Write                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Escribe un mensaje sobre TCP/IP                            |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int TCP::Write(char *sMensaj, int iTamMsj)
{
	int iRet = -1;

	sysLastError = 0;
	tcpLastError = 0;
	if (AddLong(sMensaj, iTamMsj, sDat, &iTam)==0)
	{
		iRet = SockTCPIP.Write(sDat, iTam);
		if (iRet)
		{
			tcpLastError = -7;
			sysLastError = SockTCPIP.GetErrno();
		}
	}
	else
	{
		tcpLastError = -8;
	}
	if (tcpLastError)
		sysLastError = SockTCPIP.GetErrno();
	return iRet;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TCP::Read                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Lee un mensaje sobre TCP/IP                                |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int TCP::Read(char *sMensaj, int *iTamMsj, int *iTypMsj)
{
	int iTam;
	int iRet;
	char sCab[16];
	unsigned short iLon;

	*iTypMsj = 0;
	*iTamMsj = 0;
	sysLastError = 0;
	tcpLastError = 0;
	
	memset(sCab, 0, sizeof(sCab));
	iRet = SockTCPIP.Read(sCab, CAB_TCP);
	if (iRet > 0)
	{
		memcpy(&iLon, sCab, CAB_TCP);
		iTam = (int) ( (iProt==SIN_CAB) ? iLon : (iLon-CAB_TCP) );
		if ( (iTam>0) && (iTam<=MAX_TAM_ISO) )
		{
			iRet = SockTCPIP.Read(sMensaj, iTam);
			if (iRet>0)
			{
				*iTamMsj = iRet;	
				iRet = 0;
			}
			else
			{
				tcpLastError = -9;
			}
		}
		else
		{
			tcpLastError = -10;
		}
	}
	else
	{
		tcpLastError = -11;
	}
	if (tcpLastError)
		sysLastError = SockTCPIP.GetErrno();
	return ( (tcpLastError ? -1 : 0) );
}
