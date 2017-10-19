# ****** Directorios ********
INCLUDE   = include
OBJ       = obj
SRC       = src
BIN       = bin

# ****** Opciones de compilación ********
AR        = ar
CXX       = g++
CXXFLAGS  = -std=c++11 -I./
THREAD    = -lpthread

# ****** Compilación de módulos **********
.PHONY: all clean mrproper crearpdf limpiarpdf
all: $(BIN)/prodcons-fifo $(BIN)/prodcons-lifo $(BIN)/fumadores

# --- Ejecutables ---
$(BIN)/prodcons-fifo :
	$(CXX) prodcons-FIFO.cpp -o prodcons-FIFO $(THREAD) $(CXXFLAGS) Semaphore.cpp

$(BIN)/prodcons-lifo :
	$(CXX) prodcons-LIFO.cpp -o prodcons-LIFO $(THREAD) $(CXXFLAGS) Semaphore.cpp

$(BIN)/fumadores :
	$(CXX) fumadores.cpp -o fumadores $(THREAD) $(CXXFLAGS) Semaphore.cpp
# ************ Limpieza ************
clean:
	-@rm -f $(OBJ)/*

mrproper: clean
	-@rm -f $(BIN)/*
