#!/bin/sh
cd /data/ais
. /root/.bashrc

ulimit -c unlimited

while [ 1 ]
do
num=`ps -ef|grep ais|grep 8888|grep -v grep|wc -l`
if [ $num -lt 1 ]
then

  /data/ais/ais -C /data/ais/conf/ais8888.conf -s /tmp/aiss8888 -d
fi
num=`ps -ef|grep ais|grep 8887|grep -v grep|wc -l`
if [ $num -lt 1 ]
then

  /data/ais/ais -C /data/ais/conf/ais8887.conf -s /tmp/aiss8887 -d
fi
num=`ps -ef|grep ais|grep 8886|grep -v grep|wc -l`
if [ $num -lt 1 ]
then

  /data/ais/ais -C /data/ais/conf/ais8886.conf -s /tmp/aiss8886 -d
fi
ps aufx|grep ass|awk '{if($3>=66) print $2}'|while read pid  
do
    kill -9 $pid
done
sleep 2
done
