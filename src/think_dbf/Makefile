ThinkHome=../..
CC=cc
CFLAGS=-g -I. -I$(ThinkHome)/include -D __THINK_WINDOWS2UNIX__
LDFLAGS=-L. -L$(ThinkHome)/lib
LDLIBS=-lthink_dbf -lthink# -lsocket -lnsl -lresolv -lrt
AR=ar
ARFLAGS=r
INCLUDES=think_dbf.h
LIBS=libthink_dbf.a
APPS=

all:$(LIBS) $(APPS)
libthink_dbf.a:think_dbf.o
	$(AR) $(ARFLAGS) $@ $?
clean:
	rm -f ? *.o *core
	rm -f $(LIBS) $(APPS)
install:
	cp -f $(INCLUDES) $(ThinkHome)/include
	cp -f $(LIBS) $(ThinkHome)/lib
update:
