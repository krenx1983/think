#include "think_error.h"
#include "think_net.h"

int think_netstart(void)
{
#ifdef __THINK_WINDOWS__
	WSADATA data;
	
	if(WSAStartup(MAKEWORD(2,2),&data)<0){
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
	struct addrinfo hints,*res;
	int sockfd;
	THINK_NET *net;
	char strport[6];

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	bzero(strport,sizeof(strport));
	sprintf(strport,"%hu",port);
	if(getaddrinfo(ip,strport,&hints,&res)<0){
		think_error(0,"[%s]:getaddrinfo error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
		return NULL;
	}

	if((sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol))<0){
		think_error(0,"[%s]:socket create error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return NULL;
	}
	if(connect(sockfd,res->ai_addr,res->ai_addrlen)<0){
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
	snprintf(net->ip,sizeof(net->ip),"%s",ip);
	net->port=port;

	return net;
}

THINK_NET *think_netlisten(const char *ip,unsigned short port)
{
	struct addrinfo hints,*res;
	int sockfd;
	struct linger so_linger;
	int option;
	THINK_NET *net;
	char strport[6];

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_flags=AI_PASSIVE;
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	bzero(strport,sizeof(strport));
	sprintf(strport,"%hu",port);
	if(strcmp(ip,"*")==0){
		if(getaddrinfo(NULL,strport,&hints,&res)<0){
			think_error(0,"[%s]:getaddrinfo error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
			return NULL;
		}
	}else{
		if(getaddrinfo(ip,strport,&hints,&res)<0){
			think_error(0,"[%s]:getaddrinfo error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
			return NULL;
		}
	}
	if((sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol))<0){
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
	if(bind(sockfd,res->ai_addr,res->ai_addrlen)<0){
		think_error(0,"[%s]:bind error.[%d:%s][%s:%d]",__func__,think_socketerrno,think_strerror(think_socketerrno),ip,port);
		socketclose(sockfd);
		return NULL;
	}
	freeaddrinfo(res);
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
	struct sockaddr_storage addr;
	struct sockaddr_in* addr_in;
	struct sockaddr_in6* addr_in6;
	socklen_t addrlen;
	int sockfd;
	const char *p;
	THINK_NET *client;
	char ip[54];
	unsigned short port;

	addrlen=sizeof(addr);
	if((sockfd=accept(net->sockfd,(struct sockaddr *)&addr,&addrlen))<0){
		think_error(0,"[%s]:accept error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
		return NULL;
	}

	bzero(ip,sizeof(ip));
	switch(addr.ss_family){
		case AF_INET:
			addr_in=(struct sockaddr_in*)&addr;
			if((p=inet_ntop(AF_INET,&addr_in->sin_addr,ip,sizeof(ip)))==NULL){
				think_error(0,"[%s]:inet_ntop error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
				return NULL;
			}
			port=ntohs(addr_in->sin_port);
			break;
		case AF_INET6:
			addr_in6=(struct sockaddr_in6*)&addr;
			if((p=inet_ntop(AF_INET6,&addr_in6->sin6_addr,ip,sizeof(ip)))==NULL){
				think_error(0,"[%s]:inet_ntop error.[%d:%s]",__func__,think_socketerrno,think_strerror(think_socketerrno));
				return NULL;
			}
			port=ntohs(addr_in6->sin6_port);
			break;
		default:
			think_error(0,"[%s]:unsupported address family",__func__);
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
	client->port=port;

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
