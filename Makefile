CC=gcc 
FLAGS = -fPIC -g -w
SHARED = --shared


all: libmyfs.so libmylibc.so

libmyfs.so: myfs.o
	$(CC) $(SHARED) $(FLAGS)  myfs.o -o libmyfs.so

libmylibc.so: myfs.o mystdio.o
	$(CC) $(SHARED) $(FLAGS)  myfs.o mystdio.o -o libmylibc.so

myfs.o: myfs.c
	$(CC) $(FLAGS) myfs.c -c 

mystdio.o: mystdio.c
	$(CC) $(FLAGS) mystdio.c -c 



clean: 
	rm -f *.so *.o