server: server.o ThreadPool.o
	g++ -pthread -o server server.o ThreadPool.o
	rm *.o

server.o: ../server.cpp ../ThreadPool.hpp
	g++ -c ../server.cpp

ThreadPool.o: ../ThreadPool.cpp ../ThreadPool.hpp
	g++ -c ../ThreadPool.cpp

clean:
	rm *.o server
