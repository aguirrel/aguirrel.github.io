#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef CACS_H
#define CACS_H

#include "Semaphores.h"

#ifndef OK
    #define OK			0
#endif
#ifndef NOOK
    #define NOOK		-1
#endif

#define MAX_CAS			8

#define STAT_DOWN		0x01	// 0b00000001
#define STAT_CIERRE		0x02	// 0b00000010
#define STAT_PING		0x04	// 0b00000100

typedef struct
{
    int		flag;
    int		nro_suc;
    int		nro_caj;
} str_cacs;

class CACStatus
{
private:
protected:
    str_cacs	*cac_status;
    int		ShMemHandle;
    int		creator;
    int		ret;
    int		ErrNo;
    int		aux;

    Semaphore	Sem;

public:
    CACStatus();
    ~CACStatus();

    int Create(long id, int flags);
    int Create(long id);
    int Open(long id);
    int Close();
    int Destroy();

    int Down(int channel=0);
    int Up(int channel=0);
    int Cierre(int channel=0, int nro_suc=0, int nro_caj=0);
    int Normal(int channel=0);
    int Ping(int channel=0);
    int NotPing(int channel=0);
    int IsDown(int channel=0);
    int IsUp(int channel=0);
    int IsCierre(int channel=0);
    int IsNormal(int channel=0);
    int IsPing(int channel=0);
    int IsNotPing(int channel=0);
    int GetSuc(int channel=0);
    int GetCaj(int channel=0);
    int GetState(int channel=0);
    int ResetState(int channel, int new_status);
    int SetState(int channel, int new_status);

    int GetErrno();
};

#endif

