#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef SOCKET_H
#define SOCKET_H

#include "Includes.h"
#ifndef OK
#define OK		0
#define NOOK		-1
#endif

class Socket
{
protected:
   int			SocketHandle;
   int			Errno;
   int			ret;
   struct sockaddr_in	local_addr;
   struct sockaddr_in	remote_addr;

public:
   Socket();
   Socket(int new_socket_handle, struct sockaddr_in new_local_addr,
      struct sockaddr_in new_remote_addr);

   int Open();
   int Connect(char *address, long port);
   int Connect(char *address, long port, int time);
   int Listen(char *address, long port);
   int ListenAny(long port);
   Socket *Accept();
   int Write(char *data, int len);
   int Read(char *data, int len);
   int Close();
   int Shutdown(int);
   int GetLocalPort();
   int GetRemotePort();
   char *GetLocalAddress();
   char *GetRemoteAddress();
   int GetErrno();
};

#endif
