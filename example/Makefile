APP = example
CFLAGS = -Wall -O2

# Link with libtrl
CFLAGS += $(shell pkg-config --cflags libtrl)
LDLIBS += $(shell pkg-config --libs libtrl)

SOURCES =			\
	  main.c		\
	  ../src/tools.c
CFLAGS += -I../src

all:
	gcc $(CFLAGS) $(SOURCES) $(LDLIBS) -o $(APP)

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
