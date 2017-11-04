# José Antonio Álvarez Ocete
# Yábir García Benchakhtir
# ****** Directorios ********
INCLUDE   = include
OBJ       = obj
SRC       = src
BIN       = bin

# ****** Opciones de compilación ********
AR        = ar
CXX       = g++
CXXFLAGS  = -std=c++11 -I./ Semaphore.cpp
THREAD    = -lpthread

# ****** Compilación de módulos **********
.PHONY: all clean mrproper crearpdf limpiarpdf
all: $(BIN)/prodcons-fifo $(BIN)/prodcons-lifo $(BIN)/fumadores $(BIN)/fumadores_con_tope

# --- Ejecutables ---
$(BIN)/prodcons-fifo :
	$(CXX) prodcons-FIFO.cpp -o prodcons-FIFO $(THREAD) $(CXXFLAGS)

$(BIN)/prodcons-lifo :
	$(CXX) prodcons-LIFO.cpp -o prodcons-LIFO $(THREAD) $(CXXFLAGS)

$(BIN)/fumadores :
	$(CXX) fumadores.cpp -o fumadores $(THREAD) $(CXXFLAGS)

$(BIN)/fumadores_con_tope :
	$(CXX) fumadores_con_tope.cpp -o fumadores_con_tope $(THREAD) $(CXXFLAGS)
# ************ Limpieza ************
clean:
	-@rm -f $(OBJ)/*

mrproper: clean
	-@rm -f $(BIN)/*
