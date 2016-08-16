CC 	= musl-gcc
CFLAGS 	= -O9 -std=c11
OBJS	= indicator.o
PROJECT = indicator
OUT_DIR = /usr/local/bin/

all: build install

build: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROJECT)

install:
	sudo cp $(PROJECT) $(OUT_DIR)
