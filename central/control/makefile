#Definicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

ALLINC = -I./ -I$(IFILES) -I$(MFILES) -I$(DFILES)
CC=gcc
CCFLG= -DOS_$(OSTYPE) -w $(ALLINC) -g

CCC=cc
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC) -g

PCC=proc
PCCINC=${ORACLE_HOME}/proc/lib

PCCFLG=include=${PCCINC} release_cursor=yes code=ansi_c lname=/dev/null select_error=no sqlcheck=syntax lines=yes ireclen=132

# Librerias de la base de datos relacional (ORACLE):

# ORACLE 7.3.2.3 (aix 4.1.4)
ORACLELIBS=-L${ORACLE_HOME}/lib32 -lclntsh -lcore9 -lnls9 -lclient9 -lgeneric9 -lcommon9 -lsql9 

#Librerias propias
USRLIBS=-L$(LFILES) 

ALLIBS=$(ORACLELIBS) $(USRLIBS) 

.SUFFIXES: .pc .C

.pc.o:
	$(PCC) $(PCCFLG) iname=$<
	$(CC) $(CCFLG) -c $*.c
	@rm $*.c

.C.o:
	$(CCC) $(CCCFLG) -c $< -o $*.o

all: trxpend 

clean:
	-rm *.o *.a *.c


$(OFILES)trxpend.o: trxpend.o
	$(PCC) $(PCCFLG) iname=trxpend.pc
	$(CC)  $(CCFLG) -c trxpend.c -o$(OFILES)trxpend.o
	@rm trxpend.c

trxpend: $(OFILES)trxpend.o 
	$(CC) $(CCFLG) $(ALLIBS) $(OFILES)trxpend.o \
	-o $(EFILES2)trxpend

