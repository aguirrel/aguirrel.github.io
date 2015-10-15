#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Pos.C                                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto para almacenar las cajas que se encuentran          |
|                 | bloqueadas                                                 | 
+-----------------+------------------------------------------------------------+
*/
#include "Log.h"
#include "Pos.h"

#define OK				0
#define NOOK			-1

extern Log	LogAlarm;

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Pos::Pos                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Crea el objeto para almacenar las cajas que se encuentran  |
|                 | bloqueadas. Inicializa las estructuras                     | 
+-----------------+------------------------------------------------------------+
*/
Pos::Pos()
{
    int	cont;
   
    Errno=OK;
    for (cont=0; cont<POS_MAX_MEMBERS; cont++)
    {
		pos[cont].nro_ca[0]  = '\0';
		pos[cont].nro_caj[0] = '\0';
		pos[cont].nro_suc[0] = '\0';
	}
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Pos::~Pos                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Pos::~Pos()
{
   
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Pos::SetPosBusy                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Agrega una caja a la lista de ocupadas                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Pos::SetPosBusy(char *ca, char *suc, char *caj)
{
    int	cont=0;

    /* Busca el siguiente disponible */
    while ((pos[cont].nro_ca[0]!='\0') && (cont<POS_MAX_MEMBERS))
	{
		cont++;
    }

	/* Si hay una posicion libre */
	if (pos[cont].nro_ca[0]=='\0')
    {
		/* Agrega el elemento */
		strcpy(pos[cont].nro_ca  , ca);
		strcpy(pos[cont].nro_caj , caj);
		strcpy(pos[cont].nro_suc , suc);
		LogAlarm.Put(5, "Objeto Pos: SetPosBusy [%s%s%s]. Posicion [%d]\n", ca, suc, caj, cont );

		/* Retorna el resultado OK */
		Errno=OK;
		return OK;
    }
	else
	{
		LogAlarm.Put(5, "Objeto Pos: SetPosBusy [%s%s%s]. NO HAY POSICIONES LIBRES\n", ca, suc, caj );
	}

	/* Retorna el resultado error */
    Errno=ENOSPC;
    return NOOK;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Pos::SetPosFree                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Elimina una caja de la lista de ocupadas                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Pos::SetPosFree(char *ca, char *suc, char *caj)
{
    int	cont=0;
    
	/* Busca la clave */   
    while ( ( (strcmp(pos[cont].nro_ca , ca )) || 
              (strcmp(pos[cont].nro_caj, caj)) ||
			  (strcmp(pos[cont].nro_suc, suc)) 
			) && (cont<POS_MAX_MEMBERS) )
    {
		cont++;
    }
    
	/* Si encuentra la clave */
	if ( (strcmp(pos[cont].nro_ca  , ca )==0) &&
		 (strcmp(pos[cont].nro_caj , caj)==0) &&
		 (strcmp(pos[cont].nro_suc , suc)==0) )
    {
		LogAlarm.Put(5, "Objeto Pos: SetPosFree [%s%s%s]. Posicion [%d]\n", ca, suc, caj, cont);

		/* Libera el elemento */
		pos[cont].nro_ca[0]  = '\0';
		pos[cont].nro_caj[0] = '\0';
		pos[cont].nro_suc[0] = '\0';
		
		/* Retorna el resultado OK */
		Errno=OK;
		return OK;
    }
	else
	{
		LogAlarm.Put(5, "Objeto Pos: SetPosFree [%s%s%s]. Posicion no encontrada\n", ca, suc, caj);
	}

	/* Retorna el resultado error */
    Errno=ENOENT;
    return NOOK;  
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Pos::IsPosFree                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Verifica si una caja esta libre                            |
|                 | Retorna: 1:No encontrado - 0:Encontrado                    | 
+-----------------+------------------------------------------------------------+
*/
int Pos::IsPosFree(char *ca, char *suc, char *caj)
{
    int	cont=0;
    int ret;

    /* Busca si la clave se encuentra */
    while ( ( (strcmp(pos[cont].nro_ca  , ca )) || 
              (strcmp(pos[cont].nro_caj , caj)) ||
              (strcmp(pos[cont].nro_suc , suc)) 
			) && (cont<POS_MAX_MEMBERS) )
	{
        cont++;
    }
    
	/* Retorna el resultado */
	ret = (cont==POS_MAX_MEMBERS);

	LogAlarm.Put(5, "Objeto Pos: IsPosFree  [%s%s%s]. Retorno [%d]\n", ca, suc, caj, ret);

    return ret;
}
/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Pos::GetErrno                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Retorna el resultado de la ultima operacion                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Pos::GetErrno()
{
    return Errno;
}
