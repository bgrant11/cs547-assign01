
all: integrate

integrate: main.o
	g++ -O0 -g -Wall -Wextra -pedantic -std=c++11  main.o -o integrate

main.o:	main.cpp
	g++ -O0 -g -Wall -Wextra -pedantic  -std=c++11 -c main.cpp

clean:
	rm -f *.o integrate

#./integrate a b n n_threads
test: integrate
	./integrate 1.2 3.6 15 8



# need to change
gdb: integrate
	gdb ./integrate 

val: integrate
	valgrind --leak-check=full ./integrate

valv: integrate
	valgrind --leak-check=full ./integrate




