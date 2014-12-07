all: resolver

resolver: resolver.o
	g++ resolver.o -o resolver.x
	
resolver.o: resolver.cpp
	g++ -c resolver.cpp
	
clean:
	rm -rf *.o resolver

