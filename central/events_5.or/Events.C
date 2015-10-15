#ifdef OS_Linux
#pragma pack(1)
#endif

/***************************************************************************/
/* Se incrementa la hora del servidor para evitar que se rechacen mensajes */
/* Campo 07 (Date&Time Transmission) debe ser superior a la union de los   */
/* Campos 13 (Date local transaction) y Campo 12 (Time local transaction)  */
/* Eliminar cuando las cajas se encuentren sincronizadas con el servidor   */
/* de sucursal y este con el de credito.                                   */
/***************************************************************************/
#define INCREMENTO_SEGUNDOS	180

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Events.C                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Clase general para el manejo de los eventos                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Event::currentTimeLog                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtencion del current time stamp. Formato log humano       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *currentTimeLog()
{
    static char cTimeLog[128];
    time_t ltime;
		
    time(&ltime);
	ltime += INCREMENTO_SEGUNDOS;

    sprintf(cTimeLog, "%s", asctime(localtime(&ltime))); 
    
	return cTimeLog ;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Event::currentTimestamp                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtencion del current time stamp. Formato mmddhhmmss       |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *currentTimestamp()
{
    static char cTimestamp[40];
    struct tm *newtime ;
    time_t ltime;
    
    time(&ltime);
	ltime += INCREMENTO_SEGUNDOS;
    newtime=localtime(&ltime);
	
    sprintf(cTimestamp, "%02d%02d%02d%02d%02d", newtime->tm_mon+1, 
	newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec );

    return cTimestamp ;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Event::ObtYear                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtencion del año                                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int ObtYear()
{
    struct tm *newtime ;
    time_t ltime;
    
    time(&ltime);
    newtime=localtime(&ltime);

    if (newtime->tm_year<90)
    {
       return newtime->tm_year+2000;
    }
    else
    {
       return newtime->tm_year+1900;
    }
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Event::Event                                               |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Event::Event()
{
    event_type=EVENT_NULL;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Event::~Event                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destruccion del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
Event::~Event()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Event::ProcessIt                                           |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Event::ProcessIt()
{
    return OK;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Event::GetType                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Tipo del objeto                                            |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Event::GetType()
{
    return event_type;
}
