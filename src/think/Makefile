ThinkHome=../..
CC=cc
CFLAGS=-g -I. -I$(ThinkHome)/include -D __THINK_WINDOWS2UNIX__
LDFLAGS=-L. -L$(ThinkHome)/lib
LDLIBS=-lthink# -lsocket -lnsl -lresolv -lrt
AR=ar
ARFLAGS=r
INCLUDES=think.h
LIBS=libthink.a
APPS=

all:$(LIBS) $(APPS)
libthink.a:think_os.o think_error.o think_utility.o think_file.o think_cfg.o think_conf.o think_db.o think_net.o think_gl.o think_log.o think_netcenter.o think_timer.o think_timercenter.o think_sem.o think_shm.o think_thread.o think_threadmutex.o think_mutex.o think_shmq.o
	$(AR) $(ARFLAGS) $@ $?
handtest:handtest.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
clean:
	rm -f ? *.o *.out *core *~
	rm -f $(LIBS) $(APPS)
install:
	cp -f $(INCLUDES) $(ThinkHome)/include
	cp -f $(LIBS) $(ThinkHome)/lib
update:
