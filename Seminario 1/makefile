.SUFFIXES:
.PHONY: x01, x02, x03, x04, clean, tarp
.PRECIOUS: ejemplo%_exe

compilador:=g++

x%: ejemplo%_exe
	./$<

ejemplo%_exe: ejemplo%.cpp
	$(compilador) -std=c++11  -o $@ $< -lpthread

xs%: ejemplo%s_exe
	./$<

ejemplo%s_exe: ejemplo%-s.cpp
		$(compilador) -std=c++11 -I. -o $@ $< Semaphore.cpp -lpthread

tarp: scdsem1.tgz

scdsem1.tgz: $(wildcard *.cpp) $(wildcard *.h) makefile
	tar czvf scdsem1.tgz *.cpp *.h makefile

clean:
	rm -f ejemplo*_exe
