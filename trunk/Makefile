VERSION = 1.9

# Commands
ifndef CC
CC = gcc
endif
ifndef CP
CP = cp
endif
ifndef MKDIR
MKDIR = mkdir -p
endif
ifndef RM
RM = rm -f
endif
ifndef TAR
TAR = tar
endif

# Variables
ifndef PREFIX
PREFIX=/usr/local
endif
ifndef ARCH
ARCH = $(shell uname -m)
endif

# Installation source & target paths
SOURCE_FOLDER=$(shell basename $(shell pwd))
INSTPATH_SBIN=$(subst //,/,$(PREFIX)/sbin)
INSTPATH_MAN=$(subst //,/,$(PREFIX)/share/man/man8)

# Rules
all: pvvgname man

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

man: doc
doc: man/pvvgname.8.gz
man/pvvgname.8.gz: man/pvvgname.man.text
	@scripts/make-man-pages.sh man $(VERSION) $(ARCH)
