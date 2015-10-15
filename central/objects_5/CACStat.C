#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | CACStat.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto para el manejo del estado de los centros            |
|                 | autorizadores                                              | 
+-----------------+------------------------------------------------------------+
*/
#include "Includes.h"
#include "CACStat.h"
#include "Semaphores.h"
#include "Config.h"
#include "Daemons.h"

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::CACStatus                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
CACStatus::CACStatus()
{
	ErrNo=OK;
	ShMemHandle=NOOK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::~CACStatus                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destructor del objeto                                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
CACStatus::~CACStatus()
{
	Close();
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Create                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Create(long id, int flags)
{
	ShMemHandle=shmget(id, sizeof(str_cacs)*MAX_CAS, flags|IPC_CREAT/*|
	IPC_EXCL*/);
	if (ShMemHandle==-1)
	{
		ErrNo=errno;
		return NOOK;
	}

	cac_status=(str_cacs *)shmat(ShMemHandle, 0, 0);
	if (cac_status==NULL)
	{
		ErrNo=errno;
		return NOOK;
	}

	ret=Sem.Create(id, flags);
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

	for (int cont=0; cont<MAX_CAS; cont++)
	{
		cac_status[cont].flag=0;
		cac_status[cont].nro_suc=0;
		cac_status[cont].nro_caj=0;
	}

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
| FUNCION         | CACStatus::Create                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Create(long id)
{
	return Create(id, 0);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Open                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Apertura del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Open(long id)
{
	ShMemHandle=shmget(id, sizeof(int), 0);
	if (ShMemHandle==-1)
	{
		ErrNo=errno;
		return NOOK;
	}

	cac_status=(str_cacs*)shmat(ShMemHandle, 0, 0);
	if (cac_status==NULL)
	{
		ErrNo=errno;
		return NOOK;
	}

	ret=Sem.Open(id);
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
| FUNCION         | CACStatus::Close                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierre del objeto                                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Close()
{
	shmdt((char *)cac_status);
	Sem.Close();

	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Destroy                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Destroy()
{
	shmdt((char *)cac_status);
	shmctl(ShMemHandle, IPC_RMID, NULL);
	Sem.Destroy();

	ErrNo=OK;
	return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Down                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Baja el estado del canal                                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Down(int channel)
{
	return SetState(channel, STAT_DOWN);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Up                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Sube el estado del canal                                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Up(int channel)
{
	return ResetState(channel, STAT_DOWN);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Cierre                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Marca el estado del canal como en cierre                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Cierre(int channel, int nro_suc, int nro_caj)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	cac_status[channel].nro_suc=nro_suc;
	cac_status[channel].nro_caj=nro_caj;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	return SetState(channel, STAT_CIERRE);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Normal                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Marca el estado del canal como normal                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Normal(int channel)
{
	ret=Sem.Set();
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	cac_status[channel].nro_suc=0;
	cac_status[channel].nro_caj=0;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	return ResetState(channel, STAT_CIERRE);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::Ping                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el canal con Ping                                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::Ping(int channel)
{
	return SetState(channel, STAT_PING);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::NotPing                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Resetea el canal con Not Ping                              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::NotPing(int channel)
{
	return ResetState(channel, STAT_PING);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::IsDown                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica si el estado del canal esta abajo                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::IsDown(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	return ((GetState(channel) & STAT_DOWN)==STAT_DOWN);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::IsUp                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica si el estado del canal esta arriba                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::IsUp(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	return ((GetState(channel) | ~STAT_DOWN)==~STAT_DOWN);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::IsCierre                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica si el estado del canal esta en cierre             |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::IsCierre(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	return ((GetState(channel) & STAT_CIERRE)==STAT_CIERRE);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::IsNormal                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica si el estado del canal esta normal                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::IsNormal(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	return ((GetState(channel) | ~STAT_CIERRE)==~STAT_CIERRE);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::IsPing                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica si el estado del canal esta en ping               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::IsPing(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	return ((GetState(channel) & STAT_PING)==STAT_PING);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::IsNotPing                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica si el estado del canal esta en not ping           |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::IsNotPing(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	return ((GetState(channel) | ~STAT_PING)==~STAT_PING);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::GetState                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el estado del canal                                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::GetState(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	aux=cac_status[channel].flag;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ErrNo=OK;
	return aux;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::ResetState                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Resetea el estado del canal                                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::ResetState(int channel, int new_status)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	cac_status[channel].flag&=~new_status;

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
| FUNCION         | CACStatus::SetState                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el estado del canal                                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::SetState(int channel, int new_status)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	cac_status[channel].flag|=new_status;

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
| FUNCION         | CACStatus::GetSuc                                          |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene la empresa del canal                               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::GetSuc(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	aux=cac_status[channel].nro_suc;

	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}

	ErrNo=OK;
	return aux;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::GetErrno                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene la caja del canal                                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::GetCaj(int channel)
{
	if (channel>MAX_CAS)
	{
		ErrNo=EINVAL;
		return NOOK;
	}
	ret=Sem.Set();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}
	
	aux=cac_status[channel].nro_caj;
	
	ret=Sem.Reset();
	if (ret==NOOK)
	{
		ErrNo=Sem.GetErrno();
		return NOOK;
	}
	
	ErrNo=OK;
	return aux;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | CACStatus::GetErrno                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el codigo de error de la ultima operacion          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int CACStatus::GetErrno()
{
	return ErrNo;
}
