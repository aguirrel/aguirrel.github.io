#
#Definicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

ALLINC = -I./ 
CCC=g++
CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC)  -g

.SUFFIXES: .C .o

.C.o:
	$(CCC) $(CCCFLG) -c $< -o ./$*.o

ALLFILES= DemoEFT 
#ClienEFT

all: $(ALLFILES)

clean:
	cd $(EFILES) ;

DemoEFT: Price.o ConexSocket.o DemoEFT.o
	$(CCC) $(CCCFLG) Price.o ConexSocket.o DemoEFT.o -o demoeft -lm
	#rm -f Price.o ConexSocket.o DemoEFT.o 

#ClienEFT: Price.o ConexSocket.o ClienEFT.o
#	$(CCC) $(CCCFLG) Price.o ConexSocket.o ClienEFT.o -o clieneft


