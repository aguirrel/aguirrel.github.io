#ifndef LINKTCP_H
#define LINKTCP_H

#include "LinkCVC.h"

#define	CAB_TCP		2	// Tamaño de la cabecera de mensaje

#define CON_CAB		1	// Longitud de mensaje si incluye cabecera: Msje 100 bytes - Cabecera 102 bytes.
#define SIN_CAB		2	// Longitud de mensaje no incluye cabecera: Msje 100 bytes - Cabecera 100 bytes.

class TCP : public CVC
{
protected:
	Socket  SockTCPIP;
    int	    tcpLastError;
    int		sysLastError;
	int		iTOut;
	int		iProt;

public:
	TCP() {};
	TCP(char*, int, int);
	int  Open(char*, char*, char*);
	int  Close();
	int  Write(char*, int);
	int  Read(char*, int*, int*);
	int  GetErrno()					{return tcpLastError;}
	int  GetErSys()					{return sysLastError;}

private:
	int iTam;
	char sDat[2048];

	int AddLong(char*, int, char*, int*);
};

#endif