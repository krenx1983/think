ThinkHome=../..
CC=cc
CFLAGS=-g -I. -I$(ThinkHome)/include -D__THINK_WINDOWS2UNIX__
LDFLAGS=-L. -L$(ThinkHome)/lib
LDLIBS=-lthink# -lsocket -lnsl -lresolv -lrt
AR=
ARFLAGS=
INCLUDES=
LIBS=
APPS=
TOOLS=showcfg showconf logsend showerr dbfstat dbfshow dbfinsert dbfcreate dbfload getconf getcfg logview dbstat dbshow dbinsert dbcreate dbload shmqcreate shmqget shmqput shmqstat shmqrm glpeek netpeek hello udpsend udprecv
SHELLS=kill.sh wtou.sh

all:$(LIBS) $(APPS) $(TOOLS)
showerr:showerr.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
dbstat:dbstat.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
dbshow:dbshow.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
dbcreate:dbcreate.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
dbinsert:dbinsert.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
dbload:dbload.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
dbfstat:dbfstat.o
	$(CC) $(LDFLAGS) -o $@ $? -lthink_dbf $(LDLIBS)
dbfshow:dbfshow.o
	$(CC) $(LDFLAGS) -o $@ $? -lthink_dbf $(LDLIBS)
dbfcreate:dbfcreate.o
	$(CC) $(LDFLAGS) -o $@ $? -lthink_dbf $(LDLIBS)
dbfinsert:dbfinsert.o
	$(CC) $(LDFLAGS) -o $@ $? -lthink_dbf $(LDLIBS)
dbfload:dbfload.o
	$(CC) $(LDFLAGS) -o $@ $? -lthink_dbf $(LDLIBS)
getconf:getconf.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
getcfg:getcfg.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
showcfg:showcfg.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
showconf:showconf.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
logsend:logsend.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
logview:logview.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
shmqcreate:shmqcreate.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
shmqget:shmqget.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
shmqput:shmqput.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
shmqstat:shmqstat.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
shmqrm:shmqrm.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
glpeek:glpeek.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
netpeek:netpeek.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
hello:hello.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS) -lpthread
udprecv:udprecv.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
udpsend:udpsend.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
clean:
	rm -f ? *.o *core
	rm -f $(LIBS) $(APPS) $(TOOLS)
install:
	cp -f $(SHELLS) $(TOOLS) $(ThinkHome)/bin
update:
