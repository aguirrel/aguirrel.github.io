#
#Definicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

ALLINC = -I./ -I$(IFILES) -I$(MFILES) -I$(DFILES) -I$(VFILES)
CCC=xlC_r
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC)  -g

# ORACLE 7.3.2.3 (4.1.4)
LIBS= -L./mensajes -L./eventos -L$(LFILES) -lmsgs -levents -ldatabase -L${ORACLE_HOME}/lib -lclntsh -lcore8 -lclient8 -lgeneric8 -lcommon8 -lsql8 

LIBSX25= -L./mensajes  -L$(LFILES) -lmsgs -ldatabase -L${ORACLE_HOME}/lib -lcore8 -lclntsh -lclient8 -lgeneric8 -lcommon8 -lsql8

LIBSTEST= -L./mensajes -L./eventos -L$(LFILES) -ldatabase -L${ORACLE_HOME}/lib -lcore8 -lclntsh -lclient8 -lgeneric8 -lcommon8 -lsql8 

LIBSTEST2= -L./mensajes -L./eventos -L$(LFILES) -lmsgs -ldatabase -L${ORACLE_HOME}/lib -lcore8 -lclntsh -lclient8 -lgeneric8 -lcommon8 -lsql8 

# ORACLE 7.3.2.3 (4.1.4) para cierre 
LIBSCIE= -L./mensajes -L$(LFILES) -lmsgs -ldatabase -L${ORACLE_HOME}/lib -lcore8 -lclntsh -lclient8 -lgeneric8 -lcommon8 -lsql8

.SUFFIXES: .C .o


.C.o:
	$(CCC) $(CCCFLG) -c $< -o $(OFILES)$*.o

ALLFILES= cacstatus CierreLote Daemon RecvMsg SendMsg SendOff \
	SendOffMsg SendPing SendRev SendSD SSend TimeOutDaemon TCPDaemon \
	X25Daemon CierreLoteCASuc CACStatus IniX25 CfgX25


all: $(ALLFILES)


clean:
	rm -f $(OFILES)*.o $(LFILES)*.a
	cd $(EFILES) ; rm $(ALLFILES)


SRX25: SRX25.o
	$(CCC) $(CCCFLG) -bloadmap:ll $(LIBS) \
		$(OFILES)SRX25.o \
		-o $(EFILES)SRX25

IniX25: IniX25.o EventQueue.o Config.o EventQueue.o \
		Log.o TraceNmbr.o PidTable.o Queues.o Semaphores.o \
		TimeOutQueue.o TrxHeap.o CACStat.o Pos.o 
	$(CCC) $(CCCFLG) -bloadmap:ll \
		$(OFILES)Config.o       $(OFILES)IniX25.o \
		$(OFILES)EventQueue.o   $(OFILES)Log.o \
		$(OFILES)TraceNmbr.o    $(OFILES)PidTable.o \
		$(OFILES)Queues.o       $(OFILES)Semaphores.o \
		$(OFILES)TimeOutQueue.o $(OFILES)TrxHeap.o \
		$(OFILES)CACStat.o      $(OFILES)Pos.o \
		-o $(EFILES)IniX25

CfgX25: CfgX25.o EventQueue.o Config.o EventQueue.o \
		Log.o TraceNmbr.o PidTable.o Queues.o Semaphores.o \
		TimeOutQueue.o TrxHeap.o CACStat.o Pos.o
		$(CCC) $(CCCFLG) -bloadmap:ll \
		$(OFILES)Config.o       $(OFILES)CfgX25.o \
		$(OFILES)EventQueue.o   $(OFILES)Log.o \
		$(OFILES)TraceNmbr.o    $(OFILES)PidTable.o \
		$(OFILES)Queues.o       $(OFILES)Semaphores.o \
		$(OFILES)TimeOutQueue.o $(OFILES)TrxHeap.o \
		$(OFILES)CACStat.o      $(OFILES)Pos.o \
		-o $(EFILES)CfgX25

CACStatus: CACStatus.o EventQueue.o Config.o EventQueue.o \
		Log.o TraceNmbr.o PidTable.o Queues.o Semaphores.o \
		TimeOutQueue.o TrxHeap.o CACStat.o Pos.o 
	$(CCC) $(CCCFLG) -bloadmap:ll \
		$(OFILES)Config.o       $(OFILES)CACStatus.o \
		$(OFILES)EventQueue.o   $(OFILES)Log.o \
		$(OFILES)TraceNmbr.o    $(OFILES)PidTable.o \
		$(OFILES)Queues.o       $(OFILES)Semaphores.o \
		$(OFILES)TimeOutQueue.o $(OFILES)TrxHeap.o \
		$(OFILES)CACStat.o      $(OFILES)Pos.o \
		-o $(EFILES)CACStatus

CierreLoteCA: CierreLoteCA.o Config.o Log.o PidTable.o CACStat.o \
              Queues.o Semaphores.o \
        $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBSCIE) \
		$(OFILES)CierreLoteCA.o	$(OFILES)Config.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o \
		$(OFILES)CACStat.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o -bloadmap:ll \
		-o $(EFILES)CierreLoteCA

CierreLoteCASuc: CierreLoteCASuc.o Config.o Log.o PidTable.o CACStat.o \
              Queues.o Semaphores.o \
        $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBSCIE) \
		$(OFILES)CierreLoteCASuc.o	$(OFILES)Config.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o \
		$(OFILES)CACStat.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o -bloadmap:ll \
		-o $(EFILES)CierreLoteCASuc

Daemon: Daemon.o EventQueue.o Config.o \
		Log.o TraceNmbr.o PidTable.o Queues.o Semaphores.o \
		TimeOutQueue.o TrxHeap.o CACStat.o Pos.o  
	$(CCC) $(CCCFLG) -bloadmap:ll $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Daemon.o \
		$(OFILES)EventQueue.o	$(OFILES)Log.o \
		$(OFILES)TraceNmbr.o	$(OFILES)PidTable.o \
		$(OFILES)Queues.o	$(OFILES)Semaphores.o \
		$(OFILES)TimeOutQueue.o $(OFILES)TrxHeap.o \
		$(OFILES)CACStat.o	$(OFILES)Pos.o \
                -o $(EFILES)Daemon

SendRev: Config.o Log.o SendRev.o Sockets.o Queues.o $(LFILES)libdatabase.a PidTable.o Semaphores.o
	$(CCC) $(CCCFLG) $(LIBSTEST2) \
		$(OFILES)Log.o		$(OFILES)Config.o \
		$(OFILES)SendRev.o	$(OFILES)Queues.o     \
		$(OFILES)PidTable.o	$(OFILES)Semaphores.o \
		$(OFILES)CACStat.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendRev

SendPing: Config.o Log.o SendPing.o Sockets.o Queues.o $(LFILES)libdatabase.a PidTable.o Semaphores.o
	$(CCC) $(CCCFLG) $(LIBSTEST2) \
		$(OFILES)Log.o		$(OFILES)Config.o \
		$(OFILES)SendPing.o	$(OFILES)Queues.o     \
		$(OFILES)PidTable.o	$(OFILES)Semaphores.o \
		$(OFILES)CACStat.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendPing

SendAnul: SendAnul.o
	$(CCC) $(CCCFLG) $(LIBSX25) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)SendAnul.o	$(OFILES)Queues.o     \
		$(OFILES)PidTable.o	$(OFILES)Semaphores.o \
		$(OFILES)CACStat.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendAnul

AnulDaemon: AnulDaemon.o
	$(CCC) $(CCCFLG) $(LIBSX25) \
		$(OFILES)Config.o \
		$(OFILES)AnulDaemon.o     \
		-o $(EFILES)AnulDaemon

TimeOutDaemon: TimeOutQueue.o TimeOutDaemon.o
	$(CCC) $(CCCFLG) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(OFILES)TimeOutQueue.o \
		$(OFILES)TimeOutDaemon.o \
		-o $(EFILES)TimeOutDaemon

TCPDaemon: Sockets.o Config.o Log.o PidTable.o Queues.o Semaphores.o  \
           TCPDaemon.o QChild.o
	$(CCC) $(CCCFLG) \
		$(OFILES)Config.o 	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(OFILES)Sockets.o \
		$(OFILES)TCPDaemon.o	$(OFILES)QChild.o \
		-o $(EFILES)TCPDaemon

X25Daemon: X25Daemon.o  LinkX25.o Status.o Queues.o
	$(CCC) $(CCCFLG) $(LIBSX25) -bloadmap:ll \
		$(OFILES)Config.o	$(OFILES)LinkX25.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o  \
		$(OFILES)Queues.o	$(OFILES)X25Daemon.o \
		$(OFILES)CACStat.o	$(OFILES)Semaphores.o \
		$(OFILES)Status.o \
		-lx25s -o $(EFILES)X25Daemon

