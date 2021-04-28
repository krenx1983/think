INSTALL_PATH=../..
CC=cc
CFLAGS=-g -I$(INSTALL_PATH)/include
LDFLAGS=-L$(INSTALL_PATH)/lib
LDLIBS=-lthink# -lpthread -lsocket -lnsl -lresolv -lrt
AR=
ARFLAGS=
INCLUDES=
LIBS=
APPS=logserver
all:$(APPS)
logserver:logserver.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
clean:
	rm -f ? *.o *core *~
	rm -f $(APPS)
install:
	cp -f $(APPS) $(INSTALL_PATH)/bin
