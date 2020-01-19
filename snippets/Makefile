CFLAGS=-std=c11 -g -static
SRCS=$(wildcard */*.c)
ASBS=$(SRCS:.c=.s)

snaps:
	$(CC) $(SRCS) -S -O0 -o $(ASBS) $(LDFLAGS)

clean:
	rm -f */*.s

.PHONY: test snaps