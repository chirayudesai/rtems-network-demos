#
#  $Id$
#

PGM=${ARCH}/osmonweb.exe

MANAGERS=all

# C source names
GEN_C_FILES= osmonweb_tar.c osmonweb_RTEID_objs.c \
  osmonweb_POSIX_objs.c osmonweb_ITRON_objs.c
C_FILES=init.c osmonweb.c htmlprintf.c \
  osmonweb_RTEID.c osmonweb_RTEID_one_sema.c osmonweb_RTEID_onetask.c \
  osmonweb_RTEID_queues.c osmonweb_RTEID_semas.c osmonweb_RTEID_tasks.c
C_FILES+=$(GEN_C_FILES)
OBJS=$(C_FILES:%.c=${ARCH}/%.o)

GEN_H_FILES= osmonweb_tar.h osmonweb_RTEID_objs.h \
  osmonweb_POSIX_objs.h osmonweb_ITRON_objs.h
H_FILES=
H_FILES+= $(GEN_H_FILES)

DOCS=

SRCS=$(DOCS) $(C_FILES)

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

#HTML_GEN=html/osmonweb_ITRON_objs.html html/osmonweb_POSIX_objs.html \
#  html/osmonweb_RTEID_objs.html
ifneq ($(HTTPD_LOGO),)
  HTML_GEN += html/$(HTTPD_LOGO)
endif

CLEAN_ADDITIONS += osmonweb_tar osmonweb_tar.c osmonweb_tar.h
CLEAN_ADDITIONS += $(HTML_GEN) $(GEN_C_FILES) $(GEN_H_FILES)
CLEAN_ADDITIONS += stamp-gen-files
CLOBBER_ADDITIONS +=

# strip out flags gcc knows but LD doesn't like -- add as needed
LD_CPU_CFLAGS=$(CPU_CFLAGS:-mstrict-align:)

all:	${ARCH} stamp-gen-files $(SRCS) $(PGM)

stamp-gen-files: $(HTML_GEN) $(GEN_C_FILES) $(GEN_H_FILES)
	echo $(HTML_GEN) $(GEN_C_FILES) $(GEN_H_FILES)
	# touch stamp-gen-files

${PGM}: $(SRCS) $(OBJS) $(LINK_FILES)
	$(make-exe)

$(ARCH)/init.o: init.c osmonweb_tar.c

osmonweb_tar: $(ARCH) $(HTML_GEN)
	cd html ; \
	    tar cf ../osmonweb_tar --exclude CVS --exclude .cvsignore .

osmonweb_tar.c osmonweb_tar.h: $(ARCH) osmonweb_tar
	$(PROJECT_ROOT)/bin/bin2c osmonweb_tar osmonweb_tar

## Rule to make .c/.h files from html
%.h %.c: htmlsrc/%.html.in
	./tools/html2c.perl --src-file=$< --dest-base=$(*F)

html/$(HTTPD_LOGO): htmlsrc/$(HTTPD_LOGO)
	cp $< $@

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests

