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

file.o: /usr/include/ctype.h
file.o: /usr/include/features.h
file.o: /usr/include/x86_64-linux-gnu/bits/predefs.h
file.o: /usr/include/x86_64-linux-gnu/sys/cdefs.h
file.o: /usr/include/x86_64-linux-gnu/bits/wordsize.h
file.o: /usr/include/x86_64-linux-gnu/gnu/stubs.h
file.o: /usr/include/x86_64-linux-gnu/gnu/stubs-64.h
file.o: /usr/include/x86_64-linux-gnu/bits/types.h
file.o: /usr/include/x86_64-linux-gnu/bits/typesizes.h
file.o: /usr/include/endian.h
file.o: /usr/include/x86_64-linux-gnu/bits/endian.h
file.o: /usr/include/x86_64-linux-gnu/bits/byteswap.h
file.o: /usr/include/xlocale.h
file.o: /usr/include/stdlib.h
file.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h
file.o: /usr/include/x86_64-linux-gnu/bits/waitflags.h
file.o: /usr/include/x86_64-linux-gnu/bits/waitstatus.h
file.o: /usr/include/x86_64-linux-gnu/sys/types.h
file.o: /usr/include/time.h
file.o: /usr/include/x86_64-linux-gnu/sys/select.h
file.o: /usr/include/x86_64-linux-gnu/bits/select.h
file.o: /usr/include/x86_64-linux-gnu/bits/sigset.h
file.o: /usr/include/x86_64-linux-gnu/bits/time.h
file.o: /usr/include/x86_64-linux-gnu/sys/sysmacros.h
file.o: /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h
file.o: /usr/include/alloca.h
file.o: cprops/config.h
file.o: cprops/util.h
file.o: cprops/common.h
file.o: cprops/config.h
file.o: /usr/include/x86_64-linux-gnu/sys/time.h
file.o: file.h
file.o: cprops/http.h
file.o: cprops/hashtable.h
file.o: cprops/collection.h
file.o: cprops/log.h
file.o: cprops/str.h
file.o: /usr/include/stdio.h
file.o: /usr/include/libio.h
file.o: /usr/include/_G_config.h
file.o: /usr/include/wchar.h
file.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdarg.h
file.o: /usr/include/x86_64-linux-gnu/bits/stdio_lim.h
file.o: /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
file.o: /usr/include/pthread.h
file.o: /usr/include/sched.h
file.o: /usr/include/x86_64-linux-gnu/bits/sched.h
file.o: /usr/include/x86_64-linux-gnu/bits/setjmp.h
file.o: cprops/trie.h
file.o: /usr/include/string.h
file.o: /usr/include/errno.h
file.o: /usr/include/x86_64-linux-gnu/bits/errno.h
file.o: /usr/include/linux/errno.h
file.o: /usr/include/x86_64-linux-gnu/asm/errno.h
file.o: /usr/include/asm-generic/errno.h
file.o: /usr/include/asm-generic/errno-base.h
file.o: cprops/vector.h
file.o: cprops/mtab.h
file.o: cprops/mempool.h
file.o: cprops/rb.h
file.o: cprops/socket.h
file.o: cprops/hashlist.h
file.o: cprops/thread.h
file.o: cprops/linked_list.h
file.o: /usr/include/netinet/in.h
file.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdint.h
file.o: /usr/include/stdint.h
file.o: /usr/include/x86_64-linux-gnu/bits/wchar.h
file.o: /usr/include/x86_64-linux-gnu/sys/socket.h
file.o: /usr/include/x86_64-linux-gnu/sys/uio.h
file.o: /usr/include/x86_64-linux-gnu/bits/uio.h
file.o: /usr/include/x86_64-linux-gnu/bits/socket.h
file.o: /usr/include/x86_64-linux-gnu/bits/sockaddr.h
file.o: /usr/include/x86_64-linux-gnu/asm/socket.h
file.o: /usr/include/asm-generic/socket.h
file.o: /usr/include/x86_64-linux-gnu/asm/sockios.h
file.o: /usr/include/asm-generic/sockios.h
file.o: /usr/include/x86_64-linux-gnu/bits/in.h
cgi.o: cprops/http.h
cgi.o: cprops/config.h
cgi.o: cprops/common.h
cgi.o: /usr/include/features.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/predefs.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/cdefs.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/wordsize.h
cgi.o: /usr/include/x86_64-linux-gnu/gnu/stubs.h
cgi.o: /usr/include/x86_64-linux-gnu/gnu/stubs-64.h
cgi.o: cprops/hashtable.h
cgi.o: cprops/collection.h
cgi.o: cprops/log.h
cgi.o: cprops/str.h
cgi.o: /usr/include/stdio.h
cgi.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/types.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/typesizes.h
cgi.o: /usr/include/libio.h
cgi.o: /usr/include/_G_config.h
cgi.o: /usr/include/wchar.h
cgi.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdarg.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/stdio_lim.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
cgi.o: /usr/include/pthread.h
cgi.o: /usr/include/endian.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/endian.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/byteswap.h
cgi.o: /usr/include/sched.h
cgi.o: /usr/include/time.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sched.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/time.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/timex.h
cgi.o: /usr/include/xlocale.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/setjmp.h
cgi.o: cprops/trie.h
cgi.o: /usr/include/string.h
cgi.o: /usr/include/errno.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/errno.h
cgi.o: /usr/include/linux/errno.h
cgi.o: /usr/include/x86_64-linux-gnu/asm/errno.h
cgi.o: /usr/include/asm-generic/errno.h
cgi.o: /usr/include/asm-generic/errno-base.h
cgi.o: cprops/vector.h
cgi.o: cprops/mtab.h
cgi.o: cprops/mempool.h
cgi.o: cprops/rb.h
cgi.o: cprops/socket.h
cgi.o: cprops/hashlist.h
cgi.o: cprops/thread.h
cgi.o: cprops/linked_list.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/time.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/select.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/select.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sigset.h
cgi.o: /usr/include/netinet/in.h
cgi.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdint.h
cgi.o: /usr/include/stdint.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/wchar.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/socket.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/uio.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/types.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/sysmacros.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/uio.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/socket.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sockaddr.h
cgi.o: /usr/include/x86_64-linux-gnu/asm/socket.h
cgi.o: /usr/include/asm-generic/socket.h
cgi.o: /usr/include/x86_64-linux-gnu/asm/sockios.h
cgi.o: /usr/include/asm-generic/sockios.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/in.h
cgi.o: cprops/str.h
cgi.o: cgi.h
cgi.o: cprops/common.h
cgi.o: /usr/include/stdlib.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/waitflags.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/waitstatus.h
cgi.o: /usr/include/alloca.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/stat.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/stat.h
cgi.o: /usr/include/unistd.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/posix_opt.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/environments.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/confname.h
cgi.o: /usr/include/getopt.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/wait.h
cgi.o: /usr/include/signal.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/signum.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/siginfo.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sigaction.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sigcontext.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sigstack.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/ucontext.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/sigthread.h
cgi.o: /usr/include/x86_64-linux-gnu/sys/resource.h
cgi.o: /usr/include/x86_64-linux-gnu/bits/resource.h
cgi.o: /usr/include/arpa/inet.h
cpsp.o: /usr/include/errno.h
cpsp.o: /usr/include/features.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/predefs.h
cpsp.o: /usr/include/x86_64-linux-gnu/sys/cdefs.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/wordsize.h
cpsp.o: /usr/include/x86_64-linux-gnu/gnu/stubs.h
cpsp.o: /usr/include/x86_64-linux-gnu/gnu/stubs-64.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/errno.h
cpsp.o: /usr/include/linux/errno.h
cpsp.o: /usr/include/x86_64-linux-gnu/asm/errno.h
cpsp.o: /usr/include/asm-generic/errno.h
cpsp.o: /usr/include/asm-generic/errno-base.h
cpsp.o: /usr/include/string.h
cpsp.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h
cpsp.o: /usr/include/xlocale.h
cpsp.o: /usr/include/stdlib.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/waitflags.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/waitstatus.h
cpsp.o: /usr/include/endian.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/endian.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/byteswap.h
cpsp.o: /usr/include/x86_64-linux-gnu/sys/types.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/types.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/typesizes.h
cpsp.o: /usr/include/time.h
cpsp.o: /usr/include/x86_64-linux-gnu/sys/select.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/select.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/sigset.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/time.h
cpsp.o: /usr/include/x86_64-linux-gnu/sys/sysmacros.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h
cpsp.o: /usr/include/alloca.h
cpsp.o: cprops/collection.h
cpsp.o: cprops/common.h
cpsp.o: cprops/log.h
cpsp.o: cprops/str.h
cpsp.o: cprops/config.h
cpsp.o: /usr/include/stdio.h
cpsp.o: /usr/include/libio.h
cpsp.o: /usr/include/_G_config.h
cpsp.o: /usr/include/wchar.h
cpsp.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdarg.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/stdio_lim.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
cpsp.o: /usr/include/pthread.h
cpsp.o: /usr/include/sched.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/sched.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/setjmp.h
cpsp.o: cprops/hashtable.h
cpsp.o: cprops/collection.h
cpsp.o: cprops/vector.h
cpsp.o: cprops/http.h
cpsp.o: cprops/hashtable.h
cpsp.o: cprops/trie.h
cpsp.o: cprops/vector.h
cpsp.o: cprops/mtab.h
cpsp.o: cprops/mempool.h
cpsp.o: cprops/rb.h
cpsp.o: cprops/socket.h
cpsp.o: cprops/hashlist.h
cpsp.o: cprops/thread.h
cpsp.o: cprops/linked_list.h
cpsp.o: /usr/include/x86_64-linux-gnu/sys/time.h
cpsp.o: /usr/include/netinet/in.h
cpsp.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdint.h
cpsp.o: /usr/include/stdint.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/wchar.h
cpsp.o: /usr/include/x86_64-linux-gnu/sys/socket.h
cpsp.o: /usr/include/x86_64-linux-gnu/sys/uio.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/uio.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/socket.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/sockaddr.h
cpsp.o: /usr/include/x86_64-linux-gnu/asm/socket.h
cpsp.o: /usr/include/asm-generic/socket.h
cpsp.o: /usr/include/x86_64-linux-gnu/asm/sockios.h
cpsp.o: /usr/include/asm-generic/sockios.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/in.h
cpsp.o: cprops/util.h
cpsp.o: cprops/str.h
cpsp.o: cpsp.h
cpsp.o: cprops/config.h
cpsp.o: cprops/common.h
cpsp.o: /usr/include/dlfcn.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/dlfcn.h
cpsp.o: /usr/include/unistd.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/posix_opt.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/environments.h
cpsp.o: /usr/include/x86_64-linux-gnu/bits/confname.h
cpsp.o: /usr/include/getopt.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/types.h
cpsp_invoker.o: /usr/include/features.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/predefs.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/cdefs.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/wordsize.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/gnu/stubs.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/gnu/stubs-64.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/types.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/typesizes.h
cpsp_invoker.o: /usr/include/time.h
cpsp_invoker.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h
cpsp_invoker.o: /usr/include/endian.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/endian.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/byteswap.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/select.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/select.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sigset.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/time.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/sysmacros.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h
cpsp_invoker.o: /usr/include/errno.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/errno.h
cpsp_invoker.o: /usr/include/linux/errno.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/asm/errno.h
cpsp_invoker.o: /usr/include/asm-generic/errno.h
cpsp_invoker.o: /usr/include/asm-generic/errno-base.h
cpsp_invoker.o: /usr/include/stdlib.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/waitflags.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/waitstatus.h
cpsp_invoker.o: /usr/include/alloca.h
cpsp_invoker.o: cprops/config.h
cpsp_invoker.o: cprops/http.h
cpsp_invoker.o: cprops/config.h
cpsp_invoker.o: cprops/common.h
cpsp_invoker.o: cprops/hashtable.h
cpsp_invoker.o: cprops/collection.h
cpsp_invoker.o: cprops/log.h
cpsp_invoker.o: cprops/str.h
cpsp_invoker.o: /usr/include/stdio.h
cpsp_invoker.o: /usr/include/libio.h
cpsp_invoker.o: /usr/include/_G_config.h
cpsp_invoker.o: /usr/include/wchar.h
cpsp_invoker.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdarg.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/stdio_lim.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
cpsp_invoker.o: /usr/include/pthread.h
cpsp_invoker.o: /usr/include/sched.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sched.h
cpsp_invoker.o: /usr/include/xlocale.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/setjmp.h
cpsp_invoker.o: cprops/trie.h
cpsp_invoker.o: /usr/include/string.h
cpsp_invoker.o: cprops/vector.h
cpsp_invoker.o: cprops/mtab.h
cpsp_invoker.o: cprops/mempool.h
cpsp_invoker.o: cprops/rb.h
cpsp_invoker.o: cprops/socket.h
cpsp_invoker.o: cprops/hashlist.h
cpsp_invoker.o: cprops/thread.h
cpsp_invoker.o: cprops/linked_list.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/time.h
cpsp_invoker.o: /usr/include/netinet/in.h
cpsp_invoker.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdint.h
cpsp_invoker.o: /usr/include/stdint.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/wchar.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/socket.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/uio.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/uio.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/socket.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sockaddr.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/asm/socket.h
cpsp_invoker.o: /usr/include/asm-generic/socket.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/asm/sockios.h
cpsp_invoker.o: /usr/include/asm-generic/sockios.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/in.h
cpsp_invoker.o: cprops/util.h
cpsp_invoker.o: cprops/log.h
cpsp_invoker.o: cpsp_invoker.h
cpsp_invoker.o: cprops/common.h
cpsp_invoker.o: cprops/collection.h
cpsp_invoker.o: cprops/hashlist.h
cpsp_invoker.o: cprops/vector.h
cpsp_invoker.o: cprops/hashtable.h
cpsp_invoker.o: cpsp.h
cpsp_invoker.o: /usr/include/unistd.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/posix_opt.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/environments.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/confname.h
cpsp_invoker.o: /usr/include/getopt.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/wait.h
cpsp_invoker.o: /usr/include/signal.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/signum.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/siginfo.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sigaction.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sigcontext.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sigstack.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/ucontext.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/sigthread.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/sys/resource.h
cpsp_invoker.o: /usr/include/x86_64-linux-gnu/bits/resource.h
