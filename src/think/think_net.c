#include "think_error.h"
#include "think_net.h"

int think_netstart(void)
{
#ifdef __THINK_WINDOWS__
	WSADATA data;
	
	if(WSAStartup(MAKEWORD(1,1),&data)<0){
		think_error(0,"[%s]:WSAStartup error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return -1;
	}
#endif
	
	return 0;
}

int think_netstop(void)
{
#ifdef __THINK_WINDOWS__
	if(WSACleanup()<0){
		think_error(0,"[%s]:WSACleanup error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return -1;
	}
#endif

	return 0;
}

THINK_NET *think_netconnect(const char *ip,unsigned short port)
{
	struct sockaddr_in servaddr;
	int sockfd;
	THINK_NET *net;
	struct hostent *hp;
	char IP[16];
	struct in_addr **ppinaddr;

	bzero(&servaddr,sizeof(struct sockaddr_in));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	if((servaddr.sin_addr.s_addr=inet_addr(ip))==INADDR_NONE){
		if((hp=gethostbyname(ip))==NULL){
			think_error(0,"[%s]:gethostbyname error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
			return NULL;
		}
		ppinaddr=(struct in_addr **)hp->h_addr_list;
		if(*ppinaddr==NULL){
			think_error(0,"[%s]:gethostname error.[no address found]",__func__);
			return NULL;
		}
		snprintf(IP,sizeof(IP),"%s",inet_ntoa(**ppinaddr));
		memcpy(&servaddr.sin_addr,*ppinaddr,sizeof(struct in_addr));
	}else{
		snprintf(IP,sizeof(IP),"%s",ip);
	}
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		think_error(0,"[%s]:socket create error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return NULL;
	}
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr_in))<0){
		think_error(0,"[%s]:connect error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
		socketclose(sockfd);
		return NULL;
	}
	if((net=malloc(sizeof(THINK_NET)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		socketclose(sockfd);
		return NULL;
	}
	memset(net,0x00,sizeof(THINK_NET));
	net->sockfd=sockfd;
	snprintf(net->ip,sizeof(net->ip),"%s",IP);
	net->port=port;

	return net;
}

THINK_NET *think_netlisten(const char *ip,unsigned short port)
{
	struct sockaddr_in servaddr;
	int sockfd;
	struct linger so_linger;
	int option;
	THINK_NET *net;

	bzero(&servaddr,sizeof(struct sockaddr_in));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	if(strcmp(ip,"*")==0){
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}else{
		if((servaddr.sin_addr.s_addr=inet_addr(ip))==INADDR_NONE){
			think_error(0,"[%s]:inet_addr error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
			return NULL;
		}
	}
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		think_error(0,"[%s]:socket create error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return NULL;
	}
	so_linger.l_onoff=1;
	so_linger.l_linger=0;
	if(setsockopt(sockfd,SOL_SOCKET,SO_LINGER,(void *)&so_linger,sizeof(so_linger))<0){
		think_error(0,"[%s]:setsockopt error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		socketclose(sockfd);
		return NULL;
	}
	option=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&option,sizeof(option))<0){
		think_error(0,"[%s]:setsockopt error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		socketclose(sockfd);
		return NULL;
	}
	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
		think_error(0,"[%s]:bind error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
		socketclose(sockfd);
		return NULL;
	}
	if(listen(sockfd,5)<0){
		think_error(0,"[%s]:listen error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
		socketclose(sockfd);
		return NULL;
	}
	if((net=malloc(sizeof(THINK_NET)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		socketclose(sockfd);
		return NULL;
	}
	memset(net,0x00,sizeof(THINK_NET));
	net->sockfd=sockfd;
	snprintf(net->ip,sizeof(net->ip),"%s",ip);
	net->port=port;

	return net;
}

THINK_NET *think_netaccept(THINK_NET *net)
{
	struct sockaddr_in addr;
	socklen_t addrlen;
	int sockfd;
	char *p;
	THINK_NET *client;

	addrlen=sizeof(addr);
	if((sockfd=accept(net->sockfd,(struct sockaddr *)&addr,&addrlen))<0){
		think_error(0,"[%s]:accept error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return NULL;
	}
	if((p=inet_ntoa(addr.sin_addr))==NULL){
		think_error(0,"[%s]:inet_ntoa error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return NULL;
	}
	if((client=malloc(sizeof(THINK_NET)))==NULL){
		think_error(0,"[%s]:malloc error.[%d:%s]",__func__,think_errno,think_strerror(think_errno));
		socketclose(sockfd);
		return NULL;
	}
	memset(client,0x00,sizeof(THINK_NET));
	client->sockfd=sockfd;
	snprintf(client->ip,sizeof(client->ip),"%s",p);
	client->port=ntohs(addr.sin_port);

	return client;
}

int think_netrecv(THINK_NET *net,void *buf,unsigned int siz,int flags)
{
	int n,nbytes;

	for(nbytes=0;siz>0;nbytes+=n,siz-=n){
		if((n=recv(net->sockfd,(char *)buf+nbytes,siz,0))<=0){
			if(n==0){
				think_error(0,"[%s]:read error![connection disconnected!]",__func__);
				return 0;
			}else if(think_errno==EAGAIN || think_errno==EWOULDBLOCK){
				if(flags & THINK_NET_WAIT){
					n=0;
					think_os_msleep(10);
					continue;
				}
				think_error(0,"[%s]:read error![%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
				return -1;
			}else if(think_errno==EINTR){
				n=0;
				continue;
			}else{
				think_error(0,"[%s]:read error![%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
				return -1;
			}
		}
		if(!(flags & THINK_NET_WAIT))
			return n;
	}

	return nbytes;
}

int think_netsend(THINK_NET *net,const void *buf,unsigned int len,int flags)
{
	int n,nbytes;

	for(nbytes=0;len>0;nbytes+=n,len-=n){
		if((n=send(net->sockfd,(char *)buf+nbytes,len,0))<=0){
			if(think_errno==EAGAIN || think_errno==EWOULDBLOCK){
				if(flags & THINK_NET_WAIT){
					n=0;
					think_os_msleep(10);
					continue;
				}
				think_error(0,"[%s]:write error![%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
				return -1;
			}else if(think_errno==EINTR){
				n=0;
				continue;
			}else{
				think_error(0,"[%s]:write error![%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
				return -1;
			}
		}
		if(!(flags & THINK_NET_WAIT))
			return n;
	}

	return nbytes;
}

int think_netclose(THINK_NET *net)
{
	socketclose(net->sockfd);
	free(net);

	return 0;
}

int think_netsendmsg(THINK_NET *net,const void *buf,unsigned int len)
{
	THINK_UINT t;

	t=len;
	t=think_htob4(t);
	if(think_netsend(net,&t,4,THINK_NET_WAIT)<0){
		think_errorerror();
		return -1;
	}
	if(think_netsend(net,buf,len,THINK_NET_WAIT)<0){
		think_errorerror();
		return -1;
	}

	return 0;
}

int think_netrecvmsg(THINK_NET *net,void *buf,unsigned int siz)
{
	THINK_UINT len;

	if(think_netrecv(net,&len,4,THINK_NET_WAIT)<0){
		think_errorerror();
		return -1;
	}
	len=think_btoh4(len);
	if(len>siz){
		think_error(0,"[%s]:msg too long![siz=%d][len=%d]",__func__,siz,len);
		return -1;
	}
	if(think_netrecv(net,buf,len,THINK_NET_WAIT)<0){
		think_errorerror();
		return -1;
	}

	return len;
}
