#include "think.h"

struct __clientnet_t;
struct __clientnet_t {
	THINK_NETCENTER_NET *centernet;
	struct __clientnet_t *peer;
};
typedef struct __clientnet_t clientnet_t;

unsigned short lsnport,servport;
char lsnip[128],servip[128];
THINK_NETCENTER *netcenter=NULL;
int n_Exists=0,q_Exists=0,s_Exists=0,c_Exists=0;

void acceptnewclient(THINK_NETCENTER_HANDLE handle);
void clientnethandler(THINK_NETCENTER_HANDLE handle);
void display_message(const char *msgbuf,unsigned int msglen);

int main(int argc,char *argv[])
{
	char usage[]="usage: netpeek [-nqsc] lsnaddr servaddr";
	THINK_NET *net;
	THINK_OPTARRAY optarray[4]={
		{'n','C',NULL,0,0,&n_Exists},
		{'q','C',NULL,0,0,&q_Exists},
		{'s','C',NULL,0,0,&s_Exists},
		{'c','C',NULL,0,0,&c_Exists}
	};
	const char* p;

#ifdef __THINK_UNIX__
	signal(SIGPIPE,SIG_IGN);
#endif

	if(think_getopts(argc,argv,optarray,4)<0){
		think_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind!=2){
		puts(usage);
		return -1;
	}

	/* listen address */
	if((p=strrchr(argv[optind],':'))==NULL){
		puts(usage);
		return -1;
	}
	memcpy(lsnip,argv[optind],p-argv[optind]);
	lsnport=atol(p+1);

	/* server address */
	if((p=strrchr(argv[optind+1],':'))==NULL){
		puts(usage);
		return -1;
	}
	memcpy(servip,argv[optind+1],p-argv[optind+1]);
	servport=atol(p+1);

	/* net start */
	think_netstart();

	/* netcenter */
	netcenter=think_netcenter_new();

	if(s_Exists){
		/* server to server */
		THINK_NET *client,*server;
		clientnet_t *clientnet,*servernet;

		if((net=think_netlisten(lsnip,lsnport))==NULL){
			think_showerror();
			return -1;
		}
		if((client=think_netaccept(net))==NULL){
			think_showerror();
			return -1;
		}
		if(!q_Exists)
			printf("connected by %s:%hu\n",client->ip,client->port);

		if((net=think_netlisten(servip,servport))==NULL){
			think_showerror();
			return -1;
		}
		if((server=think_netaccept(net))==NULL){
			think_showerror();
			return -1;
		}
		if(!q_Exists)
			printf("connected by %s:%hu\n",server->ip,server->port);

		clientnet=malloc(sizeof(clientnet_t));
		servernet=malloc(sizeof(clientnet_t));
		bzero(clientnet,sizeof(clientnet_t));
		bzero(servernet,sizeof(clientnet_t));
		clientnet->peer=servernet;
		servernet->peer=clientnet;
		clientnet->centernet=think_netcenter_register(netcenter,client,clientnethandler,clientnet);
		servernet->centernet=think_netcenter_register(netcenter,server,clientnethandler,servernet);
	}else if(c_Exists){
		/* client to client */
		THINK_NET *client,*server;
		clientnet_t *clientnet,*servernet;

		if((client=think_netconnect(lsnip,lsnport))==NULL){
			printf("connect to %s:%hu failed.[%s]\n",lsnip,lsnport,__think_errmsg__);
			return 0;
		}
		if((server=think_netconnect(servip,servport))==NULL){
			printf("connect to %s:%hu failed.[%s]\n",servip,servport,__think_errmsg__);
			return 0;
		}

		clientnet=malloc(sizeof(clientnet_t));
		servernet=malloc(sizeof(clientnet_t));
		bzero(clientnet,sizeof(clientnet_t));
		bzero(servernet,sizeof(clientnet_t));
		clientnet->peer=servernet;
		servernet->peer=clientnet;
		clientnet->centernet=think_netcenter_register(netcenter,client,clientnethandler,clientnet);
		servernet->centernet=think_netcenter_register(netcenter,server,clientnethandler,servernet);
	}else{
		/* server to client */
		if((net=think_netlisten(lsnip,lsnport))==NULL){
			think_showerror();
			return -1;
		}
		think_netcenter_register(netcenter,net,acceptnewclient,net);
	}
	while(1){
		if(think_netcenter_idle(netcenter,-1)<0){
			think_showerror();
			break;
		}
	}

	/* net stop */
	think_netstop();

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
	if(!q_Exists)
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
	clientnet->centernet=think_netcenter_register(netcenter,client,clientnethandler,clientnet);
	servernet->centernet=think_netcenter_register(netcenter,server,clientnethandler,servernet);
}
void clientnethandler(THINK_NETCENTER_HANDLE handle)
{
	char msgbuf[8192];
	unsigned int msglen;
	int r;
	clientnet_t *clientnet;
	THINK_NETCENTER_NET *centernet;
	THINK_NET *net;

	clientnet=handle;
	centernet=clientnet->centernet;
	net=centernet->net;
	/* receive message */
	if((r=think_netrecv(centernet->net,msgbuf,sizeof(msgbuf),0))<=0){
		if(r<0 || (r==0 && !q_Exists))
			think_showerror();
		think_netcenter_unregister(netcenter,centernet);
		think_netclose(net);

		/* use shutdown to do sync before close */
		think_netcenter_shutdown(netcenter,clientnet->peer->centernet);

		free(clientnet->peer);
		free(clientnet);
		return;
	}
	msglen=r;

	/* display message */
	if(!n_Exists)
		printf("%u bytes from %s:%hu to %s:%hu:\n",msglen,clientnet->centernet->net->ip,clientnet->centernet->net->port,clientnet->peer->centernet->net->ip,clientnet->peer->centernet->net->port);
	display_message(msgbuf,msglen);
	if(!n_Exists)
		puts("\n");

	/* send message */
	think_netcenter_send(clientnet->peer->centernet,msgbuf,msglen);
}
void display_message(const char *msgbuf,unsigned int msglen)
{
	if(!n_Exists)
		write(1,msgbuf,msglen);
}
