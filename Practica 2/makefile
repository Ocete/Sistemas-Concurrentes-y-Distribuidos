.SUFFIXES:
.PHONY: x1, x2, clean

compilador:=g++
opcionesc:= -std=c++11 -pthread -I.
i:= HoareMonitor.cpp

all : fumadores barbero barberoModificado

fumadores: fumadores.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

barbero: barbero.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

barberoModificado: barberoModificado.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

clean:
	rm -f fumadores barbero barberoModificado
