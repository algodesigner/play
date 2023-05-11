clean:
	rm -rf play

build:
	gcc -std=c99 -o example play.c example.c

all: clean build
