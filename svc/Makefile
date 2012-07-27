# cpsvc - an http server - Makefile
#
# This is the Makefile for cpsvc. If it has the name "Makefile.in" then it is a
# template for a Makefile;  to generate the actual Makefile, run "./configure"
# in the cprops base directory. 
#
# Copyright (c) 2005-2007 by Ilan Aelion
# iaelion@users.sourceforge.net
# 
# 2007-11-30 V 0.1.7 beta
#
###############################################

top_builddir = /home/sabit/devel/libcprops-0.1.12

prefix = /usr/local
exec_prefix = ${prefix}

bindir = $(DESTDIR)${exec_prefix}/bin
libdir = $(DESTDIR)${exec_prefix}/lib
incdir = $(DESTDIR)${prefix}/include/cprops
mandir = $(DESTDIR)${datarootdir}/man
datadir = ${prefix}/share

CC = gcc
CFLAGS = -D_REENTRANT -I $(top_builddir)/..  -D_GNU_SOURCE -O2
LDFLAGS = -L../.libs -L$(libdir) 
LIBS = -lcprops -ldl  -lpthread

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

MAKEDEPEND = /home/sabit/devel/libcprops-0.1.12/makedepend.sh

SVC_SRC = file.c cgi.c cpsp.c cpsp_invoker.c
SVC_HDR = $(SVC_SRC:.c=.h)
SVC_OBJ = $(SVC_SRC:.c=.o) cpsvc.o

DEPEND=$(MAKEDEPEND) $(CFLAGS)

all: linkdir cpsvc

linkdir:
	@if ! test -e cprops ; then ln -s .. cprops ; fi

cpsvc: $(SVC_OBJ)
	$(CC) $(LDFLAGS) $(SVC_OBJ) -o $@ $(LIBS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean install uninstall linkdir

clean: 
	rm -f $(SVC_OBJ) cpsvc core* *core vgcore* *.log

install: all installdirs
	@$(INSTALL) cpsvc $(bindir)
	@$(INSTALL_DATA) mime.types $(bindir)

installdirs:
	$(SHELL) $(top_builddir)/mkinstalldirs $(bindir)

uninstall:
	rm -f $(bindir)/cpsvc
	rm -f $(bindir)/mime.types

$(SVC_OBJ): Makefile

depend: $(SVC_SRC)
	@$(DEPEND) $(SVC_SRC) > /dev/null 2>&1
# DO NOT DELETE

