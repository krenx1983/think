#include "think.h"

struct __clientnet_t;
struct __clientnet_t {
	THINK_NETCENTER_NET *centernet;
	struct __clientnet_t *peer;
	char *msgbuf;
};
typedef struct __clientnet_t clientnet_t;

unsigned short lsnport,servport;
char lsnip[16]="127.0.0.1",servip[16]="127.0.0.1";
THINK_NETCENTER *netcenter=NULL;
int d_Exists=0;	/* display detail information */
int b_Exists=0;	/* display binary data ( using 0x ... ) */
int max_msg_size=8192;
unsigned int tabstop;
THINK_GL_RULE *rulearray=NULL;
int rulecount=0;

void acceptnewclient(THINK_NETCENTER_HANDLE handle);
void clientnethandler(THINK_NETCENTER_HANDLE handle);
void display_message(const char *msgbuf,unsigned int msglen);
int display_attr(const char *attrbuf,unsigned short attrlen);
int display_data(const char *databuf,unsigned int datalen,char type,int flags);
void display_flags(int flags);
THINK_GL_RULE* findrule(unsigned int no);
int display_attr_of_no(unsigned int no);

int main(int argc,char *argv[])
{
	char usage[]="usage: \nglpeek [-bd] [-f rulefile] [-s maxsize] lsnaddr servaddr\nglpeek [-bd] [-f rulefile] [-s maxsize] msgfile";
	char rulefile[100]="";
	THINK_OPTARRAY optarray[]={
		{'b','C',NULL,0,0,&b_Exists},
		{'d','C',NULL,0,0,&d_Exists},
		{'f','C',rulefile,sizeof(rulefile),THINK_OPT_ARG|THINK_OPT_NOT_NULL,NULL},
		{'s','I',&max_msg_size,0,THINK_OPT_NOT_NULL,NULL}
	};

	THINK_NET *net;
	int fd;
	int mode=0;
	int r;
	char *msgbuf;
	int msglen;

#ifdef __THINK_UNIX__
	signal(SIGPIPE,SIG_IGN);
#endif

	if(think_getopts(argc,argv,optarray,sizeof(optarray)/sizeof(THINK_OPTARRAY))<0){
		think_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind==1){
		mode=0;
	}else if(argc-optind==2){
		mode=1;
	}else{
		puts(usage);
		return -1;
	}

	/* load rule file */
	if(strlen(rulefile)>0){
		if((rulecount=think_gl_loadrules(rulefile,&rulearray))<0){
			think_showerror();
			return -1;
		}
	}

	if(mode==0){
		if((fd=open(argv[optind],O_RDONLY|_O_BINARY))<0){
			think_showerror();
			return -1;
		}
		msgbuf=malloc(max_msg_size);
		while((r=read(fd,&msglen,sizeof(msglen)))!=0){
			if(r<0){
				think_showerror();
				return -1;
			}
			if(r!=sizeof(msglen)){
				printf("pkgfile's format is wrong!\n");
				return -1;
			}
			msglen=ntohl(msglen);
			if(msglen>max_msg_size-sizeof(msglen)){
				printf("message too long![msglen=%d]\n",msglen);
				return -1;
			}
			if(read(fd,msgbuf,msglen)!=msglen){
				printf("pkgfile is not completed.\n");
				return -1;
			}
			display_message(msgbuf,msglen);
		}
		close(fd);
	}else{
		/* listen address */
		if(think_strgetfield(argv[optind],':',0,'C',lsnip,sizeof(lsnip),THINK_STR_NOT_NULL)<0){
			think_showerror();
			puts(usage);
			return -1;
		}
		if(think_strgetfield(argv[optind],':',1,'S',&lsnport,0,THINK_STR_NOT_NULL)<0){
			think_showerror();
			puts(usage);
			return -1;
		}

		/* server address */
		if(think_strgetfield(argv[optind+1],':',0,'C',servip,sizeof(servip),THINK_STR_NOT_NULL)<0){
			think_showerror();
			puts(usage);
			return -1;
		}
		if(think_strgetfield(argv[optind+1],':',1,'S',&servport,0,THINK_STR_NOT_NULL)<0){
			think_showerror();
			puts(usage);
			return -1;
		}

		/* net start */
		think_netstart();

		/* listening ... */
		if((net=think_netlisten(lsnip,lsnport))==NULL){
			think_showerror();
			return -1;
		}
		netcenter=think_netcenter_new();
		think_netcenter_register(netcenter,net,acceptnewclient,net);
		while(1){
			if(think_netcenter_idle(netcenter,-1)<0){
				think_showerror();
				break;
			}
		}

		/* net stop */
		think_netstop();
	}

	return 0;
}
void acceptnewclient(THINK_NETCENTER_HANDLE handle)
{
	THINK_NET *lsnnet;
	THINK_NET *client,*server;
	clientnet_t *clientnet,*servernet;

	lsnnet=handle;
	if((client=think_netaccept(lsnnet))==NULL)
		return;
	printf("connected by %s:%hu\n",client->ip,client->port);
	if((server=think_netconnect(servip,servport))==NULL){
		printf("connect to %s:%hu failed.[%s]\n",servip,servport,__think_errmsg__);
		think_netclose(client);
		return;
	}
	clientnet=malloc(sizeof(clientnet_t));
	servernet=malloc(sizeof(clientnet_t));
	bzero(clientnet,sizeof(clientnet_t));
	bzero(servernet,sizeof(clientnet_t));
	clientnet->peer=servernet;
	servernet->peer=clientnet;
	clientnet->msgbuf=malloc(max_msg_size);
	servernet->msgbuf=malloc(max_msg_size);
	clientnet->centernet=think_netcenter_register(netcenter,client,clientnethandler,clientnet);
	servernet->centernet=think_netcenter_register(netcenter,server,clientnethandler,servernet);
}
void clientnethandler(THINK_NETCENTER_HANDLE handle)
{
	char *msgbuf;
	int msglen;
	int r;
	clientnet_t *clientnet;
	THINK_NETCENTER_NET *centernet;
	THINK_NET *net;

	clientnet=handle;
	centernet=clientnet->centernet;
	net=centernet->net;
	msgbuf=clientnet->msgbuf;
	think_netcenter_recvto(centernet);
	/* receive message */
	while((r=think_netcenter_ismsgok(centernet))!=0){
		if(r<0){
			think_showerror();
			think_netcenter_unregister(netcenter,centernet);
			think_netclose(net);

			/* use shutdown to do sync before close */
			think_netcenter_shutdown(netcenter,clientnet->peer->centernet);

			free(clientnet->peer->msgbuf);
			free(clientnet->peer);
			free(clientnet->msgbuf);
			free(clientnet);
			return;
		}
		if((r=think_netcenter_recvmsg(centernet,msgbuf,max_msg_size))<0){
			think_showerror();
			think_netcenter_unregister(netcenter,centernet);
			think_netclose(net);

			/* use shutdown to do sync before close */
			think_netcenter_shutdown(netcenter,clientnet->peer->centernet);

			free(clientnet->peer->msgbuf);
			free(clientnet->peer);
			free(clientnet->msgbuf);
			free(clientnet);
			return;
		}
		msglen=r;

		/* display message */
		printf("%d bytes from %s:%hu to %s:%hu:\n",msglen,clientnet->centernet->net->ip,clientnet->centernet->net->port,clientnet->peer->centernet->net->ip,clientnet->peer->centernet->net->port);
		tabstop=0;
		display_message(msgbuf,msglen);

		/* send message */
		think_netcenter_sendmsg(clientnet->peer->centernet,msgbuf,msglen);
	}
}
void display_message(const char *msgbuf,unsigned int msglen)
{
	const char *p;
	THINK_UINT pos;
	THINK_UINT len;
	THINK_USHORT attrlen;
	int data_display;
	THINK_USHORT no;	/* no */
	char type;			/* type */
	THINK_INT flags;			/* flags */
	THINK_INT i,r;
	THINK_GL_RULE *rule;

	pos=0;
	no=0;
	while(pos!=msglen){
		if(msglen-pos<8){
			printf("message not complete.[no=%d][pos=%d][msglen=%d]\n",no,pos,msglen);
			return;
		}
		p=msgbuf+pos;
		data_display=0;

		/* no */
		memcpy(&no,p,2);
		no=think_btoh2(no);

		p+=2;
		pos+=2;

		for(i=0;i<tabstop;i++)
			printf("\t");
		printf("<%d",no);

		/* attr len */
		memcpy(&attrlen,p,2);
		attrlen=think_btoh2(attrlen);
		p+=2;
		pos+=2;

		/* attr data */
		if(msglen-pos<attrlen){
			printf("attr not complete.[no=%d][pos=%d][msglen=%d][attrlen=%d]",no,pos,msglen,attrlen);
			return;
		}
			
		/* display attributes */
		if(d_Exists){
			if(attrlen>0){
				if(display_attr(p,attrlen)<0)
					return;
			}else{
				display_attr_of_no(no);
			}
		}

		if(think_gl_attr_find(p,attrlen,THINK_GL_TYPE)){
			data_display=1;
			think_gl_attr_get(p,attrlen,THINK_GL_TYPE,&type,sizeof(type));
			think_gl_attr_get(p,attrlen,THINK_GL_FLAGS,&flags,sizeof(flags));
			flags=think_btoh4(flags);
		}else{
			if((rule=findrule(no))){
				data_display=1;
				type=rule->type;
				flags=rule->flags;
			}
		}

		p+=attrlen;
		pos+=attrlen;

		printf(">");

		/* len */
		memcpy(&len,p,4);
		len=think_btoh4(len);
		p+=4;
		pos+=4;

		/* data */
		if(msglen-pos<len){
			printf("data not complete.[no=%d][pos=%d][msglen=%d][len=%d]",no,pos,msglen,len);
			return;
		}
		
		/* display data */
		if(data_display){
			if((r=display_data(p,len,type,flags))<0){
				return;
			}else if(r!=0){
				for(i=0;i<tabstop;i++)
					printf("\t");
			}
		}
		p+=len;
		pos+=len;
		printf("</%d>\n",no);
	}
}

int display_attr_of_no(unsigned int no)
{
	THINK_GL_RULE *rule;

	if((rule=findrule(no))==NULL)
		return -1;
	printf(" name=\"%s\"",rule->name);
	printf(" type=\"%c\"",rule->type);
	printf(" maxlen=\"%u\"",rule->maxlen);
	printf(" flags=\"");
	display_flags(rule->flags);
	printf("\"");
	printf(" desc=\"%s\"",rule->desc);

	return 0;
}
int display_attr(const char *attrbuf,unsigned short attrlen)
{
	const char *p;
	THINK_USHORT pos;
	unsigned char len;
	unsigned char attrno;
	char name[32];		/* name */
	char type;			/* type */
	THINK_UINT maxlen;			/* maxlen */
	THINK_INT flags;			/* flags */
	char desc[128];		/* desc */

	pos=0;
	attrno=0;
	while(pos!=attrlen){
		if(attrlen-pos<2){
			printf("attr not complete.[attrno=%d][pos=%d][attren=%d]",attrno,pos,attrlen);
			return -1;
		}
		p=attrbuf+pos;

		/* attr no */
		memcpy(&attrno,p,1);
		p++;
		pos++;

		/* len */
		memcpy(&len,p,1);
		p++;
		pos++;

		if(len>attrlen-pos){
			printf("attr data not complete.[attrno=%d][pos=%d][attrlen=%d][len=%d]",attrno,pos,attrlen,len);
			return -1;
		}
		switch(attrno){
			case THINK_GL_NAME:
				if(len>=sizeof(name)){
					printf("attr:name too long.[attrno=%d][pos=%d][attrlen=%d][len=%d]",attrno,pos,attrlen,len);
					return -1;
				}
				bzero(name,sizeof(name));
				memcpy(name,p,len);
				think_strtrim(name);
				printf(" name=\"%s\"",name);
				break;
			case THINK_GL_TYPE:
				if(len!=1){
					printf("attr:type len not equal to 1.[attrno=%d][pos=%d][attrlen=%d][len=%d]",attrno,pos,attrlen,len);
					return -1;
				}
				type=*p;
				printf(" type=\"%c\"",type);
				break;
			case THINK_GL_MAXLEN:
				if(len!=4){
					printf("attr:maxlen len not equal to 4.[attrno=%d][pos=%d][attrlen=%d][len=%d]",attrno,pos,attrlen,len);
					return -1;
				}
				memcpy(&maxlen,p,4);
				maxlen=think_btoh4(maxlen);
				printf(" maxlen=\"%u\"",maxlen);
				break;
			case THINK_GL_FLAGS:
				if(len!=4){
					printf("attr:flags len not equal to 4.[attrno=%d][pos=%d][attrlen=%d][len=%d]",attrno,pos,attrlen,len);
					return -1;
				}
				memcpy(&flags,p,4);
				flags=think_btoh4(flags);
				printf(" flags=\"");
				display_flags(flags);
				printf("\"");
				break;
			case THINK_GL_DESC:
				if(len>=sizeof(desc)){
					printf("attr:desc too long.[attrno=%d][pos=%d][attrlen=%d][len=%d]",attrno,pos,attrlen,len);
					return -1;
				}
				bzero(desc,sizeof(desc));
				memcpy(desc,p,len);
				think_strtrim(desc);
				printf(" desc=\"%s\"",desc);
				break;
			default:
				break;
		}

		pos+=len;
	}

	return 0;
}
int display_data(const char *databuf,unsigned int datalen,char type,int flags)
{
	THINK_SHORT s;
	THINK_INT i;
	THINK_LONG l;
	THINK_UINT reclen;
	THINK_DOUBLE d;
	THINK_FLOAT f;
	int n;

	if(flags & THINK_GL_RECORD){
		puts("");
		tabstop++;
		display_message(databuf,datalen);
		tabstop--;
		return 1;
	}
	if(flags & THINK_GL_RECORDSET){
		n=0;
		while(datalen>0){
			n++;
			memcpy(&reclen,databuf,sizeof(reclen));
			reclen=think_btoh4(reclen);
			puts("");
			tabstop++;
			for(i=0;i<tabstop;i++)
				printf("\t");
			printf("<%d>\n",n);
			tabstop++;
			display_message(databuf+sizeof(reclen),reclen);
			tabstop--;
			for(i=0;i<tabstop;i++)
				printf("\t");
			printf("</%d>\n",n);
			tabstop--;
			databuf+=sizeof(reclen)+reclen;
			datalen-=sizeof(reclen)+reclen;
		}
		return 1;
	}
	switch(type){
		case THINK_GL_CHAR:		/* char */
			if(datalen>0){
				if(flags & THINK_GL_BINARY){
					if(!b_Exists){
						if(datalen==1)	/* display it if length is 1. */
							printf("0x%02X",databuf[0]);
						break;
					}
					printf("0x");
					for(i=0;i<datalen;i++)
						printf("%02X",databuf[i]);
				}else{
					printf("%*.*s",datalen,datalen,databuf);
				}
			}
			break;
		case THINK_GL_SHORT:		/* short */
			if(datalen!=2){
				printf("field length not match.[type='%c'][len=%d]",type,datalen);
				return -1;
			}
			memcpy(&s,databuf,2);
			s=think_btoh2(s);
			if(flags & THINK_GL_UNSIGNED)
				printf("%hu",s);
			else
				printf("%hd",s);
			break;
		case THINK_GL_INT:	/* int */
			if(datalen!=4){
				printf("field length not match.[type='%c'][len=%d]",type,datalen);
				return -1;
			}
			memcpy(&i,databuf,4);
			i=think_btoh4(i);
			if(flags & THINK_GL_UNSIGNED)
				printf("%u",(THINK_UINT)i);
			else
				printf("%d",i);
			break;
		case THINK_GL_LONG:	/* long */
			if(datalen!=8){
				printf("field length not match.[type='%c'][len=%d]",type,datalen);
				return -1;
			}
			memcpy(&l,databuf,8);
			l=think_btoh8(l);
#ifdef __THINK_WINDOWS__
			if(flags & THINK_GL_UNSIGNED)
				printf("%I64u",(THINK_ULONG)l);
			else
				printf("%I64d",l);
#else
			if(flags & THINK_GL_UNSIGNED)
				printf("%llu",(THINK_ULONG)l);
			else
				printf("%lld",l);
#endif
			break;
		case THINK_GL_FLOAT:		/* float */
			if(datalen!=4){
				printf(0,"field length not match.[type='%c'][len=%d]",type,datalen);
				return -1;
			}
			memcpy(&i,databuf,4);
			i=think_btoh4(i);
			memcpy(&f,&i,sizeof(f));
			printf("%f",f);
			break;
		case THINK_GL_DOUBLE:		/* double */
			if(datalen!=8){
				printf("field length not match.[type='%c'][len=%d]",type,datalen);
				return -1;
			}
			memcpy(&l,databuf,8);
			l=think_btoh8(l);
			memcpy(&d,&l,sizeof(d));
			printf("%lf",d);
			break;
		default:
			printf("illegal field type![type='%c']",type);
			return -1;
	}

	return 0;
}
void display_flags(int flags)
{
	int first=1;

	if(flags & THINK_GL_BINARY){
		if(!first)
			printf(",");
		else
			first=0;
		printf("binary");
	}
	if(flags & THINK_GL_UNSIGNED){
		if(!first)
			printf(",");
		else
			first=0;
		printf("unsigned");
	}
	if(flags & THINK_GL_RECORD){
		if(!first)
			printf(",");
		else
			first=0;
		printf("record");
	}
	if(flags & THINK_GL_RECORDSET){
		if(!first)
			printf(",");
		else
			first=0;
		printf("recordset");
	}
}

THINK_GL_RULE* findrule(unsigned int no)
{
	int i;
	THINK_GL_RULE* p;

	for(i=0,p=rulearray;i<rulecount;i++,p++){
		if(p->no==no)
			return p;
	}

	return NULL;
}

