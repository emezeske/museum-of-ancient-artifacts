server: main.cpp socketobject.o clientobject.o zoneobject.o playerobject.o
	g++ -g -o server main.cpp socketobject.o clientobject.o zoneobject.o playerobject.o -lpthread -Wall -Wno-deprecated
socketobject.o: socketobject.cpp socketobject.h
	g++ -c -g socketobject.cpp -Wno-deprecated
clientobject.o: clientobject.cpp clientobject.h
	g++ -c -g clientobject.cpp -Wno-deprecated
zoneobject.o: zoneobject.cpp zoneobject.h
	g++ -c -g zoneobject.cpp -Wno-deprecated
playerobject.o: playerobject.cpp playerobject.h
	g++ -c -g playerobject.cpp -Wno-deprecated
