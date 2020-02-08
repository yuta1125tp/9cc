CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

SRCS2=$(wildcard snippets/*/*.c)
OBJS2=$(SRCS2:.c=.s)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./test.sh

snippets: $(OBJS2)
$(OBJS2): $(SRCS2)
	cd snippets && $(MAKE)

clean:
	rm -f 9cc *.o *~ tmp*
	cd snippets && $(MAKE) clean

.PHONY: test clean