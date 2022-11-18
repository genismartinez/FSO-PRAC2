
fronton0 : fronton0.c winsuport.o winsuport.h
	gcc -Wall fronton0.c winsuport.o -o fronton0 -lcurses

fronton1 : fronton1.c winsuport.o winsuport.h
	gcc -Wall fronton1.c winsuport.o -o fronton1 -lcurses -lpthread

fronton2 : fronton2.c winsuport.o winsuport.h
	gcc -Wall fronton2.c winsuport.o -o fronton2 -lcurses -lpthread

fronton2.0 : fronton2.0.c winsuport.o winsuport.h
	gcc -Wall fronton2.0.c winsuport.o -o fronton2.0 -lcurses -lpthread

fronton3 : fronton3.c winsuport2.o winsuport2.h
	gcc -Wall fronton3.c memoria.o winsuport2.o -o fronton3 -lcurses -lpthread
	gcc -Wall pilota3.c memoria.o winsuport2.o -o pilota3 -lcurses

pilota3 : pilota3.c winsuport2.o winsuport2.h
	gcc -Wall pilota3.c memoria.o winsuport2.o -o pilota3 -lcurses

fronton4 : fronton4.c winsuport2.o winsuport2.h
	gcc -Wall fronton4.c memoria.o semafor.o missatge.o winsuport2.o -o fronton4 -lcurses -lpthread
	gcc -Wall pilota4.c memoria.o semafor.o missatge.o winsuport2.o -o pilota4 -lcurses -lpthread

pilota4 : pilota4.c winsuport2.o winsuport2.h
	gcc -Wall pilota4.c memoria.o semafor.o missatge.o winsuport2.o -o pilota4 -lcurses -lpthread

winsuport.o : winsuport.c winsuport.h
	gcc -c -Wall winsuport.c -o winsuport.o 

