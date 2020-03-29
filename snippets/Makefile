CFLAGS=-std=c11 -S -fverbose-asm -masm=intel -O0

all: ops/plus.s controls/if_0.s function_call/callee.s function_call/caller.s main/main.s main/mainsub.s main/simple.s

ops/plus.s: ops/plus.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

controls/if_0.s: controls/if_0.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

function_call/callee.s: function_call/callee.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

function_call/caller.s: function_call/caller.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

main/main.s: main/main.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

main/mainsub.s: main/mainsub.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

main/simple.s: main/simple.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

clean:
	rm -f */*.s

.PHONY: snippets