.SUFFIXES:
.PHONY:    pc,pc2,pcm,fp,f,fi,fc,clean

compilador := mpicxx -std=c++11
flagsc     := -g -Wall

prodcons: prodcons_exe
	mpirun -np  10 ./$<

filosofos-interb: filosofos-interb_exe
	mpirun -np 10 ./$<

filosofos: filosofos_exe
	mpirun -np 10 ./$<

filosofos-cam: filosofos-cam_exe
	mpirun -np 11 ./$<

filosofos-cam-Modificado-NoSincrono: filosofos-cam-Modificado-NoSincrono_exe
	mpirun -np 11 ./$<

filosofos-cam-Modificado-ParImpar: filosofos-cam-Modificado-ParImpar_exe
	mpirun -np 11 ./$<

%_exe: %.cpp
	$(compilador) $(flagsc) -o $@ $<

clean:
	rm -rf *_exe *.dSYM
