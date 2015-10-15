#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Log.C                                                      | 
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto para el manejo del archivo de Log                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Includes.h"
#include "Log.h"

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Log::Log                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del objeto                                     |
|                 | debug_level=1:Imprime en pantalla-0:No imprime en pantalla | 
+-----------------+------------------------------------------------------------+
*/
Log::Log()
{
    level=0;
    debug_level=-1; 
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Log::Open                                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Abre el archivo de log                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void Log::Open(char *logname)
{
    log_fd=fopen(logname, "a");
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Log::SetLevel                                              |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Configura el nivel de log deseado                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void Log::SetLevel(int new_level)
{
    level=new_level;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Log::Log                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Configura el nivel de log deseado                          |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void Log::SetDebugLevel(int new_debug_level)
{
    debug_level=new_debug_level;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Log::Put                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Escribe un mensaje en el arhivo de log                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void Log::Put(int priority, char *fmt, ...)
{
    va_list args;
    if ( (priority<=level) || (priority<=debug_level) )
    {
		va_start(args, fmt);
		if (priority<=debug_level)
		{
			vprintf(fmt, args);
		}
		if (priority<=level)
		{
			vfprintf(log_fd, fmt, args);
		}
		fflush(log_fd);
		va_end(args);
    }
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Log::Put                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Escribe un mensaje en el arhivo de log (no necesariamente  |
|                 | en formato ascii                                           | 
+-----------------+------------------------------------------------------------+
*/
void Log::PutMsg(int priority, char *szCab, char *szMsj, int iLon) 
{
    int iCont;
    int iCar;

    if ( (priority<=level) )
    {
		for (iCont=0,iCar=0;iCont<iLon;iCont++,iCar++)
		{
			switch (iCar)
			{
			case 0:  fprintf(log_fd, "%s [%02x", szCab, szMsj[iCont]);
				 break;
			case 25: fprintf(log_fd, "]\n%s [%02x", szCab, szMsj[iCont]);
				 iCar=0;
				 break;
			default: fprintf(log_fd, "%02x", szMsj[iCont]);
				 break;			 
			}	
		}
		fprintf(log_fd, "]\n");
		fflush(log_fd);
    }
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Log::Clsoe                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Cierra el archivo de log                                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void Log::Close()
{
    fclose(log_fd);
}

