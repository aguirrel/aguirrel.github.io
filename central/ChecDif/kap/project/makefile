#WORKING DIRECTORY
#WD=/home/desa/codigos/coto/v220
WD=/codigos/coto/v220

#PRODUCT DIRECTORY
#PD=/eft_coto/col/src/ChecDif
PD=/servofer/src/ChecDif/

#LEVEL DIRECTORY
LD=/kap

#OBJ DIRECTORY
OD=$(WD)$(PD)$(LD)/obj

#SOURCE DIRECTORY
SD=$(WD)$(PD)$(LD)/source

#INCLUDE DIRECTORY
ID1=$(WD)$(PD)$(LD)/include

OTHERLIBS = `cat $(ORACLE_HOME)/lib/sysliblist`

#TARGET DIRECTORY
TD=$(WD)$(PD)$(LD)/target

#COMPILER FLAGS
CFLAGS= -g -qalign=packed

INFILE=ChecDif

OSTYPE = AIX

ALLINC = -I$(ID1) -I$(ID2)

PCC_INCLUDES =  include=$(ID1)

#CC=cc_r
CC=gcc
CCFLG= -g -T512 -DOS_$(OSTYPE) -w $(ALLINC) -g
COMP_PROC=$(CC) $(CCFLG) $(LIBS)
PROCFLAGS= $(PCC_INCLUDES) release_cursor=yes ireclen=300 oreclen=132 select_error=no

#LIBS=-L${ORACLE_HOME}/lib32 -lclntsh ${ORACLE_HOME}/lib32/osntab.o -lm -lld -lcurses -lclient -lgeneric -lcommon $(LD1)
LIBS=-L${ORACLE_HOME}/lib32 -lclntsh ${ORACLE_HOME}/lib32/osntabst.o -lm -lld -lcurses -lclient -lgeneric -lcommon $(LD1)

all: $(INFILE)


$(INFILE): $(INFILE).pc 
	$(CC) $(CCFLG) $(LIBS) $(SD)/$(INFILE).o -o $(TD)/$(INFILE)
	@rm $(SD)/*.o

$(INFILE).pc:
	proc ${PROCFLAGS} iname=$(SD)/$(INFILE).pc oname=$(SD)/$(INFILE).c
	$(COMP_PROC) $(SD)/$(INFILE).c -c -o $(SD)/$(INFILE).o
#	@rm $(OD)/$(INFILE).c

