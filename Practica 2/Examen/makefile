.SUFFIXES:
.PHONY: x1, x2, clean

compilador:=g++
opcionesc:= -std=c++11 -pthread -I.
i:= HoareMonitor.cpp

all : barberia2

barberia2: barberia2.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp

clean:
	rm -f fumadores barbero barberoModificado
