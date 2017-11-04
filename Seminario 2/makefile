.SUFFIXES:
.PHONY: x1, x2, clean

compilador:=g++
opcionesc:= -std=c++11 -pthread -I.
i:= HoareMonitor.cpp

x1: monitor_em
	./$<

x2: barrera1_sc
	./$<

x3: barrera2_sc
	./$<

x4: barrera2_su
	./$<

x5: prodcons1_sc
	./$<

x6: prodcons2_sc
	./$<

x7: prodcons2_su
	./$<

monitor_em: monitor_em.cpp
	$(compilador) $(opcionesc)  -o $@ $<

barrera1_sc: barrera1_sc.cpp
	$(compilador) $(opcionesc)  -o $@ $<

barrera2_sc: barrera2_sc.cpp
	$(compilador) $(opcionesc)  -o $@ $<

barrera2_su: barrera2_su.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

all : prodcons1_suLIFO prodcons1_suFIFO prodconsN_suLIFO prodconsN_suFIFO

prodcons1_suLIFO: prodcons1_suLIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

prodcons1_suFIFO: prodcons1_suFIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

prodconsN_suLIFO: prodconsN_suLIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

prodconsN_suFIFO: prodconsN_suFIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

prodcons2_sc: prodcons2_sc.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $<

prodcons2_su: prodcons2_su.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $<  HoareMonitor.cpp

clean:
	rm -f monitor_em barrera1_sc barrera2_sc barrera2_su prodcons1_sc prodcons2_sc prodcons2_su
