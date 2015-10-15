#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef LINKCVC_H
#define LINKCVC_H

#include "Includes.h"
#include "Status.h"

#include <x25sdefs.h>
#include <NLchar.h>
#include <Sockets.h>

#ifndef OK
	#define OK          0
	#define NOOK        -1
#endif

#define OK1				1
#define OK2				2
#define OK3				3
#define OK4				4

#define MAX_SVC			50
#define TYPE_MSG_CONF	99 
#define MAX_TAM_ISO		500

class CVC
{
public:
	virtual int  Open(char*, char*, char*);
	virtual int  Close();
	virtual int  Write(char *data, int len);
	virtual int  Read(char *data, int *len, int *tipo);
	virtual int  MsgPnd();
	virtual	int  setCodUserData(char data[2][11]);
	virtual int  GetErrno();
	virtual int  GetErSys();
};

typedef struct
{
   char				NUA[30];
   char				NUP[30];
   char				IDE[30];
   pthread_mutex_t	lock_status;
   pthread_cond_t	cond_status;
   CVC				*SVC;
   int				CodigoCa;
   int				flag_tpdu;
   int				flag_CUData;
   char				CallUserData[2][11];
   CaStatus			Status;
   Semaphore		SemLector;
   long				RQueueID,RQueuePerm;
   long				WQueueID,WQueuePerm;
   int				idCaLogi;
   char				NumberTPDU[11];
   int				OnLineLector;
   time_t			tLastTime;
}  StrArgThread;

typedef struct 
{
   pthread_t		Lector;
   pthread_t		Escritor;
   StrArgThread		Argumentos;
   int				libre;
} StrCaLogi;

#endif
