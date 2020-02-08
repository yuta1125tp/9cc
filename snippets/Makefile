CFLAGS=-std=c11 -g -static
SRCS=$(wildcard */*.c)
ASBS=$(SRCS:.c=.s)

$(ASBS): $(SRCS)
	$(CC) $(SRCS) -S -O0 -o $(ASBS) $(LDFLAGS)

clean:
	rm -f */*.s

.PHONY: snippets