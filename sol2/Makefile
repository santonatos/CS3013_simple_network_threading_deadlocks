all: sol2_sema

sol2_sema: sol2_sema.o
	gcc sol2_sema.o -o sol2_sema -lpthread  
	rm -f *.o

sol2_sema.o: sol2_sema.c
	gcc -c -g sol2_sema.c -o sol2_sema.o -lpthread

clean:
	rm sol2_sema

clean-txt:
	rm -f *.txt

