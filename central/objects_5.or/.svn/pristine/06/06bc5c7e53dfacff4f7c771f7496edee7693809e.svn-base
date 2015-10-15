#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | IniX25.C                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso que inicializa el canal X25 para un determinado    |
|                 | centro autorizador                                         | 
+-----------------+------------------------------------------------------------+
*/
#include "Queues.h"
#include "Config.h"
#include "Daemons.h"

Config		Cfg(CONFIG_FILE);
Queue		X;
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | main                                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Funcion principal de la aplicacion                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
main(int argc,char **argv)
{
    char buffer[100];
    char buffer2[100];
    int  ret;

	if ( (argc==2) && (strlen(argv[1])<=2) )
	{
		Cfg.GetItem("CAVisaDaemon","QueueID",buffer);
		Cfg.GetItem("CAVisaDaemon","QueuePerm",buffer2);

		ret=X.Open(atol(buffer),atol(buffer2));

		sprintf(buffer,"I%02d",atoi(argv[1]));
		X.SendMsg(99,buffer,sizeof(buffer));
	}
	else
	{	
		if ( (argc==3) && (strlen(argv[1])<=2) && (strlen(argv[2])==1) && (argv[2][0]=='U') )
		{
			Cfg.GetItem("CAVisaDaemon","QueueID",buffer);
			Cfg.GetItem("CAVisaDaemon","QueuePerm",buffer2);

			ret=X.Open(atol(buffer),atol(buffer2));

			sprintf(buffer,"U%02d",atoi(argv[1]));
			X.SendMsg(99,buffer,sizeof(buffer));
		}
		else
		{
			printf("Error en parametros\n");
			printf("Uso: IniX25 <Nro CA>   : Inicia todos los canales configurados\n");
			printf("Uso: IniX25 <Nro CA> U : Inicia solo uno de los canales coonfigurados\n");
		}
	}
}
    
    


 
