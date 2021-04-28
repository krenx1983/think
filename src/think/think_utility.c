#include "think_error.h"
#include "think_utility.h"

char *think_strtrim(char *str)
{
	char *p,*t;

	t=str;
	if(strlen(str)==0)/* already no spaces */
		return str;

	/* trim left spaces */
	p=str;/* use a move pointer */
	while(*p!='\0'){
		if(!(*p==' ' || *p=='\t' || *p=='\n' || *p=='\r' || *p=='\v' || *p=='\f')){
			break;
		}
		p++;
	}
	if(*p=='\0')/* every char in str is space */
		*str='\0';
	else{
		if(p!=str){/* has spaces */
			for(;*p!='\0';)
				*str++=*p++;
			*str='\0';
		}/* else has none spaces */
	}
	str=t;

	if(strlen(str)==0)/* already no spaces */
		return str;

	/* trim right spaces */
	p=str+(strlen(str)-1);/* point to the last char */
	while(p>=str){
		if(!(*p==' ' || *p=='\t' || *p=='\n' || *p=='\r' || *p=='\v' || *p=='\f')){
			break;
		}
		p--;
	}
	*(p+1)='\0';

	return str;
}
char *think_strreplace(char *srcstr,char *pattern,char *repstr)
{
	char *p,*q;

	p=srcstr;
	while(*p!='\0'){
		if((q=strstr(p,pattern))==NULL)
			break;
		memmove(q+strlen(repstr),q+strlen(pattern),strlen(q+strlen(pattern))+1);
		memcpy(q,repstr,strlen(repstr));
		p=q+strlen(repstr);
	}

	return srcstr;
}
char *think_chrreplace(char *srcstr,char pattern,char repstr)
{
	char tmp1[2],tmp2[2];

	tmp1[0]=pattern;
	tmp1[1]='\0';
	tmp2[0]=repstr;
	tmp2[1]='\0';

	return think_strreplace(srcstr,tmp1,tmp2);
}
char *think_strinsert(char *srcstr,int pos,char *insstr)
{
	memmove(srcstr+pos+strlen(insstr),srcstr+pos,strlen(srcstr+pos)+1);
	memcpy(srcstr+pos,insstr,strlen(insstr));

	return srcstr;
}
char *think_chrinsert(char *srcstr,int pos,char insstr)
{
	memmove(srcstr+pos+1,srcstr+pos,strlen(srcstr+pos)+1);
	*(srcstr+pos)=insstr;

	return srcstr;
}
int think_chrfind(const char *str,char chr,int index)
{
	const char *p,*start,*end;
	int pos,shift,flag;

	if(index>=0){
		start=str;
		end=str+strlen(str);
		shift=1;
	}else{
		start=strlen(str)==0?str:str+strlen(str)-1;
		end=str-1;
		shift=-1;
		index=-1*index-1;
	}
	flag=0;
	p=start;
	while(p!=end){
		if(*p!=chr){
			p+=shift;
			continue;
		}
		index--;
		if(index!=-1){
			p+=shift;
			continue;
		}
		flag=1;
		break;
	}
	if(flag!=1){
		return -1;
	}
	pos=p-str;

	return pos;
}
/*
 * strgetfield()
 * format:---|-----|----|--------|---
 */

int think_strgetfield(const char *str,char sepchr,int index,char type,void *value,unsigned int size,int flags)
{
	int ppos,qpos,len=0;
	char fieldvalue[128];
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if(index>=0){
		if(index==0){
			ppos=0;
			if((qpos=think_chrfind(str,sepchr,0))<0)
				len=strlen(str);
			else
				len=qpos-ppos;
		}else{
			if((ppos=think_chrfind(str,sepchr,index-1))<0){
				think_error(0,"[%s]:field not exists.[index=%d]",__func__,index);
				return -1;
			}else{
				ppos++;
				if((qpos=think_chrfind(str+ppos,sepchr,0))<0)
					len=strlen(str+ppos);
				else
					len=qpos;
			}
		}
	}else{
		if(index==-1){
			if((ppos=think_chrfind(str,sepchr,-1))<0){
				ppos=0;
				len=strlen(str);
			}else{
				ppos++;
				len=strlen(str+ppos);
			}
		}else{
			if((qpos=think_chrfind(str,sepchr,index+1))<0){
				think_error(0,"[%s]:field not exists.[index=%d]",__func__,index);
				return -1;
			}else{
				if((ppos=think_chrfind(str,sepchr,index))<0){
					ppos=0;
					len=qpos;
				}else{
					ppos++;
					len=qpos-ppos;
				}
			}
		}
	}
	if(len==0 && (flags & THINK_STR_NOT_NULL)){
		think_error(0,"[%s]:field is empty.[index=%d]",__func__,index);
		return -1;
	}
	switch(type){
		case THINK_STR_CHAR:		/* char */
			if(len>size){
				think_error(0,"[%s]:field too long![index=%d][field=%*.*s]",__func__,index,len,len,str+ppos);
				return -1;
			}
			memcpy(value,str+ppos,len);
			if(flags & THINK_STR_NOT_STRING)
				return len;
			if(len==size){
				think_error(0,"[%s]:field value equal to size![index=%d][field=%*.*s]",__func__,index,len,len,str+ppos);
				return -1;
			}
			*((char *)value+len)='\0';
			if(!(flags & THINK_STR_NOT_TRIM))
				think_strtrim(value);
			if(strlen(value)==0 && (flags & THINK_STR_NOT_NULL)){
				think_error(0,"[%s]:field is empty.[index=%d]",__func__,index);
				return -1;
			}
			return strlen(value);
		case THINK_STR_SHORT:		/* short */
			if(len>=sizeof(fieldvalue)){
				think_error(0,"[%s]:field too long![index=%d][field=%*.*s]",__func__,index,len,len,str+ppos);
				return -1;
			}
			memcpy(fieldvalue,str+ppos,len);
			*((char *)fieldvalue+len)='\0';
			t_short=atol(fieldvalue);
			if(t_short==0 && (flags & THINK_STR_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[index=%d]",__func__,index);
				return -1;
			}
			memcpy(value,&t_short,2);
			break;
		case THINK_STR_INT:		/* int */
			if(len>=sizeof(fieldvalue)){
				think_error(0,"[%s]:field too long![index=%d][field=%*.*s]",__func__,index,len,len,str+ppos);
				return -1;
			}
			memcpy(fieldvalue,str+ppos,len);
			*((char *)fieldvalue+len)='\0';
			t_int=atol(fieldvalue);
			if(t_int==0 && (flags & THINK_STR_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[index=%d]",__func__,index);
				return -1;
			}
			memcpy(value,&t_int,4);
			break;
		case THINK_STR_LONG:		/* long */
			if(len>=sizeof(fieldvalue)){
				think_error(0,"[%s]:field too long![index=%d][field=%*.*s]",__func__,index,len,len,str+ppos);
				return -1;
			}
			memcpy(fieldvalue,str+ppos,len);
			*((char *)fieldvalue+len)='\0';
			t_long=atoll(fieldvalue);
			if(t_long==0 && (flags & THINK_STR_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[index=%d]",__func__,index);
				return -1;
			}
			memcpy(value,&t_long,8);
			break;
		case THINK_STR_FLOAT:		/* float */
			if(len>=sizeof(fieldvalue)){
				think_error(0,"[%s]:field too long![index=%d][field=%*.*s]",__func__,index,len,len,str+ppos);
				return -1;
			}
			memcpy(fieldvalue,str+ppos,len);
			*((char *)fieldvalue+len)='\0';
			t_float=atof(fieldvalue);
			if(t_float==0.0 && (flags & THINK_STR_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[index=%d]",__func__,index);
				return -1;
			}
			memcpy(value,&t_float,4);
			break;
		case THINK_STR_DOUBLE:		/* double */
			if(len>=sizeof(fieldvalue)){
				think_error(0,"[%s]:field too long![index=%d][field=%*.*s]",__func__,index,len,len,str+ppos);
				return -1;
			}
			memcpy(fieldvalue,str+ppos,len);
			*((char *)fieldvalue+len)='\0';
			t_double=atof(fieldvalue);
			if(t_double==0.0 && (flags & THINK_STR_NOT_NULL)){
				think_error(0,"[%s]:field value is 0.[index=%d]",__func__,index);
				return -1;
			}
			memcpy(value,&t_double,8);
			break;
		default:
			think_error(0,"[%s]:illegal field type.[index=%d][type='%c']",__func__,index,type);
			return -1;
	}

	return 0;
}
/**********************************************
 *int think_chrstat(const char *str,char ch);
 **********************************************/
int think_chrstat(const char *str,char ch)
{
    int n=0;

    while(*str!='\0'){
        if(*str==ch)
            n++;
        str++;
    }

    return n;
}
int think_strstat(const char *str,const char *pattern)
{
	int n=0;

	while(str=strstr(str,pattern)){
		n++;
		str+=strlen(pattern);
	}
	return n;
}
int think_bin_to_hex(char *dest,const char *src,int len)
{
	int i;

	for(i=0;i<len;i++){
		if(think_bintohex(dest,*src)<0)
			return -1;
		src++;
		dest+=2;
	}
	*dest='\0';

	return 0;
}
int think_bin_to_bcd(char *dest,const char *src,int len)
{
	int i;

	for(i=0;i<len;i++){
		if(think_bintobcd(dest,*src)<0)
			return -1;
		src++;
		dest+=2;
	}
	*dest='\0';

	return 0;
}
int think_hex_to_bin(char *dest,const char *src,int len)
{
	int i;

	if(len%2 != 0){
		think_error(0,"hex string length not equal to 2x![len=%d]",len);
		return -1;
	}
	for(i=0;i<len/2;i++){
		if(think_hextobin(dest,src)<0)
			return -1;
		dest++;
		src+=2;
	}

	return 0;
}
int think_bcd_to_bin(char *dest,const char *src,int len)
{
	int i;

	if(len%2 != 0){
		think_error(0,"bcd string length not equal to 2x![len=%d]",len);
		return -1;
	}
	for(i=0;i<len/2;i++){
		if(think_bcdtobin(dest,src)<0)
			return -1;
		dest++;
		src+=2;
	}

	return 0;
}
int think_hextobin(char *dest,const char *src)
{
	int firstbyte,lastbyte;
	
	if((firstbyte=think_hextoint(src[0]))<0)
		return -1;
	if((lastbyte=think_hextoint(src[1]))<0)
		return -1;
	*dest=(firstbyte << 4) | (lastbyte & 15);

	return 0;
}
int think_bcdtobin(char *dest,const char *src)
{
	int firstbyte,lastbyte;
	
	if((firstbyte=think_bcdtoint(src[0]))<0)
		return -1;
	if((lastbyte=think_bcdtoint(src[1]))<0)
		return -1;
	*dest=(firstbyte << 4) | (lastbyte & 15);

	return 0;
}
int think_bintohex(char *dest,char src)
{
	int firstbyte,lastbyte,t;

	firstbyte=src>>4;
	firstbyte&=15;
	if((t=think_inttohex(firstbyte))<0)
		return -1;
	*dest=t;
	
	dest++;
	lastbyte=src&15;
	if((t=think_inttohex(lastbyte))<0)
		return -1;
	*dest=t;

	return 0;
}
int think_bintobcd(char *dest,char src)
{
	int firstbyte,lastbyte,t;

	firstbyte=src>>4;
	firstbyte&=15;
	if((t=think_inttobcd(firstbyte))<0)
		return -1;
	*dest=t;

	dest++;
	lastbyte=src&15;
	if((t=think_inttobcd(lastbyte))<0)
		return -1;
	*dest=t;

	return 0;
}
int think_inttohex(int bin)
{
	int result;

	if(bin>=0 && bin<=9){
		result='0'+bin;
	}else if(bin>=10 && bin<=15){
		result='A'+(bin-10);
	}else{
		think_error(0,"[%s]:wrong hex value![%d]",__func__,bin);
		return -1;
	}

	return result;
}
int think_inttobcd(int bin)
{
	int result;

	if(bin>=0 && bin<=9){
		result='0'+bin;
	}else{
		think_error(0,"[%s]:wrong bcd value![%d]",__func__,bin);
		return -1;
	}

	return result;
}
int think_hextoint(int hex)
{
	int result;

	if(hex>=48 && hex<=57){
		result=hex-'0';
	}else if(hex>=97 && hex<=102){
		result=hex-'a'+10;
	}else if(hex>=65 && hex<=90){
		result=hex-'A'+10;
	}else{
		think_error(0,"[%s]:invalid hex character![%c]",__func__,hex);
		return -1;
	}

	return result;
}
int think_bcdtoint(int hex)
{
	int result;

	if(hex>=48 && hex<=57){
		result=hex-'0';
	}else{
		think_error(0,"[%s]:invalid bcd character![%c]",__func__,hex);
		return -1;
	}

	return result;
}

int think_printbin(const char *buf,const int len,int linesize)
{
	int i;
	int linepos;
	int linechangeflag;
	int printway;
	const char *p;
	char tmpbuf[3];
	int r;

	if(len==0)
		return 0;
	if(linesize==0)
		linechangeflag=0;
	else
		linechangeflag=1;
	
	if(linesize>=0){
		printway=0;
		p=buf;
	}else{
		printway=1;
		p=buf+len-1;
	}
	linepos=0;
	linesize=abs(linesize);
	for(i=0;i<len;i++){
		if(linechangeflag==1){
			if(linepos==linesize){
				printf("\n");
				linepos=0;
			}
		}
		linepos++;
		bzero(tmpbuf,sizeof(tmpbuf));
		r=think_bintohex(tmpbuf,*p);
		if(r<0){
			return -1;
		}
		printf("%s ",tmpbuf);
		if(printway==1)
			p--;
		else
			p++;
	}
	printf("\n");
	return 0;
}
/*****************************************************
 *int think_bitset(char *bitmap,int bit);
 *****************************************************/
int think_bitset(char *bitmap,int bit)
{
	bit--;
	bitmap[bit/8] |= (0x01<<(7-bit%8));
	return 0;
}

/*****************************************************
 *int think_bittest(const char *bitmap,int bit);
 *****************************************************/
int think_bittest(const char *bitmap,int bit)
{
	int existflag;
	bit--;
	existflag=bitmap[bit/8] & (0x01<<(7-bit%8));
	return existflag;
}

/*****************************************************
 *int think_bitclear(char *bitmap,int bit);
 *****************************************************/
int think_bitclear(char *bitmap,int bit)
{
	bit--;
	bitmap[bit/8]&=~(0x01<<(7-bit%8));
	return 0;
}
int think_getopt(int argc,char *argv[],char opt,char type,void *value,unsigned int size,int flags)
{
	char optstring[32];
	int c;
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if(flags & THINK_OPT_ARG)
		snprintf(optstring,sizeof(optstring),":%c:",opt);
	else
		snprintf(optstring,sizeof(optstring),":%c",opt);
	optind=1;
	while((c=getopt(argc,argv,optstring))!=EOF){
		if(c==opt){
			if(!(flags & THINK_OPT_ARG))
				return 0;
			if(strlen(optarg)==0 && (flags & THINK_OPT_NOT_NULL)){
				think_error(0,"[%s]:option is be empty. -- [-%c]",__func__,opt);
				return -1;
			}
			switch(type){
				case THINK_OPT_CHAR:	/* char */
					if(strlen(optarg)>size){
						think_error(0,"[%s]:option value too long.[size=%d][len=%d] -- [-%c]",__func__,size,strlen(optarg),opt);
						return -1;
					}
					if(strlen(optarg)==size && !(flags & THINK_OPT_NOT_STRING)){
						think_error(0,"[%s]:option value size equal to len.[size=%d] -- [-%c]",__func__,size,opt);
						return -1;
					}
					strncpy(value,optarg,size);
					if(flags & THINK_OPT_NOT_STRING)
						return strlen(optarg);
					if(!(flags & THINK_OPT_NOT_TRIM))
						think_strtrim(value);
					if(strlen(value)==0 && (flags & THINK_OPT_NOT_NULL)){
						think_error(0,"[%s]:option is empty. -- [-%c]",__func__,opt);
						return -1;
					}
					return strlen(value);
				case THINK_OPT_SHORT:	/* short */
					t_short=atol(optarg);
					if(t_short==0 && (flags & THINK_OPT_NOT_NULL)){
						think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,opt);
						return -1;
					}
					memcpy(value,&t_short,2);
					break;
				case THINK_OPT_INT:	/* int */
					t_int=atol(optarg);
					if(t_int==0 && (flags & THINK_OPT_NOT_NULL)){
						think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,opt);
						return -1;
					}
					memcpy(value,&t_int,4);
					break;
				case THINK_OPT_LONG:	/* long */
					t_long=atoll(optarg);
					if(t_long==0 && (flags & THINK_OPT_NOT_NULL)){
						think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,opt);
						return -1;
					}
					memcpy(value,&t_long,8);
					break;
				case THINK_OPT_FLOAT:	/* float */
					t_float=atof(optarg);
					if(t_float==0.0 && (flags & THINK_OPT_NOT_NULL)){
						think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,opt);
						return -1;
					}
					memcpy(value,&t_float,4);
					break;
				case THINK_OPT_DOUBLE:	/* double */
					t_double=atof(optarg);
					if(t_double==0.0 && (flags & THINK_OPT_NOT_NULL)){
						think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,opt);
						return -1;
					}
					memcpy(value,&t_double,8);
					break;
				default:
					think_error(0,"[%s]:illegal type. -- [-%c][type=%c]",__func__,opt,type);
					return -1;
			}
			return 0;
		}
		switch(c){
			case ':':
				think_error(0,"[%s]:option need an argument. -- [-%c]",__func__,optopt);
				return -1;
			case '?':
				think_error(0,"[%s]:illegal option -- [-%c]",__func__,optopt);
				return -1;
			default:
				think_error(0,"[%s]:getopt error.",__func__);
				return -1;
		}
	}
	if(flags & THINK_OPT_NOT_IGNORE){
		think_error(0,"[%s]:option not exists -- [-%c]",__func__,opt);
		return -1;
	}

	return 0;
}
int think_getopts(int argc,char *argv[],THINK_OPTARRAY *optarray,unsigned int optcount)
{
	THINK_OPTARRAY *p;
	char optstring[1024],*sp;
	int c,i;
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	bzero(optstring,sizeof(optstring));
	sp=optstring;
	*sp++=':';
	for(i=0,p=optarray;i<optcount;i++,p++){
		*sp++=p->opt;
		if(p->flags & THINK_OPT_ARG)
			*sp++=':';
		if(p->exists)
			*(p->exists)=0;	/* reset to not exists */
	}
	optind=1;
	while((c=getopt(argc,argv,optstring))!=EOF){
		for(i=0,p=optarray;i<optcount;i++,p++){
			if(p->opt==c){
				if(p->exists)
					*(p->exists)=1;	/* exists */
				if(p->flags & THINK_OPT_ARG){
					if(strlen(optarg)==0){
						if(p->flags & THINK_OPT_NOT_NULL){
							think_error(0,"[%s]:option can't be empty. -- [-%c]",__func__,c);
							return -1;
						}
					}
					switch(p->type){
						case THINK_OPT_CHAR:	/* char */
							if(strlen(optarg)>p->size){
								think_error(0,"[%s]:option value too long.[size=%d][len=%d] -- [-%c]",__func__,p->size,strlen(optarg),c);
								return -1;
							}
							if(strlen(optarg)==p->size && !(p->flags & THINK_OPT_NOT_STRING)){
								think_error(0,"[%s]:option value size equal to len.[size=%d] -- [-%c]",__func__,p->size,c);
								return -1;
							}
							strncpy(p->value,optarg,p->size);
							if(p->flags & THINK_OPT_NOT_STRING)
								break;
							if(!(p->flags & THINK_OPT_NOT_TRIM))
								think_strtrim(p->value);
							if(strlen(p->value)==0 && (p->flags & THINK_OPT_NOT_NULL)){
								think_error(0,"[%s]:option is empty. -- [-%c]",__func__,c);
								return -1;
							}
							break;
						case THINK_OPT_SHORT:	/* short */
							t_short=atol(optarg);
							if(t_short==0 && (p->flags & THINK_OPT_NOT_NULL)){
								think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,c);
								return -1;
							}
							memcpy(p->value,&t_short,2);
							break;
						case THINK_OPT_INT:	/* int */
							t_int=atol(optarg);
							if(t_int==0 && (p->flags & THINK_OPT_NOT_NULL)){
								think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,c);
								return -1;
							}
							memcpy(p->value,&t_int,4);
							break;
						case THINK_OPT_LONG:	/* long */
							t_long=atoll(optarg);
							if(t_long==0 && (p->flags & THINK_OPT_NOT_NULL)){
								think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,c);
								return -1;
							}
							memcpy(p->value,&t_long,8);
							break;
						case THINK_OPT_FLOAT:	/* float */
							t_float=atof(optarg);
							if(t_float==0.0 && (p->flags & THINK_OPT_NOT_NULL)){
								think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,c);
								return -1;
							}
							memcpy(p->value,&t_float,4);
							break;
						case THINK_OPT_DOUBLE:	/* double */
							t_double=atof(optarg);
							if(t_double==0.0 && (p->flags & THINK_OPT_NOT_NULL)){
								think_error(0,"[%s]:option value is 0. -- [-%c]",__func__,c);
								return -1;
							}
							memcpy(p->value,&t_double,8);
							break;
						default:
							think_error(0,"[%s]:illegal type. -- [-%c][type=%c]",__func__,c,p->type);
							return -1;
					}
				}
				break;
			}
		}
		if(i==optcount){
			switch(c){
				case ':':
					think_error(0,"[%s]:option need an argument. -- [-%c]",__func__,optopt);
					return -1;
				case '?':
					think_error(0,"[%s]:illegal option -- [-%c]",__func__,optopt);
					return -1;
				default:
					think_error(0,"[%s]:getopt error.",__func__);
					return -1;
			}
		}
	}
	for(i=0,p=optarray;i<optcount;i++,p++){
		if((p->flags & THINK_OPT_NOT_IGNORE) && (!*p->exists)){
			think_error(0,"[%s]:option not exists -- [-%c]",__func__,p->opt);
			return -1;
		}
	}

	return 0;	/* count of args */
}
int think_getenv(const char *name,char type,void *value,int size,int flags)
{
	const char *p;
	THINK_SHORT t_short;
	THINK_INT t_int;
	THINK_LONG t_long;
	float t_float;
	double t_double;

	if((p=getenv(name))==NULL){
		if(flags & THINK_ENV_NOT_IGNORE){
			think_error(0,"[%s]:env not set.[name=%s]",__func__,name);
			return -1;
		}
		return 0;
	}
	if(strlen(p)==0 && (flags & THINK_ENV_NOT_NULL)){
		think_error(0,"[%s]:env is empty.[name=%s]",__func__,name);
		return -1;
	}
	switch(type){
		case THINK_ENV_CHAR:	/* char */
			if(strlen(p)>size){
				think_error(0,"[%s]:env too long.[size=%d][len=%d][name=%s][value=%s]",__func__,size,strlen(p),name,p);
				return -1;
			}
			if(strlen(p)==size){
				think_error(0,"[%s]:env value len equal to size.[size=%d][name=%s][value=%s]",__func__,size,name,p);
				return -1;
			}
			strncpy(value,p,size);
			if(flags & THINK_ENV_NOT_STRING)
				return strlen(p);
			if(!(flags & THINK_ENV_NOT_TRIM))
				think_strtrim(value);
			if(strlen(p)==0 && (flags & THINK_ENV_NOT_NULL)){
				think_error(0,"[%s]:env is empty.[name=%s]",__func__,name);
				return -1;
			}
			return strlen(p);
		case THINK_ENV_SHORT:	/* short */
			t_short=atol(p);
			if(t_short==0 && (flags & THINK_ENV_NOT_NULL)){
				think_error(0,"[%s]:env value is 0.[name=%s]",__func__,name);
				return -1;
			}
			memcpy(value,&t_short,2);
			break;
		case THINK_ENV_INT:	/* int */
			t_int=atol(p);
			if(t_int==0 && (flags & THINK_ENV_NOT_NULL)){
				think_error(0,"[%s]:env value is 0.[name=%s]",__func__,name);
				return -1;
			}
			memcpy(value,&t_int,4);
			break;
		case THINK_ENV_LONG:	/* long */
			t_long=atoll(p);
			if(t_long==0 && (flags & THINK_ENV_NOT_NULL)){
				think_error(0,"[%s]:env value is 0.[name=%s]",__func__,name);
				return -1;
			}
			memcpy(value,&t_long,8);
			break;
		case THINK_ENV_FLOAT:	/* float */
			t_float=atof(p);
			if(t_float==0.0 && (flags & THINK_ENV_NOT_NULL)){
				think_error(0,"[%s]:env value is 0.[name=%s]",__func__,name);
				return -1;
			}
			memcpy(value,&t_float,4);
			break;
		case THINK_ENV_DOUBLE:	/* double */
			t_double=atof(p);
			if(t_double==0.0 && (flags & THINK_ENV_NOT_NULL)){
				think_error(0,"[%s]:env value is 0.[name=%s]",__func__,name);
				return -1;
			}
			memcpy(value,&t_double,8);
			break;
		default:
			think_error(0,"[%s]:illegal type![name=%s][type=%c]",__func__,name,type);
			return -1;
	}

	return 0;
}
int think_getenvs(THINK_ENVARRAY *envarray,unsigned int envcount)
{
	THINK_ENVARRAY *p;
	int i;
	const char *value;

	/* init */
	for(i=0,p=envarray;i<envcount;i++,p++){
		if(p->exists)
			*(p->exists)=0;	/* reset to not exists */
	}

	p=envarray;
	for(i=0;i<envcount;i++,p++){
		if((value=getenv(p->name))==NULL){
			if(p->flags & THINK_ENV_NOT_IGNORE){
				think_error(0,"[%s]:env not set.[name=%s]",__func__,p->name);
				return -1;
			}
			continue;
		}
		if(p->exists)
			*(p->exists)=1;
		if(think_getenv(p->name,p->type,p->value,p->size,p->flags)<0){
			think_errorerror();
			return -1;
		}
	}

	return 0;
}
