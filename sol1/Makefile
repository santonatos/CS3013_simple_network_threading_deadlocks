all: sol1_mutex

sol1_mutex: sol1_mutex.o
	gcc sol1_mutex.o -o sol1_mutex -pthread
	rm *.o

sol1_mutex.o: sol1_mutex.c
	gcc -c -g sol1_mutex.c -o sol1_mutex.o -pthread

clean:
	rm -f sol1_mutex

clean-txt:
	rm *.txt
