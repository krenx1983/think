#include "think.h"

void udpunicast(const char *ip,unsigned short port,const char *msg);
void udpbroadcast(const char *ip,unsigned short port,const char *msg);

int main(int argc,char *argv[])
{
	think_netstart();
	if(argc==1){
		printf("usage:\n\
%s ip port messsage\n\
%s -b [ip] port message\n",argv[0],argv[0]);
		return -1;
	}

	if(strcmp(argv[1],"-b")==0){
		if(argc==4){
			udpbroadcast(NULL,atol(argv[2]),argv[3]);
		}else if(argc==5){
			udpbroadcast(argv[2],atol(argv[3]),argv[4]);
		}else{
			printf("usage:\n\
%s ip port messsage\n\
%s -b [ip] port message\n",argv[0],argv[0]);
			return -1;
		}
	}else{
		if(argc!=4){
			printf("usage:\n\
%s ip port messsage\n\
%s -b [ip] port message\n",argv[0],argv[0]);
			return -1;
		}
		if(argc==4)
			udpunicast(argv[1],atol(argv[2]),argv[3]);
	}

	return 0;
}

void udpbroadcast(const char *ip,unsigned short port,const char *msg)
{
	int sockfd;
	char buffer[8192];
	struct sockaddr_in addr={AF_INET};
	int sockopt=1;

	if(!ip){
		addr.sin_addr.s_addr=INADDR_ANY;
		addr.sin_port=htons(port);
	}else{
		addr.sin_addr.s_addr=inet_addr(ip);
		addr.sin_port=htons(port);
	}

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		return;
	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&sockopt,4);
	if(!msg){
		while(gets(buffer))
			sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr *)&addr,sizeof(addr));
	}else{
		sendto(sockfd,msg,strlen(msg),0,(struct sockaddr *)&addr,sizeof(addr));
	}
}

void udpunicast(const char *ip,unsigned short port,const char *msg)
{
	int sockfd;
	char buffer[8192];
	struct sockaddr_in addr={AF_INET};

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		return;
	addr.sin_addr.s_addr=inet_addr(ip);
	addr.sin_port=htons(port);
	if(!msg){
		while(gets(buffer))
			sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr *)&addr,sizeof(addr));
	}else{
		sendto(sockfd,msg,strlen(msg),0,(struct sockaddr *)&addr,sizeof(addr));
	}
}
