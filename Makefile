# Version
MAJOR=0
MINOR=1
PATCH=0
VERSION=$(MAJOR).$(MINOR).$(PATCH)

# File names
LIBNAME		= libtrl
LIB_DYN		= $(LIBNAME).so.$(VERSION)
LIB_STATIC	= $(LIBNAME).a
LIB_PC		= $(LIBNAME).pc
# Links
LIB_DYN_SONAME	= $(LIBNAME).so.$(MAJOR)
LIB_DYN_SO	= $(LIBNAME).so

# Paths
PREFIX		?= /usr/local
INCLUDEDIR	= $(PREFIX)/include
DEB_HOST_MULTIARCH = $(shell dpkg-architecture -qDEB_HOST_MULTIARCH 2>/dev/null)
ifneq "$(DEB_HOST_MULTIARCH)" ""
  LIBDIR	= $(PREFIX)/lib/$(DEB_HOST_MULTIARCH)
else
  # For most other systems
  ifeq "$(shell uname -m)" "x86_64"
    LIBDIR	= $(PREFIX)/lib64
  else
    LIBDIR	= $(PREFIX)/lib
  endif
endif
PKGCONFIGDIR	= $(LIBDIR)/pkgconfig

# Build tools
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)gcc

# Generic build flags
CPPFLAGS	+= -Iinclude -Isrc -MD
CFLAGS		+= -Wall -Wundef -Wpointer-arith -Wcast-qual
CFLAGS		+= -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS		+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS		+= -std=gnu89 -fstack-protector-strong
CFLAGS		+= -O2

# Discard unused symbols (to reduce library size)
CFLAGS		+= -ffunction-sections -fdata-sections
LDFLAGS		+= -Wl,--gc-sections
ifeq ($(V),99)
LDFLAGS		+= -Wl,--print-gc-sections
endif

# Tune up linking options with shared libraries
LDFLAGS		+= -Wl,--as-needed -Wl,--no-undefined

# Link with libftdi1
CFLAGS		+= $(shell pkg-config --cflags libftdi1)
LDLIBS		+= $(shell pkg-config --libs libftdi1)

# Dynamic library
CFLAGS		+= -fPIC -fvisibility=hidden
LDFLAGS		+= -shared -Wl,-soname,$(LIB_DYN_SONAME)

# Be silent per default, but 'make V=1' will show all compiler calls
ifneq ($(V),1)
Q := @
endif

# Commands
MKDIR_P		:= install -d
INSTALL		:= install -m 0755
INSTALL_DATA	:= install -m 0644

SOURCES =			\
	src/trl.c		\
	src/tools.c

OBJS = $(SOURCES:%.c=%.o)

all: $(LIB_DYN) $(LIB_STATIC) $(LIB_PC)

$(LIB_DYN): $(OBJS)
	@printf "  LD      $@\n"
	$(Q)$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(LIB_STATIC): $(OBJS)
	@printf "  AR      $@\n"
	$(Q)$(AR) rcs $@ $(OBJS)

%.o: %.c
	@printf "  CC      $(*).c\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

clean:
	@printf "  CLEAN\n"
	$(Q)$(RM) -f $(LIB_DYN) $(LIB_STATIC) $(LIB_PC)
	$(Q)find src/ -name '*.o' -exec $(RM) {} \;
	$(Q)find src/ -name '*.d' -exec $(RM) {} \;

distclean: clean
	@printf "  DISTCLEAN\n"
	$(Q)$(RM) -f cscope.* tags

install: $(LIB_DYN) $(LIB_STATIC) $(LIB_PC)
	@printf "  INSTALL\n"
	$(Q)$(MKDIR_P) $(DESTDIR)$(LIBDIR)
	$(Q)$(MKDIR_P) $(DESTDIR)$(PKGCONFIGDIR)
	$(Q)$(MKDIR_P) $(DESTDIR)$(INCLUDEDIR)
	$(Q)$(INSTALL_DATA) $(LIB_DYN) $(DESTDIR)$(LIBDIR)
	$(Q)$(INSTALL_DATA) $(LIB_STATIC) $(DESTDIR)$(LIBDIR)
	$(Q)$(INSTALL_DATA) $(LIB_PC) $(DESTDIR)$(PKGCONFIGDIR)
	$(Q)$(INSTALL_DATA) include/*.h $(DESTDIR)$(INCLUDEDIR)
	$(Q)ln -rs $(DESTDIR)$(LIBDIR)/$(LIB_DYN) \
		$(DESTDIR)$(LIBDIR)/$(LIB_DYN_SONAME)
	$(Q)ln -rs $(DESTDIR)$(LIBDIR)/$(LIB_DYN_SONAME) \
		$(DESTDIR)$(LIBDIR)/$(LIB_DYN_SO)

uninstall:
	@printf "  UNINSTALL\n"
	$(Q)$(RM) -f $(DESTDIR)$(LIBDIR)/$(LIB_DYN_SO)
	$(Q)$(RM) -f $(DESTDIR)$(LIBDIR)/$(LIB_DYN_SONAME)
	$(Q)$(RM) -f $(DESTDIR)$(LIBDIR)/$(LIB_DYN)
	$(Q)$(RM) -f $(DESTDIR)$(LIBDIR)/$(LIB_STATIC)
	$(Q)$(RM) -f $(DESTDIR)$(PKGCONFIGDIR)/$(LIB_PC)
	$(Q)$(RM) -f $(patsubst %.h, $(DESTDIR)$(PREFIX)/%.h, \
		$(wildcard include/*))

$(LIB_PC):
	@printf "  GEN     $(LIB_PC)\n"
	@echo "prefix=$(PREFIX)"			> $(LIB_PC)
	@echo "exec_prefix=\$${prefix}"			>> $(LIB_PC)
	@echo "libdir=$(LIBDIR)"			>> $(LIB_PC)
	@echo "includedir=$(INCLUDEDIR)"		>> $(LIB_PC)
	@echo ""					>> $(LIB_PC)
	@echo "Name: libtrl"				>> $(LIB_PC)
	@echo "Description: Library for controlling traffic lights" >> $(LIB_PC)
	@echo "Requires: libftdi1"			>> $(LIB_PC)
	@echo "Version: $(VERSION)"			>> $(LIB_PC)
	@echo "Libs: -L\$${libdir} -ltrl"		>> $(LIB_PC)
	@echo "Cflags: -I\$${includedir}"		>> $(LIB_PC)

debian:
	@printf "  DEBIAN\n"
	$(Q)dpkg-buildpackage -us -uc -tc

index:
	@printf "  CSCOPE\n"
	$(Q)find . -name '*.[ch]' > cscope.files
	$(Q)find /usr/include/libftdi1/ -name '*.h' >> cscope.files
	$(Q)cscope -b -q
	@printf "  CTAGS\n"
	$(Q)ctags -L cscope.files
	$(Q)$(RM) -f cscope.files

.PHONY: all clean distclean install uninstall debian index

-include $(OBJS:.o=.d)
