#include "think.h"

THINK_THREADMUTEX *threadmutex;

void mprintf(const char *fmt,...)
{
	va_list args;
	
	think_threadmutex_lock(threadmutex,-1);
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	think_threadmutex_unlock(threadmutex);
}

void func1(void *s)
{
	const char *str;
	int i;

	str=s;
	for(i=0;i<15;i++){
		think_os_sleep(1);
		mprintf("func1 str=%s\n",str);
	}
	think_threadexit(1);
}
void func2(void *s)
{
	const char *str;
	int i;

	str=s;
	for(i=0;i<20;i++){
		think_os_sleep(1);
		mprintf("func2 str=%s\n",str);
	}
	think_threadexit(9);
}

int main(int argc,char *argv[])
{
	THINK_THREAD *thread1,*thread2;
	int exitcode,r;

	if((threadmutex=think_threadmutex_open())==NULL){
		think_showerror();
		return -1;
	}
	if((thread1=think_threadcreate(func1,"thread1"))==NULL){
		think_showerror();
		return -1;
	}
	if((thread2=think_threadcreate(func2,"thread2"))==NULL){
		think_showerror();
		return -1;
	}

	think_os_sleep(10);
	mprintf("waiting for thread1 to exit ...\n");
	if((r=think_threadjoin(thread1,&exitcode,20*1000))<0){
		think_showerror();
		return -1;
	}
	if(r==0)
		mprintf("wait thread1 timeout.");
	else
		mprintf("thread1 exit %d\n",exitcode);

	mprintf("waiting for thread2 to exit ...\n");
	if((r=think_threadjoin(thread2,&exitcode,25*1000))<0){
		think_showerror();
		return -1;
	}
	if(r==0)
		mprintf("wait thread2 timeout.");
	else
		mprintf("thread2 exit %d\n",exitcode);

	think_threadmutex_close(threadmutex);

	return 0;
}
