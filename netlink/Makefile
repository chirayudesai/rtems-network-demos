#
#  $Id$
#

SAMPLE=netlink
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=all

# C source names, if any, go here -- minus the .c
C_PIECES=init
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

H_FILES=

DOCTYPES=
DOCS=$(DOCTYPES:%=$(SAMPLE).%)

SRCS=$(DOCS) $(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES) $(ARCH)/tarfile.o

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
CFLAGS_LD += 
# CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0xC0000 
CFLAGS_OPTIMIZE_V   +=
CFLAGS_DEBUG_V   += -v -qrtems_debug

LD_PATHS  +=
LD_LIBS   +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += tarfile 
CLOBBER_ADDITIONS +=

all:	${ARCH} $(SRCS) $(PGM)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

$(ARCH)/tarfile.o: 
	tar cf tarfile index.html
	$(LD) -r -o $(ARCH)/tarfile.o -b binary tarfile

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
