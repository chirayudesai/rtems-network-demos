#
#  $Id$
#

RTEMS_MAKEFILE_PATH=/home/joel/rtems-4.8-work/bsp-install/powerpc-rtems4.8/icecube/
SAMPLE=osmonweb
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=all

# C source names, if any, go here -- minus the .c
C_PIECES=init osmonweb htmlprintf osmonweb_tar \
  osmonweb_RTEID osmonweb_RTEID_objs osmonweb_RTEID_one_sema \
  osmonweb_RTEID_onetask osmonweb_RTEID_queues \
  osmonweb_RTEID_semas osmonweb_RTEID_tasks \
  osmonweb_POSIX_objs osmonweb_ITRON_objs 


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
## CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0xC0000 
LD_PATHS  +=

#
# Define parameters to control building.
#

USE_GOAHEAD=no
USE_SIMPLE=yes
USE_FTPD=yes
USE_DEBUG=no
ifeq ($(USE_GOAHEAD),yes)
  HTTPD       = GoAhead Web Server
  HTTPD_LOGO  = webserver_logo2.gif
  CFLAGS     += -DWEBS -DUEMF -DUSE_GOAHEAD_HTTPD
  LD_LIBS    += -lhttpd
endif

ifeq ($(USE_SIMPLE),yes)
  HTTPD       = Simple HTTPD Web Server
  HTTPD_LOGO  = SimpleHTTPD.png
  CFLAGS     += -DUSE_SIMPLE_HTTPD
  LD_LIBS    += -lshttpd
endif

ifeq ($(USE_FTPD),yes)
  CFLAGS   += -DUSE_FTPD
  LD_LIBS  += -lftpd
endif

ifeq ($(USE_DEBUG),yes)
  DEFINES += -DSTART_DEBUG_STUB
  LD_LIBS += -lrtems-gdb-stub
endif

CFLAGS += -DUSE_MONITOR

LD_LIBS +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

HTML_GEN=html/osmonweb_ITRON_objs.html html/osmonweb_POSIX_objs.html \
  html/osmonweb_RTEID_objs.html
ifneq ($(HTTPD_LOGO),)
  HTML_GEN += html/$(HTTPD_LOGO)
endif

CLEAN_ADDITIONS += osmonweb_tar osmonweb_tar.c osmonweb_tar.h
CLEAN_ADDITIONS += $(HTML_GEN)
CLOBBER_ADDITIONS +=

# strip out flags gcc knows but LD doesn't like -- add as needed
LD_CPU_CFLAGS=$(CPU_CFLAGS:-mstrict-align:)

COPY_ARCH=$(shell echo $(OBJCOPY) | cut -d'-' -f1)

all:	${ARCH} $(HTML_GEN) $(SRCS) $(PGM)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

$(ARCH)/init.c: init.c osmonweb_tar.c

osmonweb_tar: $(ARCH) $(HTML_GEN)
	cd html ; \
	    tar cf ../osmonweb_tar --exclude CVS --exclude .cvsignore .

osmonweb_tar.c: $(ARCH) osmonweb_tar
	$(PROJECT_ROOT)/bin/bin2c osmonweb_tar osmonweb_tar

## Rule to make html we sed info into
html/%.html: htmlsrc/%.html.in
	sed -e 's/@HTTPD_SERVER@/$(HTTPD)/' \
	    -e 's/@HTTPD_LOGO@/$(HTTPD_LOGO)/' <$< >$@

## Rule to make .c/.h files from html
%.h %.c: html/%.html
        ./tools/html2c.perl --src-file=$< --dest-base=$(*F)

html/$(HTTPD_LOGO): htmlsrc/$(HTTPD_LOGO)
	cp $< $@

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests

