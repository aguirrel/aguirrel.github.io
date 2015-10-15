/************************************************************************/
/*									*/
/* SimX25.C	    : Simulador de X25                                  */
/*                    Proceso que envia las transacciones de credito    */
/*                    a los centros autorizadores 			*/
/*									*/
/* Proyecto	    : Autorizacion de tarjetas de credito - StoreFlow.	*/
/* Cliente	    : Coto						*/
/* Autor	    : Denise Lucien                                     */ 
/* Fecha	    : 11/11/97						*/
/* Compilador	    : C++						*/
/* Base de datos    : Oracle						*/
/*									*/
/*									*/
/* Modificaciones :							*/ 
/*									*/
/* Fecha : 26/09/96							*/
/* Responsable : Rolando Loustric					*/
/* Funciones : chkOrdenes, SendX25Msg					*/
/* Objetivo : Resetear el canal luego de producirse TimeOut 		*/
/* Se verifica la existencia del file de verificacion, que puede ser    */
/* creado por dos motivos:						*/
/* 1-Desde la aplicacion, luego de producido un TimeOut.		*/	
/* 2-Por los operadores. 						*/
/*									*/
/*									*/
/* Fecha : 03/10/96. 							*/
/* Responsable : Rolando Loustric					*/
/* Se modifica para impedir que se resetee el canal en un lapso 	*/	
/* corto de tpo. Ese lapso sera de 60 segundos				*/
/* Asignar a variable booleana BajarCa lo sig:  			*/
/* BajarCa = (hora2 - hora1) > 60					*/
/* Utilizar esta variable en la condicion if ((rc==0) && (BajarCa))     */
/*									*/
/************************************************************************/

#include "8583.h"
#include "Config.h"
#include "Daemons.h"
#include "LinkX25.h"
#include "Log.h"
#include "LogError.h"
#include "Fifo.h"
#include "PidTable.h"
#include "Queues.h"
#include "CACStat.h"

#define TPDU_LEN         5
#define NRO_CHANS        15 
#define MAX_QERRS        9
#define VERSION_ID      "V01"

struct strRelMsg 
{
    int SndEvn;             // Evento de envio.
    int RtaEvn;             // Evento de respuesta.
};    

// Pares Tipo Mensaje = Evento Respuesta...
//
struct strRelMsg DefEventPairs[]=
{
    {TYPE_VISA_VENTA,               EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_VENTA_RTA,           EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_DEVOL,               EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_DEVOL_RTA,           EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_ANUL_VENTA,          EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_ANUL_VENTA_RTA,      EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_ANUL_DEVOL,          EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_ANUL_DEVOL_RTA,      EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_VENTA_LOCAL,         EVENT_RTATRXOFFLINE},
    {TYPE_VISA_VENTA_LOCAL_RTA,     EVENT_RTATRXOFFLINE},
    {TYPE_VISA_DEVOL_LOCAL,         EVENT_RTATRXOFFLINE},
    {TYPE_VISA_DEVOL_LOCAL_RTA,     EVENT_RTATRXOFFLINE},
    {TYPE_VISA_ANUL_VENTA_LOCAL,    EVENT_RTATRXOFFLINE},
    {TYPE_VISA_ANUL_VENTA_LOCAL_RTA,EVENT_RTATRXOFFLINE},
    {TYPE_VISA_ANUL_DEVOL_LOCAL,    EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_ANUL_DEVOL_LOCAL_RTA,EVENT_PRICEONLINE_RTA},
    {TYPE_VISA_REV_VENTA,           EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_REV_VENTA_RTA,       EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_REV_DEVOL,           EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_REV_DEVOL_RTA,       EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_REV_ANUL_VENTA,      EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_REV_ANUL_VENTA_RTA,  EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_REV_ANUL_DEVOL,      EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_REV_ANUL_DEVOL_RTA,  EVENT_SNDREVERSO_RTA},
    {TYPE_VISA_CIERRE,              EVENT_CIERRE_RTA}, 
    {TYPE_VISA_CIERRE_RTA,          EVENT_CIERRE_RTA}, 
    {TYPE_VISA_CIERRE_B,            EVENT_CIERRE_RTA},
    {TYPE_VISA_CIERRE_B_RTA,        EVENT_CIERRE_RTA},
    {TYPE_VISA_BATCH,               EVENT_BATCH_RTA},
    {TYPE_VISA_BATCH_RTA,           EVENT_BATCH_RTA},
    {TYPE_VISA_TEST,                EVENT_ECHOTEST_RTA},
    {TYPE_VISA_TEST_RTA,            EVENT_ECHOTEST_RTA},
    {-1,-1}                                         //Fin de tabla.
};

// Nuevas definiciones. 
//
int         cont;
PidTable    PTable;			// Tabla de PIDs.
Config      Cfg(CONFIG_FILE);           // Objeto de cofiguracion.
X25         *X25LogiCha[NRO_CHANS];     // Logica de canales.
LogError    LogAlarm;                   // Logging de actividades.
Log         LogOperator;                // Logging para alarmas del operador.
CACStatus   CaStsCtrl;                  // Status del centro autorizador.
Queue       EQueue;                     // Cola de eventos del Daemon Princp.
Queue       XQueue;                     // Cola de eventos para X25.
time_t      hora_ult;
int         cant=0;
       
// Prototipos de funciones internas.

void            init(void);
int             testOpen(int codigoCa);
void            dumpstr(char *, long);
static int      getCaFromMsg(char * DaemonMsg,int largoMsg);
int             SendX25Msg(X25 unX25, char * msg_str, int len);
static void     shut_down(int sig_id=0);
int             CloseCaChannels(int CA);
int             OpenCaChannels(int CA);
int             ResetCaChannels(int codigoCa);      
int             chkOrdenes(int codigoCa);     // Chequea ordenes via archivo.
void            armaFileNameCmd(int caCode,char * fileCmd);
int             getTypeMsgFromMsg(char *msg_string, int msg_long);

//
// Funcion principal .....
//       Inicializacion y main loop.
//  Descripcion:
//     1. Inicializa los canales.
//     2. Forever Loop
//        2.1 Recuperar mensaje X.25
//        

void main(int argc, char *argv[])
{
    int        queueErrs=0;         // Contador de errores de cola.
    int        MsgSended;           // Flag de envio de mensaje.
    int        len;                 // Longitud del mensaje.
    int        counter;             // Contador de uso General.
    int        ret;                 // Codigos de retorno.
    int        codigoCa;            // Codigo de centro autorizador.
    char       msg_str[3000]={0};   // Mensaje recibido del Daemon Principal.
    char       reOpenAddress[50];   // Address para reapertura.
    int        counterSat=0;        // Contador de saturacion de canal

    if (argc==2)
        Cfg.SetCfgName(argv[1]);

    init();

    while (1)
        {
        LogAlarm.Put(10, "main(): Esperando mensaje...\n");

        len=ret=XQueue.GetMsg(0, msg_str, sizeof(msg_str));
        if (ret==NOOK)
            {                       // Hubo error 
            queueErrs++;
            LogAlarm.Put( 0, "main(): "
                             "Error (%d) en lectura de cola X25.[%d]\n", 
                             XQueue.GetErrno(), queueErrs);

            if (queueErrs>=MAX_QERRS)
                {
                LogAlarm.Put( 0, "main(): "
                                 "Demasiados errores en cola de X25\n");
                break;
                }
            else
                continue;
            }
        else
            queueErrs=0;            

        //
        // Obtiene el centro autorizador al cual se dirige el mensaje.
        //

        time_t hIni;
        hIni=time(&hIni);
        LogAlarm.Put( 5, "main(): Extraigo mensaje %s\n",
                         asctime(localtime(&hIni))); 

        codigoCa=getCaFromMsg(msg_str, len);
        LogAlarm.Put( 5, "main(): "
                         "CA al cual se dirige el msg: %ld\n",
                          codigoCa);

        chkOrdenes(codigoCa);     // Chequea ordenes via archivo.

        ret=testOpen(codigoCa);
        if (ret!=OK)
            {
            LogAlarm.Put( 5, "main(): "
                             "Centro %d Sin canales - Rearrancando\n",
                              codigoCa);
            OpenCaChannels(codigoCa);
            }

        // Se espera el envio.
        counterSat=0;
        for (MsgSended=0; !MsgSended; )
            {
            // Busca un canal disponible para el CA.

            for (counter=0; counter<NRO_CHANS && !MsgSended; counter++)
                {
                if (X25LogiCha[counter]==NULL)
                    continue;

                if (X25LogiCha[counter]->NroCA()!=codigoCa)
                    continue;

                if (X25LogiCha[counter]->isFree(codigoCa))
                    break;

                ret=X25LogiCha[counter]->chkCircuit();
                if (ret==NOOK)
                   {
                   X25LogiCha[counter]->calledAddr(reOpenAddress);
                   LogAlarm.Put( 0, "main(): "
                                    "ERROR - Cerrando Canal\n");
                   ret=X25LogiCha[counter]->Close();
                   LogAlarm.Put( 0, "main(): "
                                    "ERROR - Canal [%s] inutilizable !!!\n",
                                    reOpenAddress);
                   ret=X25LogiCha[counter]->Open(reOpenAddress);
                   if (ret==NOOK)   // Vinculo Abierto Correctamente..
                       {
                       LogAlarm.Put(0,"main(): "
                                      "ERROR en Vinculo- Ver CA \n");
                       LogAlarm.Put(1,"main(): "
                                      "Error (%d) al reinit Canal logico\n", 
                                      X25LogiCha[counter]->GetErrno());
                       LogAlarm.Put(1,"main(): "
                                      "Causa (%d) Diagnostico (%d)\n" ,
                                      X25LogiCha[counter]->GetX25ClearCause(),
                                  X25LogiCha[counter]->GetX25DiagnosticCause());
                       }
                    else
                      break;
                   }
                }

            // Si existe un canal disponible intenta enviar mensaje. 
            if (counter<NRO_CHANS )
                {
                LogAlarm.Put(10, "main(): "
                                 "Escribiendo mensaje credito por canal %d \n", 
                                 counter);
                ret=X25LogiCha[counter]->chkCircuit();
                if (ret==NOOK)
                       {
                       X25LogiCha[counter]->calledAddr(reOpenAddress);
                       LogAlarm.Put( 1, "main(): "
                                        "ERROR - Canal [%s] inutilizable !!!\n",
                                        reOpenAddress);
                       ret=X25LogiCha[counter]->Close();
                       LogAlarm.Put( 1, "main(): "
                                        "Tratando de reabrir canal\n");
                       ret=X25LogiCha[counter]->Open(reOpenAddress);
                       if (ret==NOOK)
                           {
                           LogAlarm.Put( 1, "main(): "
                                       "ERROR al ReArrancar Vinculo - KILL\n");
                           break; 
                           // exit(12);
                           }
                       }
                ret=SendX25Msg(*X25LogiCha[counter],msg_str,len);
                if (ret != OK)
                    {
                    LogAlarm.Put( 1, "main(): "
                                     "Error (%d && %d) en envio de mensaje\n", 
                                     X25LogiCha[counter]->GetErrno());
                    }
                else
                    MsgSended=1;
                }
            else
                { // No se pudo enviar se dormir el proceso.
                if (counterSat++<4)
                   {
                   LogAlarm.Put( 5, "main(): "
                                 "Canales X25 Saturados (x %d). Durmiendo.\n",
                                  counterSat);
                   sleep(1);
                   }
                 else
                   {
                   time_t horaErr;
                   horaErr=time(&horaErr);
                   LogOperator.Put( 0, "X25Daemon: %s",
                                     asctime(localtime(&horaErr)));
                   LogOperator.Put( 0, "X25Daemon:: Canal %d Down State\n",
                                        codigoCa);
                   LogAlarm.Put( 1, "main(): "
                                 "Error en Canal X25. Descartando Msg.\n",
                                  counterSat);
                   break;
                   }
                }
            }
        }   
    shut_down();
}

////
///
//// Funcion de envio y recepcion de un mensaje X.25.
////
////

int SendX25Msg(X25 unX25, char *msgBuffer, int largoMsg)
{
    int        ret;            		// Retornos de funciones.
    int        versionDaemon;        	// Version del Daemon.
    int        rtaEvent=0;        	// ID de evento de respuesta.
    int        counter;        		// Contador de uso General.
    int        useTpdu;        		// Flag de uso de TPDU.
    int        logMsg;            	// Flag de logging de mensajes.
    int        codigoCa;        	// Codigo de centro autorizador.
    int        codigoMsg;               // Tipo del mensajes. 
    FILE    *fout;
    FILE    *fd;
 
    Iso_Msg    *toSendMsg;        	// Mensaje ISO a enviar
    Iso_Msg    *RtaIsoMsg;        	// Mensaje ISO Recibido.

    static char    x25FmtedMsg[3000];   // Mensaje ISO ya formateado.
    static char    x25RtaMsg[3000];     // Mensaje de respuesta sin headers.
    static char    cfgString[80];       // String para configurador.
    static char    logDir[80];       	// Directorio de logging de mensajes.
    static char    TPDUcfg[20];       	// Directorio de logging de mensajes.
    static char    Msgcfg[20];          // Directorio de logging de mensajes. 
    static char    fileLog[100];
    static char    CaString[80];        // String del centro autorizador.


    codigoCa=getCaFromMsg(msgBuffer, largoMsg);

    LogAlarm.Put( 5, "SendX25Msg(): "
                     "CA al cual se dirige el msg: %ld\n",
                      codigoCa);

    toSendMsg=new Iso_Msg(msgBuffer, largoMsg);
    sprintf(CaString, "%-15.15s", toSendMsg->GetField(42));

    // Obtencion de requerir TPDU.
    sprintf(TPDUcfg, "TPDU%d", codigoCa);
    Cfg.GetItem("CAVisaDaemon", TPDUcfg, cfgString);
    useTpdu=((strcmp(cfgString, "on")==0) ? 1 : 0);

    LogAlarm.Put(5, "SendX25Msg(): "
			"Utilizacion de TPDU %s\n", cfgString);

    //Obtencion de directorio de loggin y necesidad del mismo.

    // Obtencion de requerir tipo de mensajes. Modificacion. 
    codigoMsg=getTypeMsgFromMsg(msgBuffer, largoMsg);
    sprintf(Msgcfg, "LogMsg0%d", codigoMsg/10);
 
    Cfg.GetItem("CAVisaDaemon", Msgcfg, cfgString);
    //Cfg.GetItem("CAVisaDaemon", "LogMsg", cfgString);
    logMsg=((strcmp(cfgString, "on")==0) ? 1 : 0);

    strcpy(cfgString,"");
    Cfg.GetItem("CAVisaDaemon", "LogDir", cfgString);
    strcpy(logDir,cfgString);

    // Loging del mensaje.
    if (logMsg)
        {
            sprintf(fileLog, "%sq%s.msg", logDir, toSendMsg->GetField(11));
            if ((fout=fopen(fileLog, "wb"))!=NULL)
                {
                fwrite(msgBuffer, largoMsg, 1, fout);
                fclose(fout);
                }
            else
                {
                LogAlarm.Put( 5, "SendX25Msg(): "
                                 "Error en Write log de respuesta \n");
                }
        }

    // Agregado del header del mensaje (TPDU).
    if (useTpdu)
        {
        memcpy(x25FmtedMsg, "\x60\x00\x05\x00\x00", TPDU_LEN);
        memcpy(x25FmtedMsg+TPDU_LEN, msgBuffer, largoMsg);
        largoMsg+=TPDU_LEN;
        }
    else
        memcpy(x25FmtedMsg, msgBuffer, largoMsg);


    LogAlarm.Put(0, "SendX25Msg(): "
                     "Mandando mensaje a CA (%d)...\n", codigoCa);

    unX25.setBussy();
    ret=fork();
    switch (ret)
        {
        case -1:   // Error en Fork.
            LogAlarm.Put( 1, "SendX25Msg(): "
				"Error en fork de proceso de lectura.\n");
            unX25.setFree();
            delete toSendMsg;
            return NOOK;

        case  0:            /// Proceso Hijo
            break;

        default:            /// Proceso Padre.
            LogAlarm.Put( 1, "SendX25Msg(): "
				"Fork Ejecutado Correctamente\n");

            cant++;
            unX25.pidUsed(ret);
            delete toSendMsg;
            // Para eliminar defunct que queden pendientes..
            int wcount;
	    for (wcount=0; wcount<10; wcount++)
		waitpid(-1, NULL, WNOHANG);
            return OK;
        }

    // !!!!! Warning !!!!!  
    // A partir de aqui solo es el proceso hijo...
    //

    LogAlarm.Put( 1, "SendX25Msg(): "
                     "Escribo mensaje en canal X.25 (Write) \n"); 

    fd=fopen("/online/log/credito", "a");
    fprintf(fd, "SendX25Msg(): "
                     "Escribo mensaje en canal X.25 (Write) \n"); 
    fclose(fd);

    ret=unX25.Write(x25FmtedMsg, largoMsg);
    if (ret==NOOK)
        {
        LogAlarm.Put( 1, "SendX25Msg(): "
                         "Error (%d, %d) en Write de mensaje al canal.!!\n", 
                         unX25.GetErrno(), unX25.GetSysErrno());
        exit(10);
        }

    largoMsg=sizeof(x25FmtedMsg);

    LogAlarm.Put(0, "SendX25Msg(): " 
                     "Esperando respuesta del Centro Autorizador... \n");

/*    if (cant!=0)

    {
      sleep(75);
    } */ 
    sleep(15); 
 
    

    ret=unX25.Read(x25FmtedMsg, &largoMsg);
    LogAlarm.Put(0, "SendX25Msg(): "
			"Respuesta recibida de CA (unX25.Read)\n");

    if (ret==NOOK)
        {
        time_t horaErr;
        horaErr=time(&horaErr);
        LogAlarm.Put( 5, "SendX25Msg(): %s",
			asctime(localtime(&horaErr)));

        switch(unX25.GetErrno()) 
            {
            case 303:
                LogAlarm.Put( 0, "SendX25Msg(): "
                                 "Clear Call Cause = %d Diagnostic = %d\n",
                                 unX25.GetX25ClearCause(),
                                 unX25.GetX25DiagnosticCause());
                break;
            case 229:
                LogAlarm.Put( 0, "SendX25Msg(): "
                                 "!!!!!! TIMEOUT -- WARNING -- TIMEOUT !!!!\n");
                                  unX25.Close();
                break;
            default:
                LogAlarm.Put( 0, "SendX25Msg(): "
                                 "Error (%d, %d) en lectura de mensaje.!!\n",
                                  unX25.GetErrno(), unX25.GetSysErrno());
             }

        char fileCmd[100];
        FILE * Cmdfs;
        armaFileNameCmd(codigoCa,fileCmd); 
       
        if ((Cmdfs=fopen(fileCmd,"a"))!=NULL)
           {
           LogAlarm.Put( 5, "SendX25Msg(): "
                            "Creando file de cmd [%s]\n",
                            fileCmd);
           fprintf(Cmdfs,"Borrar !!!\n"); 
           fclose(Cmdfs);
           }
         else 
           {
           LogAlarm.Put( 0, "SendX25Msg(): "
                            "Error. No se pudo crear file de cmd [%s]\n",
                            fileCmd);
          }
        exit(12);
        }

    // Elimina el header de Centro Autorizador TPDU.
    if (largoMsg<TPDU_LEN)
        {
        LogAlarm.Put(0, "SendX25Msg(): "
			"Error en largo del mensaje recibido %d.\n", largoMsg);
        exit(12);
        }

    if (useTpdu)
        {
        memcpy(x25RtaMsg,x25FmtedMsg+TPDU_LEN,largoMsg-TPDU_LEN);
        largoMsg-=TPDU_LEN;
        }
    else
        memcpy(x25RtaMsg,x25FmtedMsg,largoMsg);

    // Envio de respuesta.
    RtaIsoMsg=new Iso_Msg(x25RtaMsg, largoMsg);

    LogAlarm.Put(5, "SendX25Msg(): "
                     "Logging del mensaje [%s] recibido.\n",
                     RtaIsoMsg->GetField(11));

    /// Logging de la respuesta.
    if (logMsg)
        {
        sprintf(fileLog, "%sr%s.msg", logDir, RtaIsoMsg->GetField(11));
        if ((fout=fopen(fileLog, "wb"))!=NULL)
            {
            fwrite(x25RtaMsg, largoMsg, 1, fout);
            fclose(fout);
            }
        else
            {
            LogAlarm.Put( 5, "SendX25Msg(): Error en Write logging de mensaje "
            "de respuesta\n");
            }
        }
    // Se agrega numero de centro autorizador en caso de que no lo tenga
    // Visa Req.

    if (RtaIsoMsg->ChkField(42)!=TRUE)
         {
         LogAlarm.Put(5, "SendX25Msg(): "
                          "Agregando Centro Autorizador : %s\n" ,
                           CaString);

         fd=fopen("/online/log/credito", "a");
         fprintf(fd, "SendX25Msg(): "
                          "Agregando Centro Autorizador : [%s]\n", CaString);
         fclose(fd);

         RtaIsoMsg->PutField(42,CaString); 
         largoMsg=RtaIsoMsg->GetMsgString(x25RtaMsg);
         }


    LogAlarm.Put(0, "SendX25Msg(): "
		    "Busqueda del evento asociado (eventos pares)\n");
    LogAlarm.Put(0, "SendX25Msg(): "
		    "Mandando respuesta a Event Queue\n");

    // busqueda del evento asociado

    int tipoFrom=RtaIsoMsg->GetMsgType();
    for (counter=0;DefEventPairs[counter].SndEvn!=-1;counter++)
        {
        if (DefEventPairs[counter].SndEvn==tipoFrom)
            {
            LogAlarm.Put( 5, "SendX25Msg(): "
                         "Par Evento CA ==> [%d] Event = [%d]\n",
                          tipoFrom, DefEventPairs[counter].RtaEvn);
            break;
            }
        }

    if (DefEventPairs[counter].SndEvn==-1)
        {
        LogAlarm.Put( 0, "SendX25Msg(): "
                         "Error No se pudo conseguir mensaje a enviar\n");
        exit(20);
        }


    fd=fopen("/online/log/credito", "a");
    fprintf(fd, "SendX25Msg(): mando a cola de eventos\n");
    fclose(fd);

    ret=EQueue.SendMsg(DefEventPairs[counter].RtaEvn, x25RtaMsg, largoMsg);
    if (ret==NOOK)
        {
        LogAlarm.Put( 5, "SendX25Msg(): Error(%d) en Write A cola de eventos\n",
			 EQueue.GetErrno());
        exit(25);
        }

    LogAlarm.Put( 5, "SendX25Msg(): "
			"Terminando proceso de lectura (%d)..\n", getpid());

    exit(0);
}


////
//
// Funcion para determinar el centro autorizador recibido.
//
//  Para esto determina la version del mensaje.
//
//

static int getCaFromMsg(char * DaemonMsg,int largoMsg)
{
    Iso_Msg    *elIsoMsg;          // Mensaje ISO a enviar
    int        ver;                // Version del Daemon.
    int        ret;                // Codigo de retorno.
    char    *ptrCaCode;         // Codigo del CA.
    int        CaCode;             // Codigo Obtenido
    FILE	*fd;

    static char    cfgString[80];      // String para configurador.
    static char    CaString[80];       // String del centro autorizador.

    // identificacion de la version del mensaje enviado
    // Se utilizan los primeros 3 bytes para indicar el numero de version.

    ret=strncmp(DaemonMsg, VERSION_ID, strlen(VERSION_ID));

    ver=((ret==0) ? 1 : 0);

    LogAlarm.Put(25, "getCaFromMsg(): "
		     "Version del mensaje: %d..\n", ver);

    if (ver==0)               // Primera version del Daemon..
    {
    elIsoMsg=new Iso_Msg(DaemonMsg, largoMsg);
    sprintf(CaString, "C%15.15s", elIsoMsg->GetField(42));

    // Elimina trailing blanks.
    char *ptrCh=CaString; 
    while (*ptrCh)
        {
        if (*ptrCh==' ')
            {
        *ptrCh='\0';
        break;
        }
        ptrCh++;
    }

    Cfg.GetItem("CACodes", CaString, cfgString);
    CaCode=atoi(cfgString);

    LogAlarm.Put(5, "getCaFromMsg(): "
		     "Centro autorizador %s para comercio "
         	     "[%s]..\n", cfgString, CaString); 

    fd=fopen("/online/log/credito", "a");
    fprintf(fd, "getCaFromMsg(): "
		     "Centro autorizador %s para comercio "
         	     "[%s]..\n", cfgString, CaString); 
    fclose(fd);

    delete elIsoMsg;
    }
    else                   // Segunda version. Incluye en el mensaje el CA.
    {
    ptrCaCode=DaemonMsg+strlen(VERSION_ID);
    CaCode=*((int *)ptrCaCode);        // En el mensaje nuevo luego del
                                       // VERSION_ID viene el CA.
    }
    return CaCode;
}

///
//
//  Funcion que determina el tipo del mensajes a partir del string 
//  del mensajes y su longitud.
//
///
int getTypeMsgFromMsg (char *msg_string, int msg_long)  
{
  Iso_Msg   *mensaje_iso;
  int       tipo;

  mensaje_iso=new Iso_Msg(msg_string, msg_long); 
  tipo=mensaje_iso->GetMsgType();
  return tipo;

}

 
///
/// Funcion de inicializacion del Daemon de X25.
/// 

void init()
{
    char    cfgString[200];             // String de configurcion
    char    NuaCa0[200];                // Nua para el centro Autorizador 1
    char    NuaCa1[200];                // Nua para el centro Autorizador 2
    char    channel[20];                // Numero de canal.

    int     QID;                        // Identificacion de cola.
    int     ret;                        // Codigos de retorno.
    int     QPerms;                     // Permisos para la Colas.
    int     ShMID;                      // Identificacion de memoria compartida.

    // Informacion de logging de actividades.
    //
    Cfg.GetItem("CAVisaDaemon", "LogName", cfgString);
    LogAlarm.Open(cfgString);

    Cfg.GetItem("CAVisaDaemon", "LogLevel", cfgString);
    LogAlarm.SetLevel(atoi(cfgString));

    // Logging para el operador.
    //
    Cfg.GetItem("CAVisaDaemon", "LogOperator", cfgString);
    LogOperator.Open(cfgString);
    LogOperator.SetLevel(10);

    // Informacion de Cola X25.
    // 
    Cfg.GetItem("CAVisaDaemon", "QueueID", cfgString);
    QID=atoi(cfgString);
    Cfg.GetItem("CAVisaDaemon", "QueuePerms", cfgString);
    QPerms=atoi(cfgString);

    ret=XQueue.Create(QID, atoi(cfgString));
    if (ret==NOOK)
        {
        LogAlarm.Put( 1, "init(): "
                         "Error (%d) al crear cola de X25!!\n",
                         XQueue.GetErrno());
        exit(1);
        }

    ret=XQueue.Flush(0);

    time_t horaErr;
    horaErr=time(&horaErr);
    LogAlarm.Put( 5, "init(): %s",
                     asctime(localtime(&horaErr)));

    // Cola de Eventos del Daemon Principal.
    Cfg.GetItem("VisaDaemon", "QueueID", cfgString);
    LogAlarm.Put(10, "init(): Abriendo cola de eventos...\n");

    ret=EQueue.Open(atoi(cfgString));
    if (ret==NOOK)
        {
        LogAlarm.Put( 1,"init(): "
                        "Error (%d) en apertura de cola de eventos!!\n",
                        EQueue.GetErrno());
        exit(1);
        }

    Cfg.GetItem("PIDTable", "ShMemID", cfgString);
    ShMID=atoi(cfgString);

    Cfg.GetItem("PIDTable", "MaxEntries", cfgString);

    // Abriendo Tabla de Pid de shutdown.
    LogAlarm.Put(0, "init(): "
		    "Abriendo tabla de PIDs (%d) [%s]\n",
		    ShMID, cfgString);

    while (PTable.Open(ShMID, atoi(cfgString))==NOOK)
        if (PTable.GetErrno()!=ENOENT)
            {
            LogAlarm.Put( 0, "init(): "
                             "Error (%d) en Apertura de Share Memory!!\n", 
                             PTable.GetErrno());

            exit(1);
            }

    LogAlarm.Put(10, "init(): "
                     "Agregando PID a la tabla de PIDs...\n");

    ret=PTable.PutPid(getpid());
    if (ret==NOOK)
        {
        LogAlarm.Put( 1,"init(): "
                        "Error (%d) al setear el Pid del proceso!!\n", 
                        PTable.GetErrno());

        exit(1);
        }
    // Abrir Canales....

    OpenCaChannels(0);          // POSNET.
    OpenCaChannels(1);          // Visa
//    OpenCaChannels(2);          // Diners

    // Controlador de Status del centro Autorizador.
    LogAlarm.Put(30, "init(): Abriendo Controlador de Status..\n");
    Cfg.GetItem("CACStatVisa", "ShMemID", cfgString);
    while (CaStsCtrl.Open(atoi(cfgString))==NOOK)
        {
        if (CaStsCtrl.GetErrno()!=ENOENT)
            {
            LogAlarm.Put( 1, "init(): "
                             "Error (%d) al acceder controlador de STS!!\n",
                              CaStsCtrl.GetErrno());
            exit(1);
            }
        }

    hora_ult=time(&hora_ult);
}    

///
/// Funcion de Shutdown del Daemon.
/// 

static void shut_down(int sig_id)
{
    int counter=0;
    int ret; 

    for(counter=0;counter<NRO_CHANS;counter++)
        {
        LogAlarm.Put(0, "shut_down: Cerrando canal X.25...\n");
        ret=X25LogiCha[counter]->Close();
        if (ret==NOOK)
            {
            LogAlarm.Put(0,"shut_down: " 
                           "Error (%d) al Cerrar Canal X.25!!\n", 
                           X25LogiCha[counter]->GetErrno());
            exit(4);
            }
        } 
    LogAlarm.Put(0, "shut_down: Borrando PID de la tabla de PIDs...\n");
    ret=PTable.RemPid(getpid());
    if (ret==NOOK)
        {
        LogAlarm.Put(0, "shut_down: Error (%d)!!\n", PTable.GetErrno());
        }

    LogAlarm.Put(0, "shut_down: Cerrando cola de X.25...\n");
    ret=XQueue.Destroy();
    if (ret==NOOK)
        {
        LogAlarm.Put(0, "shut_down: Error (%d)!!\n", XQueue.GetErrno());
        }

    CloseCaChannels(0);         // Bajo Canales de centro autorizador 0
    CloseCaChannels(1);         // Bajo Canales de centro autorizador 1

    LogAlarm.Put(0, "shut_down: Hecho.\n");

    exit(0);
}



/// Apertura de centros Autorizadores 
///
/// Se le pasa como parametro el centro autorizador para el cual 
/// se abriran los canales.


int OpenCaChannels(int CA)
{
int QtyChan; 
int counter=0;
int ndx=0;
int ret=0;

static char cfgString[80];
static char cfgItem[80];

sprintf(cfgItem,"NroChannels%d",CA);
Cfg.GetItem("CAVisaDaemon", cfgItem, cfgString);
QtyChan=atoi(cfgString);           

sprintf(cfgItem,"NUA%d",CA);
Cfg.GetItem("CAVisaDaemon", cfgItem, cfgString);

ndx=0;
for (counter=0; counter<QtyChan; counter++)
    {
    for (;ndx<NRO_CHANS; ndx++)
        {
        if (X25LogiCha[ndx] == NULL)
            break;
        }

    if ( !(ndx<NRO_CHANS) )
        {
        LogAlarm.Put( 1, "OpenCaChannels(): "
                        "Error no se encuentran mas canales disponibles\n");
        return 10;
        }

    LogAlarm.Put(10, "OpenCaChannels(): "
                     "Abriendo canal: %d para CA: %d NUA: [%s]\n",
                     ndx, CA, cfgString);

    X25LogiCha[ndx]=new X25("x25s0");
    X25LogiCha[ndx]->setCodigoCa(CA);

    LogAlarm.Put(10, "OpenCaChannels(): Llamo al Open: \n");

    ret=X25LogiCha[ndx]->Open(cfgString);

    if (ret==NOOK)
        {
        LogAlarm.Put(1, "OpenCaChannels(): "
                        "Error (%d) al inicializar Canal logico\n", 
                        X25LogiCha[ndx]->GetErrno());

        LogAlarm.Put(1, "OpenCaChannels(): "
                        "Causa (%d) Diagnostico (%d)\n" ,
                        X25LogiCha[ndx]->GetX25ClearCause(),
                        X25LogiCha[ndx]->GetX25DiagnosticCause());

        //delete X25LogiCha[ndx];
        //X25LogiCha[ndx]=NULL; 
        }
    }

return OK;
}

//// Close Channels X.25
///
///


int CloseCaChannels(int CA)
{

int counter=0;

LogAlarm.Put(10, "CloseCaChannels(): "
                 "Cerrando canales para CA: %d \n",
                  CA);

for (counter=0;counter<NRO_CHANS; counter++)
    {
    if (X25LogiCha[counter] != NULL)
        {
        if (X25LogiCha[counter]->NroCA()==CA)
            {
            LogAlarm.Put(10, "CloseCaChannels(): " 
                             "Cerrando canal: %d para CA: %d \n",
                             counter, CA);
            X25LogiCha[counter]->Close();
            delete X25LogiCha[counter];
            X25LogiCha[counter]=NULL;
            }
        }

    }
return OK;
}

int ResetCaChannels(int codigoCa)
{
int counter;
int ret;

// Verificar estado del device driver del X25.
// Si esta caido resetear todos los canales y retornar.
// Recorrer todos los canales de codigo CA.
// Verificar si el canal esta ok.
// Si esta ok continuar.
// Si no verificar estado de X25
//     si hay error verificar este. 

for (counter=0;counter<NRO_CHANS;counter++)
    {
    if (X25LogiCha[counter]!=NULL)
       {
       ret=X25LogiCha[counter]->chkCircuit();
       }
    }
return OK;
}

// Verifica que existan canales disponibles para el centro autorizador.
int testOpen(int CaCode)
{
     int counter;
     int qtyChannel=0;

     for (counter=0; counter<NRO_CHANS; counter++)
         {
         if (X25LogiCha[counter]==NULL)
             continue;

         if (X25LogiCha[counter]->NroCA()==CaCode)
             qtyChannel++;
         }
    return qtyChannel>0 ? OK : NOOK;

}

         
int chkOrdenes(int caCode) 
{
int rc=0;
char fileCmd[100];
int counter=0;
int sec;
char aux_str1[3];
int Time2Reset;
time_t hora_act;

armaFileNameCmd(caCode,fileCmd);
Cfg.GetItem("CAVisaDaemon", "TimeToReset", aux_str1);

LogAlarm.Put( 5, "chkOrdenes(): "
                 "Test para Archivo: [%s]\n",fileCmd); 
     
rc=unlink(fileCmd);

// Setear hora actual 
hora_act=time(&hora_act);
LogAlarm.Put( 0,"chkOrdenes(): Hora Actual(): %s", 
                 asctime(localtime(&hora_act))); 

sec = difftime(hora_act, hora_ult); 

Time2Reset=atoi(aux_str1);

if ((rc==0) && (sec > Time2Reset)) // Pudo Borrar el archivo ==> Ergo Existia..
    {
    LogAlarm.Put( 5, "chkOrdenes(): "
                     "Orden de Cerrar Canales Centro Autorizador %d\n",
                     caCode); 
    for (counter=0; counter<NRO_CHANS; counter++)
        {

        if (X25LogiCha[counter]==NULL)
            continue;

        if (X25LogiCha[counter]->NroCA()==caCode)
            X25LogiCha[counter]->Close();
        }

    // Setear hora ultimo reseteo del canal
    hora_ult=time(&hora_ult);
    LogAlarm.Put( 0,"chkOrdenes(): Hora de reseteo de canal(): %s", 
                     asctime(localtime(&hora_ult))); 
    return 1;
    }
return 0;
}

// Arma el nombre del archivo que se utiliza como archivo de comandos.
// Para eso utiliza el directorio de logging.

void armaFileNameCmd(int caCode,char * fileName)
{
char cfgString[100];
strcpy(cfgString,"");
Cfg.GetItem("CAVisaDaemon", "LogDir", cfgString);

sprintf(fileName,"%sResetCa.%02d",cfgString,caCode);

}
