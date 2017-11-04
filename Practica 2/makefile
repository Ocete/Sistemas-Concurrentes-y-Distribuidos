.SUFFIXES:
.PHONY: x1, x2, clean

compilador:=g++
opcionesc:= -std=c++11 -pthread -I.
i:= HoareMonitor.cpp

all : fumadores

fumadores: fumadores.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

clean:
	rm -f monitor_em barrera1_sc barrera2_sc barrera2_su prodcons1_sc prodcons2_sc prodcons2_su
