#include "think_error.h"

int __think_errno__=0;
char __think_errmsg__[THINK_ERROR_ERRMSG_SIZE]={'\0'};

void think_error(unsigned int e,const char *fmt,...)
{
	va_list args;
	char errmsg[THINK_ERROR_ERRMSG_SIZE];

	va_start(args,fmt);
	vsnprintf(errmsg,sizeof(errmsg),fmt,args);
	va_end(args);

	__think_errno__=e;
	strcpy(__think_errmsg__,errmsg);
}
void think_showerror(void)
{
	printf("%d:%s\n",__think_errno__,__think_errmsg__);
}
