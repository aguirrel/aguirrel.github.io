#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | ShutDown.C                                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Clase general para destruccion de objetos                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Events.h"

char					aux_str1[100];	/* String de configuracion      */
char					aux_str2[100];	/* String de configuracion      */ 

extern BatchTable		DBBatch;		/* Para tabla de cierres        */ 
extern CACStatus		CACS;			/* Status Canal X.25            */
extern Config           Cfg;			/* Objeto de configuracion      */
extern Log				LogAlarm;		/* Logging de actividades       */
extern PidTable			PTable;			/* Tabla de PIDs                */
extern TimeOutQueue		timeout_queue;	/* Cola de timeouts             */
extern TraceNmbr		TraceNumber;	/* Nros de trace recibidos      */ 
extern TrxHeap			Heap;			/* Objeto Heap                  */
extern TrxTable			DBTrx;			/* Para tabla de transacciones  */
extern Queue            SQueue;			/* Cola de Sockets              */
extern Queue            XQueue;			/* Cola de X.25                 */

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ShutDown::ShutDown                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Creacion del objeto                                        |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
ShutDown::ShutDown(int msg_pid, char *msg_data, int data_len)
{
    event_type=EVENT_SHUTDOWN;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | ShutDown::ProcessIt                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Proceso del objeto                                         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int ShutDown::ProcessIt()
{
    PTable.KillAll();
    return NOOK;
}


