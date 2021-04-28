#!/bin/sh
#
# wtou means windows to unix.
# usage:wtou.sh file ...
# version: 1.0
# OS: AIX,HP-UX,Solaris,FreeBSD,Linux
# author: luojian(luojian1983@sohu.com)
# history:
#   2008-10-28  1.0 released
#   2008-11-25	1.1 released
#		use tr instead of sed for problems on AIX HP-UX Solaris
#

if [ $# -eq 0 ]
then
	printf "usage: %s file ...\n" $0
	exit 1
fi
for file in $*
do
	echo $file
	if [ ! -f $file ]
	then
		printf "warn: $file is not a file!\n" $file
	else
		cat $file | tr -d '' > $file.tmp
		mv -f $file.tmp $file
	fi
done
