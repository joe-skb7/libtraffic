APP = example
CFLAGS = -Wall -O2
CFLAGS += $(shell pkg-config --cflags libtrl)
LDLIBS += $(shell pkg-config --libs libtrl)

all:
	gcc $(CFLAGS) main.c $(LDLIBS) -o $(APP)

clean:
	-rm -f $(APP) cscope.* tags

index:
	@printf "  CSCOPE\n"
	@echo main.c > cscope.files
	@echo /usr/include/trl.h >> cscope.files
	@cscope -b -q
	@printf "  CTAGS\n"
	@ctags -L cscope.files
	@-rm -f cscope.files

.PHONY: all clean index
