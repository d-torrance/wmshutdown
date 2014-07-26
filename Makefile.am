bin_PROGRAMS = wmshutdown wmshutdown-run
wmshutdown_SOURCES = wmshutdown.c
wmshutdown_run_SOURCES = wmshutdown-run.c

AM_CFLAGS = $(gtk_CFLAGS) $(x11_CFLAGS)
AM_CPPFLAGS =  -DDATADIR=\"$(pkgdatadir)\"
LIBS += $(gtk_LIBS) $(x11_LIBS)

dist_pkgdata_DATA = wmshutdown.xpm
dist_man_MANS = wmshutdown.1 wmshutdown-run.1

install-exec-hook:
	chmod ug+s $(DESTDIR)$(bindir)/wmshutdown-run
