APP1 = example1
APP2 = example2
CFLAGS = -Wall -O2
INCFLAGS = -I../src

# Link with libtrl
CFLAGS += $(shell pkg-config --cflags libtrl)
LDLIBS += $(shell pkg-config --libs libtrl)

SOURCES1 = example1.c
SOURCES2 = example2.c ../src/tools.c

all:
	gcc $(CFLAGS) $(SOURCES1) -o $(APP1) $(LDLIBS)
	gcc $(CFLAGS) $(INCFLAGS) $(SOURCES2) -o $(APP2) $(LDLIBS)

clean:
	-rm -f $(APP1) $(APP2) cscope.* tags

index:
	@printf "  CSCOPE\n"
	@find . -name "*.[ch]" > cscope.files
	@find ../src -name "*.[ch]" >> cscope.files
	@find ../include -name "*.[ch]" >> cscope.files
	@echo /usr/include/trl.h >> cscope.files
	@cscope -b -q
	@printf "  CTAGS\n"
	@ctags -L cscope.files
	@-rm -f cscope.files

.PHONY: all clean index
