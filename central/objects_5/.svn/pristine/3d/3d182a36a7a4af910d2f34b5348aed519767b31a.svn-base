#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef TRX_HEAP_H
#define TRX_HEAP_H

#include "Includes.h"

#define TRXHEAP_MAX_MEMBERS	1000
#define MAXKEY                    90

class TrxHeap
{
protected:
    int	Errno;
    struct
    {
	char            key[MAXKEY];
	int		data_size;
	char		*data;
    } heap[TRXHEAP_MAX_MEMBERS];

public:
    TrxHeap();
    ~TrxHeap();

    int	PutData(char * key, char *data, int data_size);
    int	GetData(char * key, char *data, int max_len);
    int	FindData(char * key, char *data, int max_len);
    int	GetErrno();
};

#endif
