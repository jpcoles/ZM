ACLOCAL_AMFLAGS  = -I m4

EXTRA_DIST = server.py
EXTRA_PROGRAMS = pkdgrav2_null
TARGETS = null

if HAVE_PTHREAD
EXTRA_PROGRAMS	+= pkdgrav2_pthread
TARGETS	+= pthread
pthread		: pkdgrav2_pthread
endif

if HAVE_MPI
EXTRA_PROGRAMS	+= pkdgrav2_mpi
TARGETS	+= mpi
mpi:
	$(MAKE) CC=@MPICC@ FC=@MPIFC@ $(AM_MAKEFLAGS) pkdgrav2_mpi
endif

if HAVE_SISCI
EXTRA_PROGRAMS += pkdgrav2_sisci
sisci		: pkdgrav2_sisci
endif

EXTRA_PROGRAMS += tostd tdiff

null	: pkdgrav2_null

all-local: $(TARGETS)

clean-local:
	rm -f $(EXTRA_PROGRAMS)

SOURCES = main.c tree.c analysis.c romberg.c bt.c
noinst_HEADERS = bt.h parameters.h grav.h walk.h cycle.h tipsydefs.h
SOURCES        += cosmo.c master.c param.c pst.c smooth.c ewald.c
noinst_HEADERS += cosmo.h master.h param.h pst.h smooth.h ewald.h meval.h qeval.h
SOURCES        += moments.c intype.c outtype.c pkd.c smoothfcn.c rbtree.c fio.c listcomp.c
noinst_HEADERS += moments.h intype.h outtype.h pkd.h smoothfcn.h rbtree.h fio.h listcomp.h
if COOLING
#SOURCES        += cooling.c cooling_module.F90
#noinst_HEADERS += cooling.h
endif

if PSD
SOURCES += psdtree.c psd.c
noinst_HEADERS += psdtree.h psd.h
endif

if PLANETS
SOURCES += ssio.c collision.c kepler.c
noinst_HEADERS += ssio.h collision.h kepler.h
endif
if LOCAL_EXPANSION
SOURCES += grav2.c walk2.c
SOURCES += ilp.c ilc.c cl.c
noinst_HEADERS += ilp.h ilc.h cl.h
if USE_SIMD
AM_CFLAGS = @SIMD_CFLAGS@
noinst_HEADERS += simd.h
endif
else
SOURCES += grav.c walk.c
endif
if USE_HDF5
SOURCES += iohdf5.c
noinst_HEADERS += iohdf5.h
endif
if USE_MDL_IO
SOURCES += io.c
noinst_HEADERS += io.h
endif
MDL=@MDL_PATH@
if USE_PNG
SOURCES += png.c
noinst_HEADERS += png.h
endif
if USE_PYTHON
SOURCES += python.c
noinst_HEADERS += python.h
endif

pkdgrav2_null_SOURCES = $(SOURCES) $(MDL)/null/mdl.c
noinst_HEADERS += $(MDL)/null/mdl.h
pkdgrav2_null_CFLAGS = -I$(MDL)/null/ $(AM_CFLAGS)
pkdgrav2_null_LDADD = $(FLIBS)
if USE_EFENCE
if HAVE_EFENCE_PATH
pkdgrav2_null_LDADD += -L @EFENCE_PATH@
endif
pkdgrav2_null_LDADD += -lefence
endif
if USE_PYTHON
pkdgrav2_null_LDFLAGS = -Xlinker -export-dynamic 
pkdgrav2_null_LDADD += -L $(PYTHON_LIB) -lpython$(PYTHON_VER) -ldl -lrt -lutil
pkdgrav2_null_CFLAGS += -I $(PYTHON_INC)
endif
if LUSTRE
pkdgrav2_null_LDADD += -llustreapi
endif

if HAVE_MPI
pkdgrav2_mpi_LDADD =
pkdgrav2_mpi_SOURCES = $(SOURCES) $(MDL)/mpi/mdl.c
noinst_HEADERS += $(MDL)/mpi/mdl.h
pkdgrav2_mpi_CFLAGS = -I$(MDL)/mpi/ $(AM_CFLAGS)
if USE_BSC
pkdgrav2_mpi_LDADD += -L /gpfs/apps/CEPBATOOLS/mpitrace-devel/64/lib -ldl -lmpitrace_burst
pkdgrav2_mpi_CFLAGS += -I/gpfs/apps/CEPBATOOLS/mpitrace-devel/64/include
endif
if USE_GRAFIC
pkdgrav2_mpi_CFLAGS += -I@GRAFIC_PATH@
pkdgrav2_mpi_LDADD += -L @GRAFIC_PATH@ -lgrafic -lsrfftw_mpi -lsfftw_mpi -lsrfftw -lsfftw -lgfortran
#CCLD=mpicxx
endif
if USE_EFENCE
if HAVE_EFENCE_PATH
pkdgrav2_mpi_LDADD += -L @EFENCE_PATH@
endif
pkdgrav2_mpi_LDADD += -lefence
endif
if USE_PYTHON
pkdgrav2_mpi_LDFLAGS = -Xlinker -export-dynamic 
pkdgrav2_mpi_LDADD += -L $(PYTHON_LIB) -lpython$(PYTHON_VER) -ldl -lutil
pkdgrav2_mpi_CFLAGS += -I $(PYTHON_INC)
endif
if LUSTRE
pkdgrav2_mpi_LDADD += -llustreapi
endif
if FFTW
pkdgrav2_mpi_LDADD += -lsrfftw_mpi -lsfftw_mpi -lsrfftw -lsfftw
endif
endif

if HAVE_PTHREAD
pkdgrav2_pthread_SOURCES = $(SOURCES) $(MDL)/pthread/mdl.c
noinst_HEADERS += $(MDL)/pthread/mdl.h
pkdgrav2_pthread_CFLAGS = -I$(MDL)/pthread/ $(AM_CFLAGS)
pkdgrav2_pthread_LDADD = -lpthread
if USE_EFENCE
if HAVE_EFENCE_PATH
pkdgrav2_pthread_LDADD += -L @EFENCE_PATH@
endif
pkdgrav2_pthread_LDADD += -lefence
endif
if USE_PYTHON
pkdgrav2_pthread_LDFLAGS = -Xlinker -export-dynamic 
pkdgrav2_pthread_LDADD += -L $(PYTHON_LIB) -lpython$(PYTHON_VER) -ldl -lutil
pkdgrav2_pthread_CFLAGS += -I $(PYTHON_INC)
endif
if LUSTRE
pkdgrav2_pthread_LDADD += -llustreapi
endif
endif

if HAVE_SISCI
pkdgrav2_sisci_SOURCES = $(SOURCES) $(MDL)/sisci/mdl.c
noinst_HEADERS += $(MDL)/sisci/mdl.h
pkdgrav2_sisci_CFLAGS = -I$(MDL)/sisci/ $(AM_CFLAGS) -I$(SISCI_PATH)/include
pkdgrav2_sisci_LDADD = -lpthread -L $(SISCI_PATH)/lib64 -lsisci
if USE_EFENCE
if HAVE_EFENCE_PATH
pkdgrav2_sisci_LDADD += -L @EFENCE_PATH@
endif
pkdgrav2_sisci_LDADD += -lefence
endif
if USE_PYTHON
pkdgrav2_sisci_LDFLAGS = -Xlinker -export-dynamic 
pkdgrav2_sisci_LDADD += -L $(PYTHON_LIB) -lpython$(PYTHON_VER) -ldl -lutil
pkdgrav2_sisci_CFLAGS += -I $(PYTHON_INC)
endif
if LUSTRE
pkdgrav2_sisci_LDADD += -llustreapi
endif
endif

tostd_SOURCES = tostd.c fio.c romberg.c
tdiff_SOURCES = tdiff.c fio.c romberg.c
