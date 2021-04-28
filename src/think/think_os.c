#include "think_os.h"
#include "think_utility.h"

#ifdef __THINK_WINDOWS__
int opterr=1,optind=1,optopt,optreset;
char *optarg;
#endif	/* __THINK_WINDOWS__ */

#ifdef __THINK_WINDOWS__
THINK_ULONG l1=0x00000000000000FFU;
THINK_ULONG l2=0x000000000000FF00U;
THINK_ULONG l3=0x0000000000FF0000U;
THINK_ULONG l4=0x00000000FF000000U;
THINK_ULONG l5=0x000000FF00000000U;
THINK_ULONG l6=0x0000FF0000000000U;
THINK_ULONG l7=0x00FF000000000000U;
THINK_ULONG l8=0xFF00000000000000U;
#else
THINK_ULONG l1=0x00000000000000FFULL;
THINK_ULONG l2=0x000000000000FF00ULL;
THINK_ULONG l3=0x0000000000FF0000ULL;
THINK_ULONG l4=0x00000000FF000000ULL;
THINK_ULONG l5=0x000000FF00000000ULL;
THINK_ULONG l6=0x0000FF0000000000ULL;
THINK_ULONG l7=0x00FF000000000000ULL;
THINK_ULONG l8=0xFF00000000000000ULL;
#endif	/* __THINK_WINDOWS__ */

#ifdef __THINK_WINDOWS__
int getopt(int argc, char *argv[], const char *ostr)
{
	static char *place = "";		/* option letter processing */
	char *oli;				/* option letter list index */

	if (optreset || !*place) {		/* update scanning pointer */
		optreset = 0;
		if (optind >= argc || *(place = argv[optind]) != '-') {
			place = "";
			return (EOF);
		}
		if (place[1] && *++place == '-') {	/* found "--" */
			++optind;
			place = "";
			return (EOF);
		}
	}					/* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' ||
	    !(oli = strchr(ostr, optopt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means EOF.
		 */
		if (optopt == (int)'-')
			return (EOF);
		if (!*place)
			++optind;
		return ('?');
	}
	if (*++oli != ':') {			/* don't need argument */
		optarg = NULL;
		if (!*place)
			++optind;
	}
	else {					/* need an argument */
		if (*place)			/* no white space */
			optarg = place;
		else if (argc <= ++optind) {	/* no arg */
			place = "";
			if (*ostr == ':')
				return (':');
			return ('?');
		}
		else				/* white space */
			optarg = argv[optind];
		place = "";
		++optind;
	}
	return (optopt);			/* dump back option letter */
}
const char *think_strerror(int e)
{
	static char errmsg[1024]={'\0'};
	
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,e,MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US),(LPSTR)errmsg,sizeof(errmsg)-1,NULL);
	think_strtrim(errmsg);

	return errmsg;
}
#endif	/* __THINK_WINDOWS__ */

#ifdef __THINK_UNIX__
int think_os_msleep(unsigned int t)
{
	int secs,msecs;

	secs=t/1000;
	msecs=t%1000;
	if(secs>0)
		sleep(secs);
	if(msecs>0)
		usleep(msecs*1000);

	return 0;
}
#endif

#ifdef __THINK_WINDOWS__
int think_os_usleep(unsigned int t)
{
	t/=1000;
	if(t==0)
		t=1;
	Sleep(t);

	return 0;
}
#endif
