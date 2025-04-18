
.PHONY:	all nes run linux run-pc web run-web clean

all: nes linux web

main.nes: main.c crt0.s tileset.chr
	cl65 -Os -t nes main.c crt0.s -o $@

nes: main.nes

run: nes
	mesen main.nes

main: main.c pc-port.c
	gcc main.c pc-port.c -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o $@

linux: main

run-pc: linux
	./main

web/main.html: main.c pc-port.c tileset.png webshell.html
	mkdir -p web
	cp icon.png web/icon.png
	emcc -o $@ main.c pc-port.c -Wall $(HOME)/raylib/src/libraylib.a -I. -I$(HOME)/raylib/src/ -L. -L$(HOME)/raylib/src/libraylib.a -s USE_GLFW=3 --shell-file webshell.html -DPLATFORM_WEOM_WEB -s ASYNCIFY --preload-file tileset.png

web: web/main.html

run-web: web
	emrun web/main.html

clean:
	rm -rf main.nes main.o crt0.o main web/