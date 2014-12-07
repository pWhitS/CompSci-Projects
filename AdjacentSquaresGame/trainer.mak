trainer: trainer.o annet.o
	g++ trainer.o annet.o -o trainer.x
	
annet.o: annet.cpp
	g++ -c annet.cpp

trainer.o: trainer.cpp
	g++ -c trainer.cpp

clean:
	rm -rf *.o

