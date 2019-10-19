CPPFLAGS=-Wall -Wextra -O2

rough_sim: rough_sim.o randn.o landscape.o
	g++ $(CPPFLAGS) -o rough_sim rough_sim.o landscape.o randn.o 
	
rough_sim.o: rough_sim.cpp rough_sim.h randn.h split.h split.cpp landscape.h
	g++ $(CPPFLAGS) -c rough_sim.cpp
	
landscape.o: landscape.cpp landscape.h
	g++ $(CPPFLAGS) -c landscape.cpp
	
randn.o: randn.cpp randn.h
	g++ $(CPPFLAGS) -c randn.cpp
	
clean:
	rm -f *.o
