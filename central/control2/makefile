#odefinicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

#Distintas versiones de ORACLE
ORACLE_HOME=/u01/app/oracle/product/7.3.2
#ORACLE_HOME=/home/oracle



ALLINC = -I./ -I$(IFILES) -I$(MFILES) -I$(DFILES)
CC=xlc
CCFLG= -DOS_$(OSTYPE) -w $(ALLINC) 

CCC=xlC
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC) 

PCC=proc
PCCINC=${ORACLE_HOME}/proc/lib

PCCFLG=include=${PCCINC} release_cursor=yes code=ansi_c lname=/dev/null select_error=no userid=u601/u601 lines=yes ireclen=132

#PCCFLG=include=${PCCINC} release_cursor=yes code=ansi_c lname=/dev/null select_error=no sqlcheck=semantics userid=u601/u601 lines=yes ireclen=132

#PCCFLG=include=${PCCINC} release_cursor=yes code=ansi_c lname=/dev/null select_error=no sqlcheck=syntax userid=u601/u601 lines=yes ireclen=132


# Librerias de la base de datos relacional (ORACLE):
ORACLELIBS= -lcore -lcv6 -lnlsrtl -lserver -lsql -lsqlnet -L${ORACLE_HOME}/lib ${ORACLE_HOME}/lib/osntab.o -bI:${ORACLE_HOME}/lib/mili.exp

# Librerias propias
USRLIBS= -L$(LFILES) -ldatabase

# Librerias del sistema
SYSLIBS= -lc -lm -lld /lib/crt0.o

ALLIBS=$(USRLIBS) $(SYSLIBS) $(LIBS)

#Existen tres versiones de librerias de ORACLE

# ORACLE 7.2 (aix 3.2.5)
#LIBS=-L./mensajes -L./eventos -L$(LFILES) -lmsgs -levents -ldatabase \
#     -lcore -lcv6 -lnlsrtl -lora -lsql -lsqlnet -L${ORACLE_HOME}/lib \
#     ${ORACLE_HOME}/lib/osntab.o -bI:${ORACLE_HOME}/lib/mili.exp -lc -lm -lld /lib/crt0.o -lbsd

# ORACLE 7.2 (aix 4.1.4)
#LIBS=-L${ORACLE_HOME}/lib -lsql ${ORACLE_HOME}/lib/osntab.o -lsqlnet -lora -lora -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 -lcore3 -lm -lld -lm -lm -lld -lm

#ORACLE 7.3.2.3 (aix 4.1.4)
LIBS= -L${ORACLE_HOME}/lib -lcore3 -lc3v6 -lnlsrtl3 -lserver -lgeneric -lcommon -lsqlnet -lncr -lepc -lsql -lclient

.SUFFIXES: .pc

.pc.o:
	$(PCC) $(PCCFLG) iname=$<
	$(CC) $(CCFLG) -c $*.c
	@rm $*.c

.C.o:
#	$(CCC) $(CCCFLG) -c -DTERMTABLE_CLASS_TEST $<
	$(CCC) $(CCCFLG) -c $<

all: vinculo     

clean:
	-rm *.o *.a *.c

$(EFILES)vinculoupd: $(OFILES)vinculoupd.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)vinculoupd.o \
                                 -o $(EFILES)vinculoupd

$(OFILES)vinculoupd.o: vinculoupd.pc 
	$(PCC) $(PCCFLG) iname=vinculoupd.pc
	$(CC) $(CCFLG) -o$(OFILES)vinculoupd.o -c vinculoupd.c 
	@rm vinculoupd.c


$(EFILES)vinculo: $(OFILES)vinculo.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)vinculo.o \
                                 -o $(EFILES)vinculo

$(OFILES)vinculo.o: vinculo.pc 
	$(PCC) $(PCCFLG) iname=vinculo.pc
	$(CC) $(CCFLG) -o$(OFILES)vinculo.o -c vinculo.c 
	@rm vinculo.c

$(EFILES)vinculo2: $(OFILES)vinculo2.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)vinculo2.o \
                                 -o $(EFILES)vinculo2

$(OFILES)vinculo2.o: vinculo2.pc 
	$(PCC) $(PCCFLG) iname=vinculo2.pc
	$(CC) $(CCFLG) -o$(OFILES)vinculo2.o -c vinculo2.c 
	@rm vinculo2.c


$(EFILES)c: $(OFILES)c.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)c.o \
                                 -o $(EFILES)c

$(OFILES)c.o: c.pc 
	$(PCC) $(PCCFLG) iname=c.pc
	$(CC) $(CCFLG) -o$(OFILES)c.o -c c.c 
	@rm c.c

gencie: $(OFILES)gencie.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)gencie.o \
                                 -o $(EFILES)gencie

$(OFILES)gencie.o: gencie.pc 
	$(PCC) $(PCCFLG) iname=gencie.pc
	$(CC) $(CCFLG) -o$(OFILES)gencie.o -c gencie.c 
	@rm gencie.c

convert: $(OFILES)convert.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)convert.o \
                                 -o $(EFILES)convert

$(OFILES)convert.o: convert.pc 
	$(PCC) $(PCCFLG) iname=convert.pc
	$(CC) $(CCFLG) -o$(OFILES)convert.o -c convert.c 
	@rm convert.c

trxpend: $(OFILES)trxpend.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)trxpend.o \
                                 -o $(EFILES)trxpend

$(OFILES)trxpend.o: trxpend.pc 
	$(PCC) $(PCCFLG) iname=trxpend.pc
	$(CC) $(CCFLG) -o$(OFILES)trxpend.o -c trxpend.c 
	@rm trxpend.c


vinculo: $(OFILES)vinculo.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)vinculo.o \
                                 -o $(EFILES)vinculo

$(OFILES)vinculo.o: vinculo.pc 
	$(PCC) $(PCCFLG) iname=vinculo.pc
	$(CC) $(CCFLG) -o$(OFILES)vinculo.o -c vinculo.c 
	@rm vinculo.c

vinculo: $(OFILES)vinculo.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)vinculo.o \
                                 -o $(EFILES)vinculo

$(OFILES)vinculo.o: vinculo.pc 
	$(PCC) $(PCCFLG) iname=vinculo.pc
	$(CC) $(CCFLG) -o$(OFILES)vinculo.o -c vinculo.c 
	@rm vinculo.c


trxpend2: $(OFILES)trxpend2.o 
	$(CCC) $(CCCFLG) $(LIBS) $(OFILES)trxpend2.o \
                                 -o $(EFILES)trxpend2

$(OFILES)trxpend2.o: trxpend2.pc 
	$(PCC) $(PCCFLG) iname=trxpend2.pc
	$(CC) $(CCFLG) -o$(OFILES)trxpend2.o -c trxpend2.c 
	@rm trxpend2.c

