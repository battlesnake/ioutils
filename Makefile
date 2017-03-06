bindir = bin
progs = iochtype ioconvert iofiltertype ioreaddr iotap

out = $(addprefix $(bindir)/,$(progs))

deps = $(patsubst %.c, %.o, $(shell find c_modules/ -name \*.c))
lib = c_modules.a

.PHONY: all
all: $(out)

.PHONY: clean
clean:
	rm -rf -- $(bindir) $(deps) $(lib)

.PHONY: lib
lib: $(lib)

$(lib): $(deps)
	ar rcs $@ $^

$(deps): %.o: %.c
	gcc -std=gnu11 -DSIMPLE_LOGGING -Ic_modules -O2 -c -o $@ $<

$(out): $(bindir)/%: %.c $(lib)
	@mkdir -p $(bindir)
	gcc -std=gnu11 -DSIMPLE_LOGGING -Ic_modules -O2 -o $@ $^ -lpthread
