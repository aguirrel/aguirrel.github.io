#Definicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

ALLINC = -I./ -I$(IFILES) -I$(MFILES) -I$(DFILES)

CCC=xlC
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC) -o $(OFILES)$*.o -g


.C.o:
	$(CCC) $(CCCFLG) -c $<

all: $(LFILES)libmsgs.a

test:TestPrice.o PriceNew.o
	$(CCC) $(CCCFLG) $(OFILES)TestPrice.o $(OFILES)PriceNew.o \
		$(OFILES)Config.o -o TestPrice -L$(LFILES) -ldatabase

$(LFILES)libmsgs.a: Price.o 8583.o
	ar r $(LFILES)libmsgs.a $(OFILES)8583.o $(OFILES)Price.o

