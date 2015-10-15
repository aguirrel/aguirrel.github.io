#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Sockets.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtjeto para el manejo de conexiones TCP/IP mediante       |
|                 | sockets                                                    |
+-----------------+------------------------------------------------------------+
*/
#include "Sockets.h"

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Socket                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del socket                                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
Socket::Socket()
{
	SocketHandle=-1;
	Errno=0;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Socket                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del socket                                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
Socket::Socket(int new_socket_handle, struct sockaddr_in new_local_addr,
			   struct sockaddr_in new_remote_addr)
{
	SocketHandle=new_socket_handle;
	local_addr=new_local_addr;
	remote_addr=new_remote_addr;
	Errno=0;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Open                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Apertura del socket                                        |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::Open()
{
	if ((SocketHandle=socket(AF_INET, SOCK_STREAM, 0))==NOOK)
	{
		Errno=errno;
		return NOOK;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Connect                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Conexion del socket                                        |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::Connect(char *address, long port)
{
	int len=sizeof(local_addr);

	remote_addr.sin_family		= AF_INET;
	remote_addr.sin_addr.s_addr	= inet_addr(address);
	remote_addr.sin_port		= htons(port);

	ret=connect(SocketHandle, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
	if (ret==NOOK)
	{
		Errno=errno;
		return NOOK;
	}

	ret=getsockname(SocketHandle, (struct sockaddr *)&local_addr, (size_t *)&len);
	if (ret==NOOK)
	{
		Errno=errno;
		return NOOK;
	}

	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Connect                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Conexion del socket con timeout. El connect retorna        |
|                 | inmediatamente y la espera se realiza en el select         |
+-----------------+------------------------------------------------------------+
*/
int Socket::Connect(char *address, long port, int time)
{
    fd_set fdset;
    struct timeval tv;
	int	   iFlags;
	int	   iError;
	socklen_t len;

	iFlags = fcntl(SocketHandle, F_GETFL, 0);
	if ( iFlags >= 0 )
	{
		iFlags = iFlags | O_NONBLOCK;
		if (fcntl(SocketHandle, F_SETFL, iFlags) >= 0)
		{
			remote_addr.sin_family      = AF_INET;
			remote_addr.sin_addr.s_addr = inet_addr(address);
			remote_addr.sin_port        = htons(port);

			connect(SocketHandle, (struct sockaddr *)&remote_addr, sizeof(remote_addr));

			FD_ZERO(&fdset);
			FD_SET(SocketHandle, &fdset);
			tv.tv_sec = time;
			tv.tv_usec = 0;

			if (select(SocketHandle + 1, NULL, &fdset, NULL, &tv) == 1)
			{
				len = sizeof(iError);
				if (getsockopt(SocketHandle, SOL_SOCKET, SO_ERROR, &iError, &len) == 0)
				{
					if (iError == 0) 
					{
						len = sizeof(local_addr);
						if (getsockname(SocketHandle, (struct sockaddr *)&local_addr, (size_t *)&len) == 0)
						{
							iFlags = iFlags & ~O_NONBLOCK;
							if (fcntl(SocketHandle, F_SETFL, iFlags) >= 0)
							{
								Errno = 0;
								return OK;
							}
						}
					}
				}
			}
		}
	}
	Errno = errno;
	return NOOK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Listen                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Listen sobre el socket                                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::Listen(char *address, long port)
{
	local_addr.sin_family		=AF_INET;
	local_addr.sin_addr.s_addr	=inet_addr(address);
	local_addr.sin_port			=htons(port);

	ret=bind(SocketHandle, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if (ret==NOOK)
	{
		Errno=errno;
		return NOOK;
	}
	
	ret=listen(SocketHandle, 5);
	if (ret==NOOK)
	{
		Errno=errno;
		return NOOK;
	}

	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::ListenAny                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Listen sobre el socket                                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::ListenAny(long port)
{
	remote_addr.sin_family		 = AF_INET;
	remote_addr.sin_addr.s_addr	 = htonl(INADDR_ANY);
	remote_addr.sin_port		 = htons(port);

	ret=bind(SocketHandle, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
	if (ret==NOOK)
	{
		Errno=errno;
		return NOOK;
	}
	
	ret=listen(SocketHandle, 5);
	if (ret==NOOK)
	{
		Errno=errno;
		return NOOK;
	}

	return OK;   
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Accept                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Accept sobre el socket                                     |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
Socket *Socket::Accept()
{
	int	clilen=sizeof(remote_addr);

	ret=accept(SocketHandle, (struct sockaddr *)&remote_addr, (size_t *)&clilen);
	if (ret==NOOK)
	{	
		Errno=errno;
		return NULL;
	}

	return (new Socket(ret, local_addr, remote_addr));
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Write                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Escritura sobre el socket                                  |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::Write(char *data, int len)
{
	ret=write(SocketHandle, data, len);
	if (ret==NOOK)
	{
		Errno=errno;
		return NOOK;
	}
	
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Read                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Lectura sobre el socket                                    |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::Read(char *data, int len)
{
	int		nleft;

	nleft=len;
	while (nleft>0)
	{
		ret=read(SocketHandle, data, nleft);

		if ((ret==NOOK) && (errno!=EWOULDBLOCK))
		{
			Errno=errno;
			return NOOK;
		}
		else
			if (ret<=0)
				break;
		nleft-=ret;
		data+=ret;
	}

	return (len-nleft);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::GetErrno                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Retorno del error del socket                               |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::GetErrno()
{
	return Errno;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Close                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra el socket                                           |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::Close()
{
	ret=close(SocketHandle);
	if (ret!=OK)
	{
		Errno=errno;
		return NOOK;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::Shutdown                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Libera el socket                                           |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::Shutdown(int iTip)
{
	ret=shutdown(SocketHandle, iTip);
	if (ret!=OK)
	{
		Errno=errno;
		return NOOK;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::GetLocalPort                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el puerto local                                    |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::GetLocalPort()
{
	return ntohs(local_addr.sin_port);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::GetRemotePort                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el puerto remoto                                   |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
int Socket::GetRemotePort()
{
	return ntohs(remote_addr.sin_port);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::GetLocalAddress                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene la direccion local                                 |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
char *Socket::GetLocalAddress()
{
	return inet_ntoa((struct in_addr)local_addr.sin_addr);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Socket::GetRemoteAddress                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene la direccion remota                                |
|                 |                                                            |
+-----------------+------------------------------------------------------------+
*/
char *Socket::GetRemoteAddress()
{
	return inet_ntoa((struct in_addr)remote_addr.sin_addr);
}

