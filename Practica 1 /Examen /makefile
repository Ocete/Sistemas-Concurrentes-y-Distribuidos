# ****** Directorios ********
INCLUDE   = include
OBJ       = obj
SRC       = src
BIN       = bin

# ****** Opciones de compilación ********
AR        = ar
CXX       = g++
CXXFLAGS  = -std=c++11 -I./ Semaphore.cpp -O2
THREAD    = -lpthread

# ****** Compilación de módulos **********
.PHONY: all clean mrproper crearpdf limpiarpdf
all: $(BIN)/AlvarezJoseAntonio

# --- Ejecutables ---
$(BIN)/AlvarezJoseAntonio :
	$(CXX) AlvarezJoseAntonio.cpp -o AlvarezJoseAntonio $(THREAD) $(CXXFLAGS)

# ************ Limpieza ************
clean:
	-@rm -f $(OBJ)/*

mrproper: clean
	-@rm -f $(BIN)/*
