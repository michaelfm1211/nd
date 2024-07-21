PREFIX=/usr/local

CFLAGS = -Wall -Wextra -Werror -pedantic
LDFLAGS = -L. -lnd

BINS = ndcat ndwrite

.PHONY: all
all: CFLAGS += -O3
all: libnd.a $(BINS)

.PHONY: dev
dev: CFLAGS += -g -O0 -fsanitize=address -fsanitize=undefined
dev: libnd.a $(BINS)

.PHONY: docs
docs: ndcat.1

libnd.a: libnd.o
	$(AR) rcs $@ $<

ndcat.1: ndcat.scd
	scdoc < $^ > $@

.PHONY: clean
clean:
	rm -rf $(BINS) libnd.a *.o *.dSYM

.PHONY: install
install: all
	sudo cp ndcat "$(PREFIX)/bin/ndcat"
	sudo chmod 755 "$(PREFIX)/bin/ndcat"
	sudo cp ndwrite "$(PREFIX)/bin/ndwrite"
	sudo chmod 755 "$(PREFIX)/bin/ndwrite"
	sudo cp libnd.a "$(PREFIX)/lib/libnd.a"
	sudo chmod 644 "$(PREFIX)/lib/libnd.a"
