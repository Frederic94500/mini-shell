CC=gcc
CFLAGS=-Wall
EXEC=mini-shell
SRC=src/
OUTPUT=bin/

default: $(EXEC)

mini-shell: $(SRC)mini-shell.c
	@if [ -d "bin/" ]; then\
		rm -r $(OUTPUT);\
	fi
	mkdir $(OUTPUT)
	gcc $(CFLAGS) -o $(OUTPUT)$@ $^

clean:
	rm -r bin/