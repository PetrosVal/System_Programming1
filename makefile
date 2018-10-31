#Makefile
OBJS1 = jms_coord.o 
OBJS2 = jms_console.o
OBJS3 = pools.o queue.o
SOURCE1 = jms_coord.c 
SOURCE2 = jms_console.c
SOURCE3 = pools.c queue.c
HEADER = queue.h
OUT1 = jms_coord
OUT2 = jms_console
OUT3 = pools
CC = gcc
FLAGS = -g -c

# -g option enables debugging mode
# -c flag generates object code for separate files
all: $(OBJS1) $(OBJS2) $(OBJS3)
	gcc -o $(OUT1) $(SOURCE1)
	gcc -o $(OUT2) $(SOURCE2)
	gcc -o $(OUT3) $(SOURCE3)

# create / compile the individual files separately
jms_coord.o: jms_coord.c
	$(CC) $(FLAGS) jms_coord.c

jms_console.o: jms_console.c
	$(CC) $(FLAGS) jms_console.c

pools.o: pools.c
	$(CC) $(FLAGS) pools.c	

queue.o: queue.c
	$(CC) $(FLAGS) queue.c

# clean
clean:
	rm -f $(OBJS1) $(OBJS2) $(OBJS3) $(OUT1) $(OUT2) $(OUT3)

clean2:
	rm /tmp/sdi1300017/*
