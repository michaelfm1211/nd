CFLAGS = -Wall -Wextra -Werror -pedantic
LDFLAGS = -L. -lnd

BINS = ndcat ndwrite

all: CFLAGS += -O3
all: libnd.a $(BINS)

dev: CFLAGS += -g -O0 -fsanitize=address -fsanitize=undefined
dev: libnd.a $(BINS)

libnd.a: libnd.o
	$(AR) rcs $@ $<

clean:
	rm -rf $(BINS) libnd.a *.o *.dSYM
