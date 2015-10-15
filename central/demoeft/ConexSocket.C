#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#ifndef WIN_32
#include "unistd.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "netdb.h"
#endif

#include "ConexSocket.h"
#define SO_DONTLINGER	0
/***************************************************************/
/*                          ATENCION:                          */ 
/* En Windows, agregar en el proyecto la libreria "ws2_32.lib" */
/***************************************************************/ 

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ConexSocket                                                       
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea la entidad preparando las funciones de socket   
|                 |                                                            
+-----------------+------------------------------------------------------------+
*/
ConexSocket::ConexSocket()
{
#ifdef WIN_32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		return;
	}
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup( );
		return;
	}
#endif
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | OpenServer                                                       
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea un socket para escuchar peticiones como un servidor   
|                 |                                                            
+-----------------+------------------------------------------------------------+
*/
CONEXION ConexSocket::OpenServer(unsigned short port)
{
#ifdef WIN_32
	SOCKADDR_IN sin;
#else
	struct sockaddr_in sin;
#endif

//	CONEXION retsock=(CONEXION)CONEXION_INVALIDA;

	if ((sockServ=socket(AF_INET, SOCK_STREAM,0))==CONEXION_INVALIDA) 
	{
		#ifdef WIN_32
		WSACleanup();
		#endif
		return (CONEXION)CONEXION_INVALIDA;
	}
	memset(&sin,'\0',sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_port=htons(port);
	sin.sin_addr.s_addr=htonl(INADDR_ANY);

	if (bind(sockServ,(struct sockaddr *)&sin,sizeof(sin))==SOCKET_ERROR) 
	{
		if (sockServ!=CONEXION_INVALIDA)
		{
			CloseServer(/*retsock*/);
		}
		#ifdef WIN_32
		WSACleanup();
		#endif
		return (CONEXION)CONEXION_INVALIDA;
	}
	if (listen(sockServ,5)==-1) 
	{
		if (sockServ!=CONEXION_INVALIDA)
		{
			CloseServer();
		}
		#ifdef WIN_32
		WSACleanup();
		#endif
		return (CONEXION)CONEXION_INVALIDA;
	}
	return sockServ;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CloseServer        
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra el socket solicitado                                
|                 |                                                            
+-----------------+------------------------------------------------------------+
*/
void ConexSocket::CloseServer()
{
	int linger=0;
	if (sockServ!=(CONEXION)CONEXION_INVALIDA) 
	{
		#ifdef WIN_32
		setsockopt(sockServ,SOL_SOCKET,SO_DONTLINGER,(const char *)&linger,sizeof(int));
		closesocket(sockServ);
		#else
		setsockopt(sockServ,SOL_SOCKET,SO_DONTLINGER,(const char *)&linger, sizeof(int));
		close(sockServ);
		#endif
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CloseClient        
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra el socket solicitado                                
|                 |                                                            
+-----------------+------------------------------------------------------------+
*/
void ConexSocket::CloseClient()
{
	int linger=0;
	if (sockClient!=(CONEXION)CONEXION_INVALIDA) 
	{
		#ifdef WIN_32
		setsockopt(sockClient,SOL_SOCKET,SO_DONTLINGER,(const char *)&linger,sizeof(int));
		closesocket(sockClient);
		#else
		setsockopt(sockClient,SOL_SOCKET,SO_DONTLINGER,(const char *)&linger,sizeof(int));
		close(sockClient);
		#endif
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | OpenClient
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea un socket como un cliente para enviar peticiones a    
|                 | algun proceso de servidor                                  
+-----------------+------------------------------------------------------------+
*/
CONEXION ConexSocket::OpenClient(char *szDirIP, unsigned short port)
{
#ifdef WIN_32
	SOCKADDR_IN sin;
	PHOSTENT hp;
#else
	struct sockaddr_in sin;
	struct hostent *hp;
#endif
	char servidor[256];
	int rec,iCont,espera;
	int iReintentos= 3;

	if (strcmp(szDirIP, "LOCAL")==0)
	{
		if (gethostname(servidor,255)==SOCKET_ERROR) 
		{
			return (CONEXION)CONEXION_INVALIDA;
		}	
	}
	else
	{
		sprintf(servidor, szDirIP);
	}
	if ((hp=gethostbyname(servidor))==NULL) 
	{
		return (CONEXION)CONEXION_INVALIDA;
	}
	memset(&sin,'\0',sizeof(sin));
#ifdef WIN_32
	memcpy((char FAR *)&(sin.sin_addr), hp->h_addr, hp->h_length);
#else
	memcpy((char*)&sin.sin_addr, hp->h_addr, hp->h_length);
#endif
	sin.sin_family=AF_INET;
	sin.sin_port=htons(port);
	if ((sockClient=socket(AF_INET, SOCK_STREAM,0))==SOCKET_ERROR) 
	{
		return (CONEXION)CONEXION_INVALIDA;
	}
	srand((unsigned int)time(NULL));
	for (iCont=0;iCont<iReintentos;iCont++) 
	{
		if ((rec=connect(sockClient,(struct sockaddr *)&sin,sizeof(sin)))!= SOCKET_ERROR) 
			break;
		else 
		{
			espera=(int)((rand()/RAND_MAX)*50);
			#ifdef WIN_32
				Sleep((DWORD)espera);
			#else
				sleep(1);
			#endif
		}
	}
	if (rec==SOCKET_ERROR) 
	{
		CloseClient();
		return (CONEXION)CONEXION_INVALIDA;
	}
	return sockClient;
}

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | SendMsg
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Envia un mensaje de determinada longitud por el socket     
|                 | solicitado                                                 
+-----------------+------------------------------------------------------------+
*/
int ConexSocket::SendMsg(char *mensa, int length)
{
	if (send((CONEXION)sockClient,mensa,length,0)!=length) 
	{
		return -1;
	}
	return 0;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Accept
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Espera la llegada de un mensaje de algun cliente           
|                 |                                                            
+-----------------+------------------------------------------------------------+
*/
CONEXION ConexSocket::Accept()
{    
/*	CONEXION ns=(CONEXION)CONEXION_INVALIDA;*/
//	char szIp[64];
//	sockaddr remote_addr;
//	int	clilen=sizeof(remote_addr);

//	ret=accept(SocketHandle, (struct sockaddr *)&remote_addr, (size_t *)&clilen);


//	if ((sockClient=accept(sockServ,(struct sockaddr *)&remote_addr, &clilen))==CONEXION_INVALIDA ) 
	if ((sockClient=accept(sockServ,NULL,NULL))==CONEXION_INVALIDA ) 
	{
		return (CONEXION)CONEXION_INVALIDA;
	}
	else
	{	
//		memset(szIp, 0, sizeof(szIp));
//		sprintf(szIp, inet_ntoa((struct in_addr)remote_addr.s_addr.sin_addr));;
		return sockClient;
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | RecvMsg
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Recibe un mensaje de determinada longitud por el socket    
|                 | solicitado. Devuelve la longitud del mensaje               
+-----------------+------------------------------------------------------------+
*/
CONEXION ConexSocket::RecvMsg(/*CONEXION sock,*/ char *msg, int *length)
{
	int ret;
	struct timeval tv;
	fd_set socklist;
	char buf[4096];
	int retornar = 0;
	int iLongEnvio;

	if (retornar) 
	{
		tv.tv_sec=0;
		tv.tv_usec=0;

		FD_ZERO(&socklist);
		FD_SET(sockClient,&socklist);

		ret=select(0,&socklist,NULL,NULL,&tv);
		if (ret==-1) 
		{
			CloseClient();
			return (CONEXION)CONEXION_INVALIDA;
		}
		if (!ret) 
		{
			*length=0;*msg='\0';
			CloseClient();
			return (CONEXION)CONEXION_INVALIDA;
		}
	}

	iLongEnvio = *length;
	if ((*length=recv(sockClient,buf,iLongEnvio,0))==-1) 
	{
		CloseClient();
		return (CONEXION)CONEXION_INVALIDA;
	}
	if (*length==0) 
	{
		*msg='\0';
		CloseClient();
		return (CONEXION)CONEXION_INVALIDA;
	}
	memcpy(msg,buf,*length);
	return ((CONEXION) sockClient);
}

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | GetIpClient
+-----------------+------------------------------------------------------------+
| DESCRIPCION     |     
|                 | 
+-----------------+------------------------------------------------------------+
*/
char *ConexSocket::GetIpClient()
{
	socklen_t iTam;
	static char szDir[24];
	struct sockaddr_in sin;

	iTam = sizeof(sin);
	getpeername(sockClient, (struct sockaddr *)&sin, &iTam);
	sprintf(szDir, "%s", inet_ntoa(sin.sin_addr));
	return szDir;
}
