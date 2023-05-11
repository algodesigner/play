clean:
	rm -rf example play.tar.gz

build:
	gcc -std=c99 -o example play.c example.c

deploy_lib:
	mkdir play
	cp play.h play
	cp play.c play/play.cpp
	cp README.md play
	tar cvfz play.tar.gz play
	rm -rf play

all: clean build
