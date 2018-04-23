LIBS= -lsfml-graphics -lsfml-window -lsfml-system 

Binary_name=game
Compiler=g++
CFLAGS=-Wall -std=c++11

all: game

main: main.o		
	$(Compiler) main.o -o $(Binary_name) $(LIBS)		

game:	player.o world.o main.o
	$(Compiler) world.o player.o main.o -o $(Binary_name) $(LIBS)

main.o: main.cpp
	$(Compiler) $(CFLAGS) -c main.cpp $(LIBS)

player.o: player.cpp
	$(Compiler) $(CFLAGS) -c player.cpp $(LIBS)

world.o: world.cpp
	$(Compiler) $(CFLAGS) -c world.cpp $(LIBS)



clean:
	@echo "** REMOVING THE GAME ** "   
	rm *.o $(Binary_name)

install:
	@echo "** INSTALLING ..."
	cp $(Binary_name) /usr/bin

uninstall:
	@echo "** UNINSTALLING..."
	rm $(Binary_name)
