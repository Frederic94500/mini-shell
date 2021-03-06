CC=gcc
CFLAGS=
EXEC=mini-shell
SRC=src/
OUTPUT=bin/

default: $(EXEC)

mini-shell: $(SRC)mini-shell.c
	@if [ -d "$(OUTPUT)" ]; then\
		rm -r $(OUTPUT);\
	fi
	mkdir $(OUTPUT)
	gcc $(CFLAGS) -o $(OUTPUT)$@ $^

clean:
	rm -r $(OUTPUT)
