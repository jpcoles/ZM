SUBDIRS = celutil celmath cel3ds celtxf celengine celestia

DEFS = -DHIP_DATA_DIR='"$(HIPDIR)"' @DEFS@

dosstuff = winbuild.mak
buildstardb_SOURCES = buildstardb.cpp
noinst_DATA = $(dosstuff)
EXTRA_DIST = $(noinst_DATA) packdb.cpp packnames.cpp readstars.cpp

noinst_PROGRAMS = buildstardb

# Non-automake directories
NONAUTO = tools

dist-hook:
	( \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(NONAUTO) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)|(/CVS)|(/\.svn)|(/\.#)|(/\.deps)' \
	        | cpio -pdum $(distdir) 2>/dev/null ; \
	    done \
	  ) \
	)
