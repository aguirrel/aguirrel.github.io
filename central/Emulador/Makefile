#
#Definicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

ALLINC = -I./ -I$(IFILES) -I$(MFILES) -I$(DFILES) -I$(VFILES)
CCC=xlC_r
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC)  -g

# Existen tres definiciones distintas de librerias. (3.2.5 y 4.1.4)
# ORACLE 7.2 (3.2.5)
#LIBS=-L./mensajes -L./eventos -L$(LFILES) -lmsgs -levents -ldatabase \
#     -lcore -lcv6 -lnlsrtl -lora -lsql -lsqlnet -L${ORACLE_HOME}/lib \
#     ${ORACLE_HOME}/lib/osntab.o -bI:${ORACLE_HOME}/lib/mili.exp -lc -lm -lld /lib/crt0.o -lbsd

# ORACLE 7.2 (4.1.4)
#LIBS=-L${ORACLE_HOME}/lib -lsql ${ORACLE_HOME}/lib/osntab.o -lsqlnet -lora -lnlsrtl3 -lc3v6 -lcore3 -lld -lm -L$(LFILES). -levents -ldatabase -lmsgs

# ORACLE 7.3.2.3 (4.1.4)
LIBS= -L./mensajes -L./eventos -L$(LFILES) -lmsgs -levents -ldatabase -L${ORACLE_HOME}/lib -lcore3 -lc3v6 -lnlsrtl3 -lclient -lserver -lgeneric -lcommon -lsqlnet -lncr -lepc -lsql 

LIBSX25= -L./mensajes  -L$(LFILES) -lmsgs -ldatabase -L${ORACLE_HOME}/lib -lcore3 -lc3v6 -lnlsrtl3 -lclient -lserver -lgeneric -lcommon -lsqlnet -lncr -lepc -lsql 

LIBSTEST= -L./mensajes -L./eventos -L$(LFILES) -ldatabase -L${ORACLE_HOME}/lib -lcore3 -lc3v6 -lnlsrtl3 -lclient -lserver -lgeneric -lcommon -lsqlnet -lncr -lepc -lsql 

LIBSTEST2= -L./mensajes -L./eventos -L$(LFILES) -lmsgs -ldatabase -L${ORACLE_HOME}/lib -lcore3 -lc3v6 -lnlsrtl3 -lclient -lserver -lgeneric -lcommon -lsqlnet -lncr -lepc -lsql 

# ORACLE 7.3.2.3 (4.1.4) para cierre 
LIBSCIE= -L./mensajes -L$(LFILES) -lmsgs -ldatabase -L${ORACLE_HOME}/lib -lcore3 -lc3v6 -lnlsrtl3 -lclient -lserver -lgeneric -lcommon -lsqlnet -lncr -lepc -lsql 

.SUFFIXES: .C .o


.C.o:
	$(CCC) $(CCCFLG) -c $< -o $(OFILES)$*.o

ALLFILES= CACStatus CierreLote Daemon ListTOQ RecvMsg SendMsg SendOff \
	SendOffMsg SendPing SendRev SendSD SSend TimeOutDaemon TCPDaemon \
	X25Daemon BackSend ListPid Prueba


all: $(ALLFILES)


clean:
	rm -f $(OFILES)*.o $(LFILES)*.a
	cd $(EFILES) ; rm $(ALLFILES)


# Modulo de envio de transacciones en background de las sucursales.   
# Envia aquellas transacciones que no fueron enviadas al price.      
#
BackSend: BackSend.o Sockets.o Log.o Config.o 
	$(CCC) $(CCCFLG) -bloadmap:ll $(LIBS) \
		$(OFILES)BackSend.o $(OFILES)Log.o \
		$(OFILES)Config.o   $(OFILES)Sockets.o \
		-o $(EFILES)BackSend


# Modulo para reporte del estado del canal X.25
#
CACStatus: CACStatus.o Config.o CACStat.o Semaphores.o
	$(CCC) $(CCCFLG) -bloadmap:ll $(LIBS) \
		$(OFILES)CACStatus.o $(OFILES)Config.o \
		$(OFILES)CACStat.o   $(OFILES)Semaphores.o \
		-o $(EFILES)CACStatus

#
CierreLote: CierreLote.o Config.o Log.o PidTable.o CACStat.o Queues.o \
            Semaphores.o \
        $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)CierreLote.o	$(OFILES)Config.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o \
		$(OFILES)CACStat.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(LFILES)libdatabase.a \
		-o $(EFILES)CierreLote

CierreLoteSuc: CierreLoteSuc.o Config.o Log.o PidTable.o CACStat.o \
                  Queues.o Semaphores.o \
        $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)CierreLoteSuc.o   $(OFILES)Config.o \
		$(OFILES)Log.o		   $(OFILES)PidTable.o \
		$(OFILES)CACStat.o	   $(OFILES)Queues.o \
		$(OFILES)Semaphores.o	   $(LFILES)libdatabase.a \
		-o $(EFILES)CierreLoteSuc

CierreLoteSucD: CierreLoteSucD.o Config.o Log.o PidTable.o CACStat.o \
                  Queues.o Semaphores.o \
        $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)CierreLoteSucD.o   $(OFILES)Config.o \
		$(OFILES)Log.o		   $(OFILES)PidTable.o \
		$(OFILES)CACStat.o	   $(OFILES)Queues.o \
		$(OFILES)Semaphores.o	   $(LFILES)libdatabase.a \
		-o $(EFILES)CierreLoteSucD

CierreLoteCA: CierreLoteCA.o Config.o Log.o PidTable.o CACStat.o \
              Queues.o Semaphores.o \
        $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBSCIE) \
		$(OFILES)CierreLoteCA.o	$(OFILES)Config.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o \
		$(OFILES)CACStat.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o -bloadmap:ll \
		-o $(EFILES)CierreLoteCA

CierreLoteCaj: CierreLoteCaj.o Config.o Log.o PidTable.o CACStat.o \
              Queues.o Semaphores.o \
        $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)CierreLoteCaj.o	$(OFILES)Config.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o \
		$(OFILES)CACStat.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(LFILES)libdatabase.a \
		-o $(EFILES)CierreLoteCaj

# Modulo principal - Conversion de mensajes
#
Daemon: Daemon.o EventQueue.o DataObject.o Config.o EventQueue.o \
                Log.o TraceNmbr.o PidTable.o Queues.o Semaphores.o \
                TimeOutQueue.o TrxHeap.o CACStat.o Pos.o LogControl.o 
	$(CCC) $(CCCFLG) -bloadmap:ll $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Daemon.o \
		$(OFILES)EventQueue.o	$(OFILES)Log.o \
		$(OFILES)TraceNmbr.o	$(OFILES)PidTable.o \
		$(OFILES)Queues.o	$(OFILES)Semaphores.o \
		$(OFILES)TimeOutQueue.o $(OFILES)TrxHeap.o \
		$(OFILES)CACStat.o	$(OFILES)Pos.o \
		$(OFILES)LogControl.o   $(OFILES)DataObject.o\
                -o $(EFILES)Daemon


Daemon2: Daemon.o  
	$(CCC) $(CCCFLG) -bloadmap:ll $(LIBS) \
                $(OFILES)Config.o       $(OFILES)Daemon.o \
                $(OFILES)EventQueue.o   $(OFILES)Log.o \
                $(OFILES)TraceNmbr.o    $(OFILES)PidTable.o \
                $(OFILES)Queues.o       $(OFILES)Semaphores.o \
                $(OFILES)TimeOutQueue.o $(OFILES)TrxHeap.o \
                $(OFILES)CACStat.o      $(OFILES)Pos.o \
                $(OFILES)LogControl.o   $(OFILES)DataObject.o\
                -o $(EFILES)Daemon

# Modulo controlador de timeouts - Controla y genera eventos de timeout
#
ListTOQ: Config.o Log.o PidTable.o Queues.o Semaphores.o ListTOQ.o
	$(CCC) $(CCCFLG) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(OFILES)ListTOQ.o \
		-o $(EFILES)ListTOQ

# Modulo para listar los pid disponibles en la tabla de control. 
#
ListPid: Config.o Log.o PidTable.o Queues.o Semaphores.o ListPid.o
	$(CCC) $(CCCFLG) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(OFILES)ListPid.o \
		-o $(EFILES)ListPid


# Programa de prueba - Manda un mensaje via sockets
# Modulo de X.25  
#
RecvMsg: Config.o Log.o PidTable.o Queues.o RecvMsg.o Semaphores.o \
         $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)RecvMsg.o	$(OFILES)Semaphores.o \
		-o $(EFILES)RecvMsg

# Programa de prueba - Manda un mensaje via sockets
#
SendMsg: Config.o Log.o SendMsg.o Sockets.o $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)SendMsg.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendMsg

# Programa de prueba - Manda un mensaje de offline via sockets
#
SendOffMsg: Config.o Log.o SendOffMsg.o Sockets.o $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Log.o  \
		$(OFILES)SendOffMsg.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendOffMsg

# Programa de envio de mensajes fuera de linea.
#
SendOff: Config.o Log.o SendOff.o Sockets.o Queues.o $(LFILES)libdatabase.a PidTable.o
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)SendOff.o	$(OFILES)Queues.o \
		$(OFILES)PidTable.o	$(OFILES)Semaphores.o \
		$(OFILES)Sockets.o	-o $(EFILES)SendOff

# Modulo de EchoTest - Genera un evento echotest
#
SendPing: Config.o Log.o PidTable.o Queues.o Semaphores.o SendPing.o Sockets.o
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(OFILES)SendPing.o \
		$(OFILES)CACStat.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendPing


# Programa de envio de reversos.
#
SendRev: Config.o Log.o SendRev.o Sockets.o Queues.o $(LFILES)libdatabase.a PidTable.o Semaphores.o
	$(CCC) $(CCCFLG) $(LIBSTEST2) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)SendRev.o	$(OFILES)Queues.o     \
		$(OFILES)PidTable.o	$(OFILES)Semaphores.o \
		$(OFILES)CACStat.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendRev

SendAnul: SendAnul.o
	$(CCC) $(CCCFLG) $(LIBSX25) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)SendAnul.o	$(OFILES)Queues.o     \
		$(OFILES)PidTable.o	$(OFILES)Semaphores.o \
		$(OFILES)CACStat.o	$(OFILES)Sockets.o \
		-o $(EFILES)SendAnul

# Modulo de shutdown - Genera evento de shutdown
#
SendSD: Config.o Queues.o SendSD.o
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Queues.o \
		$(OFILES)SendSD.o	-o $(EFILES)SendSD    

# Programa de prueba - Manda un mensaje via sockets
#
SSend: Config.o Log.o SSend.o Sockets.o $(LFILES)libdatabase.a
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)SSend.o	$(OFILES)Sockets.o \
		-o $(EFILES)SSend

# Modulo controlador de timeouts - Controla y genera eventos de timeout
#
TimeOutDaemon: TimeOutQueue.o TimeOutDaemon.o
	$(CCC) $(CCCFLG) \
		$(OFILES)Config.o	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(OFILES)TimeOutQueue.o \
		$(OFILES)TimeOutDaemon.o \
		-o $(EFILES)TimeOutDaemon


# Modulo de sockets - Recibe los mensajes via socket y los manda a la cola de
#		      mensajes 
#
TCPDaemon: Sockets.o Config.o Log.o PidTable.o Queues.o Semaphores.o  \
           TCPDaemon.o QChild.o
	$(CCC) $(CCCFLG) \
		$(OFILES)Config.o 	$(OFILES)Log.o \
		$(OFILES)PidTable.o	$(OFILES)Queues.o \
		$(OFILES)Semaphores.o	$(OFILES)Sockets.o \
		$(OFILES)TCPDaemon.o	$(OFILES)QChild.o \
		-o $(EFILES)TCPDaemon


# Modulo de X.25 
#
Em: Em.o
	$(CCC) $(CCCFLG) $(LIBSX25) -bloadmap:ll \
		$(OFILES)Config.o	$(OFILES)LinkX25.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o  \
		$(OFILES)Queues.o	$(OFILES)Em.o \
		$(OFILES)CACStat.o	$(OFILES)Semaphores.o \
		$(OFILES)Status.o \
		-lx25s -o $(EFILES)EmDaemon

X25Prueba: X25Prueba.o  
	$(CCC) $(CCCFLG) $(LIBSX25) -bloadmap:ll \
		$(OFILES)Config.o	$(OFILES)LinkX25.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o  \
		$(OFILES)Queues.o	$(OFILES)X25Prueba.o \
		$(OFILES)CACStat.o	$(OFILES)Semaphores.o \
		-lx25s -o $(EFILES)X25Prueba
X25pp: Config.o Log.o  LinkX25.o \
           PidTable.o Queues.o X25pp.o  \
           Semaphores.o CACStat.o 
	$(CCC) $(CCCFLG) $(LIBS) \
		$(OFILES)Config.o	$(OFILES)LinkX25.o \
		$(OFILES)Log.o		$(OFILES)PidTable.o  \
		$(OFILES)Queues.o	$(OFILES)X25pp.o \
		$(OFILES)CACStat.o	$(OFILES)Semaphores.o \
		-lx25s -o $(EFILES)X25pp

msgdump: msgdump.o 
	$(CCC) $(CCCFLG) $(LIBSTEST2) -bloadmap::ll \
		$(OFILES)msgdump.o $(OFILES)Config.o \
		 -o msgdump

#########################################################################
#########################################################################
#									#
#	Programas de prueba de los distintos modulos			#
#									#
#########################################################################

DBTest: DBTest.o 
	$(CCC) $(CCCFLG) DBTest.o -o $(EFILES)DBTest

DBTest2: DBTest2.o 
	$(CCC) $(CCCFLG) DBTest2.o -o $(EFILES)DBTest2

ExConf: ExConf.o Config.o
	$(CCC) $(CCCFLG) Config.o ExConf.o -o $(EFILES)ExConf

PTTest: PTTest.o Semaphores.o
	$(CCC) $(CCCFLG) PTTest.o PidTable.o Semaphores.o -o $(EFILES)PTTest

PidTableTest: PidTable.C Semaphores.o
	$(CCC) -c $(CCCFLG) -DPIDTABLE_CLASS_TEST PidTable.C PidTableTest.o
	$(CCC) $(CCCFLG) PidTableTest.o Semaphores.o -o $(EFILES)PidTableTest

SemTest1: Semaphores.C
	$(CCC) $(CCCFLG) -DSEMAPHORE_CLASS_TEST Semaphores.C -o $(EFILES)SemTest1

SemTest2: Semaphores.o SemTest.o
	$(CCC) $(CCCFLG) Semaphores.o SemTest.o -o $(EFILES)SemTest2

HeapTest: TrxHeap.C Log.o
	$(CCC) $(CCCFLG) TrxHeap.C Log.o -DTRXHEAP_CLASS_TEST -o $(EFILES)HeapTest

TestPrice: Config.o TestPrice.o EventQueue.o Log.o TraceNmbr.o PidTable.o \
        Queues.o Semaphores.o TimeOutQueue.o TrxHeap.o LogError.o Fifo.o \
        CACStat.o \
        $(LFILES)libdatabase.a $(LFILES)libevents.a 
	$(CCC) $(CCCFLG) -bloadmap:ll $(LIBS) \
		$(OFILES)Config.o	$(OFILES)TestPrice.o \
		$(OFILES)EventQueue.o	$(OFILES)Log.o \
		$(OFILES)TraceNmbr.o	$(OFILES)PidTable.o \
		$(OFILES)Queues.o	$(OFILES)Semaphores.o \
		$(OFILES)TimeOutQueue.o $(OFILES)TrxHeap.o \
		$(OFILES)LogError.o     $(OFILES)Fifo.o \
		$(OFILES)CACStat.o	-o $(EFILES)TestPrice

TestLog: TestLog.o LogControl.o Log.o
	$(CCC) $(CCFLG) $(LIBS) \
		$(OFILES)TestLog.o	$(OFILES)Log.o \
		$(OFILES)LogControl.o  $(OFILES)Config.o \
		-o $(EFILES)TestLog

Test: Test.o
	$(CCC) $(CCFLG) $(LIBSTEST) \
		$(OFILES)Test.o -o Test.o

#########################################################################

dist: CierreLote Daemon RecvMsg X25Daemon TCPDaemon TimeOutDaemon SendMsg SendPing SendSD Prueba
	tar cvf daemons.tar CierreLote Daemon RecvMsg X25Daemon TCPDaemon TimeOutDaemon SendMsg SendPing SendSD 

