CFLAGS=-std=c11

all: ops/plus.s controls/if_0.s

ops/plus.s: ops/plus.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

controls/if_0.s: controls/if_0.c
	$(CC) $(@:.s=.c) -S -O0 -o $@ $(CFLAGS)

clean:
	rm -f */*.s

.PHONY: snippets