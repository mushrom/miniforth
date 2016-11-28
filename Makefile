INCLUDE = -Iinclude/
CFLAGS  = -Os $(INCLUDE) -Wall -std=c11
LIBSRC  = $(wildcard src/*.c)
STUBSRC = $(wildcard stubs/$(STUBS)/*.c)
LIBOBJ  = $(LIBSRC:.c=.o)
STUBOBJ = $(STUBSRC:.c=.o)

.PHONY: all
all: out/miniforth

.PHONY: lib
lib: out/miniforth.a

.PHONY: clean
clean:
	rm -f $(LIBOBJ) $(STUBOBJ)
	rm -rf ./out

out:
	mkdir out

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

out/miniforth.a: out $(LIBOBJ)
	ar rvs $@ $(LIBOBJ)

out/miniforth: out out/miniforth.a $(STUBOBJ)
	$(CC) $(CFLAGS) -o $@ $(STUBOBJ) out/miniforth.a
