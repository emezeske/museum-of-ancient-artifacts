gameserver_objects = client.o gameserver.o 

all: gameserver

client.o: messages.h 

gameserver.o: messages.h 

gameserver: $(gameserver_objects) 
	g++ -o gameserver $(gameserver_objects) -lpthread

run: $(gameserver_objects) 
	g++ -o gameserver $(gameserver_objects) -lpthread
	./gameserver

clean:
	rm -f *.o gameserver

