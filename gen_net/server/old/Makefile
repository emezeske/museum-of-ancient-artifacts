SOCK_OBJS = Driver.o SocketServer.o

all: $(SOCK_OBJS)
	g++ -o serv $(SOCK_OBJS) -lpthread

Driver.o: Driver.cc
	g++ -c -g -o Driver.o Driver.cc

SocketServer.o: SocketServer.cc
	g++ -c -g -o SocketServer.o SocketServer.cc

clean:
	rm *.o serv
