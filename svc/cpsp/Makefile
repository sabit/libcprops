# cpsp - a server page compiler - Makefile
#
# This is the Makefile for cpsp. If it has the name "Makefile.in" then it is a
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

libdir = $(DESTDIR)${exec_prefix}/lib
incdir = $(DESTDIR)${prefix}/include
bindir = $(DESTDIR)${exec_prefix}/bin

LEX = flex
YACC = bison -y
CC = gcc
CFLAGS = -I /home/sabit/devel/libcprops-0.1.12/..  -D_GNU_SOURCE -O2 
LDFLAGS = -L/home/sabit/devel/libcprops-0.1.12/.libs 
LIBS = -lcprops -ldl  -lpthread

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

CPSPCFLAGS=$(shell grep 'CPSPCFLAGS=' $(CPSP_SRC) | sed 's/.*=//'|sed 's/%>//')
CPSPLDFLAGS=$(shell grep 'CPSPLDFLAGS=' $(CPSP_SRC)|sed 's/.*=//'|sed 's/%>//')
CPSPLIBS=$(shell grep 'CPSPLIBS=' $(CPSP_SRC) | sed 's/.*=//'|sed 's/%>//')

all: linkdir cpsp

.PHONY: all linkdir clean install uninstall

linkdir: 
	@if ! test -d cprops ; then ln -s ../.. cprops ; fi

cpsp: lex.yy.c y.tab.c
	$(CC) -g -O2 $(CFLAGS) $(LDFLAGS) lex.yy.c y.tab.c -o cpsp $(LIBS)
	@chmod +x cpsp-gen.sh

lex.yy.c: cpsp.l Makefile
	$(LEX) cpsp.l

y.tab.c: cpsp.y Makefile
	$(YACC) -d cpsp.y

clean:
	rm -f core* cpsp.o cpsp lex.yy.c y.tab.c y.tab.h *.o *.so _cpsp_*

install: all
	@$(INSTALL) cpsp $(bindir)
	@$(INSTALL) cpsp-gen.sh $(bindir)
	@$(INSTALL_DATA) Makefile.cpsp $(bindir)

uninstall:
	rm -f $(bindir)/cpsp
	rm -f $(bindir)/cpsp-gen.sh
	rm -f $(bindir)/Makefile.cpsp

cpsp: Makefile
