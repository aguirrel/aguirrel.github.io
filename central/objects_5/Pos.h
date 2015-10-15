#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef POS_H
#define POS_H

#include "Includes.h"

#define POS_MAX_MEMBERS	      1000
#define NRO_CA_LEN	          2
#define NRO_CAJ_LEN	          5
#define NRO_SUC_LEN			  3

class Pos
{
protected:
    int	Errno;
    struct
    {
		char nro_ca [NRO_CA_LEN+1];
		char nro_caj[NRO_CAJ_LEN+1];
		char nro_suc[NRO_SUC_LEN+1];
		long lTime;
    } pos[POS_MAX_MEMBERS];

public:
    Pos();
    ~Pos();

    int	SetPosBusy(char *ca, char *suc, char *caj);
    int	SetPosFree(char *ca, char *suc, char *caj);
    int	IsPosFree(char  *ca, char *suc, char *caj);
    int	GetErrno();
};

#endif
