MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\"

bin_PROGRAMS = getwakeup

getwakeup_DEPENDENCIES = $(top_builddir)/src/lib/libsvdrp.la
getwakeup_LDADD = $(top_builddir)/src/lib/libsvdrp.la

getwakeup_SOURCES = getwakeup.c
