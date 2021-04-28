#!/bin/sh
#
# kill.sh: like kill but use process name as the parameter
# usage: kill.sh [-n] [-s signal] pname ...
# version: 1.0
# OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X
# author: luojian(luojian1983@sohu.com)
# history:
# 	2008-10-24	1.0 released
#	2008-11-04	option [-v] changed to [-n]
#	2009-06-18	display process name
#	2009-07-09	support Mac OS X

VIRTUAL="no"
SIGNAL="TERM"
while getopts :ns: arg
do
	case $arg in
	n)
		VIRTUAL="yes"
		;;
	s)
		SIGNAL="$OPTARG"
		;;
	:)
		printf "kill.sh: option requires an argument -- %s\n" "$OPTARG"
		printf "usage: kill.sh [-n] [-s signal] pname ...\n"
		exit 1
		;;
	?)
		printf "kill.sh: illegal option -- %s\n" "$OPTARG"
		printf "usage: kill.sh [-n] [-s signal] pname ...\n"
		exit 1
		;;
	esac
done
shift `expr $OPTIND - 1`
if [ $# -eq 0 ]
then
	printf "usage: kill.sh [-n] [-s signal] pname ...\n"
	exit 1
fi
OS=`uname -s`
while [ "$1" != "" ]
do
	case $OS in
		AIX)
			list=`ps -U $LOGNAME -o pid,comm|sed -n '2,$p'|grep $1|awk '{print $1"|"$2}'`
			;;
		FreeBSD|Darwin)
			list=`ps -c -U $LOGNAME -o pid,comm|sed -n '2,$p'|grep $1|awk '{print $1"|"$2}'`
			;;
		HP-UX|SunOS|Linux)
			list=`ps -u $LOGNAME|sed -n '2,$p'|grep $1|awk '{print $1"|"$4}'`
			;;
	esac
	for element in $list
	do
		pid=`echo $element|cut -d"|" -f1`
		pname=`echo $element|cut -d"|" -f2`
		if [ "$pname" = "$1" ]
		then
			echo "kill -s $SIGNAL $pid($pname)"
			if [ $VIRTUAL = "no" ]
			then
				kill -s $SIGNAL $pid
			fi
		fi
	done
	shift
done
