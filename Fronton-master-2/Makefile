


winsuport2.o : winsuport2.c winsuport2.h
	gcc -c -Wall winsuport2.c -o winsuport2.o 

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o
	
semafor.o : semafor.c semafor.h
	gcc -c -Wall semafor.c -o semafor.o
	
missatge.o : missatge.c missatge.h
	gcc -c -Wall missatge.c -o missatge.o
	
fronton4 : fronton4.c winsuport2.o semafor.o memoria.o missatge.o winsuport2.h pilota4
	gcc -Wall fronton4.c winsuport2.o memoria.o semafor.o missatge.o -g -o fronton4 -lcurses -lpthread    
pilota4 : pilota4.c winsuport2.o winsuport2.h 
	gcc -Wall pilota4.c memoria.o winsuport2.o semafor.o missatge.o -g -o pilota4 -lcurses

	

run0:
	./fronton4 prova0.txt 5 50

run:
	./fronton4 prova1.txt 2 50
	
run2:
	./fronton4 prova2.txt 4 50

run3:
	./fronton4 prova3.txt 10 50

run4:
	./fronton4 prova4.txt 10 50

clean:
	rm *.o fronton4 pilota4
	