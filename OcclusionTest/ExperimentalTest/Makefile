BASEDIR=$(HOME)/src/g++-3.2/berlios
OCTESTDIR=$(BASEDIR)/OcclusionTest/ExperimentalTest/
CC = g++
MAKE = make

INCLUDEPATH = -I$(OCTESTDIR)/../Experimental/ -I$(HOME)/packages/g++-3.2/include/ -I$(HOME)/packages/g++-3.2/include/OpenSG/ -I$(HOME)/packages/g++-3.2/include/ -I.
DEFINES = -DOSG_WITH_QT=1 -DQT_CLEAN_NAMESPACE -D__GCC__ -DUseOpenGL -DOPEN_GL -DUseAnsiCPlusPlus -D__linux -DUNIX -DOSG_OPT_DRAWTREE
CFLAGS = -g -O3 -Wno-unknown-pragmas $(DEFINES) $(INCLUDEPATH)
LIBS = -lOSGSystem -lOSGBase -lOSGWindowX -lOSGWindowQT -ltiff -lqt-mt -lGL -lGLU -lpthread -lm -ldl
LIBPATH = -L$(HOME)/packages/g++-3.2/lib/dbg/ -L$(HOME)/packages/g++-3.2/lib -L/usr/lib
MOC = /usr/bin/moc

SRCS = testOCosg_qt.cpp OCRenderAction.cpp
OBJS = $(subst .cpp,.o,$(SRCS))
DEPS = $(subst .cpp,.d,$(SRCS))
QTFILES = testOCosg_qt.cpp
MOCFILES = $(subst _qt.cpp,_qt_moc.cpp,$(QTFILES))

all: testOCosg_qt

testOCosg_qt: $(MOCFILES) $(OBJS)
	$(CC) $(CFLAGS) $(LIBPATH) $(OBJS) -o testOCosg_qt $(OCLIBS) $(LIBS)

$(MOCFILES):
	$(MOC) $(subst _qt_moc.cpp,_qt.cpp,$@) -o $@
	
clean:
	rm -f $(OBJS) $(DEPS) $(MOCFILES)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

.SUFFIXES: .cpp .o 

