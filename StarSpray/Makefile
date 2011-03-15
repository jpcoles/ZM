CC			 =	g++
CXX			 =	g++

PLATFORM		 =	Darwin
#PLATFORM		 =	Linux
#PLATFORM		 =	$(shell uname)
USERSRC			 =	$(HOME)

USE_GPU			 =	False
USE_NVIDIA 		 =	False

OBJS			 = 	main.o client.o \
	 			display.o visualize.o \
		 		simulator-local.o io.o \
		 		color_ramp.o net.o \
		 		tipsyio.o ic.o kiosk.o
HDRS			 = 	client.h color_ramp.h \
		 		display.h io.h keys.h \
		 		net.h server.h \
		 		simulation.h simulator.h \
		 		starspray.h tipsyio.h \
		 		visualize.h ic.h kiosk.h

SVR_OBJS		 = 	net.o
SVR_CXXOBJS		 =	server.o tipsyio.o
SVR_HDRS		 =	server.h starspray.h tipsyio.h 

CFLAGS  		+=	-Wall -g 
CFLAGS  		+=	-O3

CFLAGS  		+=	-Itipsylib
CFLAGS 			+=	-Ijpeg-6b 
CFLAGS 			+=	-I/usr/X11/include

LDFLAGS 		+= 	-Ltipsylib -L/usr/X11/lib

LIBS			+= 	-ltipsy -lpng


ifeq ($(PLATFORM), Darwin)
LDFLAGS 		+= 	-framework OpenGL -framework GLUT -framework ApplicationServices
LDFLAGS			+= 	-framework Carbon
LIBS 		 	+= 	-lpthread 
endif

ifeq ($(PLATFORM), Linux)
LIBS 			+= 	-glut 
endif

CXXFLAGS		+=	$(CFLAGS)


starspray: $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) jpeg-6b/libjpeg.a -o starspray $(LIBS)

starspray-server: $(SVR_OBJS) $(SVR_CXXOBJS) $(SVR_HDRS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SVR_OBJS) $(SVR_CXXOBJS) -o starspray-server $(LIBS)

all: starspray starspray-server

testgl: testgl.o
	$(CC) $(CFLAGS) $(LDFLAGS) testgl.o -o testgl $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.cu
	$(NVCC) $(CUFLAGS) -c $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(SVR_OBJS): %.o: %.c
	$(CC) -DSERVER $(CFLAGS) -c $< -o $@

$(SVR_CXXOBJS): %.o: %.cpp
	$(CXX) -DSERVER $(CXXFLAGS) -c $< -o $@

#$(OBJS): $(INCL)

.PHONY : clean
clean:
	rm -f $(OBJS) $(SVR_OBJS) $(SVR_CXXOBJS) starspray starspray-server

dist: clean
	mkdir -p dist
	cp README Makefile *.c *.cpp *h run_ss jonathan.par pkdgrav2m_pthread dist/
	cp *.jpg dist/
	cp -R jpeg-6b dist/
	cp -R tipsylib dist/


