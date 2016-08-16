all: build install

build:
	gcc -o indicator indicator.c -O9

install:
	sudo cp indicator /usr/local/bin/
