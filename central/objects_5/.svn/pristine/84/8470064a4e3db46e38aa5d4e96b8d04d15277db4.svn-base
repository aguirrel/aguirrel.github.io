#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef INCLUDES_H
#define INCLUDES_H

/* Headers dependientes del compilador	*/
#ifdef OS_Linux
    #define UNIX
    #include <iostream.h>
#endif

#ifdef OS_AIX
    #define UNIX
    #include <iostream>
#else
	#ifdef OS_Linux
		#define UNIX
		#include <iostream.h>

		#ifdef __cplusplus
			extern "C" 
			{
		#endif
		#ifdef __cplusplus
		}
		#endif
	#endif
#endif

#ifdef __cplusplus
    extern "C" {		/* Se debe definir asi ya que los headers */
#endif				/* de AIX no detectan automaticamente c++ */

#ifdef UNIX
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
    #include <sys/sem.h>
    #include <sys/shm.h>
    #include <sys/socket.h>
    #include <sys/wait.h>
    #include <unistd.h>
#ifdef OS_AIX
    #include <mp.h>
#endif
#endif

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
}
#endif

#endif
