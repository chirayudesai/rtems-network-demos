#
#  $Id$
#

SAMPLE=http
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=io event semaphore timer rate_monotonic

# C source names, if any, go here -- minus the .c
C_PIECES=init
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

H_FILES=

DOCTYPES=
DOCS=$(DOCTYPES:%=$(SAMPLE).%)

SRCS=$(DOCS) $(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

PRINT_SRCS=$(DOCS)

PGM=${ARCH}/$(SAMPLE).exe

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

DEFINES  +=
CPPFLAGS +=
CFLAGS   +=
CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0xC0000 
CFLAGS_OPTIMIZE_V   +=
CFLAGS_DEBUG_V   += -v -qrtems_debug

LD_PATHS  +=
LD_LIBS   +=
NATIVE_OBJ = $(shell $(OBJCOPY) 2>&1  | grep copy: | cut -d':' -f3 | cut -d' ' -f2)

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS +=
CLOBBER_ADDITIONS +=

all:	${ARCH} $(SRCS) $(PGM)

${PGM}: $(OBJS) $(LINK_FILES)
	cd rootfs ; tar cf ../$(ARCH)/tarfile web etc
	cp $(ARCH)/tarfile .
	$(OBJCOPY) -I binary -O $(NATIVE_OBJ) tarfile $(ARCH)/tarfile.o
	$(LD) -r -o $(ARCH)/temp.o  $(ARCH)/init.o $(ARCH)/tarfile.o
	$(MV) $(ARCH)/temp.o  $(ARCH)/init.o
	$(RM) $(ARCH)/temp.o
	$(make-exe)

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests
