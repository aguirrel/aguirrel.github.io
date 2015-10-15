CCC=xlC

#Definicion  del directorio donde realiza las busquedas de las dependencias
# Ver manual mak.

ALLINC=-I./ -I$(IFILES) -I$(MFILES) -I$(DFILES)

CCCFLG= -DOS_$(OSTYPE) -w $(ALLINC) -g

EINCS = $(MFILES)8583.h $(IFILES)CACStat.h $(IFILES)Config.h  \
        $(IFILES)Daemons.h $(IFILES)Log.h $(IFILES)PidTable.h \
        $(IFILES)Queues.h  \
        $(DFILES)Table.h $(IFILES)TimeOutQueue.h $(IFILES)TraceNmbr.h \
        $(IFILES)TrxHeap.h Batch.h BatchRta.h \
        Cierre.h CierreRta.h EchoTest.h EchoTestRta.h PriceAnul.h \
        PriceOffLine.h PriceOn.h PriceOnRta.h RevTrx.h RevTrxRta.h \
        ShutDown.h TimeOut.h TOCierre.h TOCierreB.h TOBatch.h \
        TOEchoTest.h TORevTrx.h PriceRev.h 

all: $(LFILES2)libevents.a 

$(OFILES)RtaRevAnul.o: RtaRevAnul.C
	$(CCC) $(CCCFLG) -c RtaRevAnul.C -o $@

$(OFILES)TORevAnul.o: TORevAnul.C
	$(CCC) $(CCCFLG) -c TORevAnul.C -o $@

$(OFILES)TrxRevAnul.o: TrxRevAnul.C
	$(CCC) $(CCCFLG) -c TrxRevAnul.C -o $@

$(OFILES)TOAnulTrx.o: TOAnulTrx.C
	$(CCC) $(CCCFLG) -c TOAnulTrx.C -o $@

$(OFILES)Batch.o: $(EINCS) Batch.C 
	$(CCC) $(CCCFLG) -c Batch.C -o $@

$(OFILES)BatchRta.o: $(EINCS) BatchRta.C
	$(CCC) $(CCCFLG) -c BatchRta.C -o $@

$(OFILES)Cierre.o: $(EINCS) Cierre.C
	$(CCC) $(CCCFLG) -c Cierre.C -o $@

$(OFILES)CierreRta.o: $(EINCS) CierreRta.C
	$(CCC) $(CCCFLG) -c CierreRta.C  -o $@

$(OFILES)EchoTest.o: $(EINCS) EchoTest.C
	$(CCC) $(CCCFLG) -c EchoTest.C -o $@

$(OFILES)EchoTestRta.o: $(EINCS) EchoTestRta.C
	$(CCC) $(CCCFLG) -c EchoTestRta.C -o $@

$(OFILES)Events.o: $(EINCS) Events.C
	$(CCC) $(CCCFLG) -c Events.C -o $@

$(OFILES)PriceAnul.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c PriceAnul.C -o $@

$(OFILES)PriceOffLine.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c PriceOffLine.C -o $@

$(OFILES)PriceOn.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c PriceOn.C -o $@

$(OFILES)PriceOnRta.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c PriceOnRta.C -o $@

$(OFILES)PriceRev.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c PriceRev.C -o $@

$(OFILES)RevTrx.o: $(EINCS) TrxOff.h $(DFILES)DBTrxOff.h
	$(CCC) $(CCCFLG) -c RevTrx.C -o $@

$(OFILES)TrxAnul.o: $(EINCS)
	$(CCC) $(CCCFLG) -c TrxAnul.C -o $@

$(OFILES)RevTrxRta.o: $(EINCS)  
	$(CCC) $(CCCFLG) -c RevTrxRta.C -o $@

$(OFILES)RtaTrxOff.o: $(EINCS)  RtaTrxOff.h TrxOff.h $(DFILES)DBTrxOff.h
	$(CCC) $(CCCFLG) -c RtaTrxOff.C -o $@

$(OFILES)ShutDown.o: $(EINCS)  
	$(CCC) $(CCCFLG) -c ShutDown.C -o $@

$(OFILES)TOBatch.o:  $(EINCS)
	$(CCC) $(CCCFLG) -c TOBatch.C -o $@

$(OFILES)TOCierre.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c TOCierre.C -o $@

$(OFILES)TOCierreB.o: $(EINCS)  
	$(CCC) $(CCCFLG) -c TOCierreB.C -o $@

$(OFILES)TOEchoTest.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c TOEchoTest.C -o $@

$(OFILES)TORevTrx.o: $(EINCS) 
	$(CCC) $(CCCFLG) -c TORevTrx.C -o $@

$(OFILES)TimeOut.o: $(EINCS)  
	$(CCC) $(CCCFLG) -c TimeOut.C -o $@

$(OFILES)TmtTrxOff.o: $(EINCS)  TmtTrxOff.h TrxOff.h $(DFILES)DBTrxOff.h
	$(CCC) $(CCCFLG) -c TmtTrxOff.C -o $@

$(OFILES)TrxOff.o: $(EINCS)  TrxOff.h TrxOff.h $(DFILES)DBTrxOff.h
	$(CCC) $(CCCFLG) -c TrxOff.C -o $@

$(OFILES)RtaTrxAnul.o: $(EINCS) TrxAnul.h
	$(CCC) $(CCCFLG) -c RtaTrxAnul.C -o $@

$(LFILES2)libevents.a: $(OFILES)RtaRevAnul.o \
                      $(OFILES)TORevAnul.o $(OFILES)TrxRevAnul.o \
                      $(OFILES)TOAnulTrx.o $(OFILES)RtaTrxAnul.o \
                      $(OFILES)BatchRta.o $(OFILES)Cierre.o \
                      $(OFILES)CierreRta.o $(OFILES)EchoTest.o  \
                      $(OFILES)EchoTestRta.o $(OFILES)Events.o \
                      $(OFILES)PriceAnul.o $(OFILES)PriceOffLine.o  \
                      $(OFILES)PriceOn.o $(OFILES)PriceOnRta.o \
                      $(OFILES)TimeOut.o $(OFILES)TOCierre.o  \
                      $(OFILES)TOCierreB.o $(OFILES)TOBatch.o \
                      $(OFILES)TOEchoTest.o $(OFILES)TORevTrx.o \
                      $(OFILES)ShutDown.o $(OFILES)TrxOff.o \
                      $(OFILES)RevTrx.o $(OFILES)RevTrxRta.o  \
                      $(OFILES)RtaTrxOff.o $(OFILES)TmtTrxOff.o \
                      $(OFILES)Batch.o $(OFILES)PriceRev.o \
		      $(OFILES)TrxAnul.o 
	ar r $(LFILES)libevents.a $?


