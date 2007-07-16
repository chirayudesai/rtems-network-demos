#
#  $Id$
#

SAMPLE=nfsClientTest
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=all

# C source names, if any, go here -- minus the .c
C_PIECES=init dirutils
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
CFLAGS_LD += # -Wl,--defsym -Wl,HeapSize=0xC0000 
CFLAGS_OPTIMIZE_V   +=
CFLAGS_DEBUG_V   += -v -qrtems_debug

LD_PATHS  +=

OBJDUMP=$(OBJCOPY:copy=dump)

CFLAGS   += 
LD_LIBS  += -lrtemsNfs

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += tarfile obj_format
CLOBBER_ADDITIONS +=

# strip out flags gcc knows but LD doesn't like -- add as needed
LD_CPU_CFLAGS=$(CPU_CFLAGS:-mstrict-align:)

all:	${ARCH} $(SRCS) $(PGM)

obj_format:
	$(OBJDUMP) -f $(ARCH)/init.o | grep .o: | \
	    sed -e 's/^.*format //' >obj_format

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

$(ARCH)/tarfile.o: $(ARCH) obj_format
	cd rootfs ; tar cf ../$(ARCH)/tarfile --exclude CVS .
	cp $(ARCH)/tarfile .
	$(OBJCOPY) -I binary -O `cat obj_format` tarfile $(ARCH)/tarfile.o
	$(LD) $(LD_CPU_CFLAGS) -r -o $(ARCH)/temp.o $(ARCH)/tarfile.o

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests
