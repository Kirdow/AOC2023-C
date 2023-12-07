all: run

run: build
	./run input.txt

test: build
	./run testinput.txt

test2: build
	./run testinput2.txt

test3: build
	./run testinput3.txt

build:
	cc -o run main.c -I../shared/ --std=c17
