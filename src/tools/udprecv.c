#include "think.h"
#ifndef __THINK_WINDOWS__
#include <netdb.h>
#endif

void udpunicastrecv(const char *ip,unsigned short port);
void udpmulticastrecv(const char *ip,unsigned short port);

int main(int argc,char *argv[])
{
	think_netstart();
	if(argc==1){
		printf("usage:\n\
%s [ip] port\n\
%s -m ip port\n",argv[0],argv[0]);
		return -1;
	}
	if(strcmp(argv[1],"-m")==0){
		if(argc!=4){
			printf("usage:\n\
%s [ip] port\n\
%s -m ip port\n",argv[0],argv[0]);
			return -1;
		}
		udpmulticastrecv(argv[2],atol(argv[3]));
	}else{
		if(argc==2){
			udpunicastrecv(NULL,atol(argv[1]));
		}else if(argc==3){
			udpunicastrecv(argv[1],atol(argv[2]));
		}else{
			printf("usage:\n\
%s [ip] port\n\
%s -m ip port\n",argv[0],argv[0]);
			return -1;
		}
	}

	return 0;
}


void udpunicastrecv(const char *ip,unsigned short port)
{
	int sockfd;
	char buffer[8192];
	struct sockaddr_in addr={AF_INET};
	socklen_t addrlen=sizeof(addr);
	int sockopt,n;

	addr.sin_port=htons(port);
	if(!ip)
		addr.sin_addr.s_addr=INADDR_ANY;
	else
		addr.sin_addr.s_addr=inet_addr(ip);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("%s\n", think_strerror(think_errno));
		return;
	}
	sockopt=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&sockopt,sizeof(sockopt))<0)
		return;
	if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0)
		return;
	memset(buffer,0x00,sizeof(buffer));
	while((n=recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&addr,&addrlen))>=0){
		printf("[%d]%s\n",n,buffer);
		memset(buffer,0x00,sizeof(buffer));
	}
}

void udpmulticastrecv(const char *ip,unsigned short port)
{
	int sockfd;
	char buffer[8192];
	struct sockaddr_in addr={AF_INET};
	socklen_t addrlen=sizeof(addr);
	int sockopt,n;

	struct in_addr ia;
	struct hostent *group;
	struct ip_mreq mreq;

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		return;
	memset(&mreq,0x00,sizeof(struct ip_mreq));
	if(!(group=gethostbyname(ip)))
		return;
	memcpy((void *)&ia,(void *)group->h_addr,group->h_length);
	memcpy(&mreq.imr_multiaddr.s_addr,&ia,sizeof(struct in_addr));
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		printf("add member ship failed:%s\n",think_strerror(think_errno));
		return;
	}

	sockopt=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&sockopt,sizeof(sockopt))<0)
		return;

	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=INADDR_ANY;
	if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0)
		return;

	memset(buffer,0x00,sizeof(buffer));
	while((n=recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&addr,&addrlen))>=0){
		printf("%d bytes from %s:%s\n",n, inet_ntoa(addr.sin_addr), buffer);
		memset(buffer,0x00,sizeof(buffer));
	}
}
