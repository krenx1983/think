ThinkHome=../..
CC=cc
CFLAGS=-g -I. -I$(ThinkHome)/include
LDFLAGS=-L. -L$(ThinkHome)/lib
LDLIBS=-lthink# -lsocket -lnsl -lresolv -lrt
AR=
ARFLAGS=
INCLUDES=
LIBS=
APPS=
TOOLS=threadtest gltest
SHELLS=

all:$(LIBS) $(APPS) $(TOOLS)
threadtest:threadtest.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS) -lpthread
gltest:gltest.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
clean:
	rm -f ? *.o *core
	rm -f $(LIBS) $(APPS) $(TOOLS)
install:
	cp -f $(SHELLS) $(TOOLS) $(ThinkHome)/bin
update:
