CC=gcc
CXX=g++
CFLAGS=-I$(IDIR)
CPPFLAGS=-I$(IDIR) -ggdb -fPIC
ODIR=obj
IDIR =../include
LDFLAGS=-L../lib
LDLIBS=-l:libBWAPILIB.so -l:libBWAPIClient.a -l:libBWEBlib.a
DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
_DEPS=
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ=BuildOrder.o Oscar.o main.o Dll.o Tools.o Macro.o Decider.o OscarMap.o Micro.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
VPATH = src:src/Map
INSTALLPATH= /home/sean/development/bwapi/build/bin/bwapi-data/AI/
BINNAME=oscar.so
LINKER =g++
LINKERFLAGS =-shared -o

#windows changes
ifeq ($(OS),Windows_NT)
	override CPPFLAGS = -I$(IDIR) -ggdb -fPIC -mabi=ms -m32
	override INSTALLPATH = /c/Users/sean/BWAPI/Starcraft/bwapi-data/AI/
	override LDLIBS =  -l:libBWEM.a -l:libBWAPIClient.a -l:libBWAPILIB.a
	override BINNAME = oscar.dll
	override LINKER = dlltool
	override LINKERFLAGS = 
endif

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CPPFLAGS)
	
$(ODIR)/%.o: Map/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CPPFLAGS)

mybot:$(OBJ)
	$(LINKER) $(LINKERFLAGS) ../bin/$(BINNAME) $(OBJ) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

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
	cp -r ../bin/read/* $(INSTALLPATH)/read/
	cp -r ../bin/write/* $(INSTALLPATH)/write/