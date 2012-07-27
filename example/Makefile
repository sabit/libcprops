# cprops examples - Makefile
#
# This is a Makefile for cprops examples. If it has the name "Makefile.in" then
# it is a template for a Makefile;  to generate the actual Makefile, run 
# "./configure" in the cprops base directory. 
#
# Copyright (c) 2005-2007 by Ilan Aelion
# iaelion@users.sourceforge.net
# 
# 2007-30-11 V 0.1.7 
#
###############################################

srcdir=.
prefix=/usr/local
exec_prefix=${prefix}

libdir=$(DESTDIR)${exec_prefix}/lib
incdir=$(DESTDIR)${prefix}/include/cprops

CC = gcc
CFLAGS = -D_REENTRANT -I $(incdir) -I .  -D_GNU_SOURCE -O2
LDFLAGS = -L../.libs -L$(libdir) 
LIBS = -lcprops -ldl  -lpthread

DBEXAMPLES = 
DBEXAMPLES_OBJ = $(DBEXAMPLES:.c=.o)
DBEXAMPLES_EXE = $(DBEXAMPLES:.c=)
DBLDFLAGS =  
DBLIBS =  

EXAMPLE_SRC = test_avl.c test_avl_multi.c test_client.c test_hashlist.c \
			  test_hashlist_iterator.c test_hashtable.c test_hashtable2.c \
			  test_httpasync.c test_httpclient.c test_list.c \
			  test_list_iterator.c test_priority_list.c test_proxy.c \
			  test_str.c test_trie.c test_trie2.c test_vector.c \
			  test_rb.c test_splay.c wf.c test_heap.c  \
			  test_mempool.c test_sorted_hash.c test_narytree.c \
			  test_multimap.c test_reindex.c test_reindex_unique.c \
			  test_wtab.c test_wtrie.c test_bstr.c test_rtrie.c

EXAMPLE_OBJ = $(EXAMPLE_SRC:.c=.o) 
EXAMPLE_EXE = $(EXAMPLE_SRC:.c=)

MAKEDEPEND=/home/sabit/devel/libcprops-0.1.12/makedepend.sh

DEPEND=$(MAKEDEPEND) $(CFLAGS)

all: linkdir examples db_examples

linkdir:
	@if ! test -e cprops ; then ln -s .. cprops ; fi

examples: $(EXAMPLE_OBJ)

.c.o: 
	$(CC) $(CFLAGS) -c $(@:.o=.c) -o $@
	$(CC) $(LDFLAGS) $@ -o $(@:.o=) $(LIBS)

db_examples: $(DBEXAMPLES_EXE)

$(DBEXAMPLES_EXE): $(DBEXAMPLES_OBJ)
	@echo DBSTUFF
	$(CC) $(CFLAGS) -c $(@:=.c) -o $(@:=.o)
	$(CC) $(LDFLAGS) $(DBLDFLAGS) $(@:=.o) -o $@ $(LIBS) $(DBLIBS)

.PHONY: all clean linkdir

clean: 
	rm -f $(EXAMPLE_OBJ) $(EXAMPLE_OBJ:.o=) $(DBEXAMPLES_OBJ) \
	   $(DBEXAMPLES_EXE) core* *core vgcore* *.log

$(EXAMPLE_OBJ): Makefile

depend: linkdir $(EXAMPLE_SRC)
	@$(DEPEND) $(EXAMPLE_SRC) > /dev/null 2>&1
# DO NOT DELETE

test_wtab.o: /usr/include/stdlib.h
test_wtab.o: /usr/include/features.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/predefs.h
test_wtab.o: /usr/include/x86_64-linux-gnu/sys/cdefs.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/wordsize.h
test_wtab.o: /usr/include/x86_64-linux-gnu/gnu/stubs.h
test_wtab.o: /usr/include/x86_64-linux-gnu/gnu/stubs-64.h
test_wtab.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/waitflags.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/waitstatus.h
test_wtab.o: /usr/include/endian.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/endian.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/byteswap.h
test_wtab.o: /usr/include/x86_64-linux-gnu/sys/types.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/types.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/typesizes.h
test_wtab.o: /usr/include/time.h
test_wtab.o: /usr/include/x86_64-linux-gnu/sys/select.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/select.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/sigset.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/time.h
test_wtab.o: /usr/include/x86_64-linux-gnu/sys/sysmacros.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h
test_wtab.o: /usr/include/alloca.h
test_wtab.o: /usr/include/stdio.h
test_wtab.o: /usr/include/libio.h
test_wtab.o: /usr/include/_G_config.h
test_wtab.o: /usr/include/wchar.h
test_wtab.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdarg.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/stdio_lim.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
test_wtab.o: cprops/wtab.h
test_wtab.o: cprops/common.h
test_wtab.o: cprops/collection.h
test_wtab.o: cprops/log.h
test_wtab.o: cprops/str.h
test_wtab.o: cprops/config.h
test_wtab.o: /usr/include/pthread.h
test_wtab.o: /usr/include/sched.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/sched.h
test_wtab.o: /usr/include/xlocale.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/setjmp.h
test_wtab.o: /usr/include/x86_64-linux-gnu/bits/wchar.h
test_wtab.o: /usr/include/wctype.h
test_bstr.o: /usr/include/stdio.h
test_bstr.o: /usr/include/features.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/predefs.h
test_bstr.o: /usr/include/x86_64-linux-gnu/sys/cdefs.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/wordsize.h
test_bstr.o: /usr/include/x86_64-linux-gnu/gnu/stubs.h
test_bstr.o: /usr/include/x86_64-linux-gnu/gnu/stubs-64.h
test_bstr.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/types.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/typesizes.h
test_bstr.o: /usr/include/libio.h
test_bstr.o: /usr/include/_G_config.h
test_bstr.o: /usr/include/wchar.h
test_bstr.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdarg.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/stdio_lim.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
test_bstr.o: /usr/include/string.h
test_bstr.o: /usr/include/xlocale.h
test_bstr.o: /usr/include/stdlib.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/waitflags.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/waitstatus.h
test_bstr.o: /usr/include/endian.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/endian.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/byteswap.h
test_bstr.o: /usr/include/x86_64-linux-gnu/sys/types.h
test_bstr.o: /usr/include/time.h
test_bstr.o: /usr/include/x86_64-linux-gnu/sys/select.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/select.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/sigset.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/time.h
test_bstr.o: /usr/include/x86_64-linux-gnu/sys/sysmacros.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h
test_bstr.o: /usr/include/alloca.h
test_bstr.o: cprops/bstr.h
test_bstr.o: cprops/collection.h
test_bstr.o: cprops/common.h
test_bstr.o: cprops/log.h
test_bstr.o: cprops/str.h
test_bstr.o: cprops/config.h
test_bstr.o: /usr/include/pthread.h
test_bstr.o: /usr/include/sched.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/sched.h
test_bstr.o: /usr/include/x86_64-linux-gnu/bits/setjmp.h
test_rtrie.o: /usr/include/stdio.h
test_rtrie.o: /usr/include/features.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/predefs.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/sys/cdefs.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/wordsize.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/gnu/stubs.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/gnu/stubs-64.h
test_rtrie.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/types.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/typesizes.h
test_rtrie.o: /usr/include/libio.h
test_rtrie.o: /usr/include/_G_config.h
test_rtrie.o: /usr/include/wchar.h
test_rtrie.o: /usr/lib/gcc/x86_64-linux-gnu/4.6/include/stdarg.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/stdio_lim.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/sys_errlist.h
test_rtrie.o: /usr/include/string.h
test_rtrie.o: /usr/include/xlocale.h
test_rtrie.o: /usr/include/stdlib.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/waitflags.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/waitstatus.h
test_rtrie.o: /usr/include/endian.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/endian.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/byteswap.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/sys/types.h
test_rtrie.o: /usr/include/time.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/sys/select.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/select.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/sigset.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/time.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/sys/sysmacros.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h
test_rtrie.o: /usr/include/alloca.h
test_rtrie.o: cprops/rtrie.h
test_rtrie.o: cprops/common.h
test_rtrie.o: cprops/config.h
test_rtrie.o: /usr/include/errno.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/errno.h
test_rtrie.o: /usr/include/linux/errno.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/asm/errno.h
test_rtrie.o: /usr/include/asm-generic/errno.h
test_rtrie.o: /usr/include/asm-generic/errno-base.h
test_rtrie.o: cprops/vector.h
test_rtrie.o: cprops/collection.h
test_rtrie.o: cprops/log.h
test_rtrie.o: cprops/str.h
test_rtrie.o: /usr/include/pthread.h
test_rtrie.o: /usr/include/sched.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/sched.h
test_rtrie.o: /usr/include/x86_64-linux-gnu/bits/setjmp.h
test_rtrie.o: cprops/bstr.h
test_rtrie.o: cprops/mempool.h
test_rtrie.o: cprops/rb.h
test_rtrie.o: cprops/hashtable.h
test_rtrie.o: cprops/vector.h
