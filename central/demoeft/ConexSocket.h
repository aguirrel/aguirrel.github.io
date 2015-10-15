#ifdef WIN_32
#include "windows.h"
#endif

#define CONEXION     unsigned int
#define CONEXION_INVALIDA   ~(0)
#define SOCKET_ERROR		-1

class ConexSocket
{
private:
	CONEXION	sockServ;
	CONEXION	sockClient;

public:
	ConexSocket();
	CONEXION OpenServer(unsigned short);
	CONEXION OpenClient(char *szDirIP, unsigned short port);

	void CloseServer();
	void CloseClient();

	int SendMsg(char *mensa, int length);
	CONEXION Accept();
	CONEXION RecvMsg(/*CONEXION sock, */char *msg, int *length);
	char *GetIpClient();
};
