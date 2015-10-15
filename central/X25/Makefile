ALLINC = -I./
CCC=xlC_r
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC)  -g

LLIBS= -L${ORACLE_HOME}/lib -lcore3 -lc3v6 -lnlsrtl3 -lclient -lserver\
 -lgeneric -lcommon -lsqlnet -lncr -lepc -lsql -lx25s

LIBS= -L${ORACLE_HOME}/lib -lx25s 

.SUFFIXES: .C .o

.C.o:
	$(CCC) $(CCCFLG) -c $< -o $*.o

ALLFILES= SRX25 svc 

all: $(ALLFILES)


clean:
	rm -f *.o *.a
	rm $(ALLFILES)


SRX25: SRX25.o
	$(CCC) $(CCCFLG) $(LIBS) \
		SRX25.o \
		-o SRX25
		@rm *.o
svc: svc.o
	$(CCC) $(CCCFLG) $(LIBS) \
		svc.o \
		-o svc
		@rm *.o

svc_servicio: svc_servicio.o
	$(CCC) $(CCCFLG) $(LIBS) \
		svc_servicio.o \
		-o svc_servicio
		@rm *.o

svc_servicio_thread: svc_servicio_thread.o
	$(CCC) $(CCCFLG) $(LIBS) \
		svc_servicio_thread.o \
		-o svc_servicio_thread
		@rm *.o

