#include "think_error.h"
#include "think_utility.h"
#include "think_file.h"

int think_automkdir(const char *path,int flags)
{
	char *p,*q,dir[1024],pdir[1024],subdir[1024],pathseparatechar='/';
	int pos,dirlevel;

	strcpy(dir,path);
	think_strtrim(dir);
	if(dir[strlen(dir)-1]==pathseparatechar)
		dir[strlen(dir)-1]='\0';
	p=dir;
	q=pdir;
	if(think_chrfind(dir,' ',0)>=0){
		think_error(0,"directory do not support space between pathname![path=%s]",dir);
		return -1;
	}
	bzero(pdir,sizeof(pdir));
	if(*p=='/'){
		*q++='/';
		p++;
	}
	dirlevel=0;
	while(*p!='\0'){
		if((pos=think_chrfind(p,pathseparatechar,0))<0){
			strcpy(subdir,p);
			if(dirlevel==0)
				strcpy(q,subdir);
			else
				sprintf(q,"/%s",subdir);
			if(access(pdir,F_OK)==0)
				return 0;
			if(think_mkdir(pdir,flags)<0){
				think_error(0,"mkdir error![think_errno=%d][path=%s]",think_errno,pdir);
				return -1;
			}
			return 0;
		}
		if(pos==0){
			think_error(0,"invalid path![dir=%s]",dir);
			return -1;
		}
		if(dirlevel==0){
			snprintf(q,pos+1,"%s",p);
			q+=pos;
		}else{
			snprintf(q,pos+2,"/%s",p);
			q+=pos+1;
		}
		p+=pos+1;
		dirlevel++;
		if(access(pdir,F_OK)==0)
			continue;
		if(think_mkdir(pdir,flags)<0){
			think_error(0,"mkdir error![think_errno=%d][path=%s]",think_errno,pdir);
			return -1;
		}
	}
	return 0;
}
