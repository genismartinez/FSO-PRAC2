
memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o

winsuport2.o : winsuport2.c winsuport2.h
	gcc -Wall -c winsuport2.c -o winsuport2.o

missatge.o : missatge.c missatge.h
	gcc -c -Wall missatge.c -o missatge.o
pilota3: pilota3.c winsuport2.o memoria.o missatge.o
	gcc -Wall pilota3.c winsuport2.o memoria.o missatge.o -o pilota3 -lcurses -lpthread
mur3: mur3.c winsuport2.o memoria.o missatge.o pilota3.o
	gcc -Wall mur3.c winsuport2.o missatge.o memoria.o -o mur3 -lcurses -lpthread
mur4: mur4.c winsuport2.o memoria.o missatge.o semafor.o pilota3
	gcc -Wall mur4.c winsuport2.o memoria.o semafor.o missatge.o -g -o mur4 -lcurses -lpthread
clean : 
	rm -f winsuport2.o mur0 mur1 mur2 mur3 mur4 pilota3 memoria.o missatge.o

