EXE := lothscastle

.PHONY:	all nes run linux run-pc web run-web clean

all: nes linux web

$(EXE).nes: main.c crt0.s tileset.chr
	cl65 -Os -t nes main.c crt0.s -o $@

nes: $(EXE).nes

run: nes
	mesen $(EXE).nes

$(EXE): main.c pc-port.c
	gcc main.c pc-port.c -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o $@

linux: $(EXE)

run-pc: linux
	./$(EXE)

web/index.html: main.c pc-port.c tileset.png webshell.html
	mkdir -p web
	cp icon.png web/icon.png
	emcc -o $@ main.c pc-port.c -Wall $(HOME)/raylib/src/libraylib.a -I. -I$(HOME)/raylib/src/ -L. -L$(HOME)/raylib/src/libraylib.a -s USE_GLFW=3 --shell-file webshell.html -DPLATFORM_WEOM_WEB -s ASYNCIFY --preload-file tileset.png

web: web/index.html

run-web: web
	emrun web/index.html

clean:
	rm -rf $(EXE)* *.o main web/