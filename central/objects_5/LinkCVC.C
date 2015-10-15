#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | LinkCVC.C                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Fuentes para el manejo de la placa X25                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "LinkCVC.h"

int	 CVC::Open(char*, char*, char*)					{return 0;}		
int  CVC::Close()									{return 0;}
int  CVC::Write(char *data, int len)				{return 0;}
int  CVC::Read(char *data, int *len, int *tipo)		{return 0;}
int  CVC::MsgPnd()									{return 0;}
int  CVC::GetErrno()								{return 0;}
int  CVC::GetErSys()								{return 0;}
int  CVC::setCodUserData(char data[2][11])			{return 0;}
