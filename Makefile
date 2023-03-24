CC=gcc
CXX=g++
CFLAGS=-I$(IDIR)
CPPFLAGS=-I$(IDIR) -ggdb -fPIC
ODIR=obj
IDIR =../include -I../
LDFLAGS=-L../lib/
LDLIBS=-l:libBWAPILIB.so -l:libBWAPIClient.a -l:libBWEBlib.a
DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
_DEPS=
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ=BuildOrder.o Oscar.o main.o Dll.o MapTools.o ReplayParser.o Tools.o Macro.o Decider.o OscarMap.o Micro.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

mybot:$(OBJ)
	$(CXX) -shared -o mybot.so $(OBJ) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.cpp=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

.PHONY: install

install:
	cp ./mybot.so /home/sean/development/bwapi/build/bin/bwapi-data/AI/mybot.so
	cp -r ./read/* /home/sean/development/bwapi/build/bin/bwapi-data/read/
	cp -r ./write/* /home/sean/development/bwapi/build/bin/bwapi-data/write/