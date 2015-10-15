#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TrxHeap.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto para el manejo del Heap de datos                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Log.h"
#include "TrxHeap.h"

#define OK			0
#define NOOK		-1

extern Log	LogAlarm;

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TrxHeap::TrxHeap                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TrxHeap::TrxHeap()
{
    int	cont;
   
    Errno=OK;
    for (cont=0; cont<TRXHEAP_MAX_MEMBERS; cont++)
    {
		heap[cont].key[0]='\0';
		heap[cont].data_size=0;
		heap[cont].data=NULL;
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TrxHeap::TrxHeap                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destructor del objeto                                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TrxHeap::~TrxHeap()
{
    int	cont;
   
    for (cont=0; cont<TRXHEAP_MAX_MEMBERS; cont++)
    {
		if (heap[cont].data!=NULL)
			free(heap[cont].data);
    }
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TrxHeap::PutData                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Pone un registro a la estructura                           |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TrxHeap::PutData(char * key, char *data, int data_len)
{
	int	cont=0;

	/* Verifica tama#o de la clave */ 
	if (strlen(key)>=MAXKEY)
	{
		Errno=E2BIG;
		return NOOK;
	}

	/* Busca el siguiente disponible */
	while ((heap[cont].key[0]!='\0') && (cont<TRXHEAP_MAX_MEMBERS))
		cont++;
	
	if (heap[cont].key[0]=='\0')
	{
		if ((heap[cont].data=(char *)malloc(data_len))==NULL)
		{
			Errno=ENOMEM;
			return NOOK;
		}
	
		memcpy(heap[cont].data, data, data_len);
		heap[cont].data_size=data_len;
		strcpy(heap[cont].key,key);
		LogAlarm.Put(3, "TrxHeap::PutData(): key: [%s]\n", heap[cont].key);
		LogAlarm.Put(90, "TrxHeap::PutData(): posicion: %d tama#o: %d\n", cont, heap[cont].data_size); 
		Errno=OK;
		return OK;
	}
	Errno=ENOSPC;
	return NOOK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TrxHeap::GetData                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Saca un registro de la estructura                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TrxHeap::GetData(char * key, char *data, int max_len)
{
	int	cont=0;
	int	len;

	/* Busqueda de la clave */  
	while ((strcmp(heap[cont].key,key)!=0) && (cont<TRXHEAP_MAX_MEMBERS))
	{
		if (heap[cont].key[0]!='\0')
		{
			LogAlarm.Put(90, "TrxHeap::GetData(): key buscado: [%s]\n", key);
			LogAlarm.Put(90, "TrxHeap::GetData(): posicion: %d ==> key: [%s]\n", cont, heap[cont].key);
		}
		cont++;
	}

	if (strcmp(heap[cont].key,key)==0)
	{
		if (heap[cont].data_size>max_len)
		{
			LogAlarm.Put(0, "TrxHeap::GetData(): Error en size de key:[%s] (%d vs max %d)\n", 
								key,heap[cont].data_size,max_len);
			Errno=E2BIG;
			return NOOK;
		}	
		LogAlarm.Put(3, "TrxHeap::GetData(): key: [%s]\n", heap[cont].key);

		memcpy(data, heap[cont].data, heap[cont].data_size);
		len=heap[cont].data_size;
		free(heap[cont].data);
		heap[cont].data=NULL;
		heap[cont].data_size=0;
		heap[cont].key[0]='\0';

		Errno=OK;
		return len;
	}
	Errno=ENOENT;
	return NOOK;  
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TrxHeap::FindData                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Busca el dato de la transaccion. Retorna distinto de 1 si  |
|                 | se encontro la clave o 1 si no se encontro                 | 
+-----------------+------------------------------------------------------------+
*/
int TrxHeap::FindData(char * key, char *data, int max_len)
{
    int	cont=0;
    int	len;
    int ret;

    /* Busqueda de la clave */  
    while ((strcmp(heap[cont].key,key)!=0) && (cont<TRXHEAP_MAX_MEMBERS))
    {
        if (heap[cont].key[0]!='\0')
        {
            LogAlarm.Put(90, "TrxHeap::FindData(): key buscado: [%s]\n", key);
            LogAlarm.Put(90, "TrxHeap::FindData(): posicion: %d ==> key: [%s]\n", cont, heap[cont].key);
        }
        cont++;
    }
	ret = (cont==TRXHEAP_MAX_MEMBERS);
	LogAlarm.Put(90, "TrxHeap::FindData(): Valor retornado %ld \n", ret);
    return ret;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TrxHeap::GetErrno                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Retorna el codigo de error                                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int TrxHeap::GetErrno()
{
    return Errno;
}

