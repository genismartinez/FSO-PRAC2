
fronton0 : fronton0.c winsuport.o winsuport.h
	gcc -Wall fronton0.c winsuport.o -o fronton0 -lcurses

fronton1 : fronton1.c winsuport.o winsuport.h
	gcc -Wall fronton1.c winsuport.o -o fronton1 -lcurses -lpthread

fronton2 : fronton2.c winsuport.o winsuport.h
	gcc -Wall fronton2.c winsuport.o -o fronton2 -lcurses -lpthread

fronton3 : fronton3.c winsuport.o winsuport.h
	gcc -Wall fronton3.c winsuport.o -o fronton3 -lcurses -lpthread

winsuport.o : winsuport.c winsuport.h
	gcc -c -Wall winsuport.c -o winsuport.o 

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o
clean :
	rm -f winsuport.o fronton0 fronton1 fronton2
