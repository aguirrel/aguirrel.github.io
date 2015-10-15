CCC=xlC_r
CCCFLG= -DOS_$(OSTYPE) -w -g

.SUFFIXES: .C .o

.C.o:
	$(CCC) $(CCCFLG) -c $< 

ALLFILES= iso2asc

all: $(ALLFILES)

iso2asc: iso2asc.o Modif8583.o
	$(CCC) $(CCCFLG) \
		iso2asc.o Modif8583.o \
		-o iso2asc
	rm -f *o

Modif8583: Modif8583.C
	$(CCC) $(CCFLG) \
		Modif8583.C -o Modif8583.o 

