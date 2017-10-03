# quiet output, but allow us to look at what commands are being
# executed by passing 'V=1' to make, without requiring temporarily
# editing the Makefile.
ifneq ($V, 1)
MAKEFLAGS += -s
endif

all: run


main.exe: main.c Makefile
	@echo "  GCC   $@"
	gcc -O1  main.c -o main.exe

main.js: main.c Makefile
	@echo "  EMCC  $@"
	emcc -O1  main.c -s WASM=1 -s BINARYEN_METHOD=\'native-wasm\' -o main.js

run: main.exe main.js
	@echo "  RUN   main.exe"
	./main.exe || true
	@echo ""
	@echo "  RUN   main.wasm"
	node main.js || true

clean:
	rm -f main.w* main.js main.exe
	rm -f *~

.PHONY: all run clean
