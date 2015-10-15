#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | QChild.C                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Manejo del objeto para obtener la cantidad de hijos en     |
|                 | ejecucion en el TCPDaemon                                  | 
+-----------------+------------------------------------------------------------+
*/
#include "QChild.h"
#include "Semaphores.h"
#include "Config.h"
#include "Daemons.h"

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::QChild                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
QChild::QChild()
{
	ErrNo=OK; 
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::~QChild                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destructor del objeto                                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
QChild::~QChild()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::Create                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea el objeto en memoria compartida                       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int QChild::Create(long SemKey, int perms)
{
	int ret;

	ShMemHandle=shmget(SemKey, sizeof(int), perms | IPC_CREAT);
	if (ShMemHandle==-1)
	{
		return NOOK;
	}

	qchild=(int *)shmat(ShMemHandle, 0, 0);
	if (qchild == NULL)
	{
		ErrNo=errno;
		return NOOK;
	}


	ret=Sem.Create(SemKey, perms);
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	*qchild=0;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::Close                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra el objeto                                           |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int QChild::Close()
{
    shmdt((int *)qchild);
    Sem.Close();
    ErrNo=OK;
    return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::Destroy                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruye el objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int QChild::Destroy()
{
	shmdt((int *)qchild);
	shmctl(ShMemHandle, IPC_RMID, NULL);
	Sem.Destroy();
	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::Open                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Abre el objeto                                             |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int QChild::Open(long SemKey)
{
	int  ret;

	ShMemHandle=shmget(SemKey, sizeof(int), 0);
	ErrNo=(ShMemHandle==NOOK? errno:OK);

	if (ShMemHandle==-1)
	{ 
		ErrNo=errno; 
		return NOOK;
	}

	qchild=(int *)shmat(ShMemHandle, 0, 0);
	if (qchild == NULL)
	{
		ErrNo=errno;
		return NOOK;
	}

	ret=Sem.Open(SemKey);
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::SetChild                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea la cantidad de hijos a cero                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int QChild::SetQChild()
{
	int  ret;

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	*qchild=0;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}
	ErrNo=OK;
	return OK;
}   
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::IncChild                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Incrementa en uno la cantidad de hijos                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/ 
int QChild::IncQChild()
{
	int  ret;
	int  aux;

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	(*qchild)++;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ErrNo=OK;
	return OK;
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::DecChild                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Decrementa en uno la cantidad de hijos                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int QChild::DecQChild()
{
	int  ret;

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	(*qchild)--;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ErrNo=OK;
	return OK;
} 
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | QChild::GetChild                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene la cantidad de hijos                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int QChild::GetQChild()
{
	int  cantidad;
	int  ret;

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	cantidad=(*qchild);

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ErrNo=OK;
	return cantidad;
}
