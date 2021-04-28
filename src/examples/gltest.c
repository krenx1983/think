#include "think.h"

int main(int argc,char *argv[])
{
	char UserID[5];
	short TraceNo;
	int ProductID;
	THINK_LONG Count;
	float Price;
	double Amount;
	char msgbufin[1024],msgbufout[1024];
	unsigned long msglenin,msglenout;
	int glflags=THINK_GL_DEBUG;
	int r;
	char name[32],desc[128];

	/* init */
	strcpy(UserID,"0100");
	TraceNo=1;
	ProductID=0;
	Count=100;
	Price=10.0;
	Amount=Count*Price;

	msglenout=0;
	if((r=think_gl_put(msgbufout,sizeof(msgbufout)-msglenout,1,"",'C',4,0,"",UserID,0,glflags))<0){
		think_showerror();
		exit(-1);
	}
	msglenout+=r;
	if((r=think_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,2,"",'S',2,0,"",&TraceNo,0,glflags))<0){
		think_showerror();
		exit(-1);
	}
	msglenout+=r;
	if((r=think_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,3,"",'I',4,0,"",&ProductID,0,glflags))<0){
		think_showerror();
		exit(-1);
	}
	msglenout+=r;
	if((r=think_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,4,"",'L',8,0,"",&Count,0,glflags))<0){
		think_showerror();
		exit(-1);
	}
	msglenout+=r;
	if((r=think_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,5,"",'F',4,0,"",&Price,0,THINK_GL_NOT_NULL|glflags))<0){
		think_showerror();
		exit(-1);
	}
	msglenout+=r;
	if((r=think_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,6,"",'D',8,0,"",&Amount,0,0|glflags))<0){
		think_showerror();
		exit(-1);
	}
	msglenout+=r;
	memcpy(msgbufin,msgbufout,msglenout);
	msglenin=msglenout;
	bzero(UserID,sizeof(UserID));
	if(think_gl_get(msgbufin,msglenin,1,name,'C',4,0,desc,UserID,sizeof(UserID),0|glflags)<0){
		think_showerror();
		exit(-1);
	}
	printf("UserID=[%s]\n",UserID);
	TraceNo=0;
	if(think_gl_get(msgbufin,msglenin,2,name,'S',2,0,"",&TraceNo,0,0|glflags)<0){
		think_showerror();
		exit(-1);
	}
	printf("TraceNo=[%d]\n",TraceNo);
	ProductID=0;
	if(think_gl_get(msgbufin,msglenin,3,name,'I',4,0,"",&ProductID,0,0|glflags)<0){
		think_showerror();
		exit(-1);
	}
	printf("ProductID=[%d]\n",ProductID);
	Count=0;
	if(think_gl_get(msgbufin,msglenin,4,name,'L',8,0,"",&Count,0,0|glflags)<0){
		think_showerror();
		exit(-1);
	}
	printf("Count=[%d]\n",Count);
	Price=0.0;
	if(think_gl_get(msgbufin,msglenin,5,name,'F',4,0,"",&Price,0,0|glflags)<0){
		think_showerror();
		exit(-1);
	}
	printf("Price=[%f]\n",Price);
	Amount=0.0;
	if(think_gl_get(msgbufin,msglenin,6,name,'D',8,0,"",&Amount,0,0|glflags)<0){
		think_showerror();
		exit(-1);
	}
	printf("Amount=[%f]\n",Amount);

	return 0;
}
