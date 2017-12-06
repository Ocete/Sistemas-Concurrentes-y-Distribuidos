.SUFFIXES:
.PHONY:    pc,pc2,pcm,fp,f,fi,fc,clean

compilador := mpicxx -std=c++11
flagsc     := -g -Wall

prodcons: prodcons_exe
	mpirun -np  10 ./$<

filosofos: filosofos_exe
	mpirun -np 10 ./$<

%_exe: %.cpp
	$(compilador) $(flagsc) -o $@ $<

clean:
	rm -rf *_exe *.dSYM
