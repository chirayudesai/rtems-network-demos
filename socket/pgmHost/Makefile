#
#  $Id$
#

# we use host compiler in this directory
USE_HOST_COMPILER=yes

# C source names, if any, go here -- minus the .c
C_PIECES=buffer serverTcp clientTcp serverUdp clientUdp 
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

H_FILES=

SRCS=$(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

PGMS=$(ARCH)/serverTcp$(EXEEXT) $(ARCH)/clientTcp$(EXEEXT) \
	$(ARCH)/serverUdp$(EXEEXT) $(ARCH)/clientUdp$(EXEEXT)

#
# (OPTIONAL) Add local stuff here using +=
#

DEFINES  +=
CPPFLAGS += -I..
CFLAGS   +=

LD_PATHS  +=
LD_LIBS   += $(ARCH)/buffer.o
LDFLAGS   +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += $(HOST_ARCH)
CLOBBER_ADDITIONS += buffer.c

all:	$(ARCH) buffer.c $(OBJS) $(SRCS) $(PGMS)
	$(INSTALL_VARIANT) -m 555 ${PGMS} ${PROJECT_RELEASE}/tests
	$(INSTALL_VARIANT) -m 555 ${PGMS} ${PROJECT_RELEASE}/samples

buffer.c:
	$(LN) ../buffer.c .
