#
#  $Id$
#

SAMPLE=http
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=all

# C source names, if any, go here -- minus the .c
C_PIECES=init FilesystemImage
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
CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0xC0000 
CFLAGS_OPTIMIZE_V   +=
CFLAGS_DEBUG_V   += -v -qrtems_debug

LD_PATHS  +=

#NATIVE_OBJ = $(shell $(OBJCOPY) 2>&1  | grep copy: | cut -d':' -f3 | cut -d' ' -f2)
OBJDUMP=$(OBJCOPY:copy=dump)

USE_GOHEAD=no
USE_SIMPLE=yes
USE_FTPD=yes
ifeq ($(USE_GOHEAD),yes)
  HTTPD     = GoAhead Web Server
  CFLAGS   += -DWEBS -DUEMF -DUSE_GOAHEAD_HTTPD
  LD_LIBS  += -lhttpd
endif

ifeq ($(USE_SIMPLE),yes)
  HTTPD     = Simple HTTPD Web Server
  CFLAGS   += -DUSE_SIMPLE_HTTPD
  LD_LIBS  += -lshttpd
endif

ifeq ($(USE_FTPD),yes)
  CFLAGS   += -DUSE_FTPD
  LD_LIBS  += -lftpd
endif

LD_LIBS +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += FilesystemImage FilesystemImage.c FilesystemImage.h
CLEAN_ADDIRIONS += stamp-index-html rootfs/index.html
CLOBBER_ADDITIONS +=

# strip out flags gcc knows but LD doesn't like -- add as needed
LD_CPU_CFLAGS=$(CPU_CFLAGS:-mstrict-align:)

COPY_ARCH=$(shell echo $(OBJCOPY) | cut -d'-' -f1)

all:	${ARCH} $(SRCS) $(PGM)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)


$(ARCH)/init.c: init.c FilesystemImage.c

FilesystemImage: $(ARCH) stamp-index-html
	cd rootfs ; \
	    tar cf ../FilesystemImage --exclude CVS --exclude .cvsignore .

FilesystemImage.c: $(ARCH) FilesystemImage
	$(PROJECT_ROOT)/bin/bin2c FilesystemImage FilesystemImage

stamp-index-html: index.html.in
ifneq ($(HTTPD),)
	sed -e 's/@SERVER@/$(HTTPD)/' <index.html.in >rootfs/index.html
endif
	touch stamp-index-html

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests

