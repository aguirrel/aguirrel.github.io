#Definicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

ALLINC = -I./ -I$(IFILES) -I$(MFILES) -I$(DFILES)
CC=cc
CCFLG= -DOS_$(OSTYPE) -w $(ALLINC) -g

CCC=xlC
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC) -g

PCC=proc
PCCINC=${ORACLE_HOME}/proc/lib

PCCFLG=include=${PCCINC} release_cursor=yes code=ansi_c lname=/dev/null select_error=no sqlcheck=syntax lines=yes ireclen=132

# Librerias de la base de datos relacional (ORACLE):

# ORACLE 7.3.2.3 (aix 4.1.4)
ORACLELIBS=-L${ORACLE_HOME}/lib -lclntsh -lcore8 -lnls8 -lclient8 -lgeneric8 -lcommon8 -lsql8 

#Librerias propias
USRLIBS=-L$(LFILES) 

# Librerias del sistema
SYSLIBS=-lc -lm -lld /lib/crt0.o
SYSLIBS=-lc -lm -lld 

ALLIBS=$(ORACLELIBS) $(USRLIBS) $(SYSLIBS)

.SUFFIXES: .pc .C

.pc.o:
	$(PCC) $(PCCFLG) iname=$<
	$(CC) $(CCFLG) -c $*.c
	@rm $*.c

.C.o:
	$(CCC) $(CCCFLG) -c $< -o $*.o

all: ChecVta 

clean:
	-rm *.o *.a *.c

$(OFILES)ChecVta.o: ChecVta.c
	$(CC) $(CCFLG) -c ChecVta.c -o $(OFILES)ChecVta.o 

$(OFILES)DBFunc.o: DBFunc.o
	$(PCC) $(PCCFLG) iname=DBFunc.pc
	$(CC)  $(CCFLG) -c DBFunc.c -o$(OFILES)DBFunc.o
	rm DBFunc.c
	rm DBFunc.o

ChecVta: $(OFILES)ChecVta.o $(OFILES)DBFunc.o
	$(CC) $(CCFLG) $(ALLIBS) $(OFILES)DBFunc.o $(OFILES)ChecVta.o \
	-o $(EFILES2)ChecVta

	mv ChecVta $(EFILES)

