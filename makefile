
all: integrate results

integrate: main.o
	g++ -O4 -g -Wall -Wextra -pedantic -std=c++11  main.o -o integrate -pthread

main.o:	main.cpp
	g++ -O4 -g -Wall -Wextra -pedantic  -std=c++11 -c main.cpp

clean:
	rm -f *.o integrate results

#./integrate a b n n_threads
test: integrate
	time ./integrate -1 1 100000000 32

#0.00062686611449010273996

# need to change
gdb: integrate
	gdb ./integrate 

val: integrate
	valgrind --leak-check=full ./integrate 100 10000 100 32

valv: integrate
	valgrind --leak-check=full ./integrate 100 10000 100 32



results: results.cpp
	g++ -o results results.cpp

re: results
	./results
	
