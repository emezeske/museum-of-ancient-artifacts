CLIENT_OBJS = SocketClient.o Driver.o 

all: $(CLIENT_OBJS) 
	g++ -g -o client $(CLIENT_OBJS) -lpthread

clean:
	rm -f *.o client
