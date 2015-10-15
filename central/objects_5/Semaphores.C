#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphores.C                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto para el manejo de semaforos                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Semaphores.h"

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::Semaphore                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del semaforo                                   |
|                 |	                                                           | 
+-----------------+------------------------------------------------------------+
*/
Semaphore::Semaphore()
{
	SemHandle=-1;
	ErrNo=0;
	return;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::Create                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea el semaforo                                           |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Semaphore::Create(long id, int flags)
{
	SemID=id;
	SemHandle=semget(SemID, 1, flags | IPC_CREAT /*| IPC_EXCL*/);
	if (SemHandle==-1)
	{
		ErrNo=errno;
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::Open                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Realiza la apertura del semaforo                           |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Semaphore::Open(long id)
{
	SemID=id;
	SemHandle=semget(SemID, 0, 0);
	if (SemHandle==-1)
	{
		ErrNo=errno;
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::Set                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Enciende el semaforo                                       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Semaphore::Set()
{
	ret=semop(SemHandle, &op_set[0], 2);

	if (ret==-1)
	{
		ErrNo=errno;
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}
/*
int Semaphore::Wait()
{
	ret=semop(SemHandle, &op_wait[0], 1);
	if (ret==-1)
	{
		ErrNo=errno;
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}*/
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::Reset                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Libera el semaforo                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Semaphore::Reset()
{
	ret=semop(SemHandle, &op_reset[0], 1);
	if (ret==-1)
	{
		ErrNo=errno;
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::Close                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra el semaforo                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Semaphore::Close()
{
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::Destroy                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruye el semaforo                                       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Semaphore::Destroy()
{
	ret=semctl(SemHandle, 0, IPC_RMID, 0);
	if (ret==-1)
	{
		ErrNo=errno;
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Semaphore::GetErrno                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el codigo de error del semaforo                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Semaphore::GetErrno()
{
   return ErrNo;
}
