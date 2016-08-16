CC 	= musl-gcc
CFLAGS 	= -O9 -std=c11
OBJS	= indicator.o

all: build install

build: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o indicator

install:
	sudo cp indicator /usr/local/bin/
