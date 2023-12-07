all: run

run: build
	./run input.txt

test: build
	./run testinput.txt

build:
	cc -o run main.c --std=c17
