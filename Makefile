VERSION = 1.1

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

all: pvvgname

pvvgname: pvvgname.c
	$(CC) pvvgname.c -DPVVGNAME_VERSION=$(VERSION) -o pvvgname

install: pvvgname
	$(MKDIR) $(INSTPATH_SBIN)
	$(CP) pvvgname $(INSTPATH_SBIN)/pvvgname

clean:
	$(RM) pvvgname

distclean: clean

dist: distclean
	cd .. && $(TAR) -c -f - $(SOURCE_FOLDER) | bzip2 -9 > pvvgname-$(VERSION).tar.bz2 && cd $(SOURCE_FOLDER)
