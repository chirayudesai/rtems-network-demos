#
#  $Id$
#

PGM=${ARCH}/osmonweb.exe
MANAGERS=all

# Application Configuration parameters
USE_GOAHEAD=no
USE_SIMPLE=yes
USE_FTPD=yes
USE_DEBUG=no

# C source names
C_FILES=init.c osmonweb.c htmlprintf.c \
  osmonweb_RTEID.c \
  osmonweb_RTEID_queues.c 
ifeq ($(USE_GOAHEAD),yes)
  C_FILES+=  osmonweb_goahead.c
endif
ifeq ($(USE_SIMPLE),yes)
  C_FILES+=  osmonweb_shttpd.c shttpd_ext.c
endif
GEN_C_FILES= osmonweb_tar.c FilesystemImage.c \
  osmonweb_RTEID_objs.c osmonweb_POSIX_objs.c osmonweb_ITRON_objs.c \
  osmonweb_RTEID_tasks.c osmonweb_RTEID_onetask.c \
  osmonweb_RTEID_semas.c osmonweb_RTEID_onesema.c
C_FILES+=$(GEN_C_FILES)
OBJS=$(C_FILES:%.c=${ARCH}/%.o)

GEN_H_FILES= osmonweb_tar.h osmonweb_RTEID_objs.h \
  osmonweb_POSIX_objs.h osmonweb_ITRON_objs.h \
  osmonweb_RTEID_tasks.h osmonweb_RTEID_onetask.h \
  osmonweb_RTEID_semas.h osmonweb_RTEID_onesema.h
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

ifeq ($(USE_GOAHEAD),yes)
  HTTPD         = GoAhead Web Server
  HTTPD_LOGO    = webserver_logo2.gif
  HTTPD_INDEX   = index_goahead.html
  CFLAGS       += -DWEBS -DUEMF -DUSE_GOAHEAD_HTTPD
  LD_LIBS      += -lhttpd
endif

ifeq ($(USE_SIMPLE),yes)
  HTTPD         = Simple HTTPD Web Server
  HTTPD_LOGO    = SimpleHTTPD.png
  HTTPD_INDEX   = index_shttpd.html
  CFLAGS       += -DUSE_SIMPLE_HTTPD
  LD_LIBS      += -lshttpd
endif

ifeq ($(USE_FTPD),yes)
  CFLAGS   += -DUSE_FTPD
  LD_LIBS  += -lftpd
endif

ifeq ($(USE_DEBUG),yes)
  DEFINES += -DSTART_DEBUG_STUB
  LD_LIBS += -lrtems-gdb-stub
endif

DEFINES  +=
CPPFLAGS +=
## CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0xC0000 
LD_PATHS  +=
CFLAGS += -DUSE_MONITOR


ifneq ($(HTTPD_LOGO),)
  HTML_GEN += html/$(HTTPD_LOGO)
endif

CLEAN_ADDITIONS += osmonweb_tar osmonweb_tar.c osmonweb_tar.h
CLEAN_ADDITIONS += FilesystemImage FilesystemImage.c FilesystemImage.h
CLEAN_ADDITIONS += $(HTML_GEN) $(GEN_C_FILES) $(GEN_H_FILES)
CLEAN_ADDITIONS += html/webserver_logo2.gif 
CLEAN_ADDITIONS += html/SimpleHTTPD.png
CLEAN_ADDITIONS += rootfs/index.html
CLEAN_ADDITIONS += stamp-gen-files
CLOBBER_ADDITIONS +=


all:	${ARCH} stamp-gen-files $(SRCS) $(PGM)

stamp-gen-files: $(HTML_GEN) $(GEN_C_FILES) $(GEN_H_FILES)
	echo $(HTML_GEN) $(GEN_C_FILES) $(GEN_H_FILES)
	# touch stamp-gen-files

${PGM}: $(SRCS) $(OBJS) $(LINK_FILES)
	$(make-exe)

$(ARCH)/init.o: init.c osmonweb_tar.h FilesystemImage.h

## FilesystemImage: $(ARCH) 

FilesystemImage: $(ARCH) rootfs/etc/host.conf rootfs/etc/hosts rootfs/index.html
	cd rootfs ; \
	    tar cf ../FilesystemImage --exclude CVS --exclude .cvsignore .

FilesystemImage.c FilesystemImage.h: $(ARCH) FilesystemImage
	$(PROJECT_ROOT)/bin/bin2c FilesystemImage FilesystemImage

rootfs/index.html: index_goahead.html index_shttpd.html
	cp $(HTTPD_INDEX) $@

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

