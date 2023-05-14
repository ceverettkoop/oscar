CC=gcc
CXX=g++
CFLAGS=-I$(IDIR)
CPPFLAGS=-I$(IDIR) -fPIC -ggdb	
ODIR=obj
IDIR =../include
LDFLAGS=-L../lib
LDLIBS=-l:libBWAPILIB.so -l:libBWAPIClient.a -l:libBWEBlib.a
DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
_DEPS=
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ=Dll.o GameState.o Macro.o Oscar.o OscarMap.o Tools.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
VPATH = src:src/Map
INSTALLPATH= /home/sean/development/bwapi/build/bin/bwapi-data/AI/
BINNAME=oscar.so
SHAREDFLAG = -shared
OFLAG=-o 
#DEFFLAG=

#windows changes
ifeq ($(OS),Windows_NT)
	override CPPFLAGS = -I$(IDIR)
	override INSTALLPATH = /c/Users/sean/BWAPI/Starcraft/bwapi-data/AI/
	override LDLIBS = ..\lib\BWEM-Release.lib ..\lib\BWAPIClient.lib ..\lib\BWAPILIB.lib
	override BINNAME = oscar.dll
	override SHAREDFLAG = -LD
	override OFLAG = -Fo./

#	override DEFFLAG = oscar.def
endif

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c $(OFLAG)$@ $< $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)
	

mybot:$(OBJ)
	$(CXX) $(SHAREDFLAG) $(OFLAG)../bin/$(BINNAME) $(OBJ) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

.PHONY: install

install:
	cp  ../bin/$(BINNAME) $(INSTALLPATH)/$(BINNAME)
	cp -r ../bin/read/* $(INSTALLPATH)/../read/
	cp -r ../bin/write/* $(INSTALLPATH)/../write/