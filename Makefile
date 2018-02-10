VERSION = 1.6
ARCH = $(shell uname -m)
CC = gcc
CP = cp
MKDIR = mkdir -p
RM = rm -f
TAR = tar

ifndef PREFIX
PREFIX=/usr/local
endif

SOURCE_FOLDER=$(shell basename $(shell pwd))
INSTPATH_SBIN=$(subst //,/,$(PREFIX)/sbin)
INSTPATH_MAN=$(subst //,/,$(PREFIX)/share/man/man8)

all: pvvgname

pvvgname: pvvgname.c
	$(CC) pvvgname.c -DPVVGNAME_VERSION=$(VERSION) -o pvvgname

install: pvvgname doc
	$(MKDIR) $(INSTPATH_SBIN)
	$(CP) pvvgname $(INSTPATH_SBIN)/pvvgname
	$(MKDIR) $(INSTPATH_MAN)
	$(CP) man/pvvgname.8.gz $(INSTPATH_MAN)/pvvgname.8.gz

clean:
	$(RM) pvvgname
	$(RM) man/pvvgname.8.gz

distclean: clean

dist: distclean
	cd .. && $(TAR) --exclude-vcs -c -f - $(SOURCE_FOLDER) | bzip2 -9 > pvvgname-$(VERSION).tar.bz2 && cd $(SOURCE_FOLDER)

doc: man/pvvgname.man.text
	@scripts/make-man-pages.sh man $(VERSION) $(ARCH)
