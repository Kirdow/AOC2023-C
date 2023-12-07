all: run

run: build
	./run input.txt

test: build
	./run testinput.txt

build:
	cc -o run main.c -I../shared/ --std=c17
