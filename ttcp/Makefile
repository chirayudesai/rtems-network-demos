#
# $Revision$    $Date$    $Author$
#

SAMPLE=ttcp
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=io event message rate_monotonic semaphore timer

# C source names, if any, go here -- minus the .c
C_PIECES= init rtems_ttcp
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
CFLAGS   += -I./include
CFLAGS_LD   += -Wl,--defsym -Wl,HeapSize=0x200000 # KA9Q needs more space
CFLAGS_OPTIMIZE_V   +=

LD_PATHS  +=
LD_LIBS   +=

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
	$(make-exe)

install: $(INSTALL_TARGET)

$(INSTALL_TARGET): all
	cp ${PGM} $(INSTALL_TARGET)
