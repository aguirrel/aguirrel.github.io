#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | PidTable.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto para el manejo de la tabla de PIDs                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Config.h"
#include "Daemons.h"
#include "Includes.h"
#include "PidTable.h"
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::PidTable                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
PidTable::PidTable()
{
   ErrNo=OK;
   ShMemHandle=NOOK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::PrintTable                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Imprime la tabla de pids                                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void PidTable::PrintTable()
{
   int	cont;

   for (cont=0; cont<MaxEntries; cont++)
   {
		printf("Cont [%02d] Pid [%05d]", cont, PidList[cont].pid);
		if (PidList[cont].used)
		{
			ret = waitpid(PidList[cont].pid , NULL,WNOHANG);
			printf(" %s [%d]\n", ret == -1 ? "Is Dead" : "Is Alive",errno);
		}
		else
			printf(" Is Unused\n");
   }
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::Create                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea la tabla de PIDs                                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::Create(long key, int perms, int max_entries)
{
	MaxEntries=max_entries;
	ShMemHandle=shmget(key, sizeof(str_pid_list)*MaxEntries, 
	perms|IPC_CREAT/*|IPC_EXCL*/);
	if (ShMemHandle==-1)
	{
		ErrNo=errno;
		return NOOK;
	}

	PidList=(str_pid_list *)shmat(ShMemHandle, 0, 0);
	if (PidList==NULL)
	{
		ErrNo=errno;
		return NOOK;
	}

	ret=Sem.Create(key, perms);
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

	memset((char *)PidList, 0, sizeof(str_pid_list)*MaxEntries);

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
| FUNCION         | PidTable::Open                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Abre la tabla de PIDs                                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::Open(long key, int max_entries)
{
	MaxEntries=max_entries;
	ShMemHandle=shmget(key, sizeof(str_pid_list)*MaxEntries, 0);
	if (ShMemHandle==-1)
	{
		ErrNo=errno;
		return NOOK;
	}

	PidList=(str_pid_list *)shmat(ShMemHandle, 0, 0);
	if (PidList==NULL)
	{
		ErrNo=errno;
		return NOOK;
	}

	ret=Sem.Open(key);
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
| FUNCION         | PidTable::PutPid                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Pone un pid en la tabla de PIDs                            |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::PutPid(int pid)
{
	int	cont;

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	for (cont=0; cont<MaxEntries; cont++)
		if (PidList[cont].used==0)
			break;

	if (cont<MaxEntries)
		if (PidList[cont].used==0)
		{
			PidList[cont].used=1;
			PidList[cont].pid=pid;
			ret=Sem.Reset();
			if (ret==NOOK)
			{
				ErrNo=Sem.GetErrno();
				return NOOK;
			}
			ErrNo=OK;
			return OK;
		}

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	for (cont=30; cont<MaxEntries; cont++)
		RemPid(PidList[cont].pid);

	ErrNo=ENOSPC;
	return NOOK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::RemPid                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Remueve un pid en la tabla de PIDs                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::RemPid(int pid)
{
	int	cont;

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	for (cont=0; cont<MaxEntries; cont++)
		if ((PidList[cont].used==1) && (PidList[cont].pid==pid))
			break;

	if ((PidList[cont].used==1) && (PidList[cont].pid==pid))
	{
		memset(&PidList[cont], 0, sizeof(str_pid_list));
		ret=Sem.Reset();
		if (ret==NOOK)
		{
			ErrNo=Sem.GetErrno();
			return NOOK;
		}
		ErrNo=OK;
		return OK;
	}
	else
	{
		ret=Sem.Reset();
		if (ret==NOOK)
		{
			ErrNo=Sem.GetErrno();
			return NOOK;
		}
		ErrNo=ENOENT;
		return NOOK;
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::KillAll                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Remueve todos los pids en la tabla de PIDs                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::KillAll()
{
	int	cont;

	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	for (cont=0; cont<MaxEntries; cont++)
	{
		if (PidList[cont].used==1)
			kill(PidList[cont].pid, SIGTERM);
	}

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::Close                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra la tabla de PIDs                                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::Close()
{
	shmdt((char *)PidList);
	Sem.Close();
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::Destroy                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruye la tabla de PIDs                                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::Destroy()
{
	shmdt((char *)PidList);
	shmctl(ShMemHandle, IPC_RMID, NULL);
	Sem.Destroy();
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | PidTable::GetErrno                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el codigo de error de la tabla de PIDs             |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int PidTable::GetErrno()
{
   return ErrNo;
}
