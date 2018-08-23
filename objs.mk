# c4-os-specific library build rules

minift-src = $(wildcard $(LIBRARY_ROOT)/miniforth/src/*.c)
minift-obj = $(minift-src:.c=.o)

$(BUILD)/lib/miniforth.a: $(minift-obj)
	ar rvs $@ $^

.PHONY: minift-clean
minift-clean:
	rm -f $(minift-obj)

ALL_CLEAN += minift-clean
